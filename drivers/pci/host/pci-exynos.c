/*
 * PCIe host controller driver for Samsung EXYNOS SoCs
 *
 * Copyright (C) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Author: Kisang Lee <kisang80.lee@samsung.com>
 * 	   Kwangho Kim <kwangho2.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/resource.h>
#include <linux/signal.h>
#include <linux/types.h>
#include <linux/exynos-pci-noti.h>
#include <linux/pm_qos.h>
#include <linux/exynos-pci-ctrl.h>
#include <dt-bindings/pci/pci.h>
#ifdef CONFIG_CPU_IDLE
#include <soc/samsung/exynos-powermode.h>
#include <soc/samsung/exynos-pm.h>
#endif

#include "pcie-designware.h"
#include "pci-exynos.h"

#include <linux/kthread.h>
#include <linux/random.h>

/* #define CONFIG_DYNAMIC_PHY_OFF */

struct exynos_pcie g_pcie[MAX_RC_NUM];
#ifdef CONFIG_PM_DEVFREQ
static struct pm_qos_request exynos_pcie_int_qos[MAX_RC_NUM];
#endif

#ifdef CONFIG_CPU_IDLE
static int exynos_pci_power_mode_event(struct notifier_block *nb,
		unsigned long event, void *data);
#endif
static int sec_argos_l1ss_notifier(struct notifier_block *notifier, unsigned long speed, void *v);
static void exynos_pcie_resumed_phydown(struct pcie_port *pp);
static void exynos_pcie_assert_phy_reset(struct pcie_port *pp);
void exynos_pcie_send_pme_turn_off(struct exynos_pcie *exynos_pcie);
void exynos_pcie_poweroff(int ch_num);
int exynos_pcie_poweron(int ch_num);
static int exynos_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
				u32 val);
static int exynos_pcie_rd_own_conf(struct pcie_port *pp, int where, int size,
				u32 *val);
static int exynos_pcie_rd_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 *val);
static int exynos_pcie_wr_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 val);
static void exynos_pcie_prog_viewport_cfg0(struct pcie_port *pp, u32 busdev);
static void exynos_pcie_prog_viewport_mem_outbound(struct pcie_port *pp);

static struct notifier_block argos_l1ss_nb = {
	.notifier_call = sec_argos_l1ss_notifier,
};

static struct notifier_block argos_l1ss_nb2 = {
	.notifier_call = sec_argos_l1ss_notifier,
};

static inline void exynos_elb_writel(struct exynos_pcie *pcie, u32 val, u32 reg)
{
	writel(val, pcie->elbi_base + reg);
}

static inline u32 exynos_elb_readl(struct exynos_pcie *pcie, u32 reg)
{
	return readl(pcie->elbi_base + reg);
}

static inline void exynos_phy_writel(struct exynos_pcie *pcie, u32 val, u32 reg)
{
	writel(val, pcie->phy_base + reg);
}

static inline u32 exynos_phy_readl(struct exynos_pcie *pcie, u32 reg)
{
	return readl(pcie->phy_base + reg);
}

static inline void exynos_blk_writel(struct exynos_pcie *pcie, u32 val, u32 reg)
{
	writel(val, pcie->sysreg_base + reg);
}

static inline u32 exynos_blk_readl(struct exynos_pcie *pcie, u32 reg)
{
	return readl(pcie->sysreg_base + reg);
}

/* Get EP pci_dev structure of BUS 1 */
static struct pci_dev *exynos_pcie_get_pci_dev(struct pcie_port *pp)
{
	struct pci_bus *ep_pci_bus;
	static struct pci_dev *ep_pci_dev;
	u32 val;

	if (ep_pci_dev != NULL)
		return ep_pci_dev;

	/* Get EP vendor/device ID to get pci_dev structure */
	ep_pci_bus = pci_find_bus(0, 1); /* Find bus Domain 0, Bus 1(WIFI) */
	exynos_pcie_rd_other_conf(pp, ep_pci_bus, 0, PCI_VENDOR_ID, 4, &val);

	ep_pci_dev = pci_get_device(val & ID_MASK, (val >> 16) & ID_MASK, NULL);

	return ep_pci_dev;

}

static int exynos_pcie_set_l1ss(int enable, struct pcie_port *pp, int id)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val;
	unsigned long flags;
	struct pci_dev *ep_pci_dev;
	u32 exp_cap_off = PCIE_CAP_OFFSET;

	/* This function is only for BCM WIFI devices */
	if (exynos_pcie->ep_device_type != EP_BCM_WIFI) {
		dev_err(pp->dev, "Can't set L1SS(It's not WIFI device)!!!\n");
		return -EINVAL;
	}

	dev_dbg(pp->dev, "%s: START (state = 0x%x, id = 0x%x, enable = %d)\n",
			__func__, exynos_pcie->l1ss_ctrl_id_state, id, enable);
	spin_lock_irqsave(&exynos_pcie->conf_lock, flags);
	if (exynos_pcie->state != STATE_LINK_UP || exynos_pcie->atu_ok == 0) {
		if (enable)
			exynos_pcie->l1ss_ctrl_id_state &= ~(id);
		else
			exynos_pcie->l1ss_ctrl_id_state |= id;
		spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);
		dev_info(pp->dev, "%s: It's not needed. This will be set later."
				"(state = 0x%x, id = 0x%x)\n",
				__func__, exynos_pcie->l1ss_ctrl_id_state, id);
		return -1;
	}

	ep_pci_dev = exynos_pcie_get_pci_dev(pp);
	if (ep_pci_dev == NULL) {
		spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);
		dev_err(pp->dev, "Failed to set L1SS %s (pci_dev == NULL)!!!\n",
					enable ? "ENABLE" : "FALSE");
		return -EINVAL;
	}

	if (enable) {
		exynos_pcie->l1ss_ctrl_id_state &= ~(id);
		if (exynos_pcie->l1ss_ctrl_id_state == 0) {
			/* RC ASPM Enable */
			exynos_pcie_rd_own_conf(pp,
					exp_cap_off + PCI_EXP_LNKCTL, 4, &val);
			val &= ~PCI_EXP_LNKCTL_ASPMC;
			val |= PCI_EXP_LNKCTL_CCC | PCI_EXP_LNKCTL_ASPM_L1;
			exynos_pcie_wr_own_conf(pp,
					exp_cap_off + PCI_EXP_LNKCTL, 4, val);

			/* EP ASPM Enable */
			pci_read_config_dword(ep_pci_dev, WIFI_L1SS_LINKCTRL,
						&val);
			val |= WIFI_ASPM_L1_ENTRY_EN;
			pci_write_config_dword(ep_pci_dev, WIFI_L1SS_LINKCTRL,
						val);
		}
	} else {
		if (exynos_pcie->l1ss_ctrl_id_state) {
			exynos_pcie->l1ss_ctrl_id_state |= id;
		} else {
			exynos_pcie->l1ss_ctrl_id_state |= id;
			/* EP ASPM Disable */
			pci_read_config_dword(ep_pci_dev,
					WIFI_L1SS_LINKCTRL, &val);
			val &= ~(WIFI_ASPM_CONTROL_MASK);
			pci_write_config_dword(ep_pci_dev,
					WIFI_L1SS_LINKCTRL, val);

			/* RC ASPM Disable */
			exynos_pcie_rd_own_conf(pp,
					exp_cap_off + PCI_EXP_LNKCTL, 4, &val);
			val &= ~PCI_EXP_LNKCTL_ASPMC;
			exynos_pcie_wr_own_conf(pp,
					exp_cap_off + PCI_EXP_LNKCTL, 4, val);
		}
	}
	spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);
	dev_dbg(pp->dev, "%s: END (state = 0x%x, id = 0x%x, enable = %d)\n",
			__func__, exynos_pcie->l1ss_ctrl_id_state, id, enable);
	return 0;
}

int exynos_pcie_l1ss_ctrl(int enable, int id)
{
	struct pcie_port *pp = NULL;
	int i;

	for (i = 0; i < MAX_RC_NUM; i++) {
		if (g_pcie[i].ep_device_type == EP_BCM_WIFI) {
			pp = &g_pcie[i].pp;
			break;
		}
	}

	if (pp != NULL)
		return	exynos_pcie_set_l1ss(enable, pp, id);
	else
		return -EINVAL;
}
EXPORT_SYMBOL(exynos_pcie_l1ss_ctrl);
static int sec_argos_l1ss_notifier(struct notifier_block *notifier,
		unsigned long speed, void *v)
{
	struct pcie_port *pp = &g_pcie[0].pp;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	printk("%s - speed : %ld, l1ss_enable = %d\n", __func__, speed, exynos_pcie->l1ss_enable);
	if (speed > TPUT_THRESHOLD && exynos_pcie->l1ss_enable == 1) {
		if(exynos_pcie_set_l1ss(0, pp, PCIE_L1SS_CTRL_ARGOS) == 0)
			exynos_pcie->l1ss_enable = 0;
		else
			exynos_pcie->l1ss_enable = 1;
	} else if (speed <= TPUT_THRESHOLD && exynos_pcie->l1ss_enable == 0) {
		if(exynos_pcie_set_l1ss(1, pp, PCIE_L1SS_CTRL_ARGOS) == 0)
			exynos_pcie->l1ss_enable = 1;
		else
			exynos_pcie->l1ss_enable = 0;
	}

	return NOTIFY_OK;
}
void exynos_pcie_register_dump(int ch_num)
{
	struct pcie_port *pp = &g_pcie[ch_num].pp;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 i, j, val;

	/* Link Reg : 0x0 ~ 0x2C4 */
	pr_err("Print ELBI region...\n");
	for (i = 0; i < 45; i++) {
		for (j = 0; j < 4; j++) {
			if (((i * 0x10) + (j * 4)) < 0x2C4) {
				pr_err("ELBI 0x%04x : 0x%08x\n",
					(i * 0x10) + (j * 4),
					readl(exynos_pcie->elbi_base
						+ (i * 0x10) + (j * 4)));
			}
		}
	}
	pr_err("\n");

	/* PHY Reg : 0x0 ~ 0x180 */
	pr_err("Print PHY region...\n");
	for (i = 0; i < 0x200; i += 4) {
		pr_err("PHY PMA 0x%04x : 0x%08x\n",
				i, readl(exynos_pcie->phy_base + i));

	}
	pr_err("\n");

	/* PHY PCS : 0x0 ~ 0x200 */
	pr_err("Print PHY PCS region...\n");
	for (i = 0; i < 0x200; i += 4) {
		pr_err("PHY PCS 0x%04x : 0x%08x\n",
				i, readl(exynos_pcie->phy_pcs_base + i));

	}
	pr_err("\n");

	/* RC Conf : 0x0 ~ 0x40 */
	pr_err("Print DBI region...\n");
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (((i * 0x10) + (j * 4)) < 0x40) {
				exynos_pcie_rd_own_conf(pp,
						(i * 0x10) + (j * 4), 4, &val);
				pr_err("DBI 0x%04x : 0x%08x\n",
						(i * 0x10) + (j * 4), val);
			}
		}
	}
	pr_err("\n");

	exynos_pcie_rd_own_conf(pp, 0x78, 4, &val);
	pr_err("RC Conf 0x0078(Device Status Register): 0x%08x\n", val);
	exynos_pcie_rd_own_conf(pp, 0x80, 4, &val);
	pr_err("RC Conf 0x0080(Link Status Register): 0x%08x\n", val);
	exynos_pcie_rd_own_conf(pp, 0x104, 4, &val);
	pr_err("RC Conf 0x0104(AER Registers): 0x%08x\n", val);
	exynos_pcie_rd_own_conf(pp, 0x110, 4, &val);
	pr_err("RC Conf 0x0110(AER Registers): 0x%08x\n", val);
	exynos_pcie_rd_own_conf(pp, 0x130, 4, &val);
	pr_err("RC Conf 0x0130(AER Registers): 0x%08x\n", val);
}
EXPORT_SYMBOL(exynos_pcie_register_dump);

static int l1ss_test_thread(void *arg)
{
	u32 msec, rnum;

	while (!kthread_should_stop()) {
		get_random_bytes(&rnum, sizeof(int));
		msec = rnum % 10000;

		if (msec % 2 == 0)
			msec %= 1000;

		pr_err("%d msec L1SS disable!!!\n", msec);
		exynos_pcie_l1ss_ctrl(0, (0x1 << 31));
		msleep(msec);

		pr_err("%d msec L1SS enable!!!\n", msec);
		exynos_pcie_l1ss_ctrl(1, (0x1 << 31));
		msleep(msec);

	}
	return 0;
}

static int chk_pcie_link(struct exynos_pcie *exynos_pcie)
{
	u32 val;
	int test_result = 0;

	exynos_pcie_poweron(exynos_pcie->ch_num);
	val = exynos_elb_readl(exynos_pcie,
					PCIE_ELBI_RDLH_LINKUP) & 0x1f;
	if (val >= 0x0d && val <= 0x14) {
		pr_info("PCIe link test Success.\n");
	} else {
		pr_info("PCIe Link test Fail...\n");
		test_result = -1;
	}

	return test_result;

}

static int chk_dbi_access(struct exynos_pcie *exynos_pcie)
{
	u32 val;
	int test_result = 0;
	struct pcie_port *pp = &exynos_pcie->pp;

	exynos_pcie_wr_own_conf(pp, PCI_COMMAND, 4, 0x140);
	exynos_pcie_rd_own_conf(pp, PCI_COMMAND, 4, &val);
	if ((val & 0xfff) == 0x140) {
		pr_info("PCIe DBI access Success.\n");
	} else {
		pr_info("PCIe DBI access Fail...\n");
		test_result = -1;
	}

	return test_result;
}

static int chk_epconf_access(struct exynos_pcie *exynos_pcie)
{
	u32 val;
	int test_result = 0;
	struct pcie_port *pp = &exynos_pcie->pp;
	struct pci_bus *ep_pci_bus;

	ep_pci_bus = pci_find_bus(0, 1);
	exynos_pcie_wr_other_conf(pp, ep_pci_bus,
					0, PCI_COMMAND, 4, 0x146);
	exynos_pcie_rd_other_conf(pp, ep_pci_bus,
					0, PCI_COMMAND, 4, &val);
	if ((val & 0xfff) == 0x146) {
		pr_info("PCIe DBI access Success.\n");
	} else {
		pr_info("PCIe DBI access Fail...\n");
		test_result = -1;
	}

	return test_result;
}

static int chk_epmem_access(struct exynos_pcie *exynos_pcie)
{
	u32 val;
	int test_result = 0;
	struct pcie_port *pp = &exynos_pcie->pp;
	struct pci_bus *ep_pci_bus;
	void __iomem *reg_addr;

	ep_pci_bus = pci_find_bus(0, 1);
	exynos_pcie_wr_other_conf(pp, ep_pci_bus, 0, PCI_BASE_ADDRESS_0,
				4, lower_32_bits(pp->mem_base));
	exynos_pcie_rd_other_conf(pp, ep_pci_bus, 0, PCI_BASE_ADDRESS_0,
				4, &val);
	pr_info("Set BAR0 : 0x%x\n", val);

	reg_addr = ioremap(pp->mem_base, SZ_4K);

	val = readl(reg_addr);
	iounmap(reg_addr);
	if (val != 0xffffffff) {
		pr_info("PCIe EP Outbound mem access Success.\n");
	} else {
		pr_info("PCIe EP Outbound mem access Fail...\n");
		test_result = -1;
	}

	return test_result;
}

static int chk_l12_mode(struct exynos_pcie *exynos_pcie)
{
	u32 val, counter;
	int test_result = 0;

	pr_info("Wait for flush workqueue...\n");
	flush_delayed_work(&exynos_pcie->l1ss_boot_delay_work);

	counter = 0;
	while (1) {
		pr_info("Wait for L1.2 state...\n");
		val = exynos_elb_readl(exynos_pcie,
				PCIE_ELBI_RDLH_LINKUP) & 0x1f;
		if (val == 0x14)
			break;
		msleep(500);
			if (counter > 10)
			break;
		counter++;
	}

	if (counter > 10) {
		pr_info("Can't wait for L1.2 state...\n");
		return -1;
	}

	val = readl(exynos_pcie->phy_base + PHY_PLL_STATE);
	if ((val & CHK_PHY_PLL_LOCK) == 0) {
		pr_info("PCIe PHY is NOT LOCKED.(Success)\n");
	} else {
		pr_info("PCIe PHY is LOCKED...(Fail)\n");
		test_result = -1;
	}

	return test_result;
}

static int chk_link_recovery(struct exynos_pcie *exynos_pcie)
{
	u32 val;
	int test_result = 0;

	exynos_elb_writel(exynos_pcie, 0x1, PCIE_APP_REQ_EXIT_L1);
	val = exynos_elb_readl(exynos_pcie, PCIE_APP_REQ_EXIT_L1_MODE);
	val &= ~APP_REQ_EXIT_L1_MODE;
	exynos_elb_writel(exynos_pcie, val, PCIE_APP_REQ_EXIT_L1_MODE);
	pr_info("%s: Before set perst, gpio val = %d\n",
		__func__, gpio_get_value(exynos_pcie->perst_gpio));
	gpio_set_value(exynos_pcie->perst_gpio, 0);
	pr_info("%s: After set perst, gpio val = %d\n",
		__func__, gpio_get_value(exynos_pcie->perst_gpio));
	val = exynos_elb_readl(exynos_pcie, PCIE_APP_REQ_EXIT_L1_MODE);
	val |= APP_REQ_EXIT_L1_MODE;
	exynos_elb_writel(exynos_pcie, val, PCIE_APP_REQ_EXIT_L1_MODE);
	exynos_elb_writel(exynos_pcie, 0x0, PCIE_APP_REQ_EXIT_L1);
	msleep(5000);

	val = exynos_elb_readl(exynos_pcie,
				PCIE_ELBI_RDLH_LINKUP) & 0x1f;
	if (val >= 0x0d && val <= 0x14) {
		pr_info("PCIe link Recovery test Success.\n");
	} else {
		/* If recovery callback is defined, pcie poweron
		 * function will not be called.
		 */
		exynos_pcie_poweroff(exynos_pcie->ch_num);
		exynos_pcie_poweron(exynos_pcie->ch_num);
		val = exynos_elb_readl(exynos_pcie,
				PCIE_ELBI_RDLH_LINKUP) & 0x1f;
		if (val >= 0x0d && val <= 0x14) {
			pr_info("PCIe link Recovery test Success.\n");
		} else {
			pr_info("PCIe Link Recovery test Fail...\n");
			test_result = -1;
		}
	}

	return test_result;
}

static int chk_pcie_dislink(struct exynos_pcie *exynos_pcie)
{
	u32 val;
	int test_result = 0;

	exynos_pcie_poweroff(exynos_pcie->ch_num);

	val = exynos_elb_readl(exynos_pcie,
				PCIE_ELBI_RDLH_LINKUP) & 0x1f;
	if (val == 0x15) {
		pr_info("PCIe link Down test Success.\n");
	} else {
		pr_info("PCIe Link Down test Fail...\n");
		test_result = -1;
	}

	return test_result;
}

static int chk_hard_access(void __iomem *check_mem)
{
	u64 count = 0;
	u32 chk_val[10];
	int i, test_result = 0;

	while (count < 1000000) {
		for (i = 0; i < 10; i++) {
			dw_pcie_cfg_read(check_mem + (i * 4),
					4, &chk_val[i]);
		}
		count++;
		usleep_range(100, 100);
		if (count % 5000 == 0) {
			pr_info("================================\n");
			for (i = 0; i < 10; i++) {
				pr_info("Check Value : 0x%x\n", chk_val[i]);
			}
			pr_info("================================\n");
		}

		/* Check Value */
		for (i = 0; i < 10; i++) {
			if (chk_val[i] != 0xffffffff)
				break;

			if (i == 9)
				test_result = -1;
		}
	}

	return test_result;
}

static ssize_t show_pcie(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	int ret = 0;

	ret += snprintf(buf + ret, PAGE_SIZE - ret, ">>>> PCIe Test <<<<\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "0 : PCIe Unit Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "1 : Link Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "2 : DisLink Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"3 : Runtime L1.2 En/Disable Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"4 : Stop Runtime L1.2 En/Disable Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"5 : SysMMU Enable\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"6 : SysMMU Disable\n");

	return ret;
}

static ssize_t store_pcie(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int op_num, test_result = 0;
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);
	int wlan_gpio = of_get_named_gpio(dev->of_node, "pcie,wlan-gpio", 0);
	static struct task_struct *task;
	u64 iommu_addr, iommu_size;

	if (sscanf(buf, "%10d%llx%llx", &op_num, &iommu_addr, &iommu_size) == 0)
		return -EINVAL;

	switch (op_num) {
	case 0:
		dev_info(dev, "PCIe UNIT test START.\n");

		if (wlan_gpio >= 0) {
			gpio_set_value(wlan_gpio, 1);
		} else {
			dev_info(dev, "WLAN_RGT_ON is not defined!!!\n");
			test_result = -1;
			break;
		}

		mdelay(100);
		/* Test PCIe Link */
		if (chk_pcie_link(exynos_pcie)) {
			dev_info(dev, "PCIe UNIT test FAIL[1/7]!!!\n");
			break;
		}

		/* Test PCIe DBI access */
		if (chk_dbi_access(exynos_pcie)) {
			dev_info(dev, "PCIe UNIT test FAIL[2/7]!!!\n");
			break;
		}

		/* Test EP configuration access */
		if (chk_epconf_access(exynos_pcie)) {
			dev_info(dev, "PCIe UNIT test FAIL[3/7]!!!\n");
			break;
		}

		/* Test EP Outbound memory region */
		if (chk_epmem_access(exynos_pcie)) {
			dev_info(dev, "PCIe UNIT test FAIL[4/7]!!!\n");
			break;
		}

		/* ASPM L1.2 test */
		if (chk_l12_mode(exynos_pcie)) {
			dev_info(dev, "PCIe UNIT test FAIL[5/7]!!!\n");
			break;
		}

		/* PCIe Link recovery test */
		if (chk_link_recovery(exynos_pcie)) {
			dev_info(dev, "PCIe UNIT test FAIL[6/7]!!!\n");
			break;
		}

		/* PCIe DisLink Test */
		if (chk_pcie_dislink(exynos_pcie)) {
			dev_info(dev, "PCIe UNIT test FAIL[7/7]!!!\n");
			break;
		}

		if (wlan_gpio >= 0)
			gpio_set_value(wlan_gpio, 0);

		dev_info(dev, "PCIe UNIT test SUCCESS!!!\n");

		break;

	case 1:
		if (wlan_gpio >= 0)
			gpio_set_value(wlan_gpio, 1);

		mdelay(100);
		exynos_pcie_poweron(exynos_pcie->ch_num);
		break;

	case 2:
		exynos_pcie_poweroff(exynos_pcie->ch_num);
		if (wlan_gpio >= 0)
			gpio_set_value(wlan_gpio, 0);
		break;

	case 3:
		dev_info(dev, "L1.2 En/Disable thread Start!!!\n");
		task = kthread_run(l1ss_test_thread, NULL, "L1SS_Test");
		break;

	case 4:
		dev_info(dev, "L1.2 En/Disable thread Stop!!!\n");
		if (task != NULL)
			kthread_stop(task);
		else
			dev_err(dev, "task is NULL!!!!\n");
		break;

	case 5:
		dev_info(dev, "Enable SysMMU feature.\n");
		exynos_pcie->use_sysmmu = true;
		pcie_sysmmu_enable();

		break;
	case 6:
		dev_info(dev, "Disable SysMMU feature.\n");
		exynos_pcie->use_sysmmu = false;
		pcie_sysmmu_disable();
		break;

	case 7:
		dev_info(dev, "SysMMU MAP - 0x%llx(sz:0x%llx)\n",
				iommu_addr, iommu_size);
		pcie_iommu_map(iommu_addr, iommu_addr, iommu_size, 0x3);
		break;

	case 8:
		dev_info(dev, "SysMMU UnMAP - 0x%llx(sz:0x%llx)\n",
			iommu_addr, iommu_size);
		pcie_iommu_unmap(iommu_addr, iommu_size);
		break;

	case 9:
		do {
			/*
			 * Hard access test is needed to check PHY configuration
			 * values. If PHY values are NOT stable, Link down
			 * interrupt will occur.
			 */
			void __iomem *ep_mem_region;
			struct pci_bus *ep_pci_bus;
			struct pcie_port *pp = &exynos_pcie->pp;
			u32 val;

			/* Prepare to access EP memory */
			ep_pci_bus = pci_find_bus(0, 1);
			exynos_pcie_wr_other_conf(pp, ep_pci_bus,
						0, PCI_COMMAND, 4, 0x146);
			exynos_pcie_rd_other_conf(pp, ep_pci_bus,
						0, PCI_COMMAND, 4, &val);
			exynos_pcie_wr_other_conf(pp, ep_pci_bus, 0,
					PCI_BASE_ADDRESS_0, 4,
					lower_32_bits(pp->mem_base));
			exynos_pcie_rd_other_conf(pp, ep_pci_bus, 0,
					PCI_BASE_ADDRESS_0, 4, &val);

			dev_info(dev, "PCIe hard access Start.(20min)\n");
			/* DBI access */
			if (chk_hard_access(exynos_pcie->rc_dbi_base)) {
				dev_info(dev, "DBI hared access test FAIL!\n");
				break;
			}

			/* EP configuration access */
			if (chk_hard_access(pp->va_cfg0_base)) {
				dev_info(dev, "EP config access test FAIL!\n");
				break;
			}

			/* EP outbound memory access */
			ep_mem_region = ioremap(pp->mem_base, SZ_4K);
			if (chk_hard_access(ep_mem_region)) {
				dev_info(dev, "EP mem access test FAIL!\n");
				iounmap(ep_mem_region);
				break;
			}
			iounmap(ep_mem_region);
			dev_info(dev, "PCIe hard access Success.\n");
		} while (0);

		break;

	case 10:
		dev_info(dev, "L1.2 Disable....\n");
		exynos_pcie_l1ss_ctrl(0, 0x40000000);
		break;
	case 11:
		dev_info(dev, "L1.2 Enable....\n");
		exynos_pcie_l1ss_ctrl(1, 0x40000000);
		break;

	default:
		dev_err(dev, "Unsupported Test Number(%d)...\n", op_num);
	}

	return count;
}

static DEVICE_ATTR(pcie_sysfs, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP,
			show_pcie, store_pcie);

static inline int create_pcie_sys_file(struct device *dev)
{
	return device_create_file(dev, &dev_attr_pcie_sysfs);
}

static inline void remove_pcie_sys_file(struct device *dev)
{
	device_remove_file(dev, &dev_attr_pcie_sysfs);
}

static void __maybe_unused exynos_pcie_notify_callback(struct pcie_port *pp,
							int event)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	if (exynos_pcie->event_reg && exynos_pcie->event_reg->callback &&
			(exynos_pcie->event_reg->events & event)) {
		struct exynos_pcie_notify *notify =
			&exynos_pcie->event_reg->notify;
		notify->event = event;
		notify->user = exynos_pcie->event_reg->user;
		dev_info(pp->dev, "Callback for the event : %d\n", event);
		exynos_pcie->event_reg->callback(notify);
	} else {
		dev_info(pp->dev, "Client driver does not have registration "
					"of the event : %d\n", event);
		dev_info(pp->dev, "Force PCIe poweroff --> poweron\n");
		exynos_pcie_poweroff(exynos_pcie->ch_num);
		exynos_pcie_poweron(exynos_pcie->ch_num);
	}
}

int exynos_pcie_dump_link_down_status(int ch_num)
{
	struct pcie_port *pp = &g_pcie[ch_num].pp;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	if (exynos_pcie->state == STATE_LINK_UP) {
		dev_info(pp->dev, "LTSSM: 0x%08x\n",
			readl(exynos_pcie->elbi_base + PCIE_ELBI_RDLH_LINKUP));
		dev_info(pp->dev, "LTSSM_H: 0x%08x\n",
			readl(exynos_pcie->elbi_base + PCIE_CXPL_DEBUG_INFO_H));
		dev_info(pp->dev, "DMA_MONITOR1: 0x%08x\n",
			readl(exynos_pcie->elbi_base + PCIE_DMA_MONITOR1));
		dev_info(pp->dev, "DMA_MONITOR2: 0x%08x\n",
			readl(exynos_pcie->elbi_base + PCIE_DMA_MONITOR2));
		dev_info(pp->dev, "DMA_MONITOR3: 0x%08x\n",
			readl(exynos_pcie->elbi_base + PCIE_DMA_MONITOR3));
	} else
		dev_info(pp->dev, "PCIE link state is %d\n",
				exynos_pcie->state);

	return 0;
}
EXPORT_SYMBOL(exynos_pcie_dump_link_down_status);

#ifdef USE_PANIC_NOTIFIER
static int exynos_pcie_dma_mon_event(struct notifier_block *nb,
				unsigned long event, void *data)
{
	int ret;
	struct exynos_pcie *exynos_pcie =
		container_of(nb, struct exynos_pcie, ss_dma_mon_nb);

	ret = exynos_pcie_dump_link_down_status(exynos_pcie->ch_num);
	if (exynos_pcie->state == STATE_LINK_UP)
		exynos_pcie_register_dump(exynos_pcie->ch_num);

	return notifier_from_errno(ret);
}
#endif

static int exynos_pcie_clock_get(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	struct exynos_pcie_clks	*clks = &exynos_pcie->clks;
	int i, total_clk_num, phy_count;

	/*
	 * CAUTION - PCIe and phy clock have to define in order.
	 * You must define related PCIe clock first in DT.
	 */
	total_clk_num = exynos_pcie->pcie_clk_num + exynos_pcie->phy_clk_num;

	for (i = 0; i < total_clk_num; i++) {
		if (i < exynos_pcie->pcie_clk_num) {
			clks->pcie_clks[i] = of_clk_get(pp->dev->of_node, i);
			if (IS_ERR(clks->pcie_clks[i])) {
				dev_err(pp->dev, "Failed to get pcie clock\n");
				return -ENODEV;
			}
		} else {
			phy_count = i - exynos_pcie->pcie_clk_num;
			clks->phy_clks[phy_count] =
				of_clk_get(pp->dev->of_node, i);
			if (IS_ERR(clks->phy_clks[i])) {
				dev_err(pp->dev, "Failed to get pcie clock\n");
				return -ENODEV;
			}
		}
	}

	return 0;
}

static int exynos_pcie_clock_enable(struct pcie_port *pp, int enable)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	struct exynos_pcie_clks	*clks = &exynos_pcie->clks;
	int i;
	int ret = 0;

	if (enable) {
		for (i = 0; i < exynos_pcie->pcie_clk_num; i++)
			ret = clk_prepare_enable(clks->pcie_clks[i]);
#ifdef CONFIG_SEC_PANIC_PCIE_ERR
			if (ret)
				panic("[PCIe PANIC Case#2] clk fail!\n");
#endif		
	} else {
		for (i = 0; i < exynos_pcie->pcie_clk_num; i++)
			clk_disable_unprepare(clks->pcie_clks[i]);
	}

	return ret;
}

static int exynos_pcie_phy_clock_enable(struct pcie_port *pp, int enable)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	struct exynos_pcie_clks	*clks = &exynos_pcie->clks;
	int i;
	int ret = 0;

	if (enable) {
		for (i = 0; i < exynos_pcie->phy_clk_num; i++)
			ret = clk_prepare_enable(clks->phy_clks[i]);
#ifdef CONFIG_SEC_PANIC_PCIE_ERR
			if (ret)
				panic("[PCIe PANIC Case#2] PHY clk fail!\n");
#endif		
	} else {
		for (i = 0; i < exynos_pcie->phy_clk_num; i++)
			clk_disable_unprepare(clks->phy_clks[i]);
	}

	return ret;
}
void exynos_pcie_print_link_history(struct pcie_port *pp)
{
	struct device *dev = pp->dev;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 history_buffer[32];
	int i;

	for (i = 31; i >= 0; i--)
		history_buffer[i] = exynos_elb_readl(exynos_pcie,
				PCIE_HISTORY_REG(i));
	for (i = 31; i >= 0; i--)
		dev_info(dev, "LTSSM: 0x%02x, L1sub: 0x%x, D state: 0x%x\n",
				LTSSM_STATE(history_buffer[i]),
				L1SUB_STATE(history_buffer[i]),
				PM_DSTATE(history_buffer[i]));
}

static void exynos_pcie_setup_rc(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 pcie_cap_off = PCIE_CAP_OFFSET;
	u32 pm_cap_off = PM_CAP_OFFSET;
	u32 val;

	/* enable writing to DBI read-only registers */
	exynos_pcie_wr_own_conf(pp, PCIE_MISC_CONTROL, 4, DBI_RO_WR_EN);

	/* change vendor ID and device ID for PCIe */
	exynos_pcie_wr_own_conf(pp, PCI_VENDOR_ID, 2, PCI_VENDOR_ID_SAMSUNG);
	exynos_pcie_wr_own_conf(pp, PCI_DEVICE_ID, 2,
			    PCI_DEVICE_ID_EXYNOS + exynos_pcie->ch_num);

	/* set max link width & speed : Gen2, Lane1 */
	exynos_pcie_rd_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCAP, 4, &val);
	val &= ~(PCI_EXP_LNKCAP_L1EL | PCI_EXP_LNKCAP_MLW | PCI_EXP_LNKCAP_SLS);
	val |= PCI_EXP_LNKCAP_L1EL_64USEC | PCI_EXP_LNKCAP_MLW_X1;

	if (exynos_pcie->ip_ver >= 0x981000 && exynos_pcie->ch_num == 1)
		val |= PCI_EXP_LNKCTL2_TLS_8_0GB;
	else
		val |= PCI_EXP_LNKCAP_SLS_5_0GB;

	exynos_pcie_wr_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCAP, 4, val);

	/* set auxiliary clock frequency: 26MHz */
	exynos_pcie_wr_own_conf(pp, PCIE_AUX_CLK_FREQ_OFF, 4,
			PCIE_AUX_CLK_FREQ_26MHZ);

	/* set duration of L1.2 & L1.2.Entry */
	exynos_pcie_wr_own_conf(pp, PCIE_L1_SUBSTATES_OFF, 4, PCIE_L1_SUB_VAL);

	/* clear power management control and status register */
	exynos_pcie_wr_own_conf(pp, pm_cap_off + PCI_PM_CTRL, 4, 0x0);

	/* initiate link retraining */
	exynos_pcie_rd_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL, 4, &val);
	val |= PCI_EXP_LNKCTL_RL;
	exynos_pcie_wr_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL, 4, val);

	/* set target speed from DT */
	exynos_pcie_rd_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL2, 4, &val);
	val &= ~PCI_EXP_LNKCTL2_TLS;
	val |= exynos_pcie->max_link_speed;
	exynos_pcie_wr_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL2, 4, val);

	/* For bifurcation PHY */
	if (exynos_pcie->ip_ver == 0x889500) {
		/* enable PHY 2Lane Bifurcation mode */
		val = exynos_blk_readl(exynos_pcie,
				PCIE_WIFI0_PCIE_PHY_CONTROL);
		val &= ~BIFURCATION_MODE_DISABLE;
		val |= LINK0_ENABLE | PCS_LANE0_ENABLE;
		exynos_blk_writel(exynos_pcie, val,
				PCIE_WIFI0_PCIE_PHY_CONTROL);
	}
}

static void exynos_pcie_prog_viewport_cfg0(struct pcie_port *pp, u32 busdev)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	/* Program viewport 0 : OUTBOUND : CFG0 */
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_VIEWPORT, 4,
			    PCIE_ATU_REGION_OUTBOUND | PCIE_ATU_REGION_INDEX1);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_BASE, 4, pp->cfg0_base);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_BASE, 4,
					(pp->cfg0_base >> 32));
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LIMIT, 4,
					pp->cfg0_base + pp->cfg0_size - 1);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_TARGET, 4, busdev);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_TARGET, 4, 0);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR1, 4, PCIE_ATU_TYPE_CFG0);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR2, 4, PCIE_ATU_ENABLE);

	exynos_pcie->atu_ok = 1;
}

static void exynos_pcie_prog_viewport_cfg1(struct pcie_port *pp, u32 busdev)
{
	/* Program viewport 1 : OUTBOUND : CFG1 */
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_VIEWPORT, 4,
			    PCIE_ATU_REGION_OUTBOUND | PCIE_ATU_REGION_INDEX1);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR1, 4, PCIE_ATU_TYPE_CFG1);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_BASE, 4, pp->cfg1_base);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_BASE, 4,
					(pp->cfg1_base >> 32));
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LIMIT, 4,
					pp->cfg1_base + pp->cfg1_size - 1);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_TARGET, 4, busdev);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_TARGET, 4, 0);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR2, 4, PCIE_ATU_ENABLE);
}

#ifdef CONFIG_SOC_EXYNOS8895
/*
 * The below code is only for 8895 to solve memory shortage problem
 * CAUTION - If you want to use the below code properly, you should
 * remove code which use Outbound ATU index2 in pcie-designware.c
 */
void dw_pcie_prog_viewport_mem_outbound_index2(struct pcie_port *pp)
{
	/* Program viewport 0 : OUTBOUND : MEM */
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_VIEWPORT, 4,
			PCIE_ATU_REGION_OUTBOUND | PCIE_ATU_REGION_INDEX2);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR1, 4, PCIE_ATU_TYPE_MEM);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_BASE, 4, 0x11B00000);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_BASE, 4, 0x0);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LIMIT, 4, 0x11B0ffff);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_TARGET, 4, 0x11C00000);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_TARGET, 4, 0x0);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR2, 4, PCIE_ATU_ENABLE);
}
#endif

static void exynos_pcie_prog_viewport_mem_outbound(struct pcie_port *pp)
{
	/* Program viewport 0 : OUTBOUND : MEM */
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_VIEWPORT, 4,
			    PCIE_ATU_REGION_OUTBOUND | PCIE_ATU_REGION_INDEX0);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR1, 4, PCIE_ATU_TYPE_MEM);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_BASE, 4, pp->mem_base);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_BASE, 4,
					(pp->mem_base >> 32));
#ifdef CONFIG_SOC_EXYNOS8895
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LIMIT, 4,
				pp->mem_base + pp->mem_size - 1 - SZ_1M);
#else
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LIMIT, 4,
				pp->mem_base + pp->mem_size - 1);
#endif
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_LOWER_TARGET, 4, pp->mem_bus_addr);
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_UPPER_TARGET, 4,
					upper_32_bits(pp->mem_bus_addr));
	exynos_pcie_wr_own_conf(pp, PCIE_ATU_CR2, 4, PCIE_ATU_ENABLE);

#ifdef CONFIG_SOC_EXYNOS8895
	dw_pcie_prog_viewport_mem_outbound_index2(pp);
#endif
}

static int exynos_pcie_establish_link(struct pcie_port *pp)
{
	struct device *dev = pp->dev;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val, busdev;
	int count = 0, try_cnt = 0;

retry:
	/* avoid checking rx elecidle when access DBI */
	if (exynos_pcie->phy_ops.phy_check_rx_elecidle != NULL)
		exynos_pcie->phy_ops.phy_check_rx_elecidle(
				exynos_pcie->phy_pcs_base, IGNORE_ELECIDLE,
				exynos_pcie->ch_num);

	exynos_elb_writel(exynos_pcie, 0x0, PCIE_SOFT_CORE_RESET);
	udelay(20);
	exynos_elb_writel(exynos_pcie, 0x1, PCIE_SOFT_CORE_RESET);

	/* set #PERST high */
	gpio_set_value(exynos_pcie->perst_gpio, 1);
	dev_info(dev, "%s: Set PERST to HIGH, gpio val = %d\n",
			__func__, gpio_get_value(exynos_pcie->perst_gpio));
	usleep_range(18000, 20000);

	/* APP_REQ_EXIT_L1_MODE : BIT0 (0x0 : S/W mode, 0x1 : H/W mode) */
	val = exynos_elb_readl(exynos_pcie, PCIE_APP_REQ_EXIT_L1_MODE);
	val |= APP_REQ_EXIT_L1_MODE;
	val |= L1_REQ_NAK_CONTROL_MASTER;
	exynos_elb_writel(exynos_pcie, val, PCIE_APP_REQ_EXIT_L1_MODE);
	exynos_elb_writel(exynos_pcie, PCIE_LINKDOWN_RST_MANUAL,
			  PCIE_LINKDOWN_RST_CTRL_SEL);

	/* Q-Channel support and L1 NAK enable when AXI pending */
	val = exynos_elb_readl(exynos_pcie, PCIE_QCH_SEL);
	if (exynos_pcie->ip_ver >= 0x889500) {
		val &= ~(CLOCK_GATING_PMU_MASK | CLOCK_GATING_APB_MASK |
				CLOCK_GATING_AXI_MASK);
	} else {
		val &= ~CLOCK_GATING_MASK;
		val |= CLOCK_NOT_GATING;
	}
	exynos_elb_writel(exynos_pcie, val, PCIE_QCH_SEL);

	exynos_pcie_assert_phy_reset(pp);

	if (exynos_pcie->ip_ver == 0x889000) {
		exynos_elb_writel(exynos_pcie, 0x1, PCIE_L1_BUG_FIX_ENABLE);
	}

	/* setup root complex */
	dw_pcie_setup_rc(pp);
	exynos_pcie_setup_rc(pp);

	if (exynos_pcie->phy_ops.phy_check_rx_elecidle != NULL)
		exynos_pcie->phy_ops.phy_check_rx_elecidle(
				exynos_pcie->phy_pcs_base, ENABLE_ELECIDLE,
				exynos_pcie->ch_num);

	dev_info(dev, "D state: %x, %x\n",
		 exynos_elb_readl(exynos_pcie, PCIE_PM_DSTATE) & 0x7,
		 exynos_elb_readl(exynos_pcie, PCIE_ELBI_RDLH_LINKUP));

	/* assert LTSSM enable */
	exynos_elb_writel(exynos_pcie, PCIE_ELBI_LTSSM_ENABLE,
				PCIE_APP_LTSSM_ENABLE);
	count = 0;
	while (count < MAX_TIMEOUT) {
		val = exynos_elb_readl(exynos_pcie,
					PCIE_ELBI_RDLH_LINKUP) & 0x1f;
		if (val >= 0x0d && val <= 0x14)
			break;

		count++;

		udelay(10);
	}

	if (count >= MAX_TIMEOUT) {
		try_cnt++;
		dev_err(dev, "%s: Link is not up, try count: %d, %x\n",
			__func__, try_cnt,
			exynos_elb_readl(exynos_pcie, PCIE_ELBI_RDLH_LINKUP));
		if (try_cnt < 10) {
			gpio_set_value(exynos_pcie->perst_gpio, 0);
			dev_info(dev, "%s: Set PERST to LOW, gpio val = %d\n",
				__func__,
				gpio_get_value(exynos_pcie->perst_gpio));
			/* LTSSM disable */
			exynos_elb_writel(exynos_pcie, PCIE_ELBI_LTSSM_DISABLE,
					  PCIE_APP_LTSSM_ENABLE);
			exynos_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
			goto retry;
		} else {
			exynos_pcie_print_link_history(pp);
#ifdef CONFIG_SEC_PANIC_PCIE_ERR
			panic("[PCIe PANIC Case#1] PCIe Link up fail!\n");
#endif			
			if ((exynos_pcie->ip_ver >= 0x889000) &&
				(exynos_pcie->ep_device_type == EP_BCM_WIFI)) {
				return -EPIPE;
			}
			BUG_ON(1);
			return -EPIPE;
		}
	} else {
		dev_info(dev, "%s: Link up:%x\n", __func__,
			 exynos_elb_readl(exynos_pcie, PCIE_ELBI_RDLH_LINKUP));

		exynos_pcie_rd_own_conf(pp, PCIE_LINK_CTRL_STAT, 4, &val);
		val = (val >> 16) & 0xf;
		dev_info(dev, "Current Link Speed is GEN%d (MAX GEN%d)\n",
				val, exynos_pcie->max_link_speed);

		val = exynos_elb_readl(exynos_pcie, PCIE_IRQ_PULSE);
		exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_PULSE);
		val = exynos_elb_readl(exynos_pcie, PCIE_IRQ_EN_PULSE);
		val |= IRQ_LINKDOWN_ENABLE;
		exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_EN_PULSE);

		/* setup ATU for cfg/mem outbound */
		busdev = PCIE_ATU_BUS(1) | PCIE_ATU_DEV(0) | PCIE_ATU_FUNC(0);
		exynos_pcie_prog_viewport_cfg0(pp, busdev);
		exynos_pcie_prog_viewport_mem_outbound(pp);
	}

	return 0;
}

void exynos_pcie_dislink_work(struct work_struct *work)
{
	struct exynos_pcie *exynos_pcie =
		container_of(work, struct exynos_pcie, dislink_work.work);
	struct pcie_port *pp = &exynos_pcie->pp;
	struct device *dev = pp->dev;

	if (exynos_pcie->state == STATE_LINK_DOWN)
		return;

	exynos_pcie_print_link_history(pp);
	exynos_pcie_dump_link_down_status(exynos_pcie->ch_num);
	exynos_pcie_register_dump(exynos_pcie->ch_num);

	exynos_pcie->linkdown_cnt++;
	dev_info(dev, "link down and recovery cnt: %d\n",
			exynos_pcie->linkdown_cnt);

#ifdef CONFIG_SEC_PANIC_PCIE_ERR
	panic("[PCIe Case#4 PANIC] PCIe Link down occurred!\n");
#endif
			
	exynos_pcie_notify_callback(pp, EXYNOS_PCIE_EVENT_LINKDOWN);
}

void exynos_pcie_work_l1ss(struct work_struct *work)
{
	struct exynos_pcie *exynos_pcie =
			container_of(work, struct exynos_pcie,
					l1ss_boot_delay_work.work);
	struct pcie_port *pp = &exynos_pcie->pp;
	struct device *dev = pp->dev;

	if (exynos_pcie->work_l1ss_cnt == 0) {
		dev_info(dev, "[%s]boot_cnt: %d, work_l1ss_cnt: %d\n",
				__func__, exynos_pcie->boot_cnt,
				exynos_pcie->work_l1ss_cnt);
		exynos_pcie_set_l1ss(1, pp, PCIE_L1SS_CTRL_BOOT);
		exynos_pcie->work_l1ss_cnt++;
	} else {
		dev_info(dev, "[%s]boot_cnt: %d, work_l1ss_cnt: %d\n",
				__func__, exynos_pcie->boot_cnt,
				exynos_pcie->work_l1ss_cnt);
		return;
	}
}

static void exynos_pcie_assert_phy_reset(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val;
	int ret;

	ret = exynos_pcie_phy_clock_enable(&exynos_pcie->pp, PCIE_ENABLE_CLOCK);
	dev_err(pp->dev, "phy clk enable, ret value = %d\n", ret);

	if (exynos_pcie->phy_ops.phy_config != NULL)
		exynos_pcie->phy_ops.phy_config(exynos_pcie->phy_base,
				exynos_pcie->phy_pcs_base,
				exynos_pcie->sysreg_base,
				exynos_pcie->elbi_base, exynos_pcie->ch_num);

	if (exynos_pcie->use_ia) {
		void __iomem *ia_base = exynos_pcie->ia_base;

		dev_info(pp->dev, "Set I/A for CDR reset.");
		/* PCIE_SUB_CON setting */
		writel(0x10, exynos_pcie->elbi_base + 0x34); /* Enable TOE */

		/* PCIE_IA SFR Setting */
		writel(0x116a0000, ia_base + 0x30); /* Base addr0 : ELBI */
		writel(0x116d0000, ia_base + 0x34); /* Base addr1 : PMA */
		writel(0x116c0000, ia_base + 0x38); /* Base addr2 : PCS */
		writel(0x11680000, ia_base + 0x3C); /* Base addr3 : PCIE_IA */

		/* L1.2 EXIT Interrupt Happens */
		/* SQ0) UIR_1 : DATA MASK */
		writel(0x50000004, ia_base + 0x100); /* DATA MASK_REG */
		writel(0x20, ia_base + 0x104); /* Enable bit 5 */

		/* SQ1) BRT */
		writel(0x205101ec, ia_base + 0x108); /* READ AFC_DONE */
		writel(0x20, ia_base + 0x10C);

		/* SQ2) Write CDR_EN 0xC0 */
		writel(0x400200d0, ia_base + 0x110);
		writel(0xc0, ia_base + 0x114);

		/* SQ3) Write CDR_EN 0x80 */
		writel(0x400200d0, ia_base + 0x118);
		writel(0x80, ia_base + 0x11C);

		/* SQ4) Write CDR_EN 0x0 */
		writel(0x400200d0, ia_base + 0x120);
		writel(0x0, ia_base + 0x124);

		/* SQ5) Clear L1.2 Exit Interrupt */
		writel(0x70000004, ia_base + 0x128);
		writel(0x10, ia_base + 0x12C);

		/* SQ6) Return to IDLE */
		writel(0x80000000, ia_base + 0x130);
		writel(0x0, ia_base + 0x134);

		/* PCIE_IA Enable */
		writel(0x1, ia_base + 0x0);
	}

	/* Bus number enable */
	val = exynos_elb_readl(exynos_pcie, PCIE_SW_WAKE);
	val &= ~(0x1 << 1);
	exynos_elb_writel(exynos_pcie, val, PCIE_SW_WAKE);
}

static void exynos_pcie_enable_interrupts(struct pcie_port *pp)
{
	u32 val;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	/* enable INTX interrupt */
	val = IRQ_INTA_ASSERT | IRQ_INTB_ASSERT |
		IRQ_INTC_ASSERT | IRQ_INTD_ASSERT;
	exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_EN_PULSE);

	/* disable TOE PULSE2 interrupt */
	exynos_elb_writel(exynos_pcie, 0x0, PCIE_IRQ_TOE_EN_PULSE2);
	/* disable TOE LEVEL interrupt */
	exynos_elb_writel(exynos_pcie, 0x0, PCIE_IRQ_TOE_EN_LEVEL);

	/* disable LEVEL interrupt */
	exynos_elb_writel(exynos_pcie, 0x0, PCIE_IRQ_EN_LEVEL);
	/* disable SPECIAL interrupt */
	exynos_elb_writel(exynos_pcie, 0x0, PCIE_IRQ_EN_SPECIAL);
}

static irqreturn_t exynos_pcie_irq_handler(int irq, void *arg)
{
	struct pcie_port *pp = arg;
	u32 val;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	/* handle PULSE interrupt */
	val = exynos_elb_readl(exynos_pcie, PCIE_IRQ_PULSE);
	exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_PULSE);

	if (val & IRQ_LINK_DOWN) {
		dev_info(pp->dev, "!!!PCIE LINK DOWN (state : 0x%x)!!!\n", val);
		exynos_pcie->state = STATE_LINK_DOWN_TRY;
		queue_work(exynos_pcie->pcie_wq,
				&exynos_pcie->dislink_work.work);
	}
#ifdef CONFIG_PCI_MSI
	if (val & IRQ_MSI_RISING_ASSERT && exynos_pcie->use_msi) {
		dw_handle_msi_irq(pp);

		/* Mask & Clear MSI to pend MSI interrupt.
		 * After clearing IRQ_PULSE, MSI interrupt can be ignored if
		 * lower MSI status bit is set while processing upper bit.
		 * Through the Mask/Unmask, ignored interrupts will be pended.
		 */
		exynos_pcie_wr_own_conf(pp, PCIE_MSI_INTR0_MASK, 4, 0xffffffff);
		exynos_pcie_wr_own_conf(pp, PCIE_MSI_INTR0_MASK, 4, 0x0);
	}
#endif

	/* handle LEVEL interrupt */
	val = exynos_elb_readl(exynos_pcie, PCIE_IRQ_LEVEL);
	exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_LEVEL);

	/* handle SPECIAL interrupt */
	val = exynos_elb_readl(exynos_pcie, PCIE_IRQ_SPECIAL);
	exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_SPECIAL);

	return IRQ_HANDLED;
}

#ifdef CONFIG_PCI_MSI
static void exynos_pcie_msi_init(struct pcie_port *pp)
{
	u32 val;
	u64 msi_target;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	if (!exynos_pcie->use_msi)
		return;

	/*
	 * dw_pcie_msi_init() function allocates msi_data.
	 * The following code is added to avoid duplicated allocation.
	 */
	msi_target = virt_to_phys((void *)pp->msi_data);

	/* program the msi_data */
	exynos_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_LO, 4,
			    (u32)(msi_target & 0xffffffff));
	exynos_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_HI, 4,
			    (u32)(msi_target >> 32 & 0xffffffff));

	/* enable MSI interrupt */
	val = exynos_elb_readl(exynos_pcie, PCIE_IRQ_EN_PULSE);
	val |= IRQ_MSI_CTRL_EN_RISING_EDG;
	exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_EN_PULSE);

	/* Enable MSI interrupt after PCIe reset */
	val = (u32)(*pp->msi_irq_in_use);
	exynos_pcie_wr_own_conf(pp, PCIE_MSI_INTR0_ENABLE, 4, val);
}
#endif

static int exynos_pcie_rd_own_conf(struct pcie_port *pp, int where, int size,
				u32 *val)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	int is_linked = 0;
	int ret = 0;
	u32 __maybe_unused reg_val;

#ifdef CONFIG_DYNAMIC_PHY_OFF
	ret = regmap_read(exynos_pcie->pmureg,
			PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			&reg_val);
	if (reg_val == 0 || ret != 0) {
		dev_info(pp->dev, "PCIe PHY is disabled...\n");
		*val = 0xffffffff;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
#endif
	if (exynos_pcie->state == STATE_LINK_UP)
		is_linked = 1;

	if (is_linked == 0) {
		exynos_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		exynos_pcie_phy_clock_enable(&exynos_pcie->pp,
				PCIE_ENABLE_CLOCK);

		if (exynos_pcie->phy_ops.phy_check_rx_elecidle != NULL)
			exynos_pcie->phy_ops.phy_check_rx_elecidle(
				exynos_pcie->phy_pcs_base, IGNORE_ELECIDLE,
				exynos_pcie->ch_num);
	}

	ret = dw_pcie_cfg_read(exynos_pcie->rc_dbi_base + (where), size, val);

	if (is_linked == 0) {
		if (exynos_pcie->phy_ops.phy_check_rx_elecidle != NULL)
			exynos_pcie->phy_ops.phy_check_rx_elecidle(
				exynos_pcie->phy_pcs_base, ENABLE_ELECIDLE,
				exynos_pcie->ch_num);

		exynos_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
		exynos_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
	}

	return ret;
}

static int exynos_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
				u32 val)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	int is_linked = 0;
	int ret = 0;
	u32 __maybe_unused reg_val;

#ifdef CONFIG_DYNAMIC_PHY_OFF
	ret = regmap_read(exynos_pcie->pmureg,
			PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			&reg_val);
	if (reg_val == 0 || ret != 0) {
		dev_info(pp->dev, "PCIe PHY is disabled...\n");
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
#endif

	if (exynos_pcie->state == STATE_LINK_UP)
		is_linked = 1;

	if (is_linked == 0) {
		exynos_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		exynos_pcie_phy_clock_enable(&exynos_pcie->pp,
				PCIE_ENABLE_CLOCK);

		if (exynos_pcie->phy_ops.phy_check_rx_elecidle != NULL)
			exynos_pcie->phy_ops.phy_check_rx_elecidle(
				exynos_pcie->phy_pcs_base, IGNORE_ELECIDLE,
				exynos_pcie->ch_num);
	}

	ret = dw_pcie_cfg_write(exynos_pcie->rc_dbi_base + (where), size, val);

	if (is_linked == 0) {
		if (exynos_pcie->phy_ops.phy_check_rx_elecidle != NULL)
			exynos_pcie->phy_ops.phy_check_rx_elecidle(
				exynos_pcie->phy_pcs_base, ENABLE_ELECIDLE,
				exynos_pcie->ch_num);

		exynos_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
		exynos_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
	}

	return ret;
}

static int exynos_pcie_rd_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 *val)
{
	int ret, type;
	u32 busdev, cfg_size;
	u64 cpu_addr;
	void __iomem *va_cfg_base;

	busdev = PCIE_ATU_BUS(bus->number) | PCIE_ATU_DEV(PCI_SLOT(devfn)) |
		 PCIE_ATU_FUNC(PCI_FUNC(devfn));

	if (bus->parent->number == pp->root_bus_nr) {
		type = PCIE_ATU_TYPE_CFG0;
		cpu_addr = pp->cfg0_base;
		cfg_size = pp->cfg0_size;
		va_cfg_base = pp->va_cfg0_base;
		/* setup ATU for cfg/mem outbound */
		exynos_pcie_prog_viewport_cfg0(pp, busdev);
	} else {
		type = PCIE_ATU_TYPE_CFG1;
		cpu_addr = pp->cfg1_base;
		cfg_size = pp->cfg1_size;
		va_cfg_base = pp->va_cfg1_base;
		/* setup ATU for cfg/mem outbound */
		exynos_pcie_prog_viewport_cfg1(pp, busdev);
	}

	ret = dw_pcie_cfg_read(va_cfg_base + where, size, val);

	return ret;
}

static int exynos_pcie_wr_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 val)
{

	int ret, type;
	u32 busdev, cfg_size;
	u64 cpu_addr;
	void __iomem *va_cfg_base;

	busdev = PCIE_ATU_BUS(bus->number) | PCIE_ATU_DEV(PCI_SLOT(devfn)) |
		 PCIE_ATU_FUNC(PCI_FUNC(devfn));

	if (bus->parent->number == pp->root_bus_nr) {
		type = PCIE_ATU_TYPE_CFG0;
		cpu_addr = pp->cfg0_base;
		cfg_size = pp->cfg0_size;
		va_cfg_base = pp->va_cfg0_base;
		/* setup ATU for cfg/mem outbound */
		exynos_pcie_prog_viewport_cfg0(pp, busdev);
	} else {
		type = PCIE_ATU_TYPE_CFG1;
		cpu_addr = pp->cfg1_base;
		cfg_size = pp->cfg1_size;
		va_cfg_base = pp->va_cfg1_base;
		/* setup ATU for cfg/mem outbound */
		exynos_pcie_prog_viewport_cfg1(pp, busdev);
	}

	ret = dw_pcie_cfg_write(va_cfg_base + where, size, val);

	return ret;
}

static int exynos_pcie_link_up(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val;

	if (exynos_pcie->state != STATE_LINK_UP)
		return 0;

	val = exynos_elb_readl(exynos_pcie, PCIE_ELBI_RDLH_LINKUP) & 0x1f;
	if (val >= 0x0d && val <= 0x15)
		return 1;

	return 0;
}

static void exynos_pcie_host_init(struct pcie_port *pp)
{
	/* Setup RC to avoid initialization faile in PCIe stack */
	dw_pcie_setup_rc(pp);
}
static struct pcie_host_ops exynos_pcie_host_ops = {
	.rd_own_conf = exynos_pcie_rd_own_conf,
	.wr_own_conf = exynos_pcie_wr_own_conf,
	.rd_other_conf = exynos_pcie_rd_other_conf,
	.wr_other_conf = exynos_pcie_wr_other_conf,
	.link_up = exynos_pcie_link_up,
	.host_init = exynos_pcie_host_init,
};

static int __init add_pcie_port(struct pcie_port *pp,
				struct platform_device *pdev)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	int ret;
	u64 __maybe_unused msi_target;

	pp->irq = platform_get_irq(pdev, 0);
	if (!pp->irq) {
		dev_err(&pdev->dev, "failed to get irq\n");
		return -ENODEV;
	}
	ret = devm_request_irq(&pdev->dev, pp->irq, exynos_pcie_irq_handler,
				IRQF_SHARED | IRQF_TRIGGER_HIGH, "exynos-pcie", pp);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq\n");
		return ret;
	}

	pp->root_bus_nr = 0;
	pp->ops = &exynos_pcie_host_ops;

	exynos_pcie_setup_rc(pp);
	spin_lock_init(&exynos_pcie->conf_lock);
	ret = dw_pcie_host_init(pp);
#ifdef CONFIG_PCI_MSI
	dw_pcie_msi_init(pp);
#ifdef MODIFY_MSI_ADDR
	/* Change MSI address to fit within a 32bit address boundary */
	free_page(pp->msi_data);

	pp->msi_data = (u64)kmalloc(4, GFP_DMA);
	msi_target = virt_to_phys((void *)pp->msi_data);

	if ((msi_target >> 32) != 0)
		dev_info(&pdev->dev,
			"MSI memory is allocated over 32bit boundary\n");

	/* program the msi_data */
	exynos_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_LO, 4,
			    (u32)(msi_target & 0xffffffff));
	exynos_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_HI, 4,
			    (u32)(msi_target >> 32 & 0xffffffff));
#endif
#endif
	if (ret) {
		dev_err(&pdev->dev, "failed to initialize host\n");
		return ret;
	}

	dev_info(&pdev->dev, "MSI Message Address : 0x%llx\n",
			virt_to_phys((void *)pp->msi_data));

	return 0;
}

static void enable_cache_cohernecy(struct pcie_port *pp, int enable)
{
	int set_val;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	if (exynos_pcie->ip_ver > 0x889500) {
		/* Set enable value */
		if (enable) {
			dev_info(pp->dev, "Enable cache coherency.\n");
			set_val = PCIE_SYSREG_SHARABLE_ENABLE;
		} else {
			dev_info(pp->dev, "Disable cache coherency.\n");
			set_val = PCIE_SYSREG_SHARABLE_DISABLE;
		}

		/* Set configurations */
		regmap_update_bits(exynos_pcie->sysreg,
			PCIE_SYSREG_SHARABILITY_CTRL, /* SYSREG address */
			(0x3 << (PCIE_SYSREG_SHARABLE_OFFSET
				+ exynos_pcie->ch_num * 4)), /* Mask */
			(set_val << (PCIE_SYSREG_SHARABLE_OFFSET
				     + exynos_pcie->ch_num * 4))); /* Set val */
	}
}
static int exynos_pcie_parse_dt(struct device_node *np, struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	const char *use_cache_coherency;
	const char *use_msi;
	const char *use_sicd;
	const char *use_sysmmu;
	const char *use_ia;
	int wlan_gpio;

	if (of_property_read_u32(np, "ip-ver", &exynos_pcie->ip_ver)) {
		dev_err(pp->dev, "Failed to parse the number of ip-ver\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "pcie-clk-num",
					&exynos_pcie->pcie_clk_num)) {
		dev_err(pp->dev, "Failed to parse the number of pcie clock\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "phy-clk-num",
					&exynos_pcie->phy_clk_num)) {
		dev_err(pp->dev, "Failed to parse the number of phy clock\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "ep-device-type",
				&exynos_pcie->ep_device_type)) {
		dev_err(pp->dev, "EP device type is NOT defined...(WIFI?)\n");
		/* Default EP device is BCM WIFI */
		exynos_pcie->ep_device_type = EP_BCM_WIFI;
	}

	if (of_property_read_u32(np, "max-link-speed",
				&exynos_pcie->max_link_speed)) {
		dev_err(pp->dev, "MAX Link Speed is NOT defined...(GEN1)\n");
		/* Default Link Speed is GEN1 */
		exynos_pcie->max_link_speed = LINK_SPEED_GEN1;
	}

	if (!of_property_read_string(np, "use-cache-coherency",
						&use_cache_coherency)) {
		if (!strcmp(use_cache_coherency, "true")) {
			dev_info(pp->dev, "Cache Coherency unit is ENABLED.\n");
			exynos_pcie->use_cache_coherency = true;
		} else if (!strcmp(use_cache_coherency, "false")) {
			exynos_pcie->use_cache_coherency = false;
		} else {
			dev_err(pp->dev, "Invalid use-cache-coherency value"
					"(Set to default -> false)\n");
			exynos_pcie->use_cache_coherency = false;
		}
	} else {
		exynos_pcie->use_cache_coherency = false;
	}

	if (!of_property_read_string(np, "use-msi", &use_msi)) {
		if (!strcmp(use_msi, "true")) {
			exynos_pcie->use_msi = true;
		} else if (!strcmp(use_msi, "false")) {
			exynos_pcie->use_msi = false;
		} else {
			dev_err(pp->dev, "Invalid use-msi value"
					"(Set to default -> true)\n");
			exynos_pcie->use_msi = true;
		}
	} else {
		exynos_pcie->use_msi = false;
	}

	if (!of_property_read_string(np, "use-sicd", &use_sicd)) {
		if (!strcmp(use_sicd, "true")) {
			exynos_pcie->use_sicd = true;
		} else if (!strcmp(use_sicd, "false")) {
			exynos_pcie->use_sicd = false;
		} else {
			dev_err(pp->dev, "Invalid use-sicd value"
				       "(set to default -> false)\n");
			exynos_pcie->use_sicd = false;
		}
	} else {
		exynos_pcie->use_sicd = false;
	}

	if (!of_property_read_string(np, "use-sysmmu", &use_sysmmu)) {
		if (!strcmp(use_sysmmu, "true")) {
			dev_info(pp->dev, "PCIe SysMMU is ENABLED.\n");
			exynos_pcie->use_sysmmu = true;
		} else if (!strcmp(use_sysmmu, "false")) {
			dev_info(pp->dev, "PCIe SysMMU is DISABLED.\n");
			exynos_pcie->use_sysmmu = false;
		} else {
			dev_err(pp->dev, "Invalid use-sysmmu value"
				       "(set to default -> false)\n");
			exynos_pcie->use_sysmmu = false;
		}
	} else {
		exynos_pcie->use_sysmmu = false;
	}

	if (!of_property_read_string(np, "use-ia", &use_ia)) {
		if (!strcmp(use_ia, "true")) {
			dev_info(pp->dev, "PCIe I/A is ENABLED.\n");
			exynos_pcie->use_ia = true;
		} else if (!strcmp(use_ia, "false")) {
			dev_info(pp->dev, "PCIe I/A is DISABLED.\n");
			exynos_pcie->use_ia = false;
		} else {
			dev_err(pp->dev, "Invalid use-ia value"
				       "(set to default -> false)\n");
			exynos_pcie->use_ia = false;
		}
	} else {
		exynos_pcie->use_ia = false;
	}

#ifdef CONFIG_PM_DEVFREQ
	if (of_property_read_u32(np, "pcie-pm-qos-int",
					&exynos_pcie->int_min_lock))
		exynos_pcie->int_min_lock = 0;

	if (exynos_pcie->int_min_lock)
		pm_qos_add_request(&exynos_pcie_int_qos[exynos_pcie->ch_num],
				PM_QOS_DEVICE_THROUGHPUT, 0);
#endif
	exynos_pcie->pmureg = syscon_regmap_lookup_by_phandle(np,
					"samsung,syscon-phandle");
	if (IS_ERR(exynos_pcie->pmureg)) {
		dev_err(pp->dev, "syscon regmap lookup failed.\n");
		return PTR_ERR(exynos_pcie->pmureg);
	}

	exynos_pcie->sysreg = syscon_regmap_lookup_by_phandle(np,
			"samsung,sysreg-phandle");
	/* Check definitions to access SYSREG in DT*/
	if (IS_ERR(exynos_pcie->sysreg) && IS_ERR(exynos_pcie->sysreg_base)) {
		dev_err(pp->dev, "SYSREG is not defined.\n");
		return PTR_ERR(exynos_pcie->sysreg);
	}

	wlan_gpio = of_get_named_gpio(np, "pcie,wlan-gpio", 0);
	if (wlan_gpio >= 0) {
		dev_info(pp->dev, "WLAN_RGT_ON is defined for test.\n");
		gpio_direction_output(wlan_gpio, 0);
	}

	return 0;
}

static int __init exynos_pcie_probe(struct platform_device *pdev)
{
	struct exynos_pcie *exynos_pcie;
	struct pcie_port *pp;
	struct device_node *np = pdev->dev.of_node;
	struct resource *temp_rsc;
	int ret = 0;
	int ch_num;

	if (create_pcie_sys_file(&pdev->dev))
		dev_err(&pdev->dev, "Failed to create pcie sys file\n");

	if (of_property_read_u32(np, "ch-num", &ch_num)) {
		dev_err(&pdev->dev, "Failed to parse the channel number\n");
		return -EINVAL;
	}

	exynos_pcie = &g_pcie[ch_num];
	pp = &exynos_pcie->pp;
	pp->dev = &pdev->dev;

	exynos_pcie->ch_num = ch_num;
	exynos_pcie->l1ss_enable = 1;
	exynos_pcie->state = STATE_LINK_DOWN;

	exynos_pcie->linkdown_cnt = 0;
	exynos_pcie->boot_cnt = 0;
	exynos_pcie->work_l1ss_cnt = 0;
	exynos_pcie->l1ss_ctrl_id_state = 0;
	exynos_pcie->atu_ok = 0;

	/* parsing pcie dts data for exynos */
	ret = exynos_pcie_parse_dt(pdev->dev.of_node, pp);
	if (ret)
		goto probe_fail;

	exynos_pcie->perst_gpio = of_get_gpio(np, 0);
	if (exynos_pcie->perst_gpio < 0) {
		dev_err(&pdev->dev, "cannot get perst_gpio\n");
	} else {
		ret = devm_gpio_request_one(pp->dev, exynos_pcie->perst_gpio,
					    GPIOF_OUT_INIT_LOW,
					    dev_name(pp->dev));
		if (ret)
			goto probe_fail;
	}

	/* Get pin state */
	exynos_pcie->pcie_pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(exynos_pcie->pcie_pinctrl)) {
		dev_err(&pdev->dev, "Can't get pcie pinctrl!!!\n");
		goto probe_fail;
	}

	exynos_pcie->pin_state[PCIE_PIN_DEFAULT] =
		pinctrl_lookup_state(exynos_pcie->pcie_pinctrl, "default");
	if (IS_ERR(exynos_pcie->pin_state[PCIE_PIN_DEFAULT])) {
		dev_err(&pdev->dev, "Can't set pcie clkerq to output high!\n");
		goto probe_fail;
	}
	exynos_pcie->pin_state[PCIE_PIN_IDLE] =
		pinctrl_lookup_state(exynos_pcie->pcie_pinctrl, "idle");
	if (IS_ERR(exynos_pcie->pin_state[PCIE_PIN_IDLE]))
		dev_err(&pdev->dev, "No idle pin state(but it's OK)!!\n");
	else
		pinctrl_select_state(exynos_pcie->pcie_pinctrl,
				exynos_pcie->pin_state[PCIE_PIN_IDLE]);

	ret = exynos_pcie_clock_get(pp);
	if (ret)
		goto probe_fail;

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "elbi");
	exynos_pcie->elbi_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(exynos_pcie->elbi_base)) {
		ret = PTR_ERR(exynos_pcie->elbi_base);
		goto probe_fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy");
	exynos_pcie->phy_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(exynos_pcie->phy_base)) {
		ret = PTR_ERR(exynos_pcie->phy_base);
		goto probe_fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "sysreg");
	exynos_pcie->sysreg_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(exynos_pcie->sysreg_base)) {
		ret = PTR_ERR(exynos_pcie->sysreg_base);
		goto probe_fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
	exynos_pcie->rc_dbi_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(exynos_pcie->rc_dbi_base)) {
		ret = PTR_ERR(exynos_pcie->rc_dbi_base);
		goto probe_fail;
	}

	pp->dbi_base = exynos_pcie->rc_dbi_base;

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pcs");
	exynos_pcie->phy_pcs_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(exynos_pcie->phy_pcs_base)) {
		ret = PTR_ERR(exynos_pcie->phy_pcs_base);
		goto probe_fail;
	}

	if (exynos_pcie->use_ia) {
		temp_rsc = platform_get_resource_byname(pdev,
						IORESOURCE_MEM, "ia");
		exynos_pcie->ia_base =
				devm_ioremap_resource(&pdev->dev, temp_rsc);
		if (IS_ERR(exynos_pcie->ia_base)) {
			ret = PTR_ERR(exynos_pcie->ia_base);
			goto probe_fail;
		}
	}

	/* Mapping PHY functions */
	exynos_pcie_phy_init(pp);

	if (exynos_pcie->use_cache_coherency)
		enable_cache_cohernecy(pp, 1);

	exynos_pcie_resumed_phydown(pp);

	regmap_update_bits(exynos_pcie->pmureg,
			   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			   PCIE_PHY_CONTROL_MASK, 1);
	ret = add_pcie_port(pp, pdev);
	if (ret)
		goto probe_fail;

	disable_irq(pp->irq);

#ifdef CONFIG_CPU_IDLE
	exynos_pcie->idle_ip_index =
			exynos_get_idle_ip_index(dev_name(&pdev->dev));
	if (exynos_pcie->idle_ip_index < 0) {
		dev_err(&pdev->dev, "Cant get idle_ip_dex!!!\n");
		ret = -EINVAL;
		goto probe_fail;
	}
	exynos_update_ip_idle_status(exynos_pcie->idle_ip_index, PCIE_IS_IDLE);
	exynos_pcie->power_mode_nb.notifier_call = exynos_pci_power_mode_event;
	exynos_pcie->power_mode_nb.next = NULL;
	exynos_pcie->power_mode_nb.priority = 0;

	ret = exynos_pm_register_notifier(&exynos_pcie->power_mode_nb);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register lpa notifier\n");
		goto probe_fail;
	}
#endif

#ifdef USE_PANIC_NOTIFIER
	/* Register Panic notifier */
	exynos_pcie->ss_dma_mon_nb.notifier_call = exynos_pcie_dma_mon_event;
	exynos_pcie->ss_dma_mon_nb.next = NULL;
	exynos_pcie->ss_dma_mon_nb.priority = 0;
	atomic_notifier_chain_register(&panic_notifier_list,
				&exynos_pcie->ss_dma_mon_nb);
#endif

	exynos_pcie->pcie_wq = create_freezable_workqueue("pcie_wq");
	if (IS_ERR(exynos_pcie->pcie_wq)) {
		dev_err(pp->dev, "couldn't create workqueue\n");
		ret = EBUSY;
		goto probe_fail;
	}
	INIT_DELAYED_WORK(&exynos_pcie->dislink_work,
				exynos_pcie_dislink_work);

	INIT_DELAYED_WORK(&exynos_pcie->l1ss_boot_delay_work,
				exynos_pcie_work_l1ss);
	platform_set_drvdata(pdev, exynos_pcie);

	if (exynos_pcie->ch_num == 0) {
		ret = sec_argos_register_notifier(&argos_l1ss_nb, "WIFI");
		if (ret < 0) {
			dev_err(&pdev->dev, "Failed to register WIFI notifier\n");
			goto probe_fail;
		}

		ret = sec_argos_register_notifier(&argos_l1ss_nb2, "P2P");
		if (ret < 0) {
			dev_err(&pdev->dev, "Failed to register P2P notifier\n");
			goto probe_fail;
		}
	}

probe_fail:
	if (ret)
		dev_err(&pdev->dev, "%s: pcie probe failed\n", __func__);
	else
		dev_info(&pdev->dev, "%s: pcie probe success\n", __func__);

	return ret;
}

static int __exit exynos_pcie_remove(struct platform_device *pdev)
{
	struct exynos_pcie *exynos_pcie = platform_get_drvdata(pdev);

#ifdef CONFIG_CPU_IDLE
	exynos_pm_unregister_notifier(&exynos_pcie->power_mode_nb);
#endif

	if (exynos_pcie->ch_num == 0) {
		sec_argos_unregister_notifier(&argos_l1ss_nb, "WIFI");
		sec_argos_unregister_notifier(&argos_l1ss_nb2, "P2P");
	}

	if (exynos_pcie->state != STATE_LINK_DOWN) {
		exynos_pcie_poweroff(exynos_pcie->ch_num);
	}

	remove_pcie_sys_file(&pdev->dev);

	return 0;
}

static void exynos_pcie_shutdown(struct platform_device *pdev)
{
	struct exynos_pcie *exynos_pcie = platform_get_drvdata(pdev);
	int ret;

	ret = atomic_notifier_chain_unregister(&panic_notifier_list,
			&exynos_pcie->ss_dma_mon_nb);
	if (ret)
		dev_err(&pdev->dev,
			"Failed to unregister snapshot panic notifier\n");
}

static const struct of_device_id exynos_pcie_of_match[] = {
	{ .compatible = "samsung,exynos8890-pcie", },
	{ .compatible = "samsung,exynos-pcie", },
	{},
};
MODULE_DEVICE_TABLE(of, exynos_pcie_of_match);

static void exynos_pcie_resumed_phydown(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	int ret;

	/* phy all power down on wifi off during suspend/resume */
	ret = exynos_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
	dev_err(pp->dev, "pcie clk enable, ret value = %d\n", ret);

	exynos_pcie_enable_interrupts(pp);
	regmap_update_bits(exynos_pcie->pmureg,
			   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			   PCIE_PHY_CONTROL_MASK, 1);

	exynos_pcie_assert_phy_reset(pp);

	/* phy all power down */
	if (exynos_pcie->phy_ops.phy_all_pwrdn != NULL) {
		exynos_pcie->phy_ops.phy_all_pwrdn(exynos_pcie->phy_base,
				exynos_pcie->phy_pcs_base,
				exynos_pcie->sysreg_base, exynos_pcie->ch_num);
	}

	exynos_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
#ifdef CONFIG_DYNAMIC_PHY_OFF
	regmap_update_bits(exynos_pcie->pmureg,
			   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			   PCIE_PHY_CONTROL_MASK, 0);
#endif
	exynos_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
}

void exynos_pcie_config_l1ss(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val;
	unsigned long flags;
	u32 exp_cap_off = PCIE_CAP_OFFSET;
	struct pci_dev *ep_pci_dev;

	/* This function is only for BCM WIFI devices */
	if (exynos_pcie->ep_device_type != EP_BCM_WIFI) {
		dev_err(pp->dev, "Can't set L1SS(It's not WIFI device)!!!\n");
		return ;
	}

	ep_pci_dev = exynos_pcie_get_pci_dev(pp);
	if (ep_pci_dev == NULL) {
		dev_err(pp->dev,
			"Failed to set L1SS Enable (pci_dev == NULL)!!!\n");
		return ;
	}

	spin_lock_irqsave(&exynos_pcie->conf_lock, flags);
	pci_write_config_dword(ep_pci_dev, WIFI_L1SS_CONTROL2,
				PORT_LINK_TPOWERON_130US);
	pci_write_config_dword(ep_pci_dev, 0x1B4, 0x10031003);
	pci_read_config_dword(ep_pci_dev, 0xD4, &val);
	pci_write_config_dword(ep_pci_dev, 0xD4, val | (1 << 10));

	exynos_pcie_rd_own_conf(pp, PCIE_LINK_L1SS_CONTROL, 4, &val);
	val |= PORT_LINK_TCOMMON_32US | PORT_LINK_L1SS_ENABLE;
	exynos_pcie_wr_own_conf(pp, PCIE_LINK_L1SS_CONTROL, 4, val);
	exynos_pcie_wr_own_conf(pp, PCIE_LINK_L1SS_CONTROL2, 4,
					PORT_LINK_TPOWERON_130US);
	/*
	val = PORT_LINK_L1SS_T_PCLKACK | PORT_LINK_L1SS_T_L1_2 |
	      PORT_LINK_L1SS_T_POWER_OFF;
	*/
	exynos_pcie_wr_own_conf(pp, PCIE_L1_SUBSTATES_OFF, 4, PCIE_L1_SUB_VAL);
	exynos_pcie_wr_own_conf(pp, exp_cap_off + PCI_EXP_DEVCTL2, 4,
					PCI_EXP_DEVCTL2_LTR_EN);

	/* Enable L1SS on Root Complex */
	exynos_pcie_rd_own_conf(pp, exp_cap_off + PCI_EXP_LNKCTL, 4, &val);
	val &= ~PCI_EXP_LNKCTL_ASPMC;
	val |= PCI_EXP_LNKCTL_CCC | PCI_EXP_LNKCTL_ASPM_L1;
	exynos_pcie_wr_own_conf(pp, exp_cap_off + PCI_EXP_LNKCTL, 4, val);

	/* Enable supporting ASPM/PCIPM */
	pci_read_config_dword(ep_pci_dev, WIFI_L1SS_CONTROL, &val);
	pci_write_config_dword(ep_pci_dev, WIFI_L1SS_CONTROL,
		val | WIFI_COMMON_RESTORE_TIME | WIFI_ALL_PM_ENABEL);

	if (exynos_pcie->l1ss_ctrl_id_state == 0) {
		pci_read_config_dword(ep_pci_dev, WIFI_L1SS_LINKCTRL, &val);
		val &= ~(WIFI_ASPM_CONTROL_MASK);
		val |= WIFI_CLK_REQ_EN | WIFI_USE_SAME_REF_CLK |
				WIFI_ASPM_L1_ENTRY_EN;
		pci_write_config_dword(ep_pci_dev, WIFI_L1SS_LINKCTRL, val);

		dev_err(pp->dev, "l1ss enabled(0x%x)\n",
				exynos_pcie->l1ss_ctrl_id_state);
	} else {
		pci_read_config_dword(ep_pci_dev, WIFI_L1SS_LINKCTRL, &val);
		val &= ~(WIFI_ASPM_CONTROL_MASK);
		val |= WIFI_CLK_REQ_EN | WIFI_USE_SAME_REF_CLK;
		pci_write_config_dword(ep_pci_dev, WIFI_L1SS_LINKCTRL, val);

		exynos_pcie_rd_own_conf(pp, exp_cap_off + PCI_EXP_LNKCTL,
						4, &val);
		val &= ~PCI_EXP_LNKCTL_ASPMC;
		exynos_pcie_wr_own_conf(pp, exp_cap_off + PCI_EXP_LNKCTL,
						4, val);
		dev_err(pp->dev, "l1ss disabled(0x%x)\n",
				exynos_pcie->l1ss_ctrl_id_state);
	}
	spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);

}

int exynos_pcie_poweron(int ch_num)
{
	struct pcie_port *pp = &g_pcie[ch_num].pp;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val, vendor_id, device_id;
	int ret;

	dev_info(pp->dev, "%s, start of poweron, pcie state: %d\n", __func__,
							 exynos_pcie->state);

	if (exynos_pcie->state == STATE_LINK_DOWN) {
		ret = exynos_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		dev_err(pp->dev, "pcie clk enable, ret value = %d\n", ret);

		/* Release wakeup maks */
		regmap_update_bits(exynos_pcie->pmureg,
			WAKEUP_MASK,
			(0x1 << (WAKEUP_MASK_PCIE_WIFI + exynos_pcie->ch_num)),
			(0x0 << (WAKEUP_MASK_PCIE_WIFI + exynos_pcie->ch_num)));
#ifdef CONFIG_CPU_IDLE
		if (exynos_pcie->use_sicd)
			exynos_update_ip_idle_status(
					exynos_pcie->idle_ip_index,
					PCIE_IS_ACTIVE);
#endif
#ifdef CONFIG_PM_DEVFREQ
		if (exynos_pcie->int_min_lock)
			pm_qos_update_request(&exynos_pcie_int_qos[ch_num],
					exynos_pcie->int_min_lock);
#endif
		/* Enable SysMMU */
		if (exynos_pcie->use_sysmmu)
			pcie_sysmmu_enable();

		pinctrl_select_state(exynos_pcie->pcie_pinctrl,
				exynos_pcie->pin_state[PCIE_PIN_DEFAULT]);

		regmap_update_bits(exynos_pcie->pmureg,
				   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
				   PCIE_PHY_CONTROL_MASK, 1);

		/* phy all power down clear */
		if (exynos_pcie->phy_ops.phy_all_pwrdn_clear != NULL) {
			exynos_pcie->phy_ops.phy_all_pwrdn_clear(
					exynos_pcie->phy_base,
					exynos_pcie->phy_pcs_base,
					exynos_pcie->sysreg_base,
					exynos_pcie->ch_num);
		}

		exynos_pcie->state = STATE_LINK_UP_TRY;

		/* Enable history buffer */
		val = exynos_elb_readl(exynos_pcie, PCIE_STATE_HISTORY_CHECK);
		exynos_elb_writel(exynos_pcie, 0, PCIE_STATE_HISTORY_CHECK);
		val = HISTORY_BUFFER_ENABLE;
		exynos_elb_writel(exynos_pcie, val, PCIE_STATE_HISTORY_CHECK);

		exynos_elb_writel(exynos_pcie, 0x200000, PCIE_STATE_POWER_S);
		exynos_elb_writel(exynos_pcie, 0xffffffff, PCIE_STATE_POWER_M);

		enable_irq(pp->irq);
		if (exynos_pcie_establish_link(pp)) {
			dev_err(pp->dev, "pcie link up fail\n");
			goto poweron_fail;
		}
		exynos_pcie->state = STATE_LINK_UP;

		if (!exynos_pcie->probe_ok) {
			exynos_pcie_rd_own_conf(pp, PCI_VENDOR_ID, 4, &val);
			vendor_id = val & ID_MASK;
			device_id = (val >> 16) & ID_MASK;

			exynos_pcie->pci_dev = pci_get_device(vendor_id,
							device_id, NULL);
			if (!exynos_pcie->pci_dev) {
				dev_err(pp->dev, "Failed to get pci device\n");
				goto poweron_fail;
			}

			pci_rescan_bus(exynos_pcie->pci_dev->bus);

			/* L1.2 ASPM enable */
			exynos_pcie_config_l1ss(&exynos_pcie->pp);

#ifdef CONFIG_PCI_MSI
			exynos_pcie_msi_init(pp);
#endif

			if (pci_save_state(exynos_pcie->pci_dev)) {
				dev_err(pp->dev, "Failed to save pcie state\n");
				goto poweron_fail;
			}
			exynos_pcie->pci_saved_configs =
				pci_store_saved_state(exynos_pcie->pci_dev);
			exynos_pcie->probe_ok = 1;
		} else if (exynos_pcie->probe_ok) {
			if (exynos_pcie->boot_cnt == 0) {
				schedule_delayed_work(
					&exynos_pcie->l1ss_boot_delay_work,
					msecs_to_jiffies(40000));
				exynos_pcie->boot_cnt++;
				exynos_pcie->l1ss_ctrl_id_state |=
							PCIE_L1SS_CTRL_BOOT;
			}
#ifdef CONFIG_PCI_MSI
			exynos_pcie_msi_init(pp);
#endif
			if (pci_load_saved_state(exynos_pcie->pci_dev,
					     exynos_pcie->pci_saved_configs)) {
				dev_err(pp->dev, "Failed to load pcie state\n");
				goto poweron_fail;
			}
			pci_restore_state(exynos_pcie->pci_dev);

			/* L1.2 ASPM enable */
			exynos_pcie_config_l1ss(pp);
		}
	}

	dev_info(pp->dev, "%s, end of poweron, pcie state: %d\n", __func__,
		 exynos_pcie->state);

	return 0;

poweron_fail:
	exynos_pcie->state = STATE_LINK_UP;
	exynos_pcie_poweroff(exynos_pcie->ch_num);

	return -EPIPE;
}
EXPORT_SYMBOL(exynos_pcie_poweron);

void exynos_pcie_poweroff(int ch_num)
{
	struct pcie_port *pp = &g_pcie[ch_num].pp;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	unsigned long flags;
	u32 val;

	dev_info(pp->dev, "%s, start of poweroff, pcie state: %d\n", __func__,
		 exynos_pcie->state);

	if (exynos_pcie->state == STATE_LINK_UP ||
	    exynos_pcie->state == STATE_LINK_DOWN_TRY) {
		exynos_pcie->state = STATE_LINK_DOWN_TRY;

		disable_irq(pp->irq);

		val = exynos_elb_readl(exynos_pcie, PCIE_IRQ_EN_PULSE);
		val &= ~IRQ_LINKDOWN_ENABLE;
		exynos_elb_writel(exynos_pcie, val, PCIE_IRQ_EN_PULSE);

		spin_lock_irqsave(&exynos_pcie->conf_lock, flags);
		exynos_pcie_send_pme_turn_off(exynos_pcie);
		exynos_pcie->state = STATE_LINK_DOWN;

		/* Disable SysMMU */
		if (exynos_pcie->use_sysmmu)
			pcie_sysmmu_disable();

		/* Disable history buffer */
		val = exynos_elb_readl(exynos_pcie, PCIE_STATE_HISTORY_CHECK);
		val &= ~HISTORY_BUFFER_ENABLE;
		exynos_elb_writel(exynos_pcie, val, PCIE_STATE_HISTORY_CHECK);

		gpio_set_value(exynos_pcie->perst_gpio, 0);
		dev_info(pp->dev, "%s: Set PERST to LOW, gpio val = %d\n",
			__func__, gpio_get_value(exynos_pcie->perst_gpio));

		/* LTSSM disable */
		writel(PCIE_ELBI_LTSSM_DISABLE,
				exynos_pcie->elbi_base + PCIE_APP_LTSSM_ENABLE);

		/* phy all power down */
		if (exynos_pcie->phy_ops.phy_all_pwrdn != NULL) {
			exynos_pcie->phy_ops.phy_all_pwrdn(
					exynos_pcie->phy_base,
					exynos_pcie->phy_pcs_base,
					exynos_pcie->sysreg_base,
					exynos_pcie->ch_num);
		}

		spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);

		exynos_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
#ifdef CONFIG_DYNAMIC_PHY_OFF
		/* EP device could try to save PCIe state after DisLink
		 * in suspend time. The below code keep PHY power up in this
		 * time.
		 */
		if (pp->dev->power.is_prepared == false)
			regmap_update_bits(exynos_pcie->pmureg,
				   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
				   PCIE_PHY_CONTROL_MASK, 0);
#endif
		exynos_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
		exynos_pcie->atu_ok = 0;

		if (!IS_ERR(exynos_pcie->pin_state[PCIE_PIN_IDLE]))
			pinctrl_select_state(exynos_pcie->pcie_pinctrl,
					exynos_pcie->pin_state[PCIE_PIN_IDLE]);

#ifdef CONFIG_PM_DEVFREQ
		if (exynos_pcie->int_min_lock)
			pm_qos_update_request(&exynos_pcie_int_qos[ch_num], 0);
#endif
#ifdef CONFIG_CPU_IDLE
		if (exynos_pcie->use_sicd)
			exynos_update_ip_idle_status(
					exynos_pcie->idle_ip_index,
					PCIE_IS_IDLE);
#endif
	}

	/* Set wakeup mask */
	regmap_update_bits(exynos_pcie->pmureg,
			WAKEUP_MASK,
			(0x1 << (WAKEUP_MASK_PCIE_WIFI + exynos_pcie->ch_num)),
			(0x1 << (WAKEUP_MASK_PCIE_WIFI + exynos_pcie->ch_num)));

	dev_info(pp->dev, "%s, end of poweroff, pcie state: %d\n",  __func__,
		 exynos_pcie->state);
}
EXPORT_SYMBOL(exynos_pcie_poweroff);

void exynos_pcie_send_pme_turn_off(struct exynos_pcie *exynos_pcie)
{
	struct pcie_port *pp = &exynos_pcie->pp;
	struct device *dev = pp->dev;
	int __maybe_unused count = 0, retry_cnt = 0;
	u32 __maybe_unused val;

	val = readl(exynos_pcie->elbi_base + PCIE_ELBI_RDLH_LINKUP) & 0x1f;
	dev_info(dev, "%s: link state:%x\n", __func__, val);
	if (!(val >= 0x0d && val <= 0x14)) {
		dev_info(dev, "%s, pcie link is not up\n", __func__);
		return;
	}

	exynos_elb_writel(exynos_pcie, 0x1, PCIE_APP_REQ_EXIT_L1);
	val = exynos_elb_readl(exynos_pcie, PCIE_APP_REQ_EXIT_L1_MODE);
	val &= ~APP_REQ_EXIT_L1_MODE;
	val |= L1_REQ_NAK_CONTROL_MASTER;
	exynos_elb_writel(exynos_pcie, val, PCIE_APP_REQ_EXIT_L1_MODE);

retry_pme_turnoff:
	val = readl(exynos_pcie->elbi_base + PCIE_ELBI_RDLH_LINKUP) & 0x1f;
	dev_err(dev, "Current LTSSM State is 0x%x with retry_cnt =%d.\n",
								val, retry_cnt);

	exynos_elb_writel(exynos_pcie, 0x1, XMIT_PME_TURNOFF);

	while (count < MAX_TIMEOUT) {
		if ((exynos_elb_readl(exynos_pcie, PCIE_IRQ_PULSE)
						& IRQ_RADM_PM_TO_ACK)) {
			dev_err(dev, "ack message is ok\n");
			udelay(10);
			break;
		}

		udelay(10);
		count++;
	}

	if (count >= MAX_TIMEOUT)
		dev_err(dev, "cannot receive ack message from wifi\n");

	exynos_elb_writel(exynos_pcie, 0x0, XMIT_PME_TURNOFF);

	count = 0;	
	do {
		val = exynos_elb_readl(exynos_pcie, PCIE_ELBI_RDLH_LINKUP);
		val = val & 0x1f;
		if (val == 0x15) {
			dev_err(dev, "received Enter_L23_READY DLLP packet\n");
			break;
		}
		udelay(10);
		count++;
	} while (count < MAX_TIMEOUT);

	if (count >= MAX_TIMEOUT) {
		if (retry_cnt < 10) {
			retry_cnt++;
			goto retry_pme_turnoff;
		}
		dev_err(dev, "cannot receive L23_READY DLLP packet(0x%x)\n", val);
#ifdef CONFIG_SEC_PANIC_PCIE_ERR
		panic("[PCIe PANIC Case#5] L2/3 READY fail!\n");
#endif
	}
}

void exynos_pcie_pm_suspend(int ch_num)
{
	struct pcie_port *pp = &g_pcie[ch_num].pp;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	unsigned long flags;

	if (exynos_pcie->state == STATE_LINK_DOWN) {
		dev_info(pp->dev, "RC%d already off\n", exynos_pcie->ch_num);
		return;
	}

	spin_lock_irqsave(&exynos_pcie->conf_lock, flags);
	exynos_pcie->state = STATE_LINK_DOWN_TRY;
	spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);

	exynos_pcie_poweroff(ch_num);
}
EXPORT_SYMBOL(exynos_pcie_pm_suspend);

int exynos_pcie_pm_resume(int ch_num)
{
	return exynos_pcie_poweron(ch_num);
}
EXPORT_SYMBOL(exynos_pcie_pm_resume);

#ifdef CONFIG_PM
static int exynos_pcie_suspend_noirq(struct device *dev)
{
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);

	if (exynos_pcie->state == STATE_LINK_DOWN) {
		dev_info(dev, "RC%d already off\n", exynos_pcie->ch_num);
		return 0;
	}

	pr_err("WARNNING - Unexpected PCIe state(try to power OFF)\n");
	exynos_pcie_poweroff(exynos_pcie->ch_num);

	return 0;
}

static int exynos_pcie_resume_noirq(struct device *dev)
{
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);

	if (exynos_pcie->state == STATE_LINK_DOWN) {
		exynos_pcie_resumed_phydown(&exynos_pcie->pp);

#ifdef CONFIG_DYNAMIC_PHY_OFF
		regmap_update_bits(exynos_pcie->pmureg,
			   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			   PCIE_PHY_CONTROL_MASK, 1);
#endif
		return 0;
	}

	pr_err("WARNNING - Unexpected PCIe state(try to power ON)\n");
	regmap_update_bits(exynos_pcie->pmureg,
			   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			   PCIE_PHY_CONTROL_MASK, 1);

	exynos_pcie_poweron(exynos_pcie->ch_num);

	return 0;
}

#ifdef CONFIG_DYNAMIC_PHY_OFF
static int exynos_pcie_suspend_prepare(struct device *dev)
{
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);

	regmap_update_bits(exynos_pcie->pmureg,
			   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			   PCIE_PHY_CONTROL_MASK, 1);
	return 0;
}

static void exynos_pcie_resume_complete(struct device *dev)
{
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);

	if (exynos_pcie->state == STATE_LINK_DOWN)
		regmap_update_bits(exynos_pcie->pmureg,
			   PCIE_PHY_CONTROL + exynos_pcie->ch_num * 4,
			   PCIE_PHY_CONTROL_MASK, 0);
}
#endif

#else
#define exynos_pcie_suspend_noirq	NULL
#define exynos_pcie_resume_noirq	NULL
#endif

static const struct dev_pm_ops exynos_pcie_dev_pm_ops = {
	.suspend_noirq	= exynos_pcie_suspend_noirq,
	.resume_noirq	= exynos_pcie_resume_noirq,
#ifdef CONFIG_DYNAMIC_PHY_OFF
	.prepare	= exynos_pcie_suspend_prepare,
	.complete	= exynos_pcie_resume_complete,
#endif
};

static struct platform_driver exynos_pcie_driver = {
	.remove		= __exit_p(exynos_pcie_remove),
	.shutdown	= exynos_pcie_shutdown,
	.driver = {
		.name	= "exynos-pcie",
		.owner	= THIS_MODULE,
		.of_match_table = exynos_pcie_of_match,
		.pm	= &exynos_pcie_dev_pm_ops,
	},
};

#ifdef CONFIG_CPU_IDLE
static void __maybe_unused exynos_pcie_set_tpoweron(struct pcie_port *pp,
							int max)
{
	void __iomem *ep_dbi_base = pp->va_cfg0_base;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val;

	if (exynos_pcie->state != STATE_LINK_UP)
		return;

	/* Disable ASPM */
	val = readl(ep_dbi_base + WIFI_L1SS_LINKCTRL);
	val &= ~(WIFI_ASPM_CONTROL_MASK);
	writel(val, ep_dbi_base + WIFI_L1SS_LINKCTRL);

	val = readl(ep_dbi_base + WIFI_L1SS_CONTROL);
	writel(val & ~(WIFI_ALL_PM_ENABEL),
			ep_dbi_base + WIFI_L1SS_CONTROL);

	if (max) {
		writel(PORT_LINK_TPOWERON_3100US,
				ep_dbi_base + WIFI_L1SS_CONTROL2);
		exynos_pcie_wr_own_conf(pp, PCIE_LINK_L1SS_CONTROL2, 4,
				PORT_LINK_TPOWERON_3100US);
	} else {
		writel(PORT_LINK_TPOWERON_130US,
				ep_dbi_base + WIFI_L1SS_CONTROL2);
		exynos_pcie_wr_own_conf(pp, PCIE_LINK_L1SS_CONTROL2, 4,
				PORT_LINK_TPOWERON_130US);
	}

	/* Enable L1ss */
	val = readl(ep_dbi_base + WIFI_L1SS_LINKCTRL);
	val |= WIFI_ASPM_L1_ENTRY_EN;
	writel(val, ep_dbi_base + WIFI_L1SS_LINKCTRL);

	val = readl(ep_dbi_base + WIFI_L1SS_CONTROL);
	val |= WIFI_ALL_PM_ENABEL;
	writel(val, ep_dbi_base + WIFI_L1SS_CONTROL);
}

static int exynos_pci_power_mode_event(struct notifier_block *nb,
					unsigned long event, void *data)
{
	int ret = NOTIFY_DONE;
	struct exynos_pcie *exynos_pcie = container_of(nb,
				struct exynos_pcie, power_mode_nb);
	u32 val;
	struct pcie_port *pp = &exynos_pcie->pp;

	switch (event) {
	case LPA_EXIT:
		if (exynos_pcie->state == STATE_LINK_DOWN)
			exynos_pcie_resumed_phydown(&exynos_pcie->pp);
		break;
	case SICD_ENTER:
		if (exynos_pcie->use_sicd) {
			if (exynos_pcie->ip_ver >= 0x889500) {
				if (exynos_pcie->state != STATE_LINK_DOWN) {
					val = readl(exynos_pcie->elbi_base +
						PCIE_ELBI_RDLH_LINKUP) & 0x1f;
					if (val == 0x14 || val == 0x15) {
						ret = NOTIFY_DONE;
						/* Change tpower on time to
						 * value
						 */
						exynos_pcie_set_tpoweron(pp, 1);
					} else {
						ret = NOTIFY_BAD;
					}
				}
			}
		}
		break;
	case SICD_EXIT:
		if (exynos_pcie->use_sicd) {
			if (exynos_pcie->ip_ver >= 0x889500) {
				if (exynos_pcie->state != STATE_LINK_DOWN) {
					/* Change tpower on time to NORMAL
					 * value */
					exynos_pcie_set_tpoweron(pp, 0);
				}
			}
		}
		break;
	default:
		ret = NOTIFY_DONE;
	}

	return notifier_from_errno(ret);
}
#endif

/* Exynos PCIe driver does not allow module unload */

static int __init pcie_init(void)
{
	return platform_driver_probe(&exynos_pcie_driver, exynos_pcie_probe);
}
device_initcall(pcie_init);

int exynos_pcie_register_event(struct exynos_pcie_register_event *reg)
{
	int ret = 0;
	struct pcie_port *pp;
	struct exynos_pcie *exynos_pcie;

	if (!reg) {
		pr_err("PCIe: Event registration is NULL\n");
		return -ENODEV;
	}
	if (!reg->user) {
		pr_err("PCIe: User of event registration is NULL\n");
		return -ENODEV;
	}
	pp = PCIE_BUS_PRIV_DATA(((struct pci_dev *)reg->user));
	exynos_pcie = to_exynos_pcie(pp);
	if (pp) {
		exynos_pcie->event_reg = reg;
		dev_info(pp->dev,
				"Event 0x%x is registered for RC %d\n",
				reg->events, exynos_pcie->ch_num);
	} else {
		pr_err("PCIe: did not find RC for pci endpoint device\n");
		ret = -ENODEV;
	}
	return ret;
}
EXPORT_SYMBOL(exynos_pcie_register_event);

int exynos_pcie_deregister_event(struct exynos_pcie_register_event *reg)
{
	int ret = 0;
	struct pcie_port *pp;
	struct exynos_pcie *exynos_pcie;

	if (!reg) {
		pr_err("PCIe: Event deregistration is NULL\n");
		return -ENODEV;
	}
	if (!reg->user) {
		pr_err("PCIe: User of event deregistration is NULL\n");
		return -ENODEV;
	}
	pp = PCIE_BUS_PRIV_DATA(((struct pci_dev *)reg->user));
	exynos_pcie = to_exynos_pcie(pp);
	if (pp) {
		exynos_pcie->event_reg = NULL;
		dev_info(pp->dev, "Event is deregistered for RC %d\n",
				exynos_pcie->ch_num);
	} else {
		pr_err("PCIe: did not find RC for pci endpoint device\n");
		ret = -ENODEV;
	}
	return ret;
}
EXPORT_SYMBOL(exynos_pcie_deregister_event);

int exynos_pcie_gpio_debug(int ch_num, int option)
{
	void __iomem *sysreg;
	u32 addr, val;

	addr = (GPIO_DEBUG_SFR);

	if (addr == 0x0) {
		pr_err("Not supported feature(%s)!!!\n", __func__);
		return -EINVAL;
	}

	sysreg = ioremap_nocache(addr, 4);
	if (!sysreg) {
		pr_err("Cannot get the sysreg address\n");
		return -EPIPE;
	}
	val = readl(sysreg);
	val &= ~FSYS1_MON_SEL_MASK;
	val |= ch_num + 2;
	val &= ~(PCIE_MON_SEL_MASK << ((ch_num + 2) * 8));
	val |= option << ((ch_num + 2) * 8);
	writel(val, sysreg);
	iounmap(sysreg);

	return 0;
}
EXPORT_SYMBOL(exynos_pcie_gpio_debug);

MODULE_AUTHOR("Kisang Lee <kisang80.lee@samsung.com>");
MODULE_AUTHOR("Kwangho Kim <kwangho2.kim@samsung.com>");
MODULE_DESCRIPTION("Samsung PCIe host controller driver");
MODULE_LICENSE("GPL v2");
