/*
 * include/linux/knox_rp.h
 *
 * Include file for the KNOX RP mode.
 */

#ifndef _LINUX_KNOX_KAP_H
#define _LINUX_KNOX_KAP_H

#include <linux/types.h>

/* For QC */

#define SEC_BOOT_CMD101_GET_DMV_DATA 101
#define QSEECOM_SBUFF_SIZE 0x1000

#ifndef _TZ_ICCC_COMDEF_H_
#define MAX_IMAGES  4
#define RESERVED_BYTES  96

struct secure_param_header {
	uint16_t magic_str;
	uint16_t used_size;
};

struct iccc_bl_status_t {
	struct secure_param_header header;
	uint32_t rp_ver;
	uint32_t kernel_rp;
	uint32_t system_rp;
	uint32_t test_bit;
	uint32_t sec_boot;
	uint32_t react_lock;
	uint32_t kiwi_lock;
	uint32_t cc_mode;
	uint32_t mdm_mode;
	uint32_t sysscope_flag;
	uint32_t trustboot_flag;
	uint32_t afw_value;
	uint8_t image_status[MAX_IMAGES];
	uint8_t reserved[RESERVED_BYTES];
} __packed;
#endif

struct dmv_bl_status_t {
	struct secure_param_header header;
	uint32_t odin_flag;
	uint32_t boot_mode;
	uint32_t security_mode;
	uint32_t mode_num;
} __packed;

struct dmv_req_t {
	uint32_t cmd_id;
	union {
		struct iccc_bl_status_t iccc_bl_status;
		struct dmv_bl_status_t dmv_bl_status;
	} data;
};

struct dmv_rsp_t {
	/** First 4 bytes should always be command id */
	uint32_t data;
	uint32_t status;
};
/* For QC - end */

extern const struct file_operations knox_kap_fops;

extern unsigned int knox_kap_status;

#endif /* _LINUX_KNOX_KAP_H */
