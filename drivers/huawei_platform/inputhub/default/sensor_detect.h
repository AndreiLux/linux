/*
 * Copyright (C) huawei company
 *
 * This	program	is free	software; you can redistribute it and/or modify
 * it under	the	terms of the GNU General Public	License	version	2 as
 * published by	the	Free Software Foundation.

  * Filename:  sensor_detect.h
 *
 * Discription: some functions of sensorhub power
 *
 * Owner:DIVS_SENSORHUB
 */
#ifndef __SENSOR_DETECT_H
#define __SENSOR_DETECT_H

#include "protocol.h"

#define MAX_CHIP_INFO_LEN (50)
#define PDC_SIZE		27
#define MAX_STR_SIZE	1024
#define MAX_PHONE_COLOR_NUM  15
#define CYPRESS_CHIPS		2
#define SENSOR_PLATFORM_EXTEND_DATA_SIZE    50

typedef uint16_t GPIO_NUM_TYPE;

typedef enum {
	ACC,
	MAG,
	GYRO,
	ALS,
	PS,
	AIRPRESS,
	HANDPRESS,
	CAP_PROX,
	GPS_4774_I2C,
	FINGERPRINT,
	KEY,
	MAGN_BRACKET,
	RPC,
	SENSOR_MAX
}SENSOR_DETECT_LIST;

typedef enum{
	BOOT_DETECT,
	REDETECT_LATER
}DETECT_MODE;

struct sleeve_detect_pare
{
	unsigned int tp_color;
	unsigned int sleeve_detect_threshhold;
};

struct sensor_combo_cfg {
	uint8_t bus_type;
	uint8_t bus_num;
	uint8_t disable_sample_thread;
	union {
		uint32_t data;
		uint32_t i2c_address;
		union SPI_CTRL ctrl;
	};
}__packed;

struct g_sensor_platform_data {
	struct sensor_combo_cfg cfg;
	uint8_t axis_map_x;
	uint8_t axis_map_y;
	uint8_t axis_map_z;
	uint8_t negate_x;
	uint8_t negate_y;
	uint8_t negate_z;
	GPIO_NUM_TYPE gpio_int1;
	GPIO_NUM_TYPE gpio_int2;
	GPIO_NUM_TYPE gpio_int2_sh;
	uint16_t poll_interval;
	int offset_x;
	int offset_y;
	int offset_z;
	int sensitivity_x;
	int sensitivity_y;
	int sensitivity_z;
	uint8_t calibrate_style;
	uint8_t g_sensor_extend_data[SENSOR_PLATFORM_EXTEND_DATA_SIZE];
};

struct gyro_platform_data {
	struct sensor_combo_cfg cfg;
	uint8_t exist;
	uint8_t position;
	uint8_t axis_map_x;
	uint8_t axis_map_y;
	uint8_t axis_map_z;
	uint8_t negate_x;
	uint8_t negate_y;
	uint8_t negate_z;
	GPIO_NUM_TYPE gpio_int1;
	GPIO_NUM_TYPE gpio_int2;
	GPIO_NUM_TYPE gpio_int2_sh;
	uint16_t poll_interval;
	uint8_t fac_fix_offset_Y;
	uint8_t gyro_extend_data[SENSOR_PLATFORM_EXTEND_DATA_SIZE];
};

struct compass_platform_data {
	struct sensor_combo_cfg cfg;
	uint8_t axis_map_x;
	uint8_t axis_map_y;
	uint8_t axis_map_z;
	uint8_t negate_x;
	uint8_t negate_y;
	uint8_t negate_z;
	uint8_t outbit;
	uint8_t calibrate_method;
	GPIO_NUM_TYPE gpio_drdy;
	GPIO_NUM_TYPE gpio_rst;
	uint8_t soft_filter;
	uint8_t charger_trigger;
	uint8_t pdc_data[PDC_SIZE];
	uint16_t poll_interval;
	uint8_t compass_extend_data[SENSOR_PLATFORM_EXTEND_DATA_SIZE];
};

struct als_platform_data {
	struct sensor_combo_cfg cfg;
	GPIO_NUM_TYPE gpio_int1;
	uint8_t atime;
	uint8_t again;
	uint16_t poll_interval;
	uint16_t init_time;
	int threshold_value;
	int GA1;
	int GA2;
	int GA3;
	int COE_B;
	int COE_C;
	int COE_D;
	uint8_t als_phone_type;
	uint8_t als_phone_version;
	uint8_t als_phone_tp_colour;
	uint8_t als_extend_data[SENSOR_PLATFORM_EXTEND_DATA_SIZE];
};

struct ps_platform_data {
	struct sensor_combo_cfg cfg;
	uint8_t ps_pulse_count;
	GPIO_NUM_TYPE gpio_int1;
	uint8_t persistent;
	uint8_t ptime;
	uint8_t p_on;		/*need to close oscillator*/
	uint8_t ps_oily_threshold;
	uint16_t poll_interval;
	uint16_t init_time;
	uint16_t use_oily_judge;
	int min_proximity_value;
	int pwindows_value;
	int pwave_value;
	int threshold_value;
	int rdata_under_sun;
	uint16_t ps_calib_20cm_threshold;
	uint16_t ps_calib_5cm_threshold;
	uint16_t ps_calib_3cm_threshold;
	uint8_t wtime;
	uint8_t pulse_len;
	uint8_t pgain;
	uint8_t led_current;
	uint8_t prox_avg;/* ps  Filtrate control*/
	uint8_t offset_max;
	uint8_t offset_min;
	uint16_t oily_max_near_pdata;
	uint16_t max_oily_add_pdata;
	uint8_t max_near_pdata_loop;
	uint8_t oily_count_size;
	uint8_t ps_extend_data[SENSOR_PLATFORM_EXTEND_DATA_SIZE];
};

struct airpress_platform_data {
	struct sensor_combo_cfg cfg;
	int offset;
	uint16_t poll_interval;
	uint8_t airpress_extend_data[SENSOR_PLATFORM_EXTEND_DATA_SIZE];
};

struct handpress_platform_data {
	struct sensor_combo_cfg cfg;
	uint8_t bootloader_type;
	uint8_t id[CYPRESS_CHIPS];
	uint8_t i2c_address[CYPRESS_CHIPS];
	uint8_t t_pionts[CYPRESS_CHIPS];
	uint16_t poll_interval;
	uint32_t irq[CYPRESS_CHIPS];
	uint8_t handpress_extend_data[SENSOR_PLATFORM_EXTEND_DATA_SIZE];
};

/*
* sar platform data
*/

#define	STG_SUPPORTED_NUM	(3)
#define	TO_MODEM_SUPPORTED_LEVEL_NUM	(8)
#define	DEFAULT_THRESHOLD	(0xC8)
#define	ADUX_REGS_NEED_INITIATED_NUM	(16)
#define	SEMTECH_REGS_NEED_INITIATED_NUM	(12)



struct adux_sar_data {
	uint32_t init_reg_val[17];
	uint16_t high_threshold;
	uint16_t low_threshold;
	uint16_t swap_flag[3];
	uint16_t cal_fact_base[3];
	uint16_t cal_offset[3];
	uint16_t digi_offset[3];
	uint16_t cap_prox_extend_data[2];//3
};

struct adux_sar_add_data_t{
	uint16_t threshold_to_ap_stg[STG_SUPPORTED_NUM];
	uint16_t threshold_to_modem_stg[STG_SUPPORTED_NUM*TO_MODEM_SUPPORTED_LEVEL_NUM];
};

struct cypress_sar_data {
	uint16_t threshold_to_ap;
	uint16_t threshold_to_modem[8];
};

struct semteck_sar_data {
	uint16_t threshold_to_ap;
	uint16_t threshold_to_modem[8];
	uint32_t init_reg_val[17];
	uint8_t ph;
	uint16_t calibrate_thred[4];
};
union sar_data {
	struct cypress_sar_data cypress_data;
	struct adux_sar_data	adux_data;
	struct semteck_sar_data	semteck_data;
	//add the others here
};

/*
*
* calibrate_type: config by bit(0~7): 0-free 1-near 2-far other-reserve;
* sar_datas: data for diffrent devices.
*/
struct sar_platform_data {
	struct sensor_combo_cfg cfg;
	GPIO_NUM_TYPE gpio_int;
	uint16_t poll_interval;
	int  calibrate_type;
	union sar_data	sar_datas;
};

struct sar_sensor_detect {
	struct sensor_combo_cfg cfg;
	uint8_t detect_flag;
	uint8_t chip_id;
	uint8_t chip_id_value[2];
};

struct cap_prox_platform_data {
	struct sensor_combo_cfg cfg;
	GPIO_NUM_TYPE gpio_int;
	uint16_t poll_interval;
	int  calibrate_type;
	uint32_t init_reg_val[17];	/* init value */
	uint16_t high_threshold;
	uint16_t low_threshold;
	uint16_t swap_flag[3];   //0x06
	uint16_t cal_fact_base[3];  //read:0x71  write:0x79
	uint16_t cal_offset[3];  //0x09
	uint16_t digi_offset[3]; //0x0a
	uint16_t cap_prox_extend_data[2];//3 //3mm and 8mm threshold
};

struct gps_4774_platform_data {
	struct sensor_combo_cfg cfg;
	uint16_t poll_interval;
	GPIO_NUM_TYPE gpio1_gps_cmd_ap;
	GPIO_NUM_TYPE gpio1_gps_cmd_sh;
	GPIO_NUM_TYPE gpio2_gps_ready_ap;
	GPIO_NUM_TYPE gpio2_gps_ready_sh;
	GPIO_NUM_TYPE gpio3_wakeup_gps_ap;
	GPIO_NUM_TYPE gpio3_wakeup_gps_sh;
};

struct fingerprint_platform_data {
	struct sensor_combo_cfg cfg;
	uint16_t reg;
	uint16_t chip_id;
	uint16_t product_id;
	GPIO_NUM_TYPE gpio_irq;
	GPIO_NUM_TYPE gpio_irq_sh;
	GPIO_NUM_TYPE gpio_cs;
	GPIO_NUM_TYPE gpio_reset;
	uint16_t poll_interval;
};

struct key_platform_data {
	struct sensor_combo_cfg cfg;
	uint8_t i2c_address_bootloader;
	GPIO_NUM_TYPE gpio_key_int;
	GPIO_NUM_TYPE gpio_key_int_sh;
	uint16_t poll_interval;
	uint8_t reserve[16];
};

struct magn_bracket_platform_data {
	struct sensor_combo_cfg cfg;
	int mag_x_change_lower;
	int mag_x_change_upper;
	int mag_y_change_lower;
	int mag_y_change_upper;
	int mag_z_change_lower;
	int mag_z_change_upper;
};
struct rpc_platform_data {
	uint16_t table[32];
	uint16_t mask[32];
	uint16_t default_value;
};
#define max_tx_rx_len 32
struct detect_word {
	struct sensor_combo_cfg cfg;
	uint32_t tx_len;
	uint8_t tx[max_tx_rx_len];
	uint32_t rx_len;
	uint8_t rx_msk[max_tx_rx_len];
	uint32_t exp_n;
	uint8_t rx_exp[max_tx_rx_len];
};

#define MAX_SENSOR_NAME_LENGTH 20
struct sensor_detect_manager{
	char sensor_name_str[MAX_SENSOR_NAME_LENGTH];
	SENSOR_DETECT_LIST sensor_id;
	uint8_t detect_result;
	int tag;
	const void *spara;
    	int cfg_data_length;
};

#define MAX_REDETECT_NUM 100
struct sensor_redetect_state{
	uint8_t need_redetect_sensor;
	uint8_t need_recovery;
	uint8_t detect_fail_num;
	uint8_t redetect_num;
};

struct sensorlist_info{
	/**
	 * Name of this sensor.
	 * All sensors of the same "type" must have a different "name".
	 */
	char     name[50];

	/** vendor of the hardware part */
	char     vendor[50];
	/**
	* version of the hardware part + driver. The value of this field
	* must increase when the driver is updated in a way that changes the
	* output of this sensor. This is important for fused sensors when the
	* fusion algorithm is updated.
	*/
	int32_t             version;

	/** maximum range of this sensor's value in SI units */
	int32_t           maxRange;

	/** smallest difference between two values reported by this sensor */
	int32_t           resolution;

	/** rough estimate of this sensor's power consumption in mA */
	int32_t           power;

	/**
	* this value depends on the reporting mode:
	*
	* continuous: minimum sample period allowed in microseconds
	* on-change : 0
	* one-shot  :-1
	* special   : 0, unless otherwise noted
	*/
	int32_t         minDelay;

	/**
	* number of events reserved for this sensor in the batch mode FIFO.
	* If there is a dedicated FIFO for this sensor, then this is the
	* size of this FIFO. If the FIFO is shared with other sensors,
	* this is the size reserved for that sensor and it can be zero.
	*/
	uint32_t        fifoReservedEventCount;

	/**
	* maximum number of events of this sensor that could be batched.
	* This is especially relevant when the FIFO is shared between
	* several sensors; this value is then set to the size of that FIFO.
	*/
	uint32_t        fifoMaxEventCount;
	/**
	* This value is defined only for continuous mode and on-change sensors. It is the delay between
	* two sensor events corresponding to the lowest frequency that this sensor supports. When lower
	* frequencies are requested through batch()/setDelay() the events will be generated at this
	* frequency instead. It can be used by the framework or applications to estimate when the batch
	* FIFO may be full.
	*
	* @note
	*   1) period_ns is in nanoseconds where as maxDelay/minDelay are in microseconds.
	*              continuous, on-change: maximum sampling period allowed in microseconds.
	*              one-shot, special : 0
	*   2) maxDelay should always fit within a 32 bit signed integer. It is declared as 64 bit
	*      on 64 bit architectures only for binary compatibility reasons.
	* Availability: SENSORS_DEVICE_API_VERSION_1_3
	*/
	int32_t maxDelay;

	/**
	* Flags for sensor. See SENSOR_FLAG_* above. Only the least significant 32 bits are used here.
	* It is declared as 64 bit on 64 bit architectures only for binary compatibility reasons.
	* Availability: SENSORS_DEVICE_API_VERSION_1_3
	*/
	uint32_t flags;
};

int init_sensors_cfg_data_from_dts(void);
extern SENSOR_DETECT_LIST get_id_by_sensor_tag(int tag);
extern int sensor_set_cfg_data(void);
extern int send_fileid_to_mcu(void);
void sensor_redetect_enter(void);
void sensor_redetect_init(void);
int sensor_set_fw_load(void);
#endif
