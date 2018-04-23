#ifndef HISI_USB_INTERFACE_H
#define HISI_USB_INTERFACE_H
enum phy_change_type {
	PHY_MODE_CHANGE_BEGIN,
	PHY_MODE_CHANGE_END,
};

enum misc_ctrl_type {
	MICS_CTRL_USB= 0,
	MICS_CTRL_COMBOPHY = 1,
};

/* defined USB31_TCA data sheet */
enum combophy_mode {
	NOT_CONNECT = 0,
	USBONLY_MODE = 1,
	DPONLY_MODE = 2,
	USB_DP_MODE = 3,
	BAD_MODE
};

int hisi_usb_combophy_notify(enum phy_change_type type);
int dwc3_misc_ctrl_get(enum misc_ctrl_type type);
void dwc3_misc_ctrl_put(enum misc_ctrl_type type);
volatile unsigned int hisi_dwc3_usbcore_read(u32 offset);
void hisi_dwc3_usbcore_write(u32 offset, u32 value);
u16 usb31phy_cr_read(u32 addr);
int usb31phy_cr_write(u32 addr, u16 value);
#endif /* hisi_usb_interface.h */
