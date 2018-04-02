/*
 * Copyrights (C) 2017 Samsung Electronics, Inc.
 * Copyrights (C) 2017 Maxim Integrated Products, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/mod_devicetable.h>
#include <linux/power_supply.h>
#include <linux/of.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/file.h>
#include <linux/syscalls.h>
#include <linux/sort.h>
#ifdef CONFIG_OF
#include <linux/of_gpio.h>
#endif
#include <linux/mfd/core.h>
#include <linux/mfd/max77705.h>
#include <linux/mfd/max77705-private.h>
#include <linux/ccic/max77705_usbc.h>
#if defined(CONFIG_USB_HOST_NOTIFY)
#include <linux/usb_notify.h>
#endif
#if defined(CONFIG_CCIC_MAX77705_DEBUG)
#include <linux/ccic/max77705_debug.h>
#endif
#if defined(CONFIG_CCIC_NOTIFIER)
#include <linux/ccic/ccic_sysfs.h>
#include <linux/ccic/ccic_core.h>
#include <linux/ccic/ccic_notifier.h>
#include <linux/ccic/max77705_alternate.h>
#if defined(CONFIG_DUAL_ROLE_USB_INTF)
#include <linux/usb/class-dual-role.h>
#endif

static enum ccic_sysfs_property max77705_sysfs_properties[] = {
	CCIC_SYSFS_PROP_CHIP_NAME,
	CCIC_SYSFS_PROP_CUR_VERSION,
	CCIC_SYSFS_PROP_SRC_VERSION,
	CCIC_SYSFS_PROP_LPM_MODE,
	CCIC_SYSFS_PROP_STATE,
	CCIC_SYSFS_PROP_RID,
	CCIC_SYSFS_PROP_CTRL_OPTION,
	CCIC_SYSFS_PROP_BOOTING_DRY,
	CCIC_SYSFS_PROP_FW_UPDATE,
	CCIC_SYSFS_PROP_FW_UPDATE_STATUS,
	CCIC_SYSFS_PROP_FW_WATER,
	CCIC_SYSFS_PROP_DEX_FAN_UVDM,
	CCIC_SYSFS_PROP_ACC_DEVICE_VERSION,
	CCIC_SYSFS_PROP_DEBUG_OPCODE,
	CCIC_SYSFS_PROP_CONTROL_GPIO,
	CCIC_SYSFS_PROP_USBPD_IDS,
	CCIC_SYSFS_PROP_USBPD_TYPE,
	CCIC_SYSFS_PROP_CC_PIN_STATUS,
};
#endif /* CONFIG_CCIC_NOTIFIER */
#if defined(CONFIG_DUAL_ROLE_USB_INTF)
static enum dual_role_property fusb_drp_properties[] = {
	DUAL_ROLE_PROP_MODE,
	DUAL_ROLE_PROP_PR,
	DUAL_ROLE_PROP_DR,
	DUAL_ROLE_PROP_VCONN_SUPPLY,
};
#endif

#define DRIVER_VER		"1.2VER"

#define MAX77705_PMIC_REG_INTSRC_MASK 0x23
#define MAX77705_PMIC_REG_INTSRC 0x22

#define MAX77705_IRQSRC_CHG	(1 << 0)
#define MAX77705_IRQSRC_FG      (1 << 2)
#define MAX77705_IRQSRC_MUIC	(1 << 3)

#define MAX77705_SYS_FW_UPDATE
#define MAX77705_GRL_ENABLE
struct max77705_usbc_platform_data *g_usbc_data;

#ifdef MAX77705_SYS_FW_UPDATE
#define MAXIM_DEFAULT_FW		"/sdcard/Firmware/usbpd/secure_max77705.bin"
static void max77705_usbc_mask_irq(struct max77705_usbc_platform_data *usbc_data);
static void max77705_usbc_umask_irq(struct max77705_usbc_platform_data *usbc_data);

#ifdef MAX77705_GRL_ENABLE
static int max77705_i2c_master_write(struct max77705_usbc_platform_data *usbpd_data,
			int slave_addr, u8 *reg_addr)
{
	int err;
	int tries = 0;
	u8 buffer[2] = { reg_addr[0], reg_addr[1]};

	struct i2c_msg msgs[] = {
		{
			.addr = slave_addr,
			.flags = usbpd_data->muic->flags & I2C_M_TEN,
			.len = 2,
			.buf = buffer,
		},
	};

	do {
		err = i2c_transfer(usbpd_data->muic->adapter, msgs, 1);
		if (err < 0)
			msg_maxim("i2c_transfer error:%d, addr : %x ,data : %x\n", err, reg_addr[0], reg_addr[1]);
	} while ((err != 1) && (++tries < 20));

	if (err != 1) {
		msg_maxim("write transfer error:%d, addr : %x ,data : %x\n", err, reg_addr[0], reg_addr[1]);
		err = -EIO;
		return err;
	}

	return 1;
}
#endif

int max77705_current_pr_state(struct max77705_usbc_platform_data *usbc_data)
{
	int current_pr = usbc_data->cc_data->current_pr;
	return current_pr;

}

void blocking_auto_vbus_control(int enable)
{
	int current_pr = 0;

	msg_maxim("disable : %d", enable);

	if (enable) {
		current_pr = max77705_current_pr_state(g_usbc_data);
		switch (current_pr) {
		case SRC:
			/* turn off the vbus */
			max77705_vbus_turn_on_ctrl(g_usbc_data, OFF, false);
			break;
		default:
			break;
		}
		g_usbc_data->mpsm_mode = MPSM_ON;
	} else {
		current_pr = max77705_current_pr_state(g_usbc_data);
		switch (current_pr) {
		case SRC:
			max77705_vbus_turn_on_ctrl(g_usbc_data, ON, false);
			break;
		default:
			break;

		}
		g_usbc_data->mpsm_mode = MPSM_OFF;
	}
	msg_maxim("current_pr : %x disable : %x", current_pr, enable);
}
EXPORT_SYMBOL(blocking_auto_vbus_control);

static void vbus_control_hard_reset(struct work_struct *work)
{
	struct max77705_usbc_platform_data *usbpd_data = g_usbc_data;

	msg_maxim("current_pr=%d", usbpd_data->cc_data->current_pr);

	if (usbpd_data->cc_data->current_pr == SRC)
		max77705_vbus_turn_on_ctrl(usbpd_data, ON, false);
}

void max77705_usbc_enable_auto_vbus(struct max77705_usbc_platform_data *usbc_data)
{
	usbc_cmd_data write_data;

	init_usbc_cmd_data(&write_data);
	write_data.opcode = 0x54;
	write_data.write_data[0] = 0x2;
	write_data.write_length = 0x1;
	write_data.read_length = 0x1;
	max77705_usbc_opcode_write(usbc_data, &write_data);
	msg_maxim("TURN ON THE AUTO VBUS");
	usbc_data->auto_vbus_en = true;
}

void max77705_usbc_disable_auto_vbus(struct max77705_usbc_platform_data *usbc_data)
{
	usbc_cmd_data write_data;

	init_usbc_cmd_data(&write_data);
	write_data.opcode = 0x54;
	write_data.write_data[0] = 0x0;
	write_data.write_length = 0x1;
	write_data.read_length = 0x1;
	max77705_usbc_opcode_write(usbc_data, &write_data);
	msg_maxim("TURN OFF THE AUTO VBUS");
	usbc_data->auto_vbus_en = false;
}

static void max77705_usbc_debug_function(struct max77705_usbc_platform_data *usbc_data)
{
	usbc_cmd_data write_data;

	msg_maxim("called");

	init_usbc_cmd_data(&write_data);
	write_data.opcode = 0x74;
	write_data.write_data[0] = 0x0;
	write_data.write_length = 0x1;
	write_data.read_length = 0xA;
	max77705_usbc_opcode_write(usbc_data, &write_data);
}

#ifdef MAX77705_GRL_ENABLE
static void max77705_set_forcetrimi(struct max77705_usbc_platform_data *usbc_data)
{
	u8 ArrSendData[2] = {0x00, 0x00};

	msg_maxim("IN++");
	mutex_lock(&usbc_data->max77705->i2c_lock);
//	ArrSendData[0] = 0xFE;
//	ArrSendData[1] = 0xC5;
//	max77705_i2c_master_write(usbc_data, 0x66, ArrSendData);
//	ArrSendData[0] = 0xb3;
//	ArrSendData[1] = 0x0c;
//	max77705_i2c_master_write(usbc_data, 0x62, ArrSendData);
	ArrSendData[0] = 0x1F;
	ArrSendData[1] = 0x04;
	max77705_i2c_master_write(usbc_data, 0x62, ArrSendData);
	msleep(100);
	mutex_unlock(&usbc_data->max77705->i2c_lock);
	msg_maxim("OUT");
}
#endif

#if defined(CONFIG_DUAL_ROLE_USB_INTF)
static void max77705_toggling_control(struct max77705_usbc_platform_data *usbpd_data, u8 mode)
{
	usbc_cmd_data write_data;

	/* 0x1 : SRC, 0x2 : SNK, 0x3: DRP */
	msg_maxim("mode=0x%x", mode);
	max77705_usbc_clear_queue(usbpd_data);
	init_usbc_cmd_data(&write_data);
	write_data.opcode = 0x37;
	write_data.write_data[0] = 0x4;
	write_data.write_length = 0x1;
	write_data.read_length = 0x1;
	max77705_usbc_opcode_write(usbpd_data, &write_data);
}

void max77705_rprd_mode_change(struct max77705_usbc_platform_data *usbpd_data, u8 mode)
{
	msg_maxim("+++ mode=0x%x", mode);

	switch (mode) {
	case TYPE_C_ATTACH_DFP: /* SRC */
	case TYPE_C_ATTACH_UFP: /* SNK */
	case TYPE_C_ATTACH_DRP: /* DRP */
		max77705_toggling_control(usbpd_data, TYPE_C_ATTACH_DRP);
		msleep(1000);
		break;
	default:
		break;
	};
	msg_maxim("--- mode=0x%x", mode);
}

void max77705_role_swap_check(struct work_struct *wk)
{
	struct delayed_work *delay_work =
		container_of(wk, struct delayed_work, work);
	struct max77705_usbc_platform_data *usbpd_data =
		container_of(delay_work, struct max77705_usbc_platform_data, role_swap_work);
	int mode;

	msg_maxim("ccic_set_dual_role check again usbpd_data->pd_state=%d",
			usbpd_data->pd_state);

	usbpd_data->try_state_change = 0;

	if (usbpd_data->pd_state == max77705_State_PE_Initial_detach) {
		msg_maxim("ccic_set_dual_role reverse failed, set mode to DRP");
		/* disable_irq(usbpd_data->irq); */
		/* exit from Disabled state and set mode to DRP */
		mode =  TYPE_C_ATTACH_DRP;
		max77705_rprd_mode_change(usbpd_data, mode);
		/* enable_irq(usbpd_data->irq); */
	}
}

static int max77705_ccic_set_dual_role(struct dual_role_phy_instance *dual_role,
				   enum dual_role_property prop,
				   const unsigned int *val)
{
	struct max77705_usbc_platform_data *usbpd_data = dual_role_get_drvdata(dual_role);
	struct i2c_client *i2c;

	USB_STATUS attached_state;
	int mode;
	int timeout = 0;
	int ret = 0;

	if (!usbpd_data) {
		msg_maxim("usbpd_data is null");
		return -EINVAL;
	}

	i2c = usbpd_data->i2c;

	/* Get Current Role */
	attached_state = usbpd_data->data_role;
	msg_maxim("request prop = %d , attached_state = %d",
			prop, attached_state);

	if (attached_state != USB_STATUS_NOTIFY_ATTACH_DFP
	    && attached_state != USB_STATUS_NOTIFY_ATTACH_UFP) {
		msg_maxim("current mode : %d - just return", attached_state);
		return 0;
	}

	if (attached_state == USB_STATUS_NOTIFY_ATTACH_DFP
	    && *val == DUAL_ROLE_PROP_MODE_DFP) {
		msg_maxim("current mode : %d - request mode : %d just return",
			attached_state, *val);
		return 0;
	}

	if (attached_state == USB_STATUS_NOTIFY_ATTACH_UFP
	    && *val == DUAL_ROLE_PROP_MODE_UFP) {
		msg_maxim("current mode : %d - request mode : %d just return",
			attached_state, *val);
		return 0;
	}

	if (attached_state == USB_STATUS_NOTIFY_ATTACH_DFP) {
		/* Current mode DFP and Source  */
		msg_maxim("try reversing, from Source to Sink");
		/* turns off VBUS first */
		/* vbus_turn_on_ctrl(0); */
#if defined(CONFIG_CCIC_NOTIFIER)
		/* muic */
		max77705_ccic_event_work(usbpd_data,
			CCIC_NOTIFY_DEV_MUIC, CCIC_NOTIFY_ID_ATTACH,
			0/*attach*/, 0/*rprd*/, 0);
#endif
		/* exit from Disabled state and set mode to UFP */
		mode =  TYPE_C_ATTACH_UFP;
		usbpd_data->try_state_change = TYPE_C_ATTACH_UFP;
		max77705_rprd_mode_change(usbpd_data, mode);
	} else {
		/* Current mode UFP and Sink  */
		msg_maxim("try reversing, from Sink to Source");
		/* exit from Disabled state and set mode to UFP */
		mode =  TYPE_C_ATTACH_DFP;
		usbpd_data->try_state_change = TYPE_C_ATTACH_DFP;
		max77705_rprd_mode_change(usbpd_data, mode);
	}

	reinit_completion(&usbpd_data->reverse_completion);
	timeout =
	    wait_for_completion_timeout(&usbpd_data->reverse_completion,
					msecs_to_jiffies
					(DUAL_ROLE_SET_MODE_WAIT_MS));

	if (!timeout && usbpd_data->try_state_change) {
		msg_maxim("reverse failed, set mode to DRP");
		/* disable_irq(usbpd_data->irq); */
		/* exit from Disabled state and set mode to DRP */
		mode =  TYPE_C_ATTACH_DRP;
		max77705_rprd_mode_change(usbpd_data, mode);
		/* enable_irq(usbpd_data->irq); */
		ret = -EIO;
	} else {
		msg_maxim("reverse success, one more check");
		schedule_delayed_work(&usbpd_data->role_swap_work, msecs_to_jiffies(DUAL_ROLE_SET_MODE_WAIT_MS));
	}

	msg_maxim("-> data role : %d", *val);
	return ret;
}

/* Decides whether userspace can change a specific property */
int max77705_dual_role_is_writeable(struct dual_role_phy_instance *drp,
				  enum dual_role_property prop)
{
	if (prop == DUAL_ROLE_PROP_MODE)
		return 1;
	else
		return 0;
}

/* Callback for "cat /sys/class/dual_role_usb/otg_default/<property>" */
int max77705_dual_role_get_local_prop(struct dual_role_phy_instance *dual_role,
				    enum dual_role_property prop,
				    unsigned int *val)
{
	struct max77705_usbc_platform_data *usbpd_data = dual_role_get_drvdata(dual_role);

	USB_STATUS attached_state;
	int power_role;

	if (!usbpd_data) {
		msg_maxim("usbpd_data is null : request prop = %d", prop);
		return -EINVAL;
	}
	attached_state = usbpd_data->data_role;
	power_role = usbpd_data->power_role;

	msg_maxim("request prop = %d , attached_state = %d, power_role = %d",
			prop, attached_state, power_role);

	if (prop == DUAL_ROLE_PROP_VCONN_SUPPLY) {
		if (usbpd_data->vconn_en)
			*val = DUAL_ROLE_PROP_VCONN_SUPPLY_YES;
		else
			*val = DUAL_ROLE_PROP_VCONN_SUPPLY_NO;
		return 0;
	}

	if (attached_state == USB_STATUS_NOTIFY_ATTACH_DFP) {
		if (prop == DUAL_ROLE_PROP_MODE)
			*val = DUAL_ROLE_PROP_MODE_DFP;
		else if (prop == DUAL_ROLE_PROP_PR)
			*val = power_role;
		else if (prop == DUAL_ROLE_PROP_DR)
			*val = DUAL_ROLE_PROP_DR_HOST;
		else
			return -EINVAL;
	} else if (attached_state == USB_STATUS_NOTIFY_ATTACH_UFP) {
		if (prop == DUAL_ROLE_PROP_MODE)
			*val = DUAL_ROLE_PROP_MODE_UFP;
		else if (prop == DUAL_ROLE_PROP_PR)
			*val = power_role;
		else if (prop == DUAL_ROLE_PROP_DR)
			*val = DUAL_ROLE_PROP_DR_DEVICE;
		else
			return -EINVAL;
	} else {
		if (prop == DUAL_ROLE_PROP_MODE)
			*val = DUAL_ROLE_PROP_MODE_NONE;
		else if (prop == DUAL_ROLE_PROP_PR)
			*val = DUAL_ROLE_PROP_PR_NONE;
		else if (prop == DUAL_ROLE_PROP_DR)
			*val = DUAL_ROLE_PROP_DR_NONE;
		else
			return -EINVAL;
	}

	return 0;
}

/* Callback for "echo <value> >
 *                      /sys/class/dual_role_usb/<name>/<property>"
 * Block until the entire final state is reached.
 * Blocking is one of the better ways to signal when the operation
 * is done.
 * This function tries to switch to Attached.SRC or Attached.SNK
 * by forcing the mode into SRC or SNK.
 * On failure, we fall back to Try.SNK state machine.
 */
int max77705_dual_role_set_prop(struct dual_role_phy_instance *dual_role,
			      enum dual_role_property prop,
			      const unsigned int *val)
{
	msg_maxim("request prop = %d , *val = %d", prop, *val);
	if (prop == DUAL_ROLE_PROP_MODE)
		return max77705_ccic_set_dual_role(dual_role, prop, val);
	else
		return -EINVAL;
}
#endif

static int max77705_firmware_update_sys(struct max77705_usbc_platform_data *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	unsigned char *fw_data;
	struct max77705_fw_header fw_header;
	struct file *fp;
	mm_segment_t old_fs;
	long fw_size, nread;
	int error = 0;
	const u8 *fw_bin;
	int fw_bin_len;
	u8 pmic_rev = 0;/* pmic Rev */
	u8 fw_enable = 0;

	if (!usbc_data) {
		msg_maxim("usbc_data is null!!");
		return -ENODEV;
	}

	pmic_rev = usbc_data->max77705->pmic_rev;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	fp = filp_open(MAXIM_DEFAULT_FW, O_RDONLY, S_IRUSR);
	if (IS_ERR(fp)) {
		msg_maxim("failed to open %s.", MAXIM_DEFAULT_FW);
		error = -ENOENT;
		goto open_err;
	}

	fw_size = fp->f_path.dentry->d_inode->i_size;

	if (fw_size > 0) {
		fw_data = kzalloc(fw_size, GFP_KERNEL);
		nread = vfs_read(fp, (char __user *)fw_data, fw_size, &fp->f_pos);

		msg_maxim("start, file path %s, size %ld Bytes",
			MAXIM_DEFAULT_FW, fw_size);
		filp_close(fp, NULL);

		if (nread != fw_size) {
			msg_maxim("failed to read firmware file, nread %ld Bytes",
					nread);
			error = -EIO;
		} else {
			fw_bin = fw_data;
			fw_bin_len = fw_size;
			/* Copy fw header */
			memcpy(&fw_header, fw_bin, FW_HEADER_SIZE);
			max77705_read_reg(usbc_data->muic,
					REG_UIC_FW_REV, &usbc_data->FW_Revision);
			max77705_read_reg(usbc_data->muic,
					REG_UIC_FW_MINOR, &usbc_data->FW_Minor_Revision);
			msg_maxim("chip %02X.%02X, fw %02X.%02X",
					usbc_data->FW_Revision, usbc_data->FW_Minor_Revision,
					fw_header.data4, fw_header.data5);
			switch (pmic_rev) {
			case MAX77705_PASS1:
			case MAX77705_PASS2:
				if (fw_header.data4 == 0x09)
					fw_enable = 1;
				break;
			case MAX77705_PASS3:
				if (fw_header.data4 > 0x09)
					fw_enable = 1;
				break;
			default:
				msg_maxim("FAILED F/W via SYS and PMIC_REVISION isn't valid");
				break;
			};
			if (fw_enable)
				max77705_usbc_fw_update(usbc_data->max77705, fw_bin, fw_bin_len, 1);
			else
				msg_maxim("FAILED F/W MISMATCH pmic_rev : 0x%x, fw_header.data4 : 0x%x",
						pmic_rev, fw_header.data4);
			goto done;

		}
done:
		kfree(fw_data);
	}

open_err:
	set_fs(old_fs);
	return error;
}

#endif

void max77705_manual_jig_on(struct max77705_usbc_platform_data *usbpd_data, int mode)
{
	usbc_cmd_data read_data;
	usbc_cmd_data write_data;

	msg_maxim("usb: mode=%s", mode ? "High" : "Low");

	init_usbc_cmd_data(&read_data);
	init_usbc_cmd_data(&write_data);
	read_data.opcode = OPCODE_CTRL3_R;
	read_data.write_length = 0x0;
	read_data.read_length = 0x1;
	write_data.opcode = OPCODE_CTRL3_W;
	if (mode)
		write_data.write_data[0] = 0x1;
	else
		write_data.write_data[0] = 0x0;

	write_data.write_data[1] = 0x1;
	write_data.write_length = 0x2;

	write_data.read_length = 0x0;

	max77705_usbc_opcode_read(usbpd_data, &read_data);
	max77705_usbc_opcode_write(usbpd_data, &write_data);
}

void max77705_control_option_command(struct max77705_usbc_platform_data *usbpd_data, int cmd)
{
	struct max77705_cc_data *cc_data = usbpd_data->cc_data;
	u8 ccstat = 0;
	usbc_cmd_data write_data;

	/* for maxim request : they want to check ccstate here */
	max77705_read_reg(usbpd_data->muic, REG_CC_STATUS0, &cc_data->cc_status0);
	ccstat =  (cc_data->cc_status0 & BIT_CCStat) >> FFS(BIT_CCStat);
	msg_maxim("usb: cmd=0x%x ccstat : %d", cmd, ccstat);

	init_usbc_cmd_data(&write_data);
	/* 1 : Vconn control option command ON */
	/* 2 : Vconn control option command OFF */
	/* 3 : Water Detect option command ON */
	/* 4 : Water Detect option command OFF */
	if (cmd == 1)
		usbpd_data->vconn_test = 0;
	else if (cmd == 2)
		usbpd_data->vconn_test = 0; /* do nothing */
	else if (cmd == 3) { /* if SBU pin is low, water interrupt is happened. */
		write_data.opcode = 0x54;
		write_data.write_data[0] = 0x3;
		write_data.write_length = 0x1;
		write_data.read_length = 0x0;
		max77705_usbc_opcode_write(usbpd_data, &write_data);
	} else if (cmd == 4) {
		write_data.opcode = 0x54;
		write_data.write_data[0] = 0x2;
		write_data.write_length = 0x1;
		write_data.read_length = 0x0;
		max77705_usbc_opcode_write(usbpd_data, &write_data);
	}
	if ((cmd & 0xF) == 0x3)
		usbpd_data->fac_water_enable = 1;
	else if ((cmd & 0xF) == 0x4)
		usbpd_data->fac_water_enable = 0;
}

void max77705_response_sbu_read(struct max77705_usbc_platform_data *usbpd_data, unsigned char *data)
{
	u8 sbu1 = 0, sbu2 = 0;

	sbu1 = data[1];
	sbu2 = data[2];

	msg_maxim("SBU1 = 0x%x, SBU2 = 0x%x", sbu1, sbu2);

	if (sbu1 == 0x0)
		usbpd_data->sbu[0] = 0;
	else
		usbpd_data->sbu[0] = 1;
	if (sbu2 == 0x0)
		usbpd_data->sbu[1] = 0;
	else
		usbpd_data->sbu[1] = 1;
	usbpd_data->is_sbu_done = 1;
}

void max77705_request_sbu_read(struct max77705_usbc_platform_data *usbpd_data)
{
	usbc_cmd_data write_data;

	init_usbc_cmd_data(&write_data);
	write_data.opcode = OPCODE_READ_SBU;
	write_data.write_data[0] = 0x1;
	write_data.write_length = 0x1;
	write_data.read_length = 0x2;
	max77705_usbc_opcode_write(usbpd_data, &write_data);
	usbpd_data->is_sbu_done = 0;
}

void max77705_response_selftest_read(struct max77705_usbc_platform_data *usbpd_data, unsigned char *data)
{
	u8 cc = 0, sbu1 = 0, sbu2 = 0;

	cc = data[1];
	sbu1 = data[2];
	sbu2 = data[3];

	msg_maxim("SELFTEST CC = %x SBU1 = 0x%x, SBU2 = 0x%x", cc, sbu1, sbu2);

	if (sbu1 >= 7 && sbu2 >= 7)
		usbpd_data->selftest = 1;
	else
		usbpd_data->selftest = 0;
	usbpd_data->is_selftest_done = 1;
}

void max77705_request_selftest_read(struct max77705_usbc_platform_data *usbpd_data)
{
	usbc_cmd_data write_data;

	init_usbc_cmd_data(&write_data);
	write_data.opcode = OPCODE_READ_SELFTEST;
	write_data.write_length = 0x1;
	write_data.write_data[0] = 0x0;
	max77705_usbc_opcode_write(usbpd_data, &write_data);

	msleep(500);
	init_usbc_cmd_data(&write_data);
	write_data.opcode = OPCODE_READ_SELFTEST;
	write_data.write_length = 0x1;
	write_data.write_data[0] = 0x1;
	write_data.read_length = 0x3;
	max77705_usbc_opcode_write(usbpd_data, &write_data);
	usbpd_data->is_selftest_done = 0;
}

void max77705_firmware_update_ums(struct max77705_usbc_platform_data *usbpd_data)
{
	usbpd_data->fw_update = 1;
	max77705_usbc_mask_irq(usbpd_data);
	max77705_write_reg(usbpd_data->muic, REG_PD_INT_M, 0xFF);
	max77705_write_reg(usbpd_data->muic, REG_CC_INT_M, 0xFF);
	max77705_write_reg(usbpd_data->muic, REG_UIC_INT_M, 0xFF);
	max77705_write_reg(usbpd_data->muic, REG_VDM_INT_M, 0xFF);
	max77705_firmware_update_sys(usbpd_data);
	max77705_write_reg(usbpd_data->muic, REG_UIC_INT_M, REG_UIC_INT_M_INIT);
	max77705_write_reg(usbpd_data->muic, REG_CC_INT_M, REG_CC_INT_M_INIT);
	max77705_write_reg(usbpd_data->muic, REG_PD_INT_M, REG_PD_INT_M_INIT);
	max77705_write_reg(usbpd_data->muic, REG_VDM_INT_M, REG_VDM_INT_M_INIT);
	// max77705_usbc_enable_auto_vbus(usbpd_data);
	max77705_set_enable_alternate_mode(ALTERNATE_MODE_START);
	max77705_usbc_umask_irq(usbpd_data);
	usbpd_data->fw_update = 0;
}

#if defined(CONFIG_CCIC_NOTIFIER)
static int max77705_sysfs_get_local_prop(struct _ccic_data_t *pccic_data,
					enum ccic_sysfs_property prop,
					char *buf)
{
	int retval = -ENODEV, i = 0;
	u8 cur_major = 0, cur_minor = 0, src_major = 0, src_minor = 0;
	struct max77705_usbc_platform_data *usbpd_data =
		(struct max77705_usbc_platform_data *)pccic_data->drv_data;

	if (!usbpd_data) {
		msg_maxim("usbpd_data is null : request prop = %d", prop);
		return -ENODEV;
	}

	switch (prop) {
	case CCIC_SYSFS_PROP_CUR_VERSION:
		retval = max77705_read_reg(usbpd_data->muic, REG_UIC_FW_REV, &cur_major);
		if (retval < 0) {
			msg_maxim("Failed to read FW_REV");
			return retval;
		}
		retval = max77705_read_reg(usbpd_data->muic, REG_UIC_FW_MINOR, &cur_minor);
		if (retval < 0) {
			msg_maxim("Failed to read FW_MINOR_REV");
			return retval;
		}
		retval = sprintf(buf, "%02X.%02X\n", cur_major, cur_minor);
		msg_maxim("usb: CCIC_SYSFS_PROP_CUR_VERSION : %02X.%02X",
				cur_major, cur_minor);
		break;
	case CCIC_SYSFS_PROP_SRC_VERSION:
		if (usbpd_data->max77705->pmic_rev == MAX77705_PASS3) {
			src_major = BOOT_FLASH_FW_PASS3[4];
			src_minor = BOOT_FLASH_FW_PASS3[5];
		} else {
			src_major = BOOT_FLASH_FW_PASS2[4];
			src_minor = BOOT_FLASH_FW_PASS2[5];
		}
		retval = sprintf(buf, "%02X.%02X\n", src_major, src_minor);
		msg_maxim("usb: CCIC_SYSFS_PROP_SRC_VERSION : %02X.%02X",
				src_major, src_minor);
		break;
	case CCIC_SYSFS_PROP_LPM_MODE:
		retval = sprintf(buf, "%d\n", usbpd_data->manual_lpm_mode);
		msg_maxim("usb: CCIC_SYSFS_PROP_LPM_MODE : %d",
				usbpd_data->manual_lpm_mode);
		break;
	case CCIC_SYSFS_PROP_STATE:
		retval = sprintf(buf, "%d\n", usbpd_data->pd_state);
		msg_maxim("usb: CCIC_SYSFS_PROP_STATE : %d",
				usbpd_data->pd_state);
		break;
	case CCIC_SYSFS_PROP_RID:
		retval = sprintf(buf, "%d\n", usbpd_data->cur_rid);
		msg_maxim("usb: CCIC_SYSFS_PROP_RID : %d",
				usbpd_data->cur_rid);
		break;
	case CCIC_SYSFS_PROP_BOOTING_DRY:
		max77705_request_selftest_read(usbpd_data);
		for (i = 0; i < 50; i++) {
			/* register update timing delay */
			msleep(100);
			/* check selftest register values after interrupt */
			msg_maxim("i : %d, is_selftest_done : %d", i, usbpd_data->is_selftest_done);
			if (usbpd_data->is_selftest_done)
				break;
		}
		retval = sprintf(buf, "%d\n", usbpd_data->selftest);
		msg_maxim("usb: CCIC_SYSFS_PROP_BOOTING_DRY : %d", usbpd_data->selftest);
		break;
	case CCIC_SYSFS_PROP_FW_UPDATE_STATUS:
		retval = sprintf(buf, "%s\n", usbpd_data->fw_update == 1 ? "UPDATE" : "NORMAL");
		msg_maxim("usb: CCIC_SYSFS_PROP_FW_UPDATE_STATUS : %s",
				usbpd_data->fw_update == 1 ? "UPDATE" : "NORMAL");
		break;
	case CCIC_SYSFS_PROP_FW_WATER:
		retval = sprintf(buf, "%d\n", usbpd_data->current_connstat == WATER ? 1 : 0);
		msg_maxim("usb: CCIC_SYSFS_PROP_FW_WATER : %d",
				usbpd_data->current_connstat == WATER ? 1 : 0);
		break;
	case CCIC_SYSFS_PROP_ACC_DEVICE_VERSION:
		retval = sprintf(buf, "%04x\n", usbpd_data->Device_Version);
		msg_maxim("usb: CCIC_SYSFS_PROP_ACC_DEVICE_VERSION : %d",
				usbpd_data->Device_Version);
		break;
	case CCIC_SYSFS_PROP_CONTROL_GPIO:
		max77705_request_sbu_read(usbpd_data);
		for (i = 0; i < 5; i++) {
			/* register update timing delay */
			msleep(100);
			/* compare SBU1, SBU2 values after interrupt */
			msg_maxim("i : %d, is_sbu_done : %d", i, usbpd_data->is_sbu_done);
			if (usbpd_data->is_sbu_done)
				break;
		}
		/* compare SBU1, SBU2 values after interrupt */
		msg_maxim("usb: CCIC_SYSFS_PROP_CONTROL_GPIO SBU1 = 0x%x ,SBU2 = 0x%x",
		usbpd_data->sbu[0], usbpd_data->sbu[1]);
		retval = sprintf(buf, "%d %d\n", usbpd_data->sbu[0], usbpd_data->sbu[1]);
		break;
	case CCIC_SYSFS_PROP_USBPD_IDS:
		retval = sprintf(buf, "%04x:%04x\n",
				le16_to_cpu(usbpd_data->Vendor_ID),
				le16_to_cpu(usbpd_data->Product_ID));
		msg_maxim("usb: CCIC_SYSFS_USBPD_IDS : %s", buf);
		break;
	case CCIC_SYSFS_PROP_USBPD_TYPE:
		retval = sprintf(buf, "%d\n", usbpd_data->acc_type);
		msg_maxim("usb: CCIC_SYSFS_USBPD_TYPE : %d",
				usbpd_data->acc_type);
		break;
	case CCIC_SYSFS_PROP_CC_PIN_STATUS:
		retval = sprintf(buf, "%d\n", usbpd_data->cc_pin_status);
		msg_maxim("usb: CCIC_SYSFS_PROP_PIN_STATUS : %d",
				usbpd_data->cc_pin_status);
		break;
	default:
		msg_maxim("prop read not supported prop (%d)", prop);
		retval = -ENODATA;
		break;
	}

	return retval;
}

static ssize_t max77705_sysfs_set_prop(struct _ccic_data_t *pccic_data,
				    enum ccic_sysfs_property prop,
				    const char *buf, size_t size)
{
	ssize_t retval = size;
	int mode = 0;
	u8 FW_Revision = 0, FW_Minor_Revision = 0;
	struct max77705_usbc_platform_data *usbpd_data =
		(struct max77705_usbc_platform_data *)pccic_data->drv_data;
	int rv;

	if (!usbpd_data) {
		msg_maxim("usbpd_data is null : request prop = %d", prop);
		return -ENODEV;
	}
	switch (prop) {
	case CCIC_SYSFS_PROP_LPM_MODE:
		rv = sscanf(buf, "%d", &mode);
		msg_maxim("usb: CCIC_SYSFS_PROP_LPM_MODE mode=%d", mode);
		switch (mode) {
		case 0:
			/* Disable Low Power Mode for App (SW JIGON Disable) */
			max77705_manual_jig_on(usbpd_data, 0);
			usbpd_data->manual_lpm_mode = 0;
			break;
		case 1:
			/* Enable Low Power Mode for App (SW JIGON Enable) */
			max77705_manual_jig_on(usbpd_data, 1);
			usbpd_data->manual_lpm_mode = 1;
			break;
		case 2:
			/* SW JIGON Enable */
			max77705_manual_jig_on(usbpd_data, 1);
			usbpd_data->manual_lpm_mode = 1;
			break;
		default:
			/* SW JIGON Disable */
			max77705_manual_jig_on(usbpd_data, 0);
			usbpd_data->manual_lpm_mode = 0;
			break;
			}
		break;
	case CCIC_SYSFS_PROP_CTRL_OPTION:
		rv = sscanf(buf, "%d", &mode);
		msg_maxim("usb: CCIC_SYSFS_PROP_CTRL_OPTION mode=%d", mode);
		max77705_control_option_command(usbpd_data, mode);
		break;
	case CCIC_SYSFS_PROP_FW_UPDATE:
		rv = sscanf(buf, "%d", &mode);
		msg_maxim("CCIC_SYSFS_PROP_FW_UPDATE mode=%d", mode);

		retval = max77705_read_reg(usbpd_data->muic, REG_UIC_FW_REV, &FW_Revision);
		if (retval < 0) {
			msg_maxim("Failed to read FW_REV");
			return retval;
		}
		retval = max77705_read_reg(usbpd_data->muic, REG_UIC_FW_MINOR, &FW_Minor_Revision);
		if (retval < 0) {
			msg_maxim("Failed to read FW_MINOR_REV");
			return retval;
		}

		pr_info("%s before : FW_REV %02X.%02X\n", __func__, FW_Revision, FW_Minor_Revision);

		/* Factory cmd for firmware update
		* argument represent what is source of firmware like below.
		*
		* 0 : [BUILT_IN] Getting firmware from source.
		* 1 : [UMS] Getting firmware from sd card.
		*/
		switch (mode) {
		case UMS:
			max77705_firmware_update_ums(usbpd_data);
			break;
		case BUILT_IN:
		default:
			msg_maxim("Not support command[%d]", mode);
			break;
		}
		retval = max77705_read_reg(usbpd_data->muic, REG_UIC_FW_REV, &FW_Revision);
		if (retval < 0) {
			msg_maxim("Failed to read FW_REV");
			return retval;
		}
		retval = max77705_read_reg(usbpd_data->muic, REG_UIC_FW_MINOR, &FW_Minor_Revision);
		if (retval < 0) {
			msg_maxim("Failed to read FW_MINOR_REV");
			return retval;
		}
		pr_info("%s after : FW_REV %02X.%02X\n", __func__, FW_Revision, FW_Minor_Revision);
	    break;
	case CCIC_SYSFS_PROP_DEX_FAN_UVDM:
		rv = sscanf(buf, "%d", &mode);
		msg_maxim("CCIC_SYSFS_PROP_DEX_FAN_UVDM mode=%d", mode);
		max77705_send_dex_fan_unstructured_vdm_message(usbpd_data, mode);
		break;
	case CCIC_SYSFS_PROP_DEBUG_OPCODE:
		rv = sscanf(buf, "%d", &mode);
		msg_maxim("CCIC_SYSFS_PROP_DEBUG_OPCODE mode=%d", mode);
		if (mode)
			max77705_usbc_debug_function(usbpd_data);
		break;
	case CCIC_SYSFS_PROP_CONTROL_GPIO:
		rv = sscanf(buf, "%d", &mode);
		msg_maxim("CCIC_SYSFS_PROP_CONTROL_GPIO mode=%d. do nothing for control gpio.", mode);
		/* orignal concept : mode 0 : SBU1/SBU2 set as open-drain status
		 *                         mode 1 : SBU1/SBU2 set as default status - Pull up
		 *  But, max77705 is always open-drain status so we don't need to control it.
		 */
		break;
	default:
		pr_info("%s prop write not supported prop (%d)\n", __func__, prop);
		retval = -ENODATA;
		return retval;
	}
	return size;
}

static int max77705_sysfs_is_writeable(struct _ccic_data_t *pccic_data,
				    enum ccic_sysfs_property prop)
{
	switch (prop) {
	case CCIC_SYSFS_PROP_LPM_MODE:
	case CCIC_SYSFS_PROP_CTRL_OPTION:
	case CCIC_SYSFS_PROP_FW_UPDATE:
	case CCIC_SYSFS_PROP_DEX_FAN_UVDM:
	case CCIC_SYSFS_PROP_DEBUG_OPCODE:
	case CCIC_SYSFS_PROP_CONTROL_GPIO:
		return 1;
	default:
		return 0;
	}
}
#endif
static ssize_t max77705_fw_update(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int start_fw_update = 0;
	usbc_cmd_data read_data;
	usbc_cmd_data write_data;

	init_usbc_cmd_data(&read_data);
	init_usbc_cmd_data(&write_data);
	read_data.opcode = OPCODE_CTRL1_R;
	read_data.write_length = 0x0;
	read_data.read_length = 0x1;

	write_data.opcode = OPCODE_CTRL1_W;
	write_data.write_data[0] = 0x09;
	write_data.write_length = 0x1;
	write_data.read_length = 0x0;

	if (kstrtou32(buf, 0, &start_fw_update)) {
		dev_err(dev,
			"%s: Failed converting from str to u32.", __func__);
	}

	msg_maxim("start_fw_update %d", start_fw_update);

	max77705_usbc_opcode_rw(g_usbc_data, &read_data, &write_data);

	switch (start_fw_update) {
	case 1:
		max77705_usbc_opcode_rw(g_usbc_data, &read_data, &write_data);
		break;
	case 2:
		max77705_usbc_opcode_read(g_usbc_data, &read_data);
		break;
	case 3:
		max77705_usbc_opcode_write(g_usbc_data, &write_data);

		break;
	case 11:
		msg_maxim("SYSTEM MESSAGE GRL COMMAND!!!");
		write_data.opcode = OPCODE_GRL_COMMAND;
		write_data.write_data[0] = 0x1;
		write_data.write_length = 0x1;
		write_data.read_length = 0x2;
		max77705_usbc_opcode_write(g_usbc_data, &write_data);

		break;
	default:
		break;
	}
	return size;
}
static DEVICE_ATTR(fw_update, S_IRUGO | S_IWUSR | S_IWGRP,
		NULL, max77705_fw_update);

static struct attribute *max77705_attr[] = {
	&dev_attr_fw_update.attr,
	NULL,
};

static struct attribute_group max77705_attr_grp = {
	.attrs = max77705_attr,
};

static void max77705_get_version_info(struct max77705_usbc_platform_data *usbc_data)
{
	u8 hw_rev[4] = {0, };
	u8 sw_main[3] = {0, };
	u8 sw_boot = 0;

	max77705_read_reg(usbc_data->muic, REG_UIC_HW_REV, &hw_rev[0]);
	max77705_read_reg(usbc_data->muic, REG_UIC_FW_MINOR, &sw_main[1]);
	max77705_read_reg(usbc_data->muic, REG_UIC_FW_REV, &sw_main[0]);

	usbc_data->HW_Revision = hw_rev[0];
	usbc_data->FW_Minor_Revision = sw_main[1];
	usbc_data->FW_Revision = sw_main[0];

	/* H/W, Minor, Major, Boot */
	msg_maxim("HW rev is %02Xh, FW rev is %02X.%02X!",
			usbc_data->HW_Revision, usbc_data->FW_Revision, usbc_data->FW_Minor_Revision);

	store_ccic_version(&hw_rev[0], &sw_main[0], &sw_boot);
}

void init_usbc_cmd_data(usbc_cmd_data *cmd_data)
{
	cmd_data->opcode = OPCODE_NONE;
	cmd_data->prev_opcode = OPCODE_NONE;
	cmd_data->response = OPCODE_NONE;
	cmd_data->val = REG_NONE;
	cmd_data->mask = REG_NONE;
	cmd_data->reg = REG_NONE;
	cmd_data->noti_cmd = OPCODE_NOTI_NONE;
	cmd_data->write_length = 0;
	cmd_data->read_length = 0;
	cmd_data->seq = 0;
	cmd_data->is_uvdm = 0;
	memset(cmd_data->write_data, REG_NONE, OPCODE_DATA_LENGTH);
	memset(cmd_data->read_data, REG_NONE, OPCODE_DATA_LENGTH);
}

static void init_usbc_cmd_node(usbc_cmd_node *usbc_cmd_node)
{
	usbc_cmd_data *cmd_data = &(usbc_cmd_node->cmd_data);

	pr_debug("%s:%s\n", "MAX77705", __func__);

	usbc_cmd_node->next = NULL;

	init_usbc_cmd_data(cmd_data);
}

static void copy_usbc_cmd_data(usbc_cmd_data *from, usbc_cmd_data *to)
{
	to->opcode = from->opcode;
	to->response = from->response;
	memcpy(to->read_data, from->read_data, OPCODE_DATA_LENGTH);
	memcpy(to->write_data, from->write_data, OPCODE_DATA_LENGTH);
	to->reg = from->reg;
	to->mask = from->mask;
	to->val = from->val;
	to->seq = from->seq;
	to->read_length = from->read_length;
	to->write_length = from->write_length;
	to->prev_opcode = from->prev_opcode;
	to->is_uvdm = from->is_uvdm;
}

bool is_empty_usbc_cmd_queue(usbc_cmd_queue_t *usbc_cmd_queue)
{
	bool ret = false;

	if (usbc_cmd_queue->front == NULL)
		ret = true;

	if (ret)
		msg_maxim("usbc_cmd_queue Empty(%c)", ret ? 'T' : 'F');

	return ret;
}

void enqueue_usbc_cmd(usbc_cmd_queue_t *usbc_cmd_queue, usbc_cmd_data *cmd_data)
{
	usbc_cmd_node	*temp_node = kzalloc(sizeof(usbc_cmd_node), GFP_KERNEL);

	if (!temp_node) {
		msg_maxim("failed to allocate usbc command queue");
		return;
	}

	init_usbc_cmd_node(temp_node);

	copy_usbc_cmd_data(cmd_data, &(temp_node->cmd_data));

	if (is_empty_usbc_cmd_queue(usbc_cmd_queue)) {
		usbc_cmd_queue->front = temp_node;
		usbc_cmd_queue->rear = temp_node;
	} else {
		usbc_cmd_queue->rear->next = temp_node;
		usbc_cmd_queue->rear = temp_node;
	}
}

static void dequeue_usbc_cmd
	(usbc_cmd_queue_t *usbc_cmd_queue, usbc_cmd_data *cmd_data)
{
	usbc_cmd_node *temp_node;

	if (is_empty_usbc_cmd_queue(usbc_cmd_queue)) {
		msg_maxim("Queue, Empty!");
		return;
	}

	temp_node = usbc_cmd_queue->front;
	copy_usbc_cmd_data(&(temp_node->cmd_data), cmd_data);

	msg_maxim("Opcode(0x%02x) Response(0x%02x)", cmd_data->opcode, cmd_data->response);

	if (usbc_cmd_queue->front->next == NULL) {
		msg_maxim("front->next = NULL");
		usbc_cmd_queue->front = NULL;
	} else
		usbc_cmd_queue->front = usbc_cmd_queue->front->next;

	if (is_empty_usbc_cmd_queue(usbc_cmd_queue))
		usbc_cmd_queue->rear = NULL;

	kfree(temp_node);
}

static bool front_usbc_cmd
	(usbc_cmd_queue_t *cmd_queue, usbc_cmd_data *cmd_data)
{
	if (is_empty_usbc_cmd_queue(cmd_queue)) {
		msg_maxim("Queue, Empty!");
		return false;
	}

	copy_usbc_cmd_data(&(cmd_queue->front->cmd_data), cmd_data);
	msg_maxim("Opcode(0x%02x)", cmd_data->opcode);
	return true;
}

static bool is_usbc_notifier_opcode(u8 opcode)
{
	bool noti = false;

	return noti;
}

/*
 * max77705_i2c_opcode_write - SMBus "opcode write" protocol
 * @chip: max77705 platform data
 * @command: OPcode
 * @values: Byte array into which data will be read; big enough to hold
 *	the data returned by the slave.
 *
 * This executes the SMBus "opcode read" protocol, returning negative errno
 * else the number of data bytes in the slave's response.
 */
int max77705_i2c_opcode_write(struct max77705_usbc_platform_data *usbc_data,
		u8 opcode, u8 length, u8 *values)
{
	u8 write_values[OPCODE_MAX_LENGTH] = { 0, };
	int ret = 0;

	if (length > OPCODE_DATA_LENGTH)
		return -EMSGSIZE;

	write_values[0] = opcode;
	if (length)
		memcpy(&write_values[1], values, length);

#if 0
	int i = 0; // To use this, move int i to the top to avoid build error
	for (i = 0; i < length + OPCODE_SIZE; i++)
		msg_maxim("[%d], 0x[%x]", i, write_values[i]);
#else
	msg_maxim("opcode 0x%x, write_length %d",
			opcode, length + OPCODE_SIZE);
	print_hex_dump(KERN_INFO, "max77705: opcode_write: ",
			DUMP_PREFIX_OFFSET, 16, 1, write_values,
			length + OPCODE_SIZE, false);
#endif

	/* Write opcode and data */
	ret = max77705_bulk_write(usbc_data->muic, OPCODE_WRITE,
			length + OPCODE_SIZE, write_values);
	/* Write end of data by 0x00 */
	if (length < OPCODE_DATA_LENGTH)
		max77705_write_reg(usbc_data->muic, OPCODE_WRITE_END, 0x00);

	return ret;
}

/**
 * max77705_i2c_opcode_read - SMBus "opcode read" protocol
 * @chip: max77705 platform data
 * @command: OPcode
 * @values: Byte array into which data will be read; big enough to hold
 *	the data returned by the slave.
 *
 * This executes the SMBus "opcode read" protocol, returning negative errno
 * else the number of data bytes in the slave's response.
 */
int max77705_i2c_opcode_read(struct max77705_usbc_platform_data *usbc_data,
		u8 opcode, u8 length, u8 *values)
{
	int size = 0;

	if (length > OPCODE_DATA_LENGTH)
		return -EMSGSIZE;

	/*
	 * We don't need to use opcode to get any feedback
	 */

	/* Read opcode data */
	size = max77705_bulk_read(usbc_data->muic, OPCODE_READ,
			length + OPCODE_SIZE, values);

#if 0
	int i = 0; // To use this, move int i to the top to avoid build error
	for (i = 0; i < length + OPCODE_SIZE; i++)
		msg_maxim("[%d], 0x[%x]", i, values[i]);
#else
	msg_maxim("opcode 0x%x, read_length %d, ret_error %d",
			opcode, length + OPCODE_SIZE, size);
	print_hex_dump(KERN_INFO, "max77705: opcode_read: ",
			DUMP_PREFIX_OFFSET, 16, 1, values,
			length + OPCODE_SIZE, false);
#endif
	return size;
}

static void max77705_notify_execute(struct max77705_usbc_platform_data *usbc_data,
		const usbc_cmd_data *cmd_data)
{
		/* to do  */
}

static void max77705_handle_update_opcode(struct max77705_usbc_platform_data *usbc_data,
		const usbc_cmd_data *cmd_data, unsigned char *data)
{
	usbc_cmd_data write_data;
	u8 read_value = data[1];
	u8 write_value = (read_value & (~cmd_data->mask)) | (cmd_data->val & cmd_data->mask);
	u8 opcode = cmd_data->response + 1; /* write opcode = read opocde + 1 */

	pr_info("%s: value update [0x%x]->[0x%x] at OPCODE(0x%x)\n", __func__,
			read_value, write_value, opcode);

	init_usbc_cmd_data(&write_data);
	write_data.opcode = opcode;
	write_data.write_length = 1;
	write_data.write_data[0] = write_value;
	write_data.read_length = 0;

	max77705_usbc_opcode_push(usbc_data, &write_data);
}

static void max77705_irq_execute(struct max77705_usbc_platform_data *usbc_data,
		const usbc_cmd_data *cmd_data)
{
	int len = cmd_data->read_length;
	unsigned char data[OPCODE_DATA_LENGTH] = {0,};
	u8 response = 0xff;
	u8 vdm_opcode_header = 0x0;
	UND_DATA_MSG_VDM_HEADER_Type vdm_header;
	u8 vdm_command = 0x0;
	u8 vdm_type = 0x0;
	u8 vdm_response = 0x0;

	memset(&vdm_header, 0, sizeof(UND_DATA_MSG_VDM_HEADER_Type));
	max77705_i2c_opcode_read(usbc_data, cmd_data->opcode,
			len, data);

	/* opcode identifying the messsage type. (0x51)*/
	response = data[0];

	if (response != cmd_data->response) {
		msg_maxim("Response [0x%02x] != [0x%02x]",
			response, cmd_data->response);
	}

	/* to do(read switch case) */
	switch (response) {
	case OPCODE_BCCTRL1_R:
	case OPCODE_BCCTRL2_R:
	case OPCODE_CTRL1_R:
	case OPCODE_CTRL2_R:
	case OPCODE_CTRL3_R:
	case OPCODE_CCCTRL1_R:
	case OPCODE_CCCTRL2_R:
	case OPCODE_CCCTRL3_R:
	case OPCODE_HVCTRL_R:
	case OPCODE_OPCODE_VCONN_ILIM_R:
	case OPCODE_CHGIN_ILIM_R:
	case OPCODE_CHGIN_ILIM2_R:
		if (cmd_data->seq == OPCODE_UPDATE_SEQ)
			max77705_handle_update_opcode(usbc_data, cmd_data, data);
		break;
#if defined(CONFIG_HV_MUIC_MAX77705_AFC)
	case COMMAND_AFC_RESULT_READ:
	case COMMAND_QC_2_0_SET:
		max77705_muic_handle_detect_dev_hv(usbc_data->muic_data, data);
		break;
#endif
	case OPCODE_CURRENT_SRCCAP:
		max77705_current_pdo(usbc_data, data);
		break;
	case OPCODE_GET_SRCCAP:
		max77705_pdo_list(usbc_data, data);
		break;
	case OPCODE_VDM_DISCOVER_GET_VDM_RESP:
		max77705_vdm_message_handler(usbc_data, data, len + OPCODE_SIZE);
		break;
	case OPCODE_READ_SBU:
		max77705_response_sbu_read(usbc_data, data);
		break;
	case OPCODE_VDM_DISCOVER_SET_VDM_REQ:
		vdm_opcode_header = data[1];
		switch (vdm_opcode_header) {
		case 0xFF:
			msg_maxim("This isn't invalid response(OPCODE : 0x48, HEADER : 0xFF)");
			break;
		default:
			memcpy(&vdm_header, &data[2], sizeof(vdm_header));
			vdm_type = vdm_header.BITS.VDM_Type;
			vdm_command = vdm_header.BITS.VDM_command;
			vdm_response = vdm_header.BITS.VDM_command_type;
			msg_maxim("vdm_type[%x], vdm_command[%x], vdm_response[%x]",
				vdm_type, vdm_command, vdm_response);
			switch (vdm_type) {
			case STRUCTURED_VDM:
				if (vdm_response == SEC_UVDM_RESPONDER_ACK) {
					switch (vdm_command) {
					case Discover_Identity:
						msg_maxim("ignore Discover_Identity");
						break;
					case Discover_SVIDs:
						msg_maxim("ignore Discover_SVIDs");
						break;
					case Discover_Modes:
						msg_maxim("ignore Discover_Modes");
						break;
					case Enter_Mode:
						msg_maxim("ignore Enter_Mode");
						break;
					case Exit_Mode:
						msg_maxim("ignore Exit_Mode");
						break;
					case Attention:
						msg_maxim("ignore Attention");
						break;
					case Configure:
						break;
					default:
						msg_maxim("vdm_command isn't valid[%x]", vdm_command);
						break;
					};
				} else
					msg_maxim("vdm_response is error value[%x]", vdm_response);
				break;
			case SEC_UVDM_UNSTRUCTURED_VDM:
				max77705_sec_unstructured_message_handler(usbc_data, data, len + OPCODE_SIZE);
				break;
			default:
				msg_maxim("vdm_type isn't valid error");
				break;
			};
			break;
		};
		break;
	case OPCODE_READ_SELFTEST:
		max77705_response_selftest_read(usbc_data, data);
		break;
#if 1
	case OPCODE_GRL_COMMAND:
		max77705_set_forcetrimi(usbc_data);
		break;
#endif
	default:
		break;
	}
}

void max77705_usbc_dequeue_queue(struct max77705_usbc_platform_data *usbc_data)
{
	usbc_cmd_data cmd_data;
	usbc_cmd_queue_t *cmd_queue = NULL;

	cmd_queue = &(usbc_data->usbc_cmd_queue);

	init_usbc_cmd_data(&cmd_data);

	if (is_empty_usbc_cmd_queue(cmd_queue)) {
		msg_maxim("Queue, Empty");
		return;
	}

	dequeue_usbc_cmd(cmd_queue, &cmd_data);
	msg_maxim("!! Dequeue queue : opcode : %x, 1st data : %x. 2st data : %x",
		cmd_data.write_data[0],
		cmd_data.read_data[0],
		cmd_data.val);
}

void max77705_usbc_clear_queue(struct max77705_usbc_platform_data *usbc_data)
{
	usbc_cmd_data cmd_data;
	usbc_cmd_queue_t *cmd_queue = NULL;

	mutex_lock(&usbc_data->op_lock);
	msg_maxim("IN");
	cmd_queue = &(usbc_data->usbc_cmd_queue);

	while (!is_empty_usbc_cmd_queue(cmd_queue)) {
		init_usbc_cmd_data(&cmd_data);
		dequeue_usbc_cmd(cmd_queue, &cmd_data);
	}

	msg_maxim("OUT");
	mutex_unlock(&usbc_data->op_lock);
}

static void max77705_usbc_cmd_run(struct max77705_usbc_platform_data *usbc_data)
{
	usbc_cmd_queue_t *cmd_queue = NULL;
	usbc_cmd_node *run_node;
	usbc_cmd_data cmd_data;

	cmd_queue = &(usbc_data->usbc_cmd_queue);


	run_node = kzalloc(sizeof(usbc_cmd_node), GFP_KERNEL);
	if (!run_node) {
		msg_maxim("failed to allocate muic command queue");
		return;
	}

	init_usbc_cmd_node(run_node);

	init_usbc_cmd_data(&cmd_data);

	if (is_empty_usbc_cmd_queue(cmd_queue)) {
		msg_maxim("Queue, Empty");
		kfree(run_node);
		return;
	}

	dequeue_usbc_cmd(cmd_queue, &cmd_data);

	if (is_usbc_notifier_opcode(cmd_data.opcode)) {
		max77705_notify_execute(usbc_data, &cmd_data);
		max77705_usbc_cmd_run(usbc_data);
	} else if (cmd_data.opcode == OPCODE_NONE) {/* Apcmdres isr */
		msg_maxim("Apcmdres ISR !!!");
		max77705_irq_execute(usbc_data, &cmd_data);
		max77705_usbc_cmd_run(usbc_data);
	} else { /* No ISR */
		msg_maxim("No ISR");
		copy_usbc_cmd_data(&cmd_data, &(usbc_data->last_opcode));
		max77705_i2c_opcode_write(usbc_data, cmd_data.opcode, cmd_data.write_length, cmd_data.write_data);
	}
	kfree(run_node);
}

void max77705_usbc_opcode_write(struct max77705_usbc_platform_data *usbc_data,
	usbc_cmd_data *write_op)
{
	usbc_cmd_queue_t *cmd_queue = &(usbc_data->usbc_cmd_queue);
	usbc_cmd_data execute_cmd_data;
	usbc_cmd_data current_cmd;

	mutex_lock(&usbc_data->op_lock);
	init_usbc_cmd_data(&current_cmd);

	/* the messages sent to USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.opcode = write_op->opcode;
	execute_cmd_data.write_length = write_op->write_length;
	execute_cmd_data.is_uvdm = write_op->is_uvdm;
	memcpy(execute_cmd_data.write_data, write_op->write_data, OPCODE_DATA_LENGTH);
	execute_cmd_data.seq = OPCODE_WRITE_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	/* the messages recevied From USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.response = write_op->opcode;
	execute_cmd_data.read_length = write_op->read_length;
	execute_cmd_data.is_uvdm = write_op->is_uvdm;
	execute_cmd_data.seq = OPCODE_WRITE_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	msg_maxim("W->W opcode[0x%02x] write_length[%d] read_length[%d]",
		write_op->opcode, write_op->write_length, write_op->read_length);

	front_usbc_cmd(cmd_queue, &current_cmd);
	if (current_cmd.opcode == write_op->opcode)
		max77705_usbc_cmd_run(usbc_data);
	else
		msg_maxim("!!! current_cmd.opcode [0x%02x], read_op->opcode[0x%02x]",
			current_cmd.opcode, write_op->opcode);

	mutex_unlock(&usbc_data->op_lock);
}

void max77705_usbc_opcode_read(struct max77705_usbc_platform_data *usbc_data,
	usbc_cmd_data *read_op)
{
	usbc_cmd_queue_t *cmd_queue = &(usbc_data->usbc_cmd_queue);
	usbc_cmd_data execute_cmd_data;
	usbc_cmd_data current_cmd;

	mutex_lock(&usbc_data->op_lock);
	init_usbc_cmd_data(&current_cmd);

	/* the messages sent to USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.opcode = read_op->opcode;
	execute_cmd_data.write_length = read_op->write_length;
	execute_cmd_data.is_uvdm = read_op->is_uvdm;
	memcpy(execute_cmd_data.write_data, read_op->write_data, read_op->write_length);
	execute_cmd_data.seq = OPCODE_READ_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	/* the messages recevied From USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.response = read_op->opcode;
	execute_cmd_data.read_length = read_op->read_length;
	execute_cmd_data.is_uvdm = read_op->is_uvdm;
	execute_cmd_data.seq = OPCODE_READ_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	msg_maxim("R->R opcode[0x%02x] write_length[%d] read_length[%d]",
		read_op->opcode, read_op->write_length, read_op->read_length);

	front_usbc_cmd(cmd_queue, &current_cmd);
	if (current_cmd.opcode == read_op->opcode)
		max77705_usbc_cmd_run(usbc_data);
	else
		msg_maxim("!!! current_cmd.opcode [0x%02x], read_op->opcode[0x%02x]",
			current_cmd.opcode, read_op->opcode);

	mutex_unlock(&usbc_data->op_lock);
}

void max77705_usbc_opcode_update(struct max77705_usbc_platform_data *usbc_data,
	usbc_cmd_data *update_op)
{
	usbc_cmd_queue_t *cmd_queue = &(usbc_data->usbc_cmd_queue);
	usbc_cmd_data execute_cmd_data;
	usbc_cmd_data current_cmd;

	switch (update_op->opcode) {
	case OPCODE_BCCTRL1_R:
	case OPCODE_BCCTRL2_R:
	case OPCODE_CTRL1_R:
	case OPCODE_CTRL2_R:
	case OPCODE_CTRL3_R:
	case OPCODE_CCCTRL1_R:
	case OPCODE_CCCTRL2_R:
	case OPCODE_CCCTRL3_R:
	case OPCODE_HVCTRL_R:
	case OPCODE_OPCODE_VCONN_ILIM_R:
	case OPCODE_CHGIN_ILIM_R:
	case OPCODE_CHGIN_ILIM2_R:
		break;
	default:
		pr_err("%s: invalid usage(0x%x), return\n", __func__, update_op->opcode);
		return;
	}

	mutex_lock(&usbc_data->op_lock);
	init_usbc_cmd_data(&current_cmd);

	/* the messages sent to USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.opcode = update_op->opcode;
	execute_cmd_data.write_length = 0;
	execute_cmd_data.is_uvdm = update_op->is_uvdm;
	memcpy(execute_cmd_data.write_data, update_op->write_data, update_op->write_length);
	execute_cmd_data.seq = OPCODE_UPDATE_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	/* the messages recevied From USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.response = update_op->opcode;
	execute_cmd_data.read_length = 1;
	execute_cmd_data.seq = OPCODE_UPDATE_SEQ;
	execute_cmd_data.val = update_op->val;
	execute_cmd_data.mask = update_op->mask;
	execute_cmd_data.is_uvdm = update_op->is_uvdm;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	msg_maxim("U->U opcode[0x%02x] write_length[%d] read_length[%d]",
		update_op->opcode, update_op->write_length, update_op->read_length);

	front_usbc_cmd(cmd_queue, &current_cmd);
	if (current_cmd.opcode == update_op->opcode)
		max77705_usbc_cmd_run(usbc_data);
	else
		msg_maxim("!!! current_cmd.opcode [0x%02x], update_op->opcode[0x%02x]",
			current_cmd.opcode, update_op->opcode);

	mutex_unlock(&usbc_data->op_lock);
}

void max77705_usbc_opcode_push(struct max77705_usbc_platform_data *usbc_data,
	usbc_cmd_data *read_op)
{
	usbc_cmd_queue_t *cmd_queue = &(usbc_data->usbc_cmd_queue);
	usbc_cmd_data execute_cmd_data;
	usbc_cmd_data current_cmd;

	init_usbc_cmd_data(&current_cmd);

	/* the messages sent to USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.opcode = read_op->opcode;
	execute_cmd_data.write_length = read_op->write_length;
	execute_cmd_data.is_uvdm = read_op->is_uvdm;
	memcpy(execute_cmd_data.write_data, read_op->write_data, read_op->write_length);
	execute_cmd_data.seq = OPCODE_PUSH_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	/* the messages recevied From USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.response = read_op->opcode;
	execute_cmd_data.read_length = read_op->read_length;
	execute_cmd_data.is_uvdm = read_op->is_uvdm;
	execute_cmd_data.seq = OPCODE_PUSH_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	msg_maxim("P->P opcode[0x%02x] write_length[%d] read_length[%d]",
		read_op->opcode, read_op->write_length, read_op->read_length);
}

void max77705_usbc_opcode_rw(struct max77705_usbc_platform_data *usbc_data,
	usbc_cmd_data *read_op, usbc_cmd_data *write_op)
{
	usbc_cmd_queue_t *cmd_queue = &(usbc_data->usbc_cmd_queue);
	usbc_cmd_data execute_cmd_data;
	usbc_cmd_data current_cmd;

	mutex_lock(&usbc_data->op_lock);
	init_usbc_cmd_data(&current_cmd);

	/* the messages sent to USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.opcode = read_op->opcode;
	execute_cmd_data.write_length = read_op->write_length;
	execute_cmd_data.is_uvdm = read_op->is_uvdm;
	memcpy(execute_cmd_data.write_data, read_op->write_data, read_op->write_length);
	execute_cmd_data.seq = OPCODE_RW_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	/* the messages recevied From USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.response = read_op->opcode;
	execute_cmd_data.read_length = read_op->read_length;
	execute_cmd_data.is_uvdm = read_op->is_uvdm;
	execute_cmd_data.seq = OPCODE_RW_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	/* the messages sent to USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.opcode = write_op->opcode;
	execute_cmd_data.write_length = write_op->write_length;
	execute_cmd_data.is_uvdm = write_op->is_uvdm;
	memcpy(execute_cmd_data.write_data, write_op->write_data, OPCODE_DATA_LENGTH);
	execute_cmd_data.seq = OPCODE_RW_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	/* the messages recevied From USBC. */
	init_usbc_cmd_data(&execute_cmd_data);
	execute_cmd_data.response = write_op->opcode;
	execute_cmd_data.read_length = write_op->read_length;
	execute_cmd_data.is_uvdm = write_op->is_uvdm;
	execute_cmd_data.seq = OPCODE_RW_SEQ;
	enqueue_usbc_cmd(cmd_queue, &execute_cmd_data);

	msg_maxim("RW->R opcode[0x%02x] write_length[%d] read_length[%d]",
		read_op->opcode, read_op->write_length, read_op->read_length);
	msg_maxim("RW->W opcode[0x%02x] write_length[%d] read_length[%d]",
		write_op->opcode, write_op->write_length, write_op->read_length);

	front_usbc_cmd(cmd_queue, &current_cmd);
	if (current_cmd.opcode == read_op->opcode)
		max77705_usbc_cmd_run(usbc_data);
	else
		msg_maxim("!!! current_cmd.opcode [0x%02x], read_op->opcode[0x%02x]",
			current_cmd.opcode, read_op->opcode);

	mutex_unlock(&usbc_data->op_lock);
}

/*
 * max77705_uic_op_send_work_func func - Send OPCode
 * @work: work_struct of max77705_i2c
 *
 * Wait for OPCode response
 */
static void max77705_uic_op_send_work_func(
		struct work_struct *work)
{
	struct max77705_usbc_platform_data *usbc_data;
	struct max77705_opcode opcodes[] = {
		{
			.opcode = OPCODE_BCCTRL1_R,
			.data = { 0, },
			.write_length = 0,
			.read_length = 1,
		},
		{
			.opcode = OPCODE_BCCTRL1_W,
			.data = { 0x20, },
			.write_length = 1,
			.read_length = 0,
		},
		{
			.opcode = OPCODE_BCCTRL2_R,
			.data = { 0, },
			.write_length = 0,
			.read_length = 1,
		},
		{
			.opcode = OPCODE_BCCTRL2_W,
			.data = { 0x10, },
			.write_length = 1,
			.read_length = 0,
		},
		{
			.opcode = OPCODE_CURRENT_SRCCAP,
			.data = { 0, },
			.write_length = 1,
			.read_length = 4,
		},
		{
			.opcode = OPCODE_AFC_HV_W,
			.data = { 0x46, },
			.write_length = 1,
			.read_length = 2,
		},
		{
			.opcode = OPCODE_SRCCAP_REQUEST,
			.data = { 0x00, },
			.write_length = 1,
			.read_length = 32,
		},
	};
	int op_loop;

	usbc_data = container_of(work, struct max77705_usbc_platform_data, op_send_work);

	msg_maxim("IN");
	for (op_loop = 0; op_loop < ARRAY_SIZE(opcodes); op_loop++) {
		if (usbc_data->op_code == opcodes[op_loop].opcode) {
			max77705_i2c_opcode_write(usbc_data, opcodes[op_loop].opcode,
				opcodes[op_loop].write_length, opcodes[op_loop].data);
			break;
		}
	}
	msg_maxim("OUT");
}

static void max77705_reset_ic(struct max77705_usbc_platform_data *usbc_data)
{
	max77705_write_reg(usbc_data->muic, 0x80, 0x0F);
	msleep(100); /* need 100ms delay */
}

void max77705_usbc_check_sysmsg(struct max77705_usbc_platform_data *usbc_data, u8 sysmsg)
{
	usbc_cmd_queue_t *cmd_queue = &(usbc_data->usbc_cmd_queue);
	bool is_empty_queue = is_empty_usbc_cmd_queue(cmd_queue);
	usbc_cmd_data cmd_data;
	usbc_cmd_data next_cmd_data;
	u8 next_opcode = 0xFF;
	u8 interrupt;
#if defined(CONFIG_USB_HW_PARAM)
	struct otg_notify *o_notify = get_otg_notify();
#endif

	if (usbc_data->shut_down) {
		msg_maxim("IGNORE SYSTEM_MSG IN SHUTDOWN MODE!!");
		return;
	}

	switch (sysmsg) {
	case SYSERROR_NONE:
		break;
	case SYSERROR_BOOT_WDT:
		usbc_data->watchdog_count++;
		msg_maxim("SYSERROR_BOOT_WDT: %d", usbc_data->watchdog_count);
		max77705_usbc_mask_irq(usbc_data);
		max77705_write_reg(usbc_data->muic, REG_UIC_INT_M, REG_UIC_INT_M_INIT);
		max77705_write_reg(usbc_data->muic, REG_CC_INT_M, REG_CC_INT_M_INIT);
		max77705_write_reg(usbc_data->muic, REG_PD_INT_M, REG_PD_INT_M_INIT);
		max77705_write_reg(usbc_data->muic, REG_VDM_INT_M, REG_VDM_INT_M_INIT);
		max77705_usbc_clear_queue(usbc_data);
		usbc_data->is_first_booting = 1;
		max77705_set_enable_alternate_mode(ALTERNATE_MODE_START);
		max77705_usbc_umask_irq(usbc_data);
		break;
	case SYSERROR_BOOT_SWRSTREQ:
		break;
	case SYSMSG_BOOT_POR:
		usbc_data->por_count++;
		max77705_usbc_mask_irq(usbc_data);
		max77705_reset_ic(usbc_data);
		max77705_write_reg(usbc_data->muic, REG_UIC_INT_M, REG_UIC_INT_M_INIT);
		max77705_write_reg(usbc_data->muic, REG_CC_INT_M, REG_CC_INT_M_INIT);
		max77705_write_reg(usbc_data->muic, REG_PD_INT_M, REG_PD_INT_M_INIT);
		max77705_write_reg(usbc_data->muic, REG_VDM_INT_M, REG_VDM_INT_M_INIT);
		/* clear UIC_INT to prevent infinite sysmsg irq */
		max77705_read_reg(usbc_data->muic, MAX77705_USBC_REG_UIC_INT, &interrupt);
		msg_maxim("SYSERROR_BOOT_POR: %d, UIC_INT:0x%02x", usbc_data->por_count, interrupt);
		max77705_usbc_clear_queue(usbc_data);
		usbc_data->is_first_booting = 1;
		max77705_set_enable_alternate_mode(ALTERNATE_MODE_START);
		max77705_usbc_umask_irq(usbc_data);
		break;
	case SYSERROR_HV_NOVBUS:
		break;
	case SYSERROR_HV_FMETHOD_RXPERR:
		break;
	case SYSERROR_HV_FMETHOD_RXBUFOW:
		break;
	case SYSERROR_HV_FMETHOD_RXTFR:
		break;
	case SYSERROR_HV_FMETHOD_MPNACK:
		break;
	case SYSERROR_HV_FMETHOD_RESET_FAIL:
		break;
	case SYSMsg_AFC_Done:
		break;
	case SYSERROR_SYSPOS:
		break;
	case SYSERROR_APCMD_UNKNOWN:
		break;
	case SYSERROR_APCMD_INPROGRESS:
		break;
	case SYSERROR_APCMD_FAIL:

		init_usbc_cmd_data(&cmd_data);
		init_usbc_cmd_data(&next_cmd_data);

		if (front_usbc_cmd(cmd_queue, &next_cmd_data))
			next_opcode = next_cmd_data.response;

		if (!is_empty_queue) {
			copy_usbc_cmd_data(&(usbc_data->last_opcode), &cmd_data);

			if (cmd_data.opcode == OPCODE_GRL_COMMAND || next_opcode == OPCODE_VDM_DISCOVER_SET_VDM_REQ) {
				max77705_usbc_dequeue_queue(usbc_data);
				cmd_data.opcode = OPCODE_NONE;
			}

			if ((cmd_data.opcode != OPCODE_NONE) && (cmd_data.opcode == next_opcode)) {
				if (next_opcode != OPCODE_VDM_DISCOVER_SET_VDM_REQ) {
					max77705_i2c_opcode_write(usbc_data,
						cmd_data.opcode,
						cmd_data.write_length,
						cmd_data.write_data);
					msg_maxim("RETRY SUCCESS : %x, %x", cmd_data.opcode, next_opcode);
				} else
					msg_maxim("IGNORE COMMAND : %x, %x", cmd_data.opcode, next_opcode);
			} else {
				msg_maxim("RETRY FAILED : %x, %x", cmd_data.opcode, next_opcode);
			}

		}

#if defined(CONFIG_HV_MUIC_MAX77705_AFC)
		max77705_muic_disable_afc_protocol(usbc_data->muic_data);
#endif
		/* TO DO DEQEUE MSG. */
		break;
#ifdef MAX77705_GRL_ENABLE
	case SYSMSG_SET_GRL:
		max77705_usbc_clear_queue(usbc_data);
		msg_maxim("SYSTEM MESSAGE GRL COMMAND!!!");

		init_usbc_cmd_data(&cmd_data);
		cmd_data.opcode = OPCODE_GRL_COMMAND;
		cmd_data.write_data[0] = 0x1;
		cmd_data.write_length = 0x1;
		cmd_data.read_length = 0x2;
		max77705_usbc_opcode_write(usbc_data, &cmd_data);
//		max77705_set_forcetrimi(usbc_data);

		break;
#endif
	case SYSMSG_CCx_5V_SHORT:
		msg_maxim("CC-VBUS SHORT");
#if defined(CONFIG_USB_HW_PARAM)
		if (o_notify)
			inc_hw_param(o_notify, USB_CCIC_VBUS_CC_SHORT_COUNT);
#endif
		break;
	case SYSMSG_SBUx_GND_SHORT:
		msg_maxim("SBU-GND SHORT");
#if defined(CONFIG_USB_HW_PARAM)
		if (o_notify)
			inc_hw_param(o_notify, USB_CCIC_GND_SBU_SHORT_COUNT);
#endif
		break;
	case SYSMSG_SBUx_5V_SHORT:
		msg_maxim("SBU-VBUS SHORT");
#if defined(CONFIG_USB_HW_PARAM)
		if (o_notify)
			inc_hw_param(o_notify, USB_CCIC_VBUS_SBU_SHORT_COUNT);
#endif
		break;
	case SYSERROR_POWER_NEGO:
		if (!usbc_data->last_opcode.is_uvdm) { /* structured vdm */
			if (usbc_data->last_opcode.opcode == OPCODE_VDM_DISCOVER_SET_VDM_REQ) {
				max77705_usbc_opcode_write(usbc_data, &usbc_data->last_opcode);
				msg_maxim("SYSMSG PWR NEGO ERR : VDM request retry");
			}
		} else { /* unstructured vdm */
			usbc_data->uvdm_error = 1;
			msg_maxim("SYSMSG PWR NEGO ERR : UVDM request error - dir : %d",
				usbc_data->is_in_sec_uvdm_out);
			if (usbc_data->is_in_sec_uvdm_out == DIR_OUT)
				complete(&usbc_data->uvdm_longpacket_out_wait);
			else if (usbc_data->is_in_sec_uvdm_out == DIR_IN)
				complete(&usbc_data->uvdm_longpacket_in_wait);
			else
				;
		}
		break;
#if defined(CONFIG_SEC_FACTORY)
	case SYSERROR_FACTORY_RID0:
		factory_execute_monitor(FAC_ABNORMAL_REPEAT_RID0);
		break;
#endif
	default:
		break;
	}
}


static irqreturn_t max77705_apcmd_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	u8 sysmsg = 0;

	msg_maxim("IRQ(%d)_IN", irq);
	max77705_read_reg(usbc_data->muic, REG_USBC_STATUS2, &usbc_data->usbc_status2);
	sysmsg = usbc_data->usbc_status2;
	msg_maxim(" [IN] sysmsg : %d", sysmsg);

	mutex_lock(&usbc_data->op_lock);
	max77705_usbc_cmd_run(usbc_data);
	mutex_unlock(&usbc_data->op_lock);

	msg_maxim("IRQ(%d)_OUT", irq);

	return IRQ_HANDLED;
}

static irqreturn_t max77705_sysmsg_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	u8 sysmsg = 0;

	max77705_read_reg(usbc_data->muic, REG_USBC_STATUS2, &usbc_data->usbc_status2);
	sysmsg = usbc_data->usbc_status2;
	msg_maxim("IRQ(%d)_IN", irq);
	max77705_usbc_check_sysmsg(usbc_data, sysmsg);
	usbc_data->sysmsg = sysmsg;
	msg_maxim("IRQ(%d)_OUT sysmsg: %x", irq, sysmsg);

	return IRQ_HANDLED;
}


static irqreturn_t max77705_vdm_identity_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	MAX77705_VDM_MSG_IRQ_STATUS_Type VDM_MSG_IRQ_State;

	memset(&VDM_MSG_IRQ_State, 0, sizeof(VDM_MSG_IRQ_State));
	msg_maxim("IRQ(%d)_IN", irq);
	VDM_MSG_IRQ_State.BITS.Vdm_Flag_Discover_ID = 1;
	max77705_receive_alternate_message(usbc_data, &VDM_MSG_IRQ_State);
	msg_maxim("IRQ(%d)_OUT", irq);

	return IRQ_HANDLED;
}

static irqreturn_t max77705_vdm_svids_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	MAX77705_VDM_MSG_IRQ_STATUS_Type VDM_MSG_IRQ_State;

	memset(&VDM_MSG_IRQ_State, 0, sizeof(VDM_MSG_IRQ_State));
	msg_maxim("IRQ(%d)_IN", irq);
	VDM_MSG_IRQ_State.BITS.Vdm_Flag_Discover_SVIDs = 1;
	max77705_receive_alternate_message(usbc_data, &VDM_MSG_IRQ_State);
	msg_maxim("IRQ(%d)_OUT", irq);
	return IRQ_HANDLED;
}

static irqreturn_t max77705_vdm_discover_mode_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	MAX77705_VDM_MSG_IRQ_STATUS_Type VDM_MSG_IRQ_State;

	memset(&VDM_MSG_IRQ_State, 0, sizeof(VDM_MSG_IRQ_State));
	msg_maxim("IRQ(%d)_IN", irq);
	VDM_MSG_IRQ_State.BITS.Vdm_Flag_Discover_MODEs = 1;
	max77705_receive_alternate_message(usbc_data, &VDM_MSG_IRQ_State);
	msg_maxim("IRQ(%d)_OUT", irq);
	return IRQ_HANDLED;
}

static irqreturn_t max77705_vdm_enter_mode_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	MAX77705_VDM_MSG_IRQ_STATUS_Type VDM_MSG_IRQ_State;

	memset(&VDM_MSG_IRQ_State, 0, sizeof(VDM_MSG_IRQ_State));
	msg_maxim("IRQ(%d)_IN", irq);
	VDM_MSG_IRQ_State.BITS.Vdm_Flag_Enter_Mode = 1;
	max77705_receive_alternate_message(usbc_data, &VDM_MSG_IRQ_State);
	msg_maxim("IRQ(%d)_OUT", irq);
	return IRQ_HANDLED;
}

static irqreturn_t max77705_vdm_dp_status_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	MAX77705_VDM_MSG_IRQ_STATUS_Type VDM_MSG_IRQ_State;

	memset(&VDM_MSG_IRQ_State, 0, sizeof(VDM_MSG_IRQ_State));
	msg_maxim("IRQ(%d)_IN", irq);
	VDM_MSG_IRQ_State.BITS.Vdm_Flag_DP_Status_Update = 1;
	max77705_receive_alternate_message(usbc_data, &VDM_MSG_IRQ_State);
	msg_maxim("IRQ(%d)_OUT", irq);
	return IRQ_HANDLED;
}

static irqreturn_t max77705_vdm_dp_configure_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	MAX77705_VDM_MSG_IRQ_STATUS_Type VDM_MSG_IRQ_State;

	memset(&VDM_MSG_IRQ_State, 0, sizeof(VDM_MSG_IRQ_State));
	msg_maxim("IRQ(%d)_IN", irq);
	VDM_MSG_IRQ_State.BITS.Vdm_Flag_DP_Configure = 1;
	max77705_receive_alternate_message(usbc_data, &VDM_MSG_IRQ_State);
	msg_maxim("IRQ(%d)_OUT", irq);
	return IRQ_HANDLED;
}

static irqreturn_t max77705_vdm_attention_irq(int irq, void *data)
{
	struct max77705_usbc_platform_data *usbc_data = data;
	MAX77705_VDM_MSG_IRQ_STATUS_Type VDM_MSG_IRQ_State;

	memset(&VDM_MSG_IRQ_State, 0, sizeof(VDM_MSG_IRQ_State));
	msg_maxim("IRQ(%d)_IN", irq);
	VDM_MSG_IRQ_State.BITS.Vdm_Flag_Attention = 1;
	max77705_receive_alternate_message(usbc_data, &VDM_MSG_IRQ_State);
	msg_maxim("IRQ(%d)_OUT", irq);
	return IRQ_HANDLED;
}

int max77705_init_irq_handler(struct max77705_usbc_platform_data *usbc_data)
{
	int ret = 0;

	wake_lock_init(&usbc_data->apcmd_wake_lock, WAKE_LOCK_SUSPEND,
			   "usbc->apcmd");
	wake_lock_init(&usbc_data->sysmsg_wake_lock, WAKE_LOCK_SUSPEND,
		"usbc->sysmsg");

	usbc_data->irq_apcmd = usbc_data->irq_base + MAX77705_USBC_IRQ_APC_INT;
	if (usbc_data->irq_apcmd) {
		ret = request_threaded_irq(usbc_data->irq_apcmd,
			   NULL, max77705_apcmd_irq,
			   0,
			   "usbc-apcmd-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_sysmsg = usbc_data->irq_base + MAX77705_USBC_IRQ_SYSM_INT;
	if (usbc_data->irq_sysmsg) {
		ret = request_threaded_irq(usbc_data->irq_sysmsg,
			   NULL, max77705_sysmsg_irq,
			   0,
			   "usbc-sysmsg-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_vdm0 = usbc_data->irq_base + MAX77705_IRQ_VDM_DISCOVER_ID_INT;
	if (usbc_data->irq_vdm0) {
		ret = request_threaded_irq(usbc_data->irq_vdm0,
			   NULL, max77705_vdm_identity_irq,
			   0,
			   "usbc-vdm0-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_vdm1 = usbc_data->irq_base + MAX77705_IRQ_VDM_DISCOVER_SVIDS_INT;
	if (usbc_data->irq_vdm1) {
		ret = request_threaded_irq(usbc_data->irq_vdm1,
			   NULL, max77705_vdm_svids_irq,
			   0,
			   "usbc-vdm1-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_vdm2 = usbc_data->irq_base + MAX77705_IRQ_VDM_DISCOVER_MODES_INT;
	if (usbc_data->irq_vdm2) {
		ret = request_threaded_irq(usbc_data->irq_vdm2,
			   NULL, max77705_vdm_discover_mode_irq,
			   0,
			   "usbc-vdm2-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_vdm3 = usbc_data->irq_base + MAX77705_IRQ_VDM_ENTER_MODE_INT;
	if (usbc_data->irq_vdm3) {
		ret = request_threaded_irq(usbc_data->irq_vdm3,
			   NULL, max77705_vdm_enter_mode_irq,
			   0,
			   "usbc-vdm3-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_vdm4 = usbc_data->irq_base + MAX77705_IRQ_VDM_DP_STATUS_UPDATE_INT;
	if (usbc_data->irq_vdm4) {
		ret = request_threaded_irq(usbc_data->irq_vdm4,
			   NULL, max77705_vdm_dp_status_irq,
			   0,
			   "usbc-vdm4-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_vdm5 = usbc_data->irq_base + MAX77705_IRQ_VDM_DP_CONFIGURE_INT;
	if (usbc_data->irq_vdm5) {
		ret = request_threaded_irq(usbc_data->irq_vdm5,
			   NULL, max77705_vdm_dp_configure_irq,
			   0,
			   "usbc-vdm5-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	usbc_data->irq_vdm6 = usbc_data->irq_base + MAX77705_IRQ_VDM_ATTENTION_INT;
	if (usbc_data->irq_vdm6) {
		ret = request_threaded_irq(usbc_data->irq_vdm6,
			   NULL, max77705_vdm_attention_irq,
			   0,
			   "usbc-vdm6-irq", usbc_data);
		if (ret) {
			pr_err("%s: Failed to Request IRQ (%d)\n", __func__, ret);
			return ret;
		}
	}

	return ret;
}

static void max77705_usbc_umask_irq(struct max77705_usbc_platform_data *usbc_data)
{
	int ret = 0;
	u8 i2c_data = 0;
	/* Unmask max77705 interrupt */
	ret = max77705_read_reg(usbc_data->i2c, 0x23,
			  &i2c_data);
	if (ret) {
		pr_err("%s fail to read muic reg\n", __func__);
		return;
	}

	i2c_data &= ~((1 << 3));	/* Unmask muic interrupt */
	max77705_write_reg(usbc_data->i2c, 0x23,
			   i2c_data);
}
static void max77705_usbc_mask_irq(struct max77705_usbc_platform_data *usbc_data)
{
	int ret = 0;
	u8 i2c_data = 0;
	/* Unmask max77705 interrupt */
	ret = max77705_read_reg(usbc_data->i2c, 0x23,
			  &i2c_data);
	if (ret) {
		pr_err("%s fail to read muic reg\n", __func__);
		return;
	}

	i2c_data |= ((1 << 3));	/* Unmask muic interrupt */
	max77705_write_reg(usbc_data->i2c, 0x23,
			   i2c_data);
}

static int pdic_handle_usb_external_notifier_notification(struct notifier_block *nb,
				unsigned long action, void *data)
{
	struct max77705_usbc_platform_data *usbpd_data = g_usbc_data;
	int ret = 0;
	int enable = *(int *)data;

	pr_info("%s : action=%lu , enable=%d\n", __func__, action, enable);
	switch (action) {
	case EXTERNAL_NOTIFY_HOSTBLOCK_PRE:
		if (enable) {
			max77705_set_enable_alternate_mode(ALTERNATE_MODE_STOP);
			if (usbpd_data->dp_is_connect)
				max77705_dp_detach(usbpd_data);
		} else {
			if (usbpd_data->dp_is_connect)
				max77705_dp_detach(usbpd_data);
		}
		break;
	case EXTERNAL_NOTIFY_HOSTBLOCK_POST:
		if (enable) {
		} else {
			max77705_set_enable_alternate_mode(ALTERNATE_MODE_START);
		}
		break;
	default:
		break;
	}

	return ret;
}

static void delayed_external_notifier_init(struct work_struct *work)
{
	int ret = 0;
	static int retry_count = 1;
	int max_retry_count = 5;
	struct max77705_usbc_platform_data *usbpd_data = g_usbc_data;

	pr_info("%s : %d = times!\n", __func__, retry_count);

	/* Register ccic handler to ccic notifier block list */
	ret = usb_external_notify_register(&usbpd_data->usb_external_notifier_nb,
		pdic_handle_usb_external_notifier_notification, EXTERNAL_NOTIFY_DEV_PDIC);
	if (ret < 0) {
		pr_err("Manager notifier init time is %d.\n", retry_count);
		if (retry_count++ != max_retry_count)
			schedule_delayed_work(&usbpd_data->usb_external_notifier_register_work, msecs_to_jiffies(2000));
		else
			pr_err("fail to init external notifier\n");
	} else
		pr_info("%s : external notifier register done!\n", __func__);
}

#if defined(CONFIG_SEC_FACTORY)
static void factory_check_abnormal_state(struct work_struct *work)
{
	struct max77705_usbc_platform_data *usbpd_data = g_usbc_data;
	int state_cnt = usbpd_data->factory_mode.FAC_Abnormal_Repeat_State;

	msg_maxim("IN ");

	if (state_cnt >= FAC_ABNORMAL_REPEAT_STATE) {
		msg_maxim("Notify the abnormal state [STATE] [ %d]", state_cnt);
		max77705_ccic_event_work(usbpd_data,
			CCIC_NOTIFY_DEV_CCIC, CCIC_NOTIFY_ID_FAC, 1, 0, 0);
	} else
		msg_maxim("[STATE] cnt :  [%d]", state_cnt);
	usbpd_data->factory_mode.FAC_Abnormal_Repeat_State = 0;
	msg_maxim("OUT ");
}

static void factory_check_normal_rid(struct work_struct *work)
{
	struct max77705_usbc_platform_data *usbpd_data = g_usbc_data;
	int rid_cnt = usbpd_data->factory_mode.FAC_Abnormal_Repeat_RID;

	msg_maxim("IN ");

	if (rid_cnt >= FAC_ABNORMAL_REPEAT_RID) {
		msg_maxim("Notify the abnormal state [RID] [ %d]", rid_cnt);
		max77705_ccic_event_work(usbpd_data,
			CCIC_NOTIFY_DEV_CCIC, CCIC_NOTIFY_ID_FAC, 1 << 1, 0, 0);
	} else
		msg_maxim("[RID] cnt :  [%d]", rid_cnt);

	usbpd_data->factory_mode.FAC_Abnormal_Repeat_RID = 0;
	msg_maxim("OUT ");
}

void factory_execute_monitor(int type)
{
	struct max77705_usbc_platform_data *usbpd_data = g_usbc_data;
	uint32_t state_cnt = usbpd_data->factory_mode.FAC_Abnormal_Repeat_State;
	uint32_t rid_cnt = usbpd_data->factory_mode.FAC_Abnormal_Repeat_RID;
	uint32_t rid0_cnt = usbpd_data->factory_mode.FAC_Abnormal_RID0;

	switch (type) {
	case FAC_ABNORMAL_REPEAT_RID0:
		if (!rid0_cnt) {
			msg_maxim("Notify the abnormal state [RID0] [%d]!!", rid0_cnt);
			usbpd_data->factory_mode.FAC_Abnormal_RID0++;
			max77705_ccic_event_work(usbpd_data,
				CCIC_NOTIFY_DEV_CCIC, CCIC_NOTIFY_ID_FAC, 1 << 2, 0, 0);
		} else {
			usbpd_data->factory_mode.FAC_Abnormal_RID0 = 0;
		}
	break;
	case FAC_ABNORMAL_REPEAT_RID:
		if (!rid_cnt) {
			schedule_delayed_work(&usbpd_data->factory_rid_work, msecs_to_jiffies(1000));
			msg_maxim("start the factory_rid_work");
		}
		usbpd_data->factory_mode.FAC_Abnormal_Repeat_RID++;
	break;
	case FAC_ABNORMAL_REPEAT_STATE:
		if (!state_cnt) {
			schedule_delayed_work(&usbpd_data->factory_state_work, msecs_to_jiffies(1000));
			msg_maxim("start the factory_state_work");
		}
		usbpd_data->factory_mode.FAC_Abnormal_Repeat_State++;
	break;
	default:
		msg_maxim("Never Calling [%d]", type);
	break;
	}
}
#endif

static int max77705_usbc_probe(struct platform_device *pdev)
{
	struct max77705_dev *max77705 = dev_get_drvdata(pdev->dev.parent);
	struct max77705_platform_data *pdata = dev_get_platdata(max77705->dev);
	struct max77705_usbc_platform_data *usbc_data = NULL;
	int ret;
#if defined(CONFIG_CCIC_NOTIFIER)
	pccic_data_t pccic_data;
	pccic_sysfs_property_t pccic_sysfs_prop;
#endif
#if defined(CONFIG_DUAL_ROLE_USB_INTF)
	struct dual_role_phy_desc *desc;
	struct dual_role_phy_instance *dual_role;
#endif
#if defined(CONFIG_USB_HOST_NOTIFY)
	struct otg_notify *o_notify = get_otg_notify();
#endif

	msg_maxim("Probing : %d", max77705->irq);
	usbc_data =  kzalloc(sizeof(struct max77705_usbc_platform_data), GFP_KERNEL);
	if (!usbc_data)
		return -ENOMEM;

	g_usbc_data = usbc_data;
	usbc_data->dev = &pdev->dev;
	usbc_data->max77705 = max77705;
	usbc_data->muic = max77705->muic;
	usbc_data->charger = max77705->charger;
	usbc_data->i2c = max77705->i2c;
	usbc_data->max77705_data = pdata;
	usbc_data->irq_base = pdata->irq_base;

	usbc_data->pd_data = kzalloc(sizeof(struct max77705_pd_data), GFP_KERNEL);
	if (!usbc_data->pd_data)
		return -ENOMEM;

	usbc_data->cc_data = kzalloc(sizeof(struct max77705_cc_data), GFP_KERNEL);
	if (!usbc_data->cc_data)
		return -ENOMEM;

	platform_set_drvdata(pdev, usbc_data);

#if defined(CONFIG_CCIC_MAX77705_DEBUG)
	mxim_debug_init();
	mxim_debug_set_i2c_client(usbc_data->muic);
#endif

	ret = sysfs_create_group(&max77705->dev->kobj, &max77705_attr_grp);
	msg_maxim("created sysfs. ret=%d", ret);

	usbc_data->HW_Revision = 0x0;
	usbc_data->FW_Revision = 0x0;
	usbc_data->plug_attach_done = 0x0;
	usbc_data->cc_data->current_pr = 0xFF;
	usbc_data->pd_data->current_dr = 0xFF;
	usbc_data->cc_data->current_vcon = 0xFF;
	usbc_data->op_code_done = 0x0;
	usbc_data->current_connstat = 0xFF;
	usbc_data->prev_connstat = 0xFF;
	usbc_data->usbc_cmd_queue.front = NULL;
	usbc_data->usbc_cmd_queue.rear = NULL;
	mutex_init(&usbc_data->op_lock);
#if defined(CONFIG_CCIC_NOTIFIER)
	pccic_data = devm_kzalloc(usbc_data->dev, sizeof(ccic_data_t), GFP_KERNEL);
	pccic_sysfs_prop = devm_kzalloc(usbc_data->dev, sizeof(ccic_sysfs_property_t), GFP_KERNEL);
	pccic_sysfs_prop->get_property = max77705_sysfs_get_local_prop;
	pccic_sysfs_prop->set_property = max77705_sysfs_set_prop;
	pccic_sysfs_prop->property_is_writeable = max77705_sysfs_is_writeable;
	pccic_sysfs_prop->properties = max77705_sysfs_properties;
	pccic_sysfs_prop->num_properties = ARRAY_SIZE(max77705_sysfs_properties);
	pccic_data->ccic_syfs_prop = pccic_sysfs_prop;
	pccic_data->drv_data = usbc_data;
	pccic_data->name = "max77705";
	pccic_data->set_enable_alternate_mode = max77705_set_enable_alternate_mode;
	ccic_core_register_chip(pccic_data);
	usbc_data->vconn_en = 1;
	usbc_data->cur_rid = RID_OPEN;
	usbc_data->cc_pin_status = NO_DETERMINATION;
	usbc_data->power_role = DUAL_ROLE_PROP_PR_NONE;
	usbc_data->auto_vbus_en = false;
#if defined(CONFIG_USB_HOST_NOTIFY)
	send_otg_notify(o_notify, NOTIFY_EVENT_POWER_SOURCE, 0);
#endif

#endif
	init_completion(&usbc_data->op_completion);
	usbc_data->op_wait_queue = create_singlethread_workqueue("op_wait");
	if (usbc_data->op_wait_queue == NULL)
		return -ENOMEM;
	usbc_data->op_send_queue = create_singlethread_workqueue("op_send");
	if (usbc_data->op_send_queue == NULL)
		return -ENOMEM;

	INIT_WORK(&usbc_data->op_send_work, max77705_uic_op_send_work_func);

#if defined(CONFIG_CCIC_NOTIFIER)
	/* Create a work queue for the ccic irq thread */
	usbc_data->ccic_wq
		= create_singlethread_workqueue("ccic_irq_event");
	if (!usbc_data->ccic_wq) {
		pr_err("%s failed to create work queue\n", __func__);
		return -ENOMEM;
	}
#endif
#if defined(CONFIG_SEC_FACTORY)
	INIT_DELAYED_WORK(&usbc_data->factory_state_work,
				factory_check_abnormal_state);
	INIT_DELAYED_WORK(&usbc_data->factory_rid_work,
				factory_check_normal_rid);
#endif
	max77705_get_version_info(usbc_data);
	max77705_init_irq_handler(usbc_data);
	max77705_muic_probe(usbc_data);
	max77705_cc_init(usbc_data);
	max77705_pd_init(usbc_data);
	max77705_write_reg(usbc_data->muic, REG_PD_INT_M, 0x1C);
	max77705_write_reg(usbc_data->muic, REG_VDM_INT_M, 0xFF);
	usbc_data->is_first_booting = 1;
	max77705_usbc_disable_auto_vbus(usbc_data);
	INIT_DELAYED_WORK(&usbc_data->vbus_hard_reset_work,
				vbus_control_hard_reset);
	/* turn on the VBUS automatically. */
	// max77705_usbc_enable_auto_vbus(usbc_data);
#if defined(CONFIG_DUAL_ROLE_USB_INTF)
	desc =
		devm_kzalloc(usbc_data->dev,
				 sizeof(struct dual_role_phy_desc), GFP_KERNEL);
	if (!desc) {
		pr_err("unable to allocate dual role descriptor\n");
		return -ENOMEM;
	}

	desc->name = "otg_default";
	desc->supported_modes = DUAL_ROLE_SUPPORTED_MODES_DFP_AND_UFP;
	desc->get_property = max77705_dual_role_get_local_prop;
	desc->set_property = max77705_dual_role_set_prop;
	desc->properties = fusb_drp_properties;
	desc->num_properties = ARRAY_SIZE(fusb_drp_properties);
	desc->property_is_writeable = max77705_dual_role_is_writeable;
	dual_role =
		devm_dual_role_instance_register(usbc_data->dev, desc);
	dual_role->drv_data = usbc_data;
	usbc_data->dual_role = dual_role;
	usbc_data->desc = desc;
	init_completion(&usbc_data->reverse_completion);
	INIT_DELAYED_WORK(&usbc_data->role_swap_work, max77705_role_swap_check);
#endif
	INIT_DELAYED_WORK(&usbc_data->acc_detach_work, max77705_acc_detach_check);
	ccic_register_switch_device(1);
	INIT_DELAYED_WORK(&usbc_data->usb_external_notifier_register_work,
				  delayed_external_notifier_init);

	init_completion(&usbc_data->uvdm_longpacket_out_wait);
	init_completion(&usbc_data->uvdm_longpacket_in_wait);
#if defined(CONFIG_CCIC_NOTIFIER)
	ccic_misc_init(pccic_data);
	pccic_data->misc_dev->uvdm_read = max77705_sec_uvdm_in_request_message;
	pccic_data->misc_dev->uvdm_write = max77705_sec_uvdm_out_request_message;
	pccic_data->misc_dev->uvdm_ready = max77705_sec_uvdm_ready;
	pccic_data->misc_dev->uvdm_close = max77705_sec_uvdm_close;
#endif
	/* Register ccic handler to ccic notifier block list */
	ret = usb_external_notify_register(&usbc_data->usb_external_notifier_nb,
		pdic_handle_usb_external_notifier_notification, EXTERNAL_NOTIFY_DEV_PDIC);
	if (ret < 0)
		schedule_delayed_work(&usbc_data->usb_external_notifier_register_work, msecs_to_jiffies(2000));
	else
		pr_info("%s : external notifier register done!\n", __func__);

	max77705->cc_booting_complete = 1;
	max77705_usbc_umask_irq(usbc_data);
	init_waitqueue_head(&usbc_data->host_turn_on_wait_q);
	max77705_set_host_turn_on_event(0);
	usbc_data->host_turn_on_wait_time = 3;


	msg_maxim("probing Complete..");
	return 0;
}

static int max77705_usbc_remove(struct platform_device *pdev)
{
	struct max77705_usbc_platform_data *usbc_data =
		platform_get_drvdata(pdev);

	mutex_destroy(&usbc_data->op_lock);
#if defined(CONFIG_DUAL_ROLE_USB_INTF)
	devm_dual_role_instance_unregister(usbc_data->dev, usbc_data->dual_role);
	devm_kfree(usbc_data->dev, usbc_data->desc);
#endif
	max77705_muic_remove(usbc_data);

	wake_lock_destroy(&usbc_data->apcmd_wake_lock);
	wake_lock_destroy(&usbc_data->sysmsg_wake_lock);
	wake_lock_destroy(&usbc_data->pd_data->pdmsg_wake_lock);
	wake_lock_destroy(&usbc_data->pd_data->datarole_wake_lock);
	wake_lock_destroy(&usbc_data->pd_data->ssacc_wake_lock);
	wake_lock_destroy(&usbc_data->pd_data->fct_id_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->vconncop_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->vsafe0v_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->detabrt_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->vconnsc_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->ccpinstat_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->ccistat_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->ccvcnstat_wake_lock);
	wake_lock_destroy(&usbc_data->cc_data->ccstat_wake_lock);

	free_irq(usbc_data->irq_apcmd, usbc_data);
	free_irq(usbc_data->irq_sysmsg, usbc_data);
	free_irq(usbc_data->irq_vdm0, usbc_data);
	free_irq(usbc_data->irq_vdm1, usbc_data);
	free_irq(usbc_data->irq_vdm2, usbc_data);
	free_irq(usbc_data->irq_vdm3, usbc_data);
	free_irq(usbc_data->irq_vdm4, usbc_data);
	free_irq(usbc_data->irq_vdm5, usbc_data);
	free_irq(usbc_data->irq_vdm6, usbc_data);
	free_irq(usbc_data->irq_vdm7, usbc_data);
	free_irq(usbc_data->pd_data->irq_pdmsg, usbc_data);
	free_irq(usbc_data->pd_data->irq_datarole, usbc_data);
	free_irq(usbc_data->pd_data->irq_ssacc, usbc_data);
	free_irq(usbc_data->pd_data->irq_fct_id, usbc_data);
	free_irq(usbc_data->cc_data->irq_vconncop, usbc_data);
	free_irq(usbc_data->cc_data->irq_vsafe0v, usbc_data);
	free_irq(usbc_data->cc_data->irq_detabrt, usbc_data);
	free_irq(usbc_data->cc_data->irq_vconnsc, usbc_data);
	free_irq(usbc_data->cc_data->irq_ccpinstat, usbc_data);
	free_irq(usbc_data->cc_data->irq_ccistat, usbc_data);
	free_irq(usbc_data->cc_data->irq_ccvcnstat, usbc_data);
	free_irq(usbc_data->cc_data->irq_ccstat, usbc_data);

	kfree(usbc_data->cc_data);
	kfree(usbc_data->pd_data);
	kfree(usbc_data);
	return 0;
}

#if defined CONFIG_PM
static int max77705_usbc_suspend(struct device *dev)
{
	struct max77705_usbc_platform_data *usbc_data =
		dev_get_drvdata(dev);

	max77705_muic_suspend(usbc_data);

	return 0;
}

static int max77705_usbc_resume(struct device *dev)
{
	struct max77705_usbc_platform_data *usbc_data =
		dev_get_drvdata(dev);

	max77705_muic_resume(usbc_data);

	return 0;
}
#else
#define max77705_usbc_suspend NULL
#define max77705_usbc_resume NULL
#endif

static void max77705_usbc_shutdown(struct platform_device *pdev)
{
	struct max77705_usbc_platform_data *usbc_data =
		platform_get_drvdata(pdev);
	struct device_node *np;
	int gpio_dp_sw_oe;
	u8 uic_int = 0;

	msg_maxim("max77705 usbc driver shutdown++++");
	if (!usbc_data->muic) {
		msg_maxim("no max77705 i2c client");
		return;
	}
	usbc_data->shut_down = 1;
	max77705_muic_shutdown(usbc_data);
	max77705_usbc_mask_irq(usbc_data);
	/* unmask */
	max77705_write_reg(usbc_data->muic, REG_PD_INT_M, 0xFF);
	max77705_write_reg(usbc_data->muic, REG_CC_INT_M, 0xFF);
	max77705_write_reg(usbc_data->muic, REG_UIC_INT_M, 0xFF);
	max77705_write_reg(usbc_data->muic, REG_VDM_INT_M, 0xFF);

	/* send the reset command */
	if (usbc_data->current_connstat == WATER)
		msg_maxim("no call the reset function(WATER STATE)");
	else if (usbc_data->cur_rid != RID_OPEN && usbc_data->cur_rid != RID_UNDEFINED)
		msg_maxim("no call the reset function(RID)");
	else {
		max77705_reset_ic(usbc_data);
		if (usbc_data->dp_is_connect) {
			pr_info("aux_sw_oe pin set to high\n");
			np = of_find_node_by_name(NULL, "displayport");
			gpio_dp_sw_oe = of_get_named_gpio(np, "dp,aux_sw_oe", 0);
			gpio_direction_output(gpio_dp_sw_oe, 1);
		}
		max77705_write_reg(usbc_data->muic, REG_PD_INT_M, 0xFF);
		max77705_write_reg(usbc_data->muic, REG_CC_INT_M, 0xFF);
		max77705_write_reg(usbc_data->muic, REG_UIC_INT_M, 0xFF);
		max77705_write_reg(usbc_data->muic, REG_VDM_INT_M, 0xFF);
		max77705_read_reg(usbc_data->muic,
				MAX77705_USBC_REG_UIC_INT, &uic_int);
	}
	msg_maxim("max77705 usbc driver shutdown----");
}

static SIMPLE_DEV_PM_OPS(max77705_usbc_pm_ops, max77705_usbc_suspend,
			 max77705_usbc_resume);

static struct platform_driver max77705_usbc_driver = {
	.driver = {
		.name = "max77705-usbc",
		.owner = THIS_MODULE,
#ifdef CONFIG_PM
		.pm = &max77705_usbc_pm_ops,
#endif
	},
	.shutdown = max77705_usbc_shutdown,
	.probe = max77705_usbc_probe,
	.remove = max77705_usbc_remove,
};

static int __init max77705_usbc_init(void)
{
	msg_maxim("init");
	return platform_driver_register(&max77705_usbc_driver);
}
module_init(max77705_usbc_init);

static void __exit max77705_usbc_exit(void)
{
	platform_driver_unregister(&max77705_usbc_driver);
}
module_exit(max77705_usbc_exit);

MODULE_DESCRIPTION("max77705 USBPD driver");
MODULE_LICENSE("GPL");
