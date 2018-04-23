#include <soc_sctrl_interface.h>
#include <soc_crgperiph_interface.h>
#include <soc_pctrl_interface.h>
#include <soc_usb31_misc_ctrl_interface.h>
#include <soc_usb31_tca_interface.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kfifo.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/wakelock.h>
#include <linux/of_address.h>
#include <linux/hisi/usb/hisi_usb.h>
#include <linux/hisi/usb/hisi_usb_interface.h>
#include <linux/hisi/contexthub/tca.h>
#include <libhwsecurec/securec.h>
#include "common.h"
#ifdef COMBOPHY_ES_BUGFIX
#include "firmware.h"
#endif
extern int hisi_dptx_triger(bool benable);
extern int hisi_dptx_hpd_trigger(TCA_IRQ_TYPE_E irq_type, TCPC_MUX_CTRL_TYPE mode, TYPEC_PLUG_ORIEN_E typec_orien);
extern int hisi_dptx_notify_switch(void);
extern int hisi_usb_otg_event_sync(enum otg_dev_event_type event);
extern int usb31phy_cr_write(u32 addr, u16 value);
extern u16 usb31phy_cr_read(u32 addr);
extern bool __clk_is_enabled(struct clk *clk);
extern void dp_dfp_u_notify_dp_configuration_done(TCPC_MUX_CTRL_TYPE mode_type, int ack);
static int tca_mode_switch(TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien);

#define HISI_TCA_DEBUG              KERN_INFO
#define PD_PLATFORM_INIT_OK (0X56781234)
#define FIFO_SIZE (128)

/*lint -e528 -esym(528,*) */
typedef struct pd_event_member_s {
	TCA_IRQ_TYPE_E irq_type;
	TCPC_MUX_CTRL_TYPE mode_type;
	TCA_DEV_TYPE_E dev_type;
	TYPEC_PLUG_ORIEN_E typec_orien;
}pd_event_t;

typedef enum {
	TCA_POWEROFF = 0,
	TCA_POWERON = 1,
	TCA_POWER_REBOOT = 2,
	TCA_POWER_MAX
}TCA_POWER_TYPE;

static int tca_mode_sw(TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien);

struct tca_device_s {
	int init;
	TCA_POWER_TYPE tca_poweron;
	volatile TCPC_MUX_CTRL_TYPE tca_cur_mode;
	TYPEC_PLUG_ORIEN_E typec_orien;
	volatile int sw_cnt;
	TCA_DEV_TYPE_E usbctrl_status;
	struct wake_lock wlock;
	struct work_struct work;
	struct workqueue_struct *wq;
	struct kfifo_rec_ptr_1 kfifo;
	struct clk *gt_clk_usb3_tcxo_en;
	void __iomem *crgperi_reg_base;
	void __iomem *sctrl_reg_base;
	void __iomem *pctrl_reg_base;
	void __iomem *usb_misc_base;
	void __iomem *tca_base;
};

static DEFINE_MUTEX(tca_mutex);
static struct tca_device_s tca_dev;

static int pd_get_resource(struct tca_device_s *res, struct device *dev)
{
	struct device_node *np;
	/*
	 * map PERI CRG region
	 */
	np = of_find_compatible_node(NULL, NULL, "hisilicon,crgctrl");
	if (!np) {
		pr_err("[%s]get peri cfg node failed!\n", __func__);
		return -EINVAL;
	}
	res->crgperi_reg_base = of_iomap(np, 0);
	if (!res->crgperi_reg_base) {
		pr_err("[%s]iomap crgperi_reg_base failed!\n", __func__);
		return -EINVAL;
	}

	/*
	 * map SCTRL region
	 */
	np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
	if (!np) {
		pr_err("[%s]get sysctrl node failed!\n", __func__);
		goto CRGCTRL_MAP_REL;
	}
	res->sctrl_reg_base = of_iomap(np, 0);
	if (!res->sctrl_reg_base) {
		pr_err("[%s]iomap sctrl_reg_base failed!\n", __func__);
		goto CRGCTRL_MAP_REL;
	}

	/*
	 * map PCTRL region
	 */
	np = of_find_compatible_node(NULL, NULL, "hisilicon,pctrl");
	if (!np) {
		pr_err("[%s]get pctrl node failed!\n",__func__);
		goto SCTRL_MAP_REL;
	}
	res->pctrl_reg_base = of_iomap(np, 0);
	if (!res->pctrl_reg_base) {
		pr_err("[%s]iomap pctrl_reg_base failed!\n",__func__);
		goto SCTRL_MAP_REL;
	}

	/*
	 * map USB2OTG bc region
	 */
	np = of_find_compatible_node(NULL, NULL, "hisilicon,usb3otg_bc");
	if (!np) {
		pr_err("[%s]get usb3otg_bc failed!\n",__func__);
		goto PCTRL_MAP_REL;
	}
	res->usb_misc_base = of_iomap(np, 0);
	if (!res->usb_misc_base) {
		pr_err("[%s]iomap usb3otg_bc failed!\n",__func__);
		goto PCTRL_MAP_REL;
	}

	res->tca_base = res->usb_misc_base + 0x200;

	res->gt_clk_usb3_tcxo_en = devm_clk_get(dev, "clk_usb3_tcxo_en");
	if (IS_ERR_OR_NULL(res->gt_clk_usb3_tcxo_en)) {
		pr_err("[%s]dgt_clk_usb3_tcxo_enfailed!\n",__func__);
		goto USB_MISC_REL;
	}

	return 0;

USB_MISC_REL:
	iounmap(res->usb_misc_base);
PCTRL_MAP_REL:
	iounmap(res->pctrl_reg_base);
SCTRL_MAP_REL:
	iounmap(res->sctrl_reg_base);
CRGCTRL_MAP_REL:
	iounmap(res->crgperi_reg_base);
	return -1;
}
#ifdef COMBOPHY_REBOOT
static int combophy_reboot(void)
{
	int ret = 0;
	printk(HISI_TCA_DEBUG"[%s]\n", __func__);

	if (tca_dev.tca_cur_mode&TCPC_DP) {
		ret = hisi_dptx_triger((bool)0);
		if (ret) {
			pr_err("[%s]hisi_dptx_triger err\n",__func__);
			return ret;
		}
	}

	if (TCA_CHARGER_CONNECT_EVENT == tca_dev.usbctrl_status) {
		ret = hisi_usb_otg_event_sync((enum otg_dev_event_type)TCA_CHARGER_DISCONNECT_EVENT);
		if (ret) {
			pr_err("[%s][%d]hisi_usb_otg_event_sync  err\n", __func__, tca_dev.usbctrl_status);
			return ret;
		}

		tca_dev.usbctrl_status = TCA_CHARGER_DISCONNECT_EVENT;
	}

	if (TCA_ID_FALL_EVENT == tca_dev.usbctrl_status) {
		ret = hisi_usb_otg_event_sync((enum otg_dev_event_type)TCA_ID_RISE_EVENT);
		if (ret) {
			pr_err("[%s][%d]hisi_usb_otg_event_sync  err\n", __func__, tca_dev.usbctrl_status);
			return ret;
		}

		tca_dev.usbctrl_status = TCA_ID_RISE_EVENT;
	}

	writel(BIT(SOC_CRGPERIPH_PERRSTEN4_ip_rst_usb3otg_32k_START)|
		BIT(SOC_CRGPERIPH_PERRSTEN4_ip_hrst_usb3otg_misc_START),
		SOC_CRGPERIPH_PERRSTEN4_ADDR(tca_dev.crgperi_reg_base));
	writel(BIT(SOC_CRGPERIPH_PERRSTDIS4_ip_rst_usb3otg_32k_START)|
		BIT(SOC_CRGPERIPH_PERRSTDIS4_ip_hrst_usb3otg_misc_START),
		SOC_CRGPERIPH_PERRSTDIS4_ADDR(tca_dev.crgperi_reg_base));

	tca_dev.tca_poweron = TCA_POWEROFF;
	return combophy_poweron(TCA_POWER_REBOOT);
}
#endif

void combophy_clk_check(void)
{
	unsigned short int reg;
	pr_err("[%s]\n", __func__);
	writel(0x30300, SOC_USB31_MISC_CTRL_USB_MISC_CFGA0_ADDR(tca_dev.usb_misc_base));
	udelay(20);
	writel(0x30303, SOC_USB31_MISC_CTRL_USB_MISC_CFGA0_ADDR(tca_dev.usb_misc_base));
	udelay(100);
	(void)usb31phy_cr_read(0);
	reg = usb31phy_cr_read(0);
	if (0x74cd != reg) {
		pr_err("[%s]no clk.CR0[0x%x]\n", __func__, reg);
	}
	pr_err("[%s]CR0[0x%x]\n", __func__, reg);
	reg = usb31phy_cr_read(0x05);
	reg |= BIT(0)|BIT(15);
	usb31phy_cr_write(5, reg);
	udelay(100);
	reg = usb31phy_cr_read(0x19);
	if (0 == ((reg>>13)&0x01)){
		pr_err("[%s] clk err.CR0x19[0x%x]\n", __func__, reg);
	}
	pr_err("[%s]CR0x19[0x%x]\n", __func__, reg);
}

void misc_dump(void)
{
	int i;
	pr_err("[USBMISC]+\n");
	for(i=0;i <= 0x250; i+=4) {
		pr_err("[%x]:[%x]\n", i,readl(i+tca_dev.usb_misc_base));
	}
	pr_err("[USBMISC]-\n");

	combophy_clk_check();
}

static int _tca_mode_switch(TCPC_MUX_CTRL_TYPE old_mode,
	TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien)
{
	struct timeval tv;
	volatile unsigned int reg_data = 0x10;
	/*1.	�����л�ǰ���BCģʽ�� 0xff200034[0] = 0���������쳣(���ʱ�����л�)�� */
	if (is_bits_set(BIT(SOC_USB31_MISC_CTRL_BC_CTRL1_bc_mode_START),SOC_USB31_MISC_CTRL_BC_CTRL1_ADDR(tca_dev.usb_misc_base))) {
		pr_err("[%s]BC_CTRL1[0x%x][now is BC status,tca switch is forbidden ]\n",
			__func__, readl(SOC_USB31_MISC_CTRL_BC_CTRL1_ADDR(tca_dev.usb_misc_base)));
		return -EPERM;
	}

	writel(0xFFFF, SOC_USB31_TCA_TCA_INTR_STS_ADDR(tca_dev.tca_base));
	udelay(1);
	/*set_bits(0x3, SOC_USB31_TCA_TCA_INTR_EN_ADDR(tca_dev.tca_base));  for irq mode,but we use poll waitting */
	reg_data |= new_mode;
	reg_data |= (0x01&typec_orien)<<SOC_USB31_TCA_TCA_TCPC_tcpc_connector_orientation_START;
	writel_mask(0x1F,reg_data, SOC_USB31_TCA_TCA_TCPC_ADDR(tca_dev.tca_base));
	tca_dev.typec_orien = typec_orien;
	udelay(1);
	pr_info("[%s]old[%d]new[%d]TCPC[0x%x][0x%x]\n", __func__,
		old_mode, new_mode, reg_data, readl(SOC_USB31_TCA_TCA_TCPC_ADDR(tca_dev.tca_base)));

	do_gettimeofday(&tv);
	pr_info("s:tv_sec %ld,tv_usec: %06ld\n", tv.tv_sec, tv.tv_usec);

	while(is_bits_clr(BIT(SOC_USB31_TCA_TCA_INTR_STS_xa_ack_evt_START),
		SOC_USB31_TCA_TCA_INTR_STS_ADDR(tca_dev.tca_base))) {
			tca_dev.sw_cnt--;
			if(is_bits_set(BIT(SOC_USB31_TCA_TCA_INTR_STS_xa_timeout_evt_START),
				SOC_USB31_TCA_TCA_INTR_STS_ADDR(tca_dev.tca_base))) {
				unsigned int a,b;
				reg_data= readl(SOC_USB31_TCA_TCA_CTRLSYNCMODE_DBG0_ADDR(tca_dev.tca_base));
				a = 0x1&(reg_data>>SOC_USB31_TCA_TCA_CTRLSYNCMODE_DBG0_ss_rxdetect_disable_START);
				b = 0x1&(reg_data>>SOC_USB31_TCA_TCA_CTRLSYNCMODE_DBG0_ss_rxdetect_disable_ack_START);
				if(tca_dev.sw_cnt >0) {
					msleep(50);
					if(a == b) {
						// cppcheck-suppress *
						(void)_tca_mode_switch(old_mode, new_mode, typec_orien);
					}else
						continue;
				}else {
					pr_err("[%s]CTRLSYNCMODE_DBG0 TIMEOUT\n",__func__);
					return -EMLINK;
				}
			}else if(tca_dev.sw_cnt > 0){
				msleep(50);
			}else {
				pr_err("[%s]soc timeout not set;soft timeout\n",__func__);
				return -ERANGE;
			}
	}

	tca_dev.tca_cur_mode = new_mode;
	return 0;
}

void cr_dump(void)
{
	int i;
	pr_err("[%s]++++\n", __func__);
	for(i=0;i <= 0x006f; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	for(i=0x1000;i <= 0x010D8; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	for(i=0x1100;i <= 0x011D8; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	for(i=0x1200;i <= 0x12D8; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	for(i=0x1300;i <= 0x13D8; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	for(i=0x2000;i <= 0x203b; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	for(i=0x3000;i <= 0x30e4; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	for(i=0x6000;i < 0x6000+2737; i++) {
		pr_err("[%x]:[%x]\n", i, usb31phy_cr_read(i));
	}

	pr_err("[%s]-----\n", __func__);
}

static int __tca_mode_sw(TCPC_MUX_CTRL_TYPE old_mode, TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien)
{
	int ret;
	struct timeval tv;

	tca_dev.sw_cnt = 12;
	ret = _tca_mode_switch(old_mode, new_mode,typec_orien);
	do_gettimeofday(&tv);
	pr_info("e:tv_sec %ld,tv_usec: %06ld\n", tv.tv_sec, tv.tv_usec);
	pr_info("[end]BC_CTRL1[0x%x]\n",readl(SOC_USB31_MISC_CTRL_BC_CTRL1_ADDR(tca_dev.usb_misc_base)));
	if (ret)
		misc_dump();
	return ret;
}

static int tca_mode_sw(TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien)
{
	int ret = 0;
	int old_mode = tca_dev.tca_cur_mode;
	if(old_mode == TCPC_NC && TCPC_DP == new_mode) {
		ret = __tca_mode_sw(TCPC_NC, TCPC_USB31_CONNECTED,typec_orien);
		if (ret) {
			pr_err("[%s]_tca_mode_switch  err1 [%d]\n", __func__, __LINE__);
			return ret;
		}
		set_bits(BIT(SOC_USB31_TCA_TCA_CTRLSYNCMODE_CFG0_block_ss_op_START),
			SOC_USB31_TCA_TCA_CTRLSYNCMODE_CFG0_ADDR(tca_dev.tca_base));
		msleep(1);
	}

	ret = __tca_mode_sw(tca_dev.tca_cur_mode, new_mode, typec_orien);
	if(ret) {
		pr_err("[%s]_tca_mode_switch  err2 [%d]\n", __func__, __LINE__);
		return ret;
	}

	if(old_mode == TCPC_NC && TCPC_DP == new_mode) {
		clr_bits(BIT(SOC_USB31_TCA_TCA_CTRLSYNCMODE_CFG0_block_ss_op_START),
			SOC_USB31_TCA_TCA_CTRLSYNCMODE_CFG0_ADDR(tca_dev.tca_base));
	}

	return ret;
}

static int tca_mode_switch(TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien)
{
	int old_mode = tca_dev.tca_cur_mode;
	int ret = 0, usb_ret= 0;
	int cnt = 50;
	volatile unsigned int reg;

	if(TCPC_DP == old_mode) {
		usb31phy_cr_write(0x05, 0x8199);
		usb31phy_cr_write(0x05, 0x8199);
		udelay(100);
	}

	pr_info("[%s]BC_CTRL1[0x%x]\n", __func__, readl(SOC_USB31_MISC_CTRL_BC_CTRL1_ADDR(tca_dev.usb_misc_base)));

	/*used -EBUSY*/
	ret = hisi_usb_combophy_notify(PHY_MODE_CHANGE_BEGIN);
	if (ret) {
		pr_err("[%s]hisi_usb_combophy_notify  err\n", __func__);
		return ret;
	}
	/*2.	�л�ʱ������usb����P3�ӿں󣩼��usb�Ĵ���0xff10c2c0[17]=1,�����쳣*/
	reg = hisi_dwc3_usbcore_read(0xc2c0);
	if (0 == (reg>>17 & 0x01)) {
		pr_err("[%s]USB 0xc2c0[0x%x]  err\n", __func__, reg);
		ret =  -ENXIO;
		goto USB_CHANGE_FIN;
	}

	/*3.	�л�ʱ������usb����P3�ӿں󣩼�鸴λ��0xff2000A0 = 0x30303,�����쳣*/
	reg = readl(SOC_USB31_MISC_CTRL_USB_MISC_CFGA0_ADDR(tca_dev.usb_misc_base));
	if (0x30303 != reg) {
		pr_err("[%s]CFGA0[0x%x]  err\n", __func__, reg);
		ret =  -EPERM;
		goto USB_CHANGE_FIN;
	}

	reg = readl(SOC_CRGPERIPH_PERRSTSTAT4_ADDR(tca_dev.crgperi_reg_base));
	reg &= BIT(SOC_CRGPERIPH_PERRSTSTAT4_ip_rst_usb3otg_32k_START)|
			BIT(SOC_CRGPERIPH_PERRSTSTAT4_ip_hrst_usb3otg_misc_START);
	if(reg){
		pr_err("[%s]PERRSTSTAT4[0x%x]\n", __func__, reg);
		ret =  -EFAULT;
		goto USB_CHANGE_FIN;
	}

	while(cnt--) {
		if(TCPC_NC == new_mode) {
			if(0x333333 == readl(SOC_USB31_TCA_TCA_PSTATE_ADDR(tca_dev.tca_base)))
				break;
		}else
			break;
		msleep(10);
	}


	reg = readl(SOC_USB31_TCA_TCA_PSTATE_ADDR(tca_dev.tca_base));
	printk(HISI_TCA_DEBUG"[%s]TCA_PSTATE[%x]cnt[%d]\n", __func__, reg,cnt);
	if (0 == reg) {
		misc_dump();
		ret = -ESRCH;
		goto USB_CHANGE_FIN;
	}


	ret = tca_mode_sw(new_mode, typec_orien);
	if (ret)
		goto USB_CHANGE_FIN;


	if(TCPC_DP == old_mode) {
		usb31phy_cr_write(0x05, 0x198);
		udelay(100);
	}

USB_CHANGE_FIN:
	usb_ret = hisi_usb_combophy_notify(PHY_MODE_CHANGE_END);
	if (usb_ret) {
		pr_err("[%s]hisi_usb_combophy_notify END  err\n", __func__);
		return usb_ret;
	}
	return ret;
}

#ifdef COMBOPHY_ES_BUGFIX
static void toggle_clock(void)
{
	int i;
	for(i=0;i<32;i++) {
		set_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG54_usb3_phy0_cr_para_clk_START),
			SOC_USB31_MISC_CTRL_USB_MISC_CFG54_ADDR(tca_dev.usb_misc_base));
		clr_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG54_usb3_phy0_cr_para_clk_START),
			SOC_USB31_MISC_CTRL_USB_MISC_CFG54_ADDR(tca_dev.usb_misc_base));
	}
}
#endif

static void combophy_firmware_update(void)
{
#ifdef COMBOPHY_ES_BUGFIX
	int i,cnt;
	int fw_size = sizeof(firmware)/sizeof(firmware[0]);
	printk(HISI_TCA_DEBUG"[%s]fw_size[%d]\n", __func__, fw_size);
//ѡ��CR �ӿڣ� MISC54[4] =  1
	set_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG54_usb3_phy0_cr_para_sel_START),
		SOC_USB31_MISC_CTRL_USB_MISC_CFG54_ADDR(tca_dev.usb_misc_base));
//toggle clock * 32�Σ� MISC54[2] =  1�� MISC54[2] =  0��ѭ��32��
	toggle_clock();
/*
3���ȴ�PHY׼����
 wait for sram_init_done��MISC5c[12]  ==1
*/
	cnt = 20;
	while(cnt--) {
		if(is_bits_set(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG5C_phy0_sram_init_done_START),
				SOC_USB31_MISC_CTRL_USB_MISC_CFG5C_ADDR(tca_dev.usb_misc_base)))
				break;
		msleep(1);
	}
/*
4������firmware:
����õ�firmware���ӵ�ַ0x6000��ʼ�� ����д�루����CRд������
*/
	i = 0;
	usb31phy_cr_write(0x6000+i,firmware[i]);
	for(i = 0; i< fw_size; i++) {
		usb31phy_cr_write(0x6000+i,firmware[i]);
	}

//toggle clock * 32�Σ� MISC54[2] =  1�� MISC54[2] =  0��ѭ��32��
	toggle_clock();
//5��֪ͨPHY��ȡ����
//sram_ext_ld_done =1: MISC5c[3]  =1
	set_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG54_usb3_phy0_cr_para_rd_en_START),
		SOC_USB31_MISC_CTRL_USB_MISC_CFG5C_ADDR(tca_dev.usb_misc_base));
//toggle clock * 32�Σ� MISC54[2] =  1�� MISC54[2] =  0��ѭ��32��
	toggle_clock();
//6���ӳ�1mS����PHY OK��
	msleep(1);
#endif
}

/*lint  -e838 -e747*/
static int combophy_poweron(TYPEC_PLUG_ORIEN_E typec_orien)
{
	int ret = 0;
	int wait_cnt = 50;
	printk(HISI_TCA_DEBUG"[%s]tca_poweron[%d]\n", __func__, tca_dev.tca_poweron);
	if (TCA_POWERON == tca_dev.tca_poweron)
		return 0;

/*	writel(BIT(SOC_CRGPERIPH_PEREN0_gt_hclk_usb3otg_misc_START),
		SOC_CRGPERIPH_PEREN0_ADDR(tca_dev.crgperi_reg_base));
	ret = clk_prepare_enable(tca_dev.gt_hclk_usb3otg);
	if (ret) {
		pr_err("[%s]clk_prepare_enable  gt_hclk_usb3otg err\n", __func__);
		return -EACCES;
	}

	printk(HISI_TCA_DEBUG"[%s]gt_hclk_usb3otg enable+\n", __func__);
	if (__clk_is_enabled(tca_dev.gt_hclk_usb3otg) == false) {
		pr_err("[%s]gt_hclk_usb3otg  check err\n", __func__);
		return -EPERM;
	}*/

	printk(HISI_TCA_DEBUG"[%s]dwc3_misc_ctrl_get\n", __func__);
	ret = dwc3_misc_ctrl_get(MICS_CTRL_COMBOPHY);
	if (ret) {
		pr_err("[%s] misc ctrl get err\n", __func__);
		return -EACCES;
	}

	writel((0x0927C/3), SOC_USB31_TCA_TCA_CTRLSYNCMODE_CFG1_ADDR(tca_dev.tca_base));

	printk(HISI_TCA_DEBUG"PERCLKEN0[%x]PERSTAT0[%x]PERRSTSTAT4[%x]\n",
		readl(SOC_CRGPERIPH_PERCLKEN0_ADDR(tca_dev.crgperi_reg_base)),
		readl(SOC_CRGPERIPH_PERSTAT0_ADDR(tca_dev.crgperi_reg_base)),
		readl(SOC_CRGPERIPH_PERRSTSTAT4_ADDR(tca_dev.crgperi_reg_base)));

/*4	open combo phy */
	writel(BIT(SOC_CRGPERIPH_ISODIS_usb_refclk_iso_en_START),
		SOC_CRGPERIPH_ISODIS_ADDR(tca_dev.crgperi_reg_base));

/*	writel(HM_EN(SOC_PCTRL_PERI_CTRL3_usb_tcxo_en_START),
			SOC_PCTRL_PERI_CTRL3_ADDR(tca_dev.pctrl_reg_base));*/
	ret = clk_prepare_enable(tca_dev.gt_clk_usb3_tcxo_en);
	if (ret) {
		pr_err("[%s]clk_prepare_enable  clk_usb3_tcxo_en err\n", __func__);
		return -EIO;
	}
	printk(HISI_TCA_DEBUG"[%s]gt_clk_usb3_tcxo_en enable+\n", __func__);
	if (__clk_is_enabled(tca_dev.gt_clk_usb3_tcxo_en) == false) {
		pr_err("[%s]gt_clk_usb3_tcxo_en  check err\n", __func__);
		return -ESPIPE;
	}

	msleep(1);
	clr_bits(BIT(SOC_PCTRL_PERI_CTRL24_sc_clk_usb3phy_3mux1_sel_START),
			SOC_PCTRL_PERI_CTRL24_ADDR(tca_dev.pctrl_reg_base));

/*5	dp-->p3 mode*/
	ret = hisi_dptx_notify_switch();
	if (ret) {
		pr_err("[%s] hisi_dptx_notify_switch err\n", __func__);
		return -EIO;
	}

/*5.5 release USB31 PHY out of TestPowerDown mode*/
	clr_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG50_usb3_phy_test_powerdown_START),
	SOC_USB31_MISC_CTRL_USB_MISC_CFG50_ADDR(tca_dev.usb_misc_base));
	udelay(50);
	set_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG54_usb3_phy0_ana_pwr_en_START)|
		BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG54_phy0_pcs_pwr_stable_START)|
		BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG54_phy0_pma_pwr_stable_START),
	SOC_USB31_MISC_CTRL_USB_MISC_CFG54_ADDR(tca_dev.usb_misc_base));

#ifdef COMBOPHY_ES_BUGFIX
//e.	����sram_bypass = 0:  MISC_CTRL 5c[2] = 0
	clr_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFG5C_usb3_phy0_sram_bypass_START),
	SOC_USB31_MISC_CTRL_USB_MISC_CFG5C_ADDR(tca_dev.usb_misc_base));
#endif

/*6	unreset combo phy*/
	set_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFGA0_usb3phy_reset_n_START),
	SOC_USB31_MISC_CTRL_USB_MISC_CFGA0_ADDR(tca_dev.usb_misc_base));

	combophy_firmware_update();

	while(is_bits_clr(BIT(SOC_USB31_TCA_TCA_INTR_STS_xa_ack_evt_START)|
		BIT(SOC_USB31_TCA_TCA_INTR_STS_xa_timeout_evt_START),
		SOC_USB31_TCA_TCA_INTR_STS_ADDR(tca_dev.tca_base))) {
		msleep(20);
		if(wait_cnt-- <= 0)
			break;
	}

	if (wait_cnt <= 0)
		pr_err("[%s]wait_cnt[%d]\n", __func__, wait_cnt);

	writel(0xFFFF, SOC_USB31_TCA_TCA_INTR_STS_ADDR(tca_dev.tca_base));
	clr_bits(BIT(SOC_USB31_TCA_TCA_TCPC_tcpc_low_power_en_START),SOC_USB31_TCA_TCA_TCPC_ADDR(tca_dev.tca_base));
	udelay(1);
	printk(HISI_TCA_DEBUG"[%s]TCA_TCPC[%x]\n", __func__, readl(SOC_USB31_TCA_TCA_TCPC_ADDR(tca_dev.tca_base)));
	writel((2* 0x0927C), SOC_USB31_TCA_TCA_CTRLSYNCMODE_CFG1_ADDR(tca_dev.tca_base));
	ret = tca_mode_switch(TCPC_NC, typec_orien);
	writel(BIT(SOC_USB31_TCA_TCA_INTR_STS_xa_ack_evt_START)|
		BIT(SOC_USB31_TCA_TCA_INTR_STS_xa_timeout_evt_START),
		SOC_USB31_TCA_TCA_INTR_STS_ADDR(tca_dev.tca_base));
	tca_dev.tca_poweron = TCA_POWERON;
	printk(HISI_TCA_DEBUG"[%s]poweron ok[%d]\n", __func__, tca_dev.tca_poweron);
	return ret;
}


static int combophy_init(TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien)
{
	int ret = combophy_poweron(typec_orien);
	if (ret)
		goto COM_FIN;

	ret = hisi_dptx_notify_switch();
	if (ret) {
		pr_err("[%s] hisi_dptx_notify_switch err\n", __func__);
		ret = -EIO;
		goto COM_FIN;
	}

	ret = tca_mode_switch(new_mode, typec_orien);
COM_FIN:
	return ret;
}

int combophy_sw_sysc(TCPC_MUX_CTRL_TYPE new_mode, TYPEC_PLUG_ORIEN_E typec_orien)
{
	int ret = 0;
	if(PD_PLATFORM_INIT_OK != tca_dev.init) {
		pr_err("[%s]probe not init fin pls wait\n", __func__);
		return -EACCES;
	}

	if (!mutex_trylock(&tca_mutex)) {
		pr_err("[%s]pd evnet handler is running\n", __func__);
		return 0;
	}
	printk(HISI_TCA_DEBUG"[%s]\n", __func__);
	ret = combophy_init(new_mode, typec_orien);
	(void)hisi_dptx_triger((bool)0);
	mutex_unlock(&tca_mutex); /*lint !e455 */
	return ret;
}

/*lint  +e838*/
/*lint -e124 */
static int usbctrl_status_update(TCA_DEV_TYPE_E dev_type)
{
	int ret = 0;
	if (dev_type <= TCA_ID_RISE_EVENT) {
		printk(HISI_TCA_DEBUG"[%s]hisi_usb_otg_event_sync[%d]\n", __func__, dev_type);
		ret = hisi_usb_otg_event_sync((enum otg_dev_event_type)dev_type);
		if (ret) {
			pr_err("hisi_usb_otg_event_sync err[%d]\n", ret);
			return ret;
		}

		tca_dev.usbctrl_status = dev_type;
	}

	return ret;
}

static int combophy_shutdown(void)
{
	volatile unsigned int reg;
	printk(HISI_TCA_DEBUG"[%s][%d]\n", __func__,tca_dev.tca_poweron);
	if(TCA_POWEROFF == tca_dev.tca_poweron)
		return 0;

	reg = readl(SOC_CRGPERIPH_PERRSTSTAT4_ADDR(tca_dev.crgperi_reg_base));
	reg &= BIT(SOC_CRGPERIPH_PERRSTSTAT4_ip_rst_usb3otg_32k_START)|
			BIT(SOC_CRGPERIPH_PERRSTSTAT4_ip_hrst_usb3otg_misc_START);
	if(reg) {
		pr_err("[%s]PERRSTSTAT4[0x%x]\n", __func__, reg);
		goto USB_MISC_CTRL_FIN;
	}


	if(is_bits_clr(BIT(SOC_CRGPERIPH_PERSTAT0_st_hclk_usb3otg_misc_START),
		SOC_CRGPERIPH_PERSTAT0_ADDR(tca_dev.crgperi_reg_base))){
		pr_err("[%s]PERSTAT0[0x%x]\n", __func__, readl(SOC_CRGPERIPH_PERSTAT0_ADDR(tca_dev.crgperi_reg_base)));
		goto USB_MISC_CTRL_FIN;
	}
/*
	reg = readl(SOC_CRGPERIPH_PERSTAT4_ADDR(tca_dev.crgperi_reg_base));
	reg &= BIT(SOC_CRGPERIPH_PERSTAT4_st_clk_usb3otg_ref_START)|
			BIT(SOC_CRGPERIPH_PERSTAT4_st_aclk_usb3otg_START);
	if(0x03 != reg)
		goto USB_MISC_CTRL_FIN;
*/
/*reset PHY*/
	clr_bits(BIT(SOC_USB31_MISC_CTRL_USB_MISC_CFGA0_usb3phy_reset_n_START),
		SOC_USB31_MISC_CTRL_USB_MISC_CFGA0_ADDR(tca_dev.usb_misc_base));

USB_MISC_CTRL_FIN:
	if(is_bits_clr((unsigned int)BIT(20), SOC_SCTRL_SCDEEPSLEEPED_ADDR(tca_dev.sctrl_reg_base))) {
		/*writel(HM_DIS(SOC_PCTRL_PERI_CTRL3_usb_tcxo_en_START),
			SOC_PCTRL_PERI_CTRL3_ADDR(tca_dev.pctrl_reg_base));*/
			clk_disable_unprepare(tca_dev.gt_clk_usb3_tcxo_en);
			printk(HISI_TCA_DEBUG"[%s]gt_clk_usb3_tcxo_en disable-\n", __func__);

	}else {
		/*writel(BIT(SOC_CRGPERIPH_PERDIS6_gt_clk_usb2phy_ref_START),
			SOC_CRGPERIPH_PERDIS6_ADDR(tca_dev.crgperi_reg_base));
		clk_gate_usb2phy_ref: clk_usb2phy_ref {
		compatible = "hisilicon,hi3xxx-clk-gate";
		#clock-cells = <0>;
		clocks = <&clkin_sys>;
		hisilicon,hi3xxx-clkgate = <0x410 0x80000>;
		clock-output-names = "clk_usb2phy_ref";
		*/
	}

	/*gt_clk_usb3otg_ref��Gt_aclk_usb3otg  ������ʱ����USBģ�鸺�� ǰ��2017/3/17 */
	/*writel(BIT(SOC_CRGPERIPH_PERDIS0_gt_hclk_usb3otg_misc_START),
		SOC_CRGPERIPH_PERDIS0_ADDR(tca_dev.crgperi_reg_base));*/
	printk(HISI_TCA_DEBUG"[%s]dwc3_misc_ctrl_put \n", __func__);
	dwc3_misc_ctrl_put(MICS_CTRL_COMBOPHY);
	/*clk_disable_unprepare(tca_dev.gt_hclk_usb3otg);*/
	printk(HISI_TCA_DEBUG"PERCLKEN0[%x]PERSTAT0[%x]PERRSTSTAT4[%x]\n",
		readl(SOC_CRGPERIPH_PERCLKEN0_ADDR(tca_dev.crgperi_reg_base)),
		readl(SOC_CRGPERIPH_PERSTAT0_ADDR(tca_dev.crgperi_reg_base)),
		readl(SOC_CRGPERIPH_PERRSTSTAT4_ADDR(tca_dev.crgperi_reg_base)));
	tca_dev.tca_poweron = TCA_POWEROFF;
	return 0;
}

int combophy_poweroff(void)
{
	int ret;
	(void)hisi_dptx_triger((bool)0);
	ret = combophy_shutdown();
	tca_dev.tca_cur_mode = TCPC_NC;
	return ret;
}

/*lint +e124 -e454 -e456*/
int pd_event_notify(TCA_IRQ_TYPE_E irq_type, TCPC_MUX_CTRL_TYPE mode_type, TCA_DEV_TYPE_E dev_type, TYPEC_PLUG_ORIEN_E typec_orien)
{
	int ret;
	pd_event_t pd_event;

	printk(HISI_TCA_DEBUG"[%s]IRQ[%d]MODEcur[%d]new[%d]DEV[%d]ORIEN[%d]\n",
		__func__, irq_type, tca_dev.tca_cur_mode,mode_type, dev_type, typec_orien);

	if(PD_PLATFORM_INIT_OK != tca_dev.init) {
		pr_err("[%s]probe not init fin pls wait\n", __func__);
		return -EIO;
	}

	if (irq_type >=TCA_IRQ_MAX_NUM || mode_type >= TCPC_MUX_MODE_MAX || dev_type >= TCA_DEV_MAX || typec_orien >= TYPEC_ORIEN_MAX)
		return -EPERM;

	if (!wake_lock_active(&tca_dev.wlock))
		wake_lock(&tca_dev.wlock);
	printk(HISI_TCA_DEBUG"\n[%s]wake lock\n", __func__);
	pd_event.irq_type = irq_type;
	pd_event.mode_type = mode_type;
	pd_event.dev_type = dev_type;
	pd_event.typec_orien = typec_orien;
	ret = kfifo_in(&tca_dev.kfifo, &pd_event, (unsigned int)sizeof(pd_event_t));
	printk(HISI_TCA_DEBUG"kfifo_in[%d]\n", ret);
	if(!queue_work(tca_dev.wq, &tca_dev.work))
		pr_err("[%s]tca wq is doing\n", __func__);
	return 0;
}


static int pd_notify_nc(pd_event_t *event)
{
	int ret;
	if ((tca_dev.tca_cur_mode&TCPC_DP)&&(event->dev_type > TCA_ID_RISE_EVENT)){
		hisi_dptx_hpd_trigger(event->irq_type, tca_dev.tca_cur_mode, event->typec_orien);
	}

	ret = usbctrl_status_update(event->dev_type);
	if (ret && -EINVAL != ret)
		goto PD_NOTIFY_NC;

	ret = combophy_poweroff();

PD_NOTIFY_NC:
	return ret;
}

/*lint  -e655 +e747 +e454 +e456*/
void pd_event_hander(pd_event_t *event)
{
	int ret =0;
	printk(HISI_TCA_DEBUG"[%s]IRQ[%d]MODEcur[%d]new[%d]DEV[%d]ORIEN[%d]\n",
		__func__, event->irq_type, tca_dev.tca_cur_mode,event->mode_type, event->dev_type, event->typec_orien);
	if(TCPC_NC == event->mode_type) {
		ret = pd_notify_nc(event);
	}else {
		if(TCPC_NC == tca_dev.tca_cur_mode) {
			ret = combophy_poweron(event->typec_orien);
			if(ret) {
				goto TCA_SW_FIN;
			}
		}

		if(TCA_IRQ_SHORT == event->irq_type)
			hisi_dptx_hpd_trigger(event->irq_type, tca_dev.tca_cur_mode, event->typec_orien);
		else if (tca_dev.tca_cur_mode == event->mode_type) {
			ret = usbctrl_status_update(event->dev_type);
			if (ret)
				goto TCA_SW_FIN;

			if (event->dev_type < TCA_DP_OUT)
				goto TCA_SW_FIN;


			if (TCA_DP_IN == event->dev_type) {
				ret = hisi_dptx_triger((bool)1);
				if (ret) {
					pr_err("[%s]hisi_dptx_triger err[%d][%d]\n",__func__, __LINE__,ret);
					ret = -EFAULT;
					goto TCA_SW_FIN;
				}
			}

			ret = hisi_dptx_hpd_trigger(event->irq_type, tca_dev.tca_cur_mode, event->typec_orien);
			if (ret) {
				pr_err("[%s]hisi_dptx_hpd_trigger err[%d][%d]\n",__func__, __LINE__,ret);
				ret = -EPIPE;
				goto TCA_SW_FIN;
			}

			if (TCA_DP_OUT == event->dev_type) {
				ret = hisi_dptx_triger((bool)0);
				if (ret) {
					pr_err("[%s]hisi_dptx_triger err[%d][%d]\n",__func__, __LINE__,ret);
					ret = -ERANGE;
					goto TCA_SW_FIN;
				}
			}
		}else {
			ret = hisi_dptx_notify_switch();
			if (ret) {
				pr_err("[%s] hisi_dptx_notify_switch err\n", __func__);
				ret = -EIO;
				goto TCA_SW_FIN;
			}

			ret = tca_mode_switch(event->mode_type, event->typec_orien);
			if (ret) {
				pr_err("[%s] tca_mode_switch err\n", __func__);
				goto TCA_SW_FIN;
			}
/*
2��USB��λ״̬�����ǵ�USB���ܴ���������
PD����ֱ����PHY��ģʽ�л�����������һ�°γ����ٵ��µ�״̬��
USB->DP4:  USB->NC->DP4
USB->USB+DP4:USB->NC->USB+DP4
*/

			ret = usbctrl_status_update(event->dev_type);
			if (ret)
				goto TCA_SW_FIN;

			if (event->dev_type >= TCA_DP_OUT)
				hisi_dptx_hpd_trigger(event->irq_type, tca_dev.tca_cur_mode, event->typec_orien);
			else {
				ret = hisi_dptx_triger((bool)0);
				if (ret) {
					pr_err("[%s]hisi_dptx_triger err[%d][%d]\n",__func__, __LINE__,ret);
					ret = -ERANGE;
					goto TCA_SW_FIN;
				}
			}
		}
	}

TCA_SW_FIN:
	if (ret)
		(void)hisi_dptx_triger((bool)0);

	printk(HISI_TCA_DEBUG"\n[%s]:CurMode[%d]RET[%d]\n", __func__, tca_dev.tca_cur_mode, ret);
	dp_dfp_u_notify_dp_configuration_done(tca_dev.tca_cur_mode, ret);
}
/*lint -e715  -e747 +e655 -e455*/
void  tca_wq(struct work_struct *data)
{
	pd_event_t pd_event;
	unsigned long len;
	while (!kfifo_is_empty(&tca_dev.kfifo)) {
		mutex_lock(&tca_mutex);
		memset_s((void*)&pd_event,sizeof(pd_event_t), 0, sizeof(pd_event_t));
		len = kfifo_out(&tca_dev.kfifo, &pd_event, (unsigned int)sizeof(pd_event_t));
		if (len != sizeof(pd_event_t))
			pr_err("[%s]kfifo_out  err\n", __func__);
		pd_event_hander(&pd_event);
		mutex_unlock(&tca_mutex);
	}

	if(wake_lock_active(&tca_dev.wlock))
		wake_unlock(&tca_dev.wlock);
	printk(HISI_TCA_DEBUG"\n[%s]wake unlock\n", __func__);
}
/*lint +e715 +e747 +e455*/
static int __init pd_probe(struct platform_device *pdev)
{
	int ret;
	struct device *dev = &pdev->dev;
	struct device_node *dev_node = dev->of_node;
	if (!of_device_is_available(dev_node))
		return -ENODEV;

	ret = pd_get_resource(&tca_dev, dev);
	if (ret) {
		pr_err("[%s] pd_get_resource err\n", __func__);
		return -EINVAL;
	}

	tca_dev.usbctrl_status = TCA_CHARGER_DISCONNECT_EVENT;
	tca_dev.tca_poweron =  TCA_POWEROFF;
	tca_dev.wq = create_singlethread_workqueue("tca_wq");
	if (NULL == tca_dev.wq) {
		pr_err("[%s]tca_wq  err\n", __func__);
		return -EPIPE;
	}

	ret = kfifo_alloc(&tca_dev.kfifo, FIFO_SIZE, GFP_KERNEL);
	if (ret) {
		pr_err("[%s]kfifo_alloc  err[%d]\n", __func__, ret);
		return ret;
	}

	wake_lock_init(&tca_dev.wlock, WAKE_LOCK_SUSPEND, "phy");
	INIT_WORK(&tca_dev.work, tca_wq);
	tca_dev.init = PD_PLATFORM_INIT_OK;

	return ret;
}

static void tca_devouces_rel(void)
{
	iounmap(tca_dev.usb_misc_base);
	iounmap(tca_dev.pctrl_reg_base);
	iounmap(tca_dev.sctrl_reg_base);
	iounmap(tca_dev.crgperi_reg_base);
}
/*lint -e705 -e715*/
static int pd_remove(struct platform_device *pdev)
{
	tca_devouces_rel();
	kfifo_free(&tca_dev.kfifo);
	wake_lock_destroy(&tca_dev.wlock);
	return 0;
}


#ifdef CONFIG_PM
/*lint -save -e454 -e455 */
static int hisi_pd_prepare(struct device *dev)
{
	pr_info("\n[%s]:+\n", __func__);
	mutex_lock(&tca_mutex);
	pr_info("\n[%s]:-\n", __func__);
	return 0;
}

static void hisi_pd_complete(struct device *dev)
{
	pr_info("\n[%s]:+\n", __func__);
	mutex_unlock(&tca_mutex);
	pr_info("\n[%s]:-\n", __func__);
}

static int combophy_suspend(struct device *dev)
{
	pr_info("\n[%s]:+\n", __func__);
	if (tca_dev.tca_cur_mode > TCPC_NC && tca_dev.tca_cur_mode < TCPC_MUX_MODE_MAX)
		(void)combophy_shutdown();
	pr_info("\n[%s]:-\n", __func__);
	return 0;
}

static int combophy_resume(struct device *dev)
{
	pr_info("\n[%s]:+\n", __func__);
	if (tca_dev.tca_cur_mode > TCPC_NC && tca_dev.tca_cur_mode < TCPC_MUX_MODE_MAX)
		combophy_init(tca_dev.tca_cur_mode, tca_dev.typec_orien);
	pr_info("\n[%s]:-\n", __func__);
	return 0;
}
#endif

/*lint +e705 +e715*/
/*lint -e785 -e64*/


const struct dev_pm_ops hisi_pd_pm_ops = {
#ifdef CONFIG_PM_SLEEP
	.prepare = hisi_pd_prepare,
	.complete = hisi_pd_complete,
	.suspend_late = combophy_suspend,
	.resume_early = combophy_resume,
#endif
};

#ifdef CONFIG_PM
#define HISI_PD_PM_OPS (&hisi_pd_pm_ops)
#else
#define HISI_PD_PM_OPS NULL
#endif


static struct of_device_id hisi_pd_of_match[] = {
	{ .compatible = "hisilicon,pd"},
	{},
};

MODULE_DEVICE_TABLE(of, hisi_pd_of_match);
static struct platform_driver pd_platdrv = {
	.driver = {
		.name		= "hisi-pd",
		.owner		= THIS_MODULE,
		.of_match_table = hisi_pd_of_match,
		.pm	= HISI_PD_PM_OPS,
	},
	.probe	= pd_probe,
	.remove	= pd_remove,
};
/*lint +e785*/
/*lint  -e721*/
module_platform_driver(pd_platdrv);
/*lint +e528 +e721*/

