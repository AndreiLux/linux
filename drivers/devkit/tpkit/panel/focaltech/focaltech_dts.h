

#ifndef __FOCALTECH_DTS_H__
#define __FOCALTECH_DTS_H__

#include <linux/i2c.h>


#define FTS_VENDOR_COMP_NAME_LEN	32
#define FTS_THRESHOLD_IN_CSV_FILE	1

#define FTS_CHIP_NAME		"fts"
#define HUAWEI_TS_KIT		"huawei,ts_kit"

#define FTS_IRQ_GPIO		"attn_gpio"
#define FTS_RST_GPIO		"reset_gpio"
#define FTS_VDDIO_GPIO_CTRL	"vddio_ctrl_gpio"
#define FTS_VCI_GPIO_CTRL	"vci_ctrl_gpio"
#define FTS_IRQ_CFG		"irq_config"
#define FTS_ALGO_ID		"algo_id"
#define FTS_VDD			"fts-vdd"
#define FTS_VBUS		"fts-io"
#define FTS_IC_TYPES		"ic_type"
#define FTS_IS_IN_CELL		"is_in_cell"
#define FTS_WD_CHECK		"need_wd_check_status"
#define FTS_CHECK_STATUS_WATCHDOG_TIMEOUT		"check_status_watchdog_timeout"
#define FTS_PRAM_PROJECTID_ADDR		"pram_projectid_addr"
#define FTS_X_MAX		"x_max"
#define FTS_Y_MAX		"y_max"
#define FTS_X_MAX_MT		"x_max_mt"
#define FTS_Y_MAX_MT		"y_max_mt"
#define FTS_VCI_GPIO_TYPE		"vci_gpio_type"
#define FTS_VCI_REGULATOR_TYPE		"vci_regulator_type"
#define FTS_VDDIO_GPIO_TYPE		"vddio_gpio_type"
#define FTS_VDDIO_REGULATOR_TYPE	"vddio_regulator_type"
#define FTS_COVER_FORCE_GLOVE		"force_glove_in_smart_cover"
#define FTS_TEST_TYPE			"tp_test_type"
#define FTS_HOLSTER_SUPPORTED			"holster_supported"
#define FTS_HOSTLER_SWITCH_ADDR		"holster_switch_addr"
#define FTS_HOSTLER_SWITCH_BIT		"holster_switch_bit"
#define FTS_GLOVE_SUPPORTED			"glove_supported"
#define FTS_GLOVE_SWITCH_ADDR		"glove_switch_addr"
#define FTS_GLOVE_SWITCH_BIT		"glove_switch_bit"
#define FTS_ROI_SUPPORTED			"roi_supported"
#define FTS_ROI_SWITCH_ADDR		"roi_switch_addr"

#define FTS_VCI_LDO_VALUE		"vci_value"
#define FTS_VDDIO_LDO_VALUE		"vddio_value"
#define FTS_POWER_SELF_CTRL		"power_self_ctrl"
#define FTS_NEED_SET_VDDIO_VALUE	"need_set_vddio_value"

#define FTS_FW_UPDATE_LOGIC		"fw_update_logic"

#define FTS_HARD_RESET_DELAY		"hard_reset_delay"
#define FTS_ERASE_MIN_DELAY		"erase_min_delay"
#define FTS_CALC_CRC_DELAY		"calc_crc_delay"
#define FTS_REBOOT_DELAY		"reboot_delay"
#define FTS_ERASE_QUERY_DELAY		"erase_query_delay"

#define FTS_WRITE_FLASH_QUERY_TIMES	"write_flash_query_times"
#define FTS_READ_ECC_QUERY_TIMES	"read_ecc_query_times"
#define FTS_ERASE_FLASH_QUERY_TIMES	"erase_flash_query_times"
#define FTS_UPGRADE_LOOP_TIMES		"upgrade_loop_times"
#define FTS_SLAVE_ADDR			"slave_address"

#define FTS_ROW_COLUMN_DELTA_TEST	"row_column_delta_test"
#define FTS_LCD_NOISE_DATA_TEST		"lcd_noise_data_test"

#define FTS_IN_CSV_FILE			"threshold,in_csv_file"
#define DTS_RAW_DATA_MIN		"threshold,raw_data_min"
#define DTS_RAW_DATA_MAX		"threshold,raw_data_max"
#define DTS_CB_TEST_MIN			"threshold,cb_test_min"
#define DTS_CB_TEST_MAX			"threshold,cb_test_max"

#define DTS_SHORT_CIRCUIT_RES_MIN	"threshold,short_circuit_min"
#define DTS_LCD_NOISE_MAX	"threshold,lcd_noise_max"
#define DTS_ROW_COLUMN_DELTA_TEST	"threshold,row_column_delta_max"
#define DTS_OPEN_TEST_CB_MIN		"threshold,open_test_cb_min"
#define DTS_SCAP_RAW_DATA_MIN         "threshold,scap_raw_data_min"
#define DTS_SCAP_RAW_DATA_MAX         "threshold,scap_raw_data_max"
#define DTS_RAW_DATA_MIN_ARRAY		"threshold,raw_data_min_array"
#define DTS_RAW_DATA_MAX_ARRAY		"threshold,raw_data_max_array"

#define FTS_CB_TEST_CSV			"cb_test_threshold"
#define FTS_SCAP_RAW_FATA_CSV		"scap_raw_data_threshold"

#define FTS_TEST_TYPE_DEFAULT	"Normalize_type:judge_last_result"

#define FTS_GESTURE_SUPPORTED			"gesture_supported"
#define FTS_POINT_BY_POINT_JUDGE	"threshold,point_by_point_judge"
#define FTS_OPEN_ONCE_THRESHOLD		"only_open_once_captest_threshold"

int focal_get_vendor_name_from_dts(const char *project_id,
	char *vendor_name, size_t size);

int focal_parse_dts(struct device_node *np,
	struct focal_platform_data *focal_pdata);

int focal_prase_ic_config_dts(struct device_node *np,
	struct ts_kit_device_data *dev_data);

#endif
