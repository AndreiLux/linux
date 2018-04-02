/*
 * Samsung EXYNOS FIMC-IS (Imaging Subsystem) driver
 *
 * Copyright (C) 2014 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef FIMC_IS_SENSOR_INTERFACE_H
#define FIMC_IS_SENSOR_INTERFACE_H

#include "fimc-is-core.h"
#include "fimc-is-mem.h"
#include "fimc-is-config.h"
#include "exynos-fimc-is-sensor.h"
#include "fimc-is-metadata.h"
#include "fimc-is-binary.h"
#include "fimc-is-device-sensor.h"
#include "fimc-is-interface-preprocessor.h"

#define SENSOR_INTERFACE_MAGIC 0xFEDCBA98

#define NUM_OF_DUMMY_FRAME		(3) /* N + 2 Frame delay */
#define NUM_FRAMES_DMA			(NUM_OF_DUMMY_FRAME + 2 + 1)
#define NEXT_NEXT_FRAME_DMA		(NUM_OF_DUMMY_FRAME + 2)

/* This OFFSET is set to 0 when enable 3AA 3FRAME DELAY, set to 1 in other cases */
#define OTF_VSYNC_3AA_FRAME_DELAY_OFFSET	0

#define NUM_FRAMES_OTF			(NEXT_NEXT_FRAME_OTF + 1)
#define NEXT_NEXT_FRAME_OTF		(2 + OTF_VSYNC_3AA_FRAME_DELAY_OFFSET)

#define NEXT_FRAME			1
#define CURRENT_FRAME			0

#define NUM_FRAMES			(NUM_OF_DUMMY_FRAME + 2 + 1)

#define EXPOSURE_GAIN_INDEX		0
#define LONG_EXPOSURE_GAIN_INDEX	1
#define SHORT_EXPOSURE_GAIN_INDEX	2
#define MAX_EXPOSURE_GAIN_PER_FRAME	3

#define ACTUATOR_MAX_SOFT_LANDING_NUM	32 /* Actuator interface */
#define ACTUATOR_MAX_FOCUS_POSITIONS	1024

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define FPS_TO_DURATION_US(x)  ((x == 0) ? (0) : ((1000 * 1000) / x))
#define DURATION_US_TO_FPS(x)  ((x == 0) ? (0) : ((1000 * 1000) / x))

enum DIFF_BET_SEN_ISP { /* Set to 0: 3AA 3frame delay, 1: 3AA 4frame delay, 3: M2M */
	DIFF_OTF_DELAY	= 0,
	DIFF_M2M_DELAY	= 3
};

enum SENSOR_CONTROL_DELAY {
	N_PLUS_TWO_FRAME = 0,
	N_PLUS_ONE_FRAME = 1,
};

/* DEVICE SENSOR INTERFACE */
#define SENSOR_REGISTER_FUNC_ADDR	(DDK_LIB_ADDR + 0x40)
#define SENSOR_REGISTER_FUNC_ADDR_RTA	(RTA_LIB_ADDR + 0x40)
#define PREPROC_REGISTER_FUNC_ADDR	(RTA_LIB_ADDR + 0xC0)

typedef int (*register_sensor_interface)(void *itf);

struct ae_param {
	union {
		u32 val;
		u32 long_val;
	};
	u32 short_val;
};

typedef struct {
	/* Normal parameter */
	/* Each CB has a specific parameter type.(Ex. SetSize Cb has size info param.) */
	void *param;
	unsigned int return_value;
} cis_setting_info;

typedef struct {
	unsigned long long exposure;
	unsigned int analog_gain;
	unsigned int digital_gain;
	unsigned long long long_exposure;
	unsigned int long_analog_gain;
	unsigned int long_digital_gain;
	unsigned long long short_exposure;
	unsigned int short_analog_gain;
	unsigned int short_digital_gain;
} ae_setting;

typedef struct {
	unsigned int long_exposure_coarse;
	unsigned int long_exposure_fine;
	unsigned int long_exposure_analog_gain;
	unsigned int long_exposure_digital_gain;
	unsigned int long_exposure_companion_digital_gain;
	unsigned int short_exposure_coarse;
	unsigned int short_exposure_fine;
	unsigned int short_exposure_analog_gain;
	unsigned int short_exposure_digital_gain;
	unsigned int short_exposure_companion_digital_gain;
} preprocessor_ae_setting;


typedef struct {
	bool stream_on;

	unsigned int config_idx;
	bool bypass;
	bool paf_stat_enable;
	bool caf_stat_enable;
	bool wdr_enable;
	bool enable_lsc;
	bool enable_drc;
	bool enable_pdaf_bpc;
	bool enable_xtalk;

	enum camera2_wdr_mode wdr_mode;
	enum camera2_wdr_mode pre_wdr_mode;
	enum camera2_disparity_mode disparity_mode;
	enum camera2_paf_mode paf_mode;
} is_shared_data;

typedef struct {
	bool ois_available;
	unsigned int mode;
	unsigned int factory_step;
} ois_shared_data;

typedef struct {
	/** The length of a frame is specified as a number of lines, frame_length_lines.
	  @remarks
	  'frame length lines' is changed in dynamic AE mode.
	 */
	unsigned int frame_length_lines;
	unsigned int frame_time; // unit: ms

	/** The length of a line is specified as a number of pixel clocks, line_length_pck. */
	unsigned int line_length_pck;
	unsigned int line_readOut_time; // unit: ns
	unsigned long long rolling_shutter_skew;

	/** Video Timing Pixel Clock, vt_pix_clk_freq. */
	unsigned int pclk;
	unsigned int min_frame_us_time;
#ifdef CAMERA_REAR2
	unsigned int min_sync_frame_us_time;
#endif

	/** Frame valid time */
	unsigned int frame_valid_us_time;

	unsigned int min_coarse_integration_time;
	unsigned int max_coarse_integration_time;
	unsigned int min_fine_integration_time;
	unsigned int max_fine_integration_time;
	unsigned int max_margin_coarse_integration_time;
	unsigned int max_margin_fine_integration_time;
	unsigned int min_analog_gain[2]; // 0: code, 1: times
	unsigned int max_analog_gain[2];
	unsigned int min_digital_gain[2];
	unsigned int max_digital_gain[2];
	unsigned int cur_coarse_integration_time_step;

	unsigned int cur_frame_us_time;
	unsigned int cur_width;
	unsigned int cur_height;
	unsigned int pre_width;
	unsigned int pre_height;

	/** Current analogue_gain_code_global */
	unsigned int cur_exposure_coarse;
	unsigned int cur_exposure_fine;
	unsigned int cur_analog_gain;
	unsigned int cur_analog_gain_permille;
	unsigned int cur_digital_gain;
	unsigned int cur_digital_gain_permille;
	unsigned int cur_long_exposure_coarse;
	unsigned int cur_long_exposure_fine;
	unsigned int cur_long_analog_gain;
	unsigned int cur_long_analog_gain_permille;
	unsigned int cur_long_digital_gain;
	unsigned int cur_long_digital_gain_permille;
	unsigned int cur_short_exposure_coarse;
	unsigned int cur_short_exposure_fine;
	unsigned int cur_short_analog_gain;
	unsigned int cur_short_analog_gain_permille;
	unsigned int cur_short_digital_gain;
	unsigned int cur_short_digital_gain_permille;

	unsigned int stream_on;

	/* Moved from SensorEntry.cpp Jong 20121008 */
	unsigned int sen_vsync_count;

	unsigned int stream_id;
	unsigned int product_name; /* sensor names such as IMX134, IMX135, and S5K3L2 */
	unsigned int sens_config_index_cur;
	unsigned int sens_config_index_pre;
	unsigned int cur_frame_rate;
	unsigned int pre_frame_rate;
	bool is_active_area;

	/* bool bFirstRegSet; */
	unsigned int low_expo_start;

	/* To deal with N + 1 or N +2 setting timing, 0: Previous input, 1: Current input */
	unsigned int analog_gain[2];
	unsigned int digital_gain[2];

	unsigned int max_fps;

	unsigned int mipi_err_int_cnt[10];
	unsigned int mipi_err_print_out_cnt[10];

	bool called_common_sensor_setting; /* [hc0105.kim, 2013/09/13] Added to avoid calling common sensor register setting again. */
/* #ifdef C1_LSC_CHANGE // [ist.song 2014.08.19] Added to inform videomode to sensor. */
	bool video_mode;
#ifdef USE_NEW_PER_FRAME_CONTROL
	unsigned int num_of_frame;
#endif
/* #endif */
	unsigned int actuator_position;

	is_shared_data is_data;
	ois_shared_data ois_data;
	ae_setting auto_exposure[2];
	preprocessor_ae_setting preproc_auto_exposure[2];

/*	SysSema_t pFlashCtrl_IsrSema; //[2014.08.13, kh14.koo] to add for sema of KTD2692 (flash dirver) */

	bool binning; /* If binning is set, sensor should binning for size */

	u32 cis_rev;
	u32 group_param_hold;

	/* set low noise mode */
	u32				cur_lownoise_mode;
	u32				pre_lownoise_mode;

#ifdef CAMERA_REAR2_SENSOR_SHIFT_CROP
	u32				sensor_shifted_num;
#endif
} cis_shared_data;

struct v4l2_subdev;
typedef int (*cis_func_type)(struct v4l2_subdev *subdev, cis_setting_info *info);
struct fimc_is_cis_ops {
        int (*cis_init)(struct v4l2_subdev *subdev);
        int (*cis_log_status)(struct v4l2_subdev *subdev);
        int (*cis_group_param_hold)(struct v4l2_subdev *subdev, bool hold);
        int (*cis_set_global_setting)(struct v4l2_subdev *subdev);
        int (*cis_mode_change)(struct v4l2_subdev *subdev, u32 mode);
        int (*cis_set_size)(struct v4l2_subdev *subdev, cis_shared_data *cis_data);
        int (*cis_stream_on)(struct v4l2_subdev *subdev);
        int (*cis_stream_off)(struct v4l2_subdev *subdev);
        int (*cis_adjust_frame_duration)(struct v4l2_subdev *subdev,
						u32 input_exposure_time,
						u32 *target_duration);
	/* Set dynamic frame duration value */
        int (*cis_set_frame_duration)(struct v4l2_subdev *subdev, u32 frame_duration);
	/* Set min fps value */
        int (*cis_set_frame_rate)(struct v4l2_subdev *subdev, u32 min_fps);
        int (*cis_get_min_exposure_time)(struct v4l2_subdev *subdev, u32 *min_expo);
        int (*cis_get_max_exposure_time)(struct v4l2_subdev *subdev, u32 *max_expo);
	cis_func_type cis_adjust_expoure_time; /* TBD */
        int (*cis_set_exposure_time)(struct v4l2_subdev *subdev, struct ae_param *target_exposure);
        int (*cis_get_min_analog_gain)(struct v4l2_subdev *subdev, u32 *min_again);
        int (*cis_get_max_analog_gain)(struct v4l2_subdev *subdev, u32 *max_again);
        int (*cis_adjust_analog_gain)(struct v4l2_subdev *subdev, u32 input_again, u32 *target_permile);
        int (*cis_set_analog_gain)(struct v4l2_subdev *subdev, struct ae_param *again);
        int (*cis_get_analog_gain)(struct v4l2_subdev *subdev, u32 *again);
        int (*cis_get_min_digital_gain)(struct v4l2_subdev *subdev, u32 *min_dgain);
        int (*cis_get_max_digital_gain)(struct v4l2_subdev *subdev, u32 *max_dgain);
	cis_func_type cis_adjust_digital_gain; /* TBD */
        int (*cis_set_digital_gain)(struct v4l2_subdev *subdev, struct ae_param *dgain);
        int (*cis_get_digital_gain)(struct v4l2_subdev *subdev, u32 *dgain);
	int (*cis_compensate_gain_for_extremely_br)(struct v4l2_subdev *subdev, u32 expo, u32 *again, u32 *dgain);
	cis_func_type cis_get_line_readout_time_ns; /* TBD */
	cis_func_type cis_read_sysreg; /* TBD */
	cis_func_type cis_read_userreg; /* TBD */
	int (*cis_wait_streamoff)(struct v4l2_subdev *subdev);
	int (*cis_wait_streamon)(struct v4l2_subdev *subdev);
	void (*cis_data_calculation)(struct v4l2_subdev *subdev, u32 mode);
	int (*cis_set_long_term_exposure)(struct v4l2_subdev *subdev);
	int (*cis_set_adjust_sync)(struct v4l2_subdev *subdev, u32 adjust_sync);
#ifdef CONFIG_SENSOR_RETENTION_USE
	int (*cis_retention_prepare)(struct v4l2_subdev *subdev);
	int (*cis_retention_crc_check)(struct v4l2_subdev *subdev);
#endif
#ifdef USE_CAMERA_MIPI_CLOCK_VARIATION
	int (*cis_update_mipi_info)(struct v4l2_subdev *subdev);
	int (*cis_get_mipi_clock_string)(struct v4l2_subdev *subdev, char *cur_mipi_str);
#endif
#ifdef USE_CAMERA_EMBEDDED_HEADER
	int (*cis_get_frame_id)(struct v4l2_subdev *subdev, u8 *embedded_buf, u16 *frame_id);
#endif
	int (*cis_set_frs_control)(struct v4l2_subdev *subdev, u32 command);
	int (*cis_set_super_slow_motion_roi)(struct v4l2_subdev *subdev, struct v4l2_rect *ssm_roi);
#ifdef CAMERA_REAR2_SENSOR_SHIFT_CROP
	int (*cis_update_pdaf_tail_size)(struct v4l2_subdev *subdev, struct fimc_is_sensor_cfg *select);
#endif
};

struct fimc_is_sensor_ctl
{
	//===================================================================================//
	// Ctl updtate timing
	// (01) ~ (04) are updated by "SensorEntry::CopyCam2P0Ctl" when "SIRC_ISP_CAMERA_EVENT_FRAME_START" interrupt is occured.

	/* (01) */  u32 ctl_frame_number;
	/* (02) */  camera2_sensor_ctl_t cur_cam20_sensor_ctrl;
	bool valid_sensor_ctrl;
	/* (03) */  camera2_flash_ctl_t cur_cam20_flash_ctrl;
	bool valid_flash_ctrl;
	/* (04) */  camera2_lens_ctl_t cur_cam20_lens_ctrl;
	bool is_valid_lens_ctrl;
	//===================================================================================//

	//===================================================================================//
	// UCtl update timing

	// Sensor
	// (05) ~ (07) are updated by "SensorEntry::ReportCam2P0SensorDone" when "SIRC_ISP_CAMERA_EVENT_FRAME_END" interrupt is occured.
	/* (05) */  u32 sensor_frame_number;
	/* (06) */  bool is_sensor_request;
	/* (07) */  camera2_sensor_uctl_t cur_cam20_sensor_udctrl;
	bool is_valid_sensor_udctrl;

	// Flash
	// (08) ~ (10) are updated by "SensorEntry::ReportCam2P0FlashDone" when "SircSenHal_RequestFlash" function is called.
	/* (08) */  u32 flash_frame_number;
	/* (09) */  bool is_flash_request;
	/* (10) */  camera2_flash_uctl_t cur_cam20_flash_udctrl;
	bool valid_flash_udctrl;

	// Lens
	// (11) ~ (13) are updated by "SensorEntry::ReportCam2P0LensDone" when "SircSenHal_SetActuatorPosition" function is called
	// or "SIRC_ISP_CAMERA_EVENT_FRAME_END" interrupt is occured.
	/* (11) */  u32 lens_frame_number;
	/* (12) */  bool is_lens_request;
	/* (13) */  camera2_lens_uctl_t cur_cam20_lens_udctrl;
	bool is_valid_lens_udctrl;
	//===================================================================================//

	bool alg_reset_flag;

	// Frame number that indicating shot. Currntly, it is not used.
	/* (14) */  bool shot_frame_number;
};

typedef enum fimc_is_sensor_adjust_direction_ {
	SENSOR_ADJUST_TO_SHORT	= 1,
	SENSOR_ADJUST_TO_LONG	= 2,
} fimc_is_sensor_adjust_direction;

/* If companion statistics are used, then 3A algorithms need to know whether current stat. are ready to use or not. */
enum itf_cis_hdr_stat_status {
	SENSOR_STAT_STATUS_NO_DATA = 0,
	SENSOR_STAT_STATUS_DONE = 1,
};

enum itf_cis_interface {
	ITF_CIS_SMIA = 0,
	ITF_CIS_SMIA_WDR,
	ITF_CIS_MAX,
};

enum itf_param_type {
	ITF_CIS_PARAM_TOTAL_GAIN,
	ITF_CIS_PARAM_ANALOG_GAIN,
	ITF_CIS_PARAM_DIGITAL_GAIN,
	ITF_CIS_PARAM_EXPOSURE,
	ITF_CIS_PARAM_FLASH_INTENSITY,
	ITF_CIS_PARAM_MAX,
};

/* This peri state is check available each sensor device. */
enum fimc_is_sensor_peri_state {
	FIMC_IS_SENSOR_ACTUATOR_AVAILABLE,
	FIMC_IS_SENSOR_FLASH_AVAILABLE,
	FIMC_IS_SENSOR_PREPROCESSOR_AVAILABLE,
	FIMC_IS_SENSOR_OIS_AVAILABLE,
	FIMC_IS_SENSOR_PDP_AVAILABLE,
	FIMC_IS_SENSOR_APERTURE_AVAILABLE,
};

enum fimc_is_actuator_pos_size_bit {
	ACTUATOR_POS_SIZE_8BIT = 8,
	ACTUATOR_POS_SIZE_9BIT = 9,
	ACTUATOR_POS_SIZE_10BIT = 10,
};

enum fimc_is_actuator_direction {
	ACTUATOR_RANGE_INF_TO_MAC = 0,
	ACTUATOR_RANGE_MAC_TO_INF,
};

enum fimc_is_actuator_status {
	ACTUATOR_STATUS_NO_BUSY = 0,
	ACTUATOR_STATUS_BUSY
};

enum fimc_is_cis_lownoise_mode {
	FIMC_IS_CIS_LNOFF = 0, /* Low Noise Off */
	FIMC_IS_CIS_LN2, /* Low Noise 2 */
	FIMC_IS_CIS_LN4, /* Low Noise 4 */
	FIMC_IS_CIS_LOWNOISE_MODE_MAX,
};

enum fimc_is_aperture_control_step {
	APERTURE_STEP_STATIONARY = 0,
	APERTURE_STEP_PREPARE,
	APERTURE_STEP_MOVING,
};

typedef int (*actuator_func_type)(struct v4l2_subdev *subdev, u32 *info);
struct fimc_is_actuator_ops {
	actuator_func_type actuator_init;
	actuator_func_type actuator_get_status;
	actuator_func_type actuator_set_pos;
	actuator_func_type actuator_cal_data;
#ifdef USE_AF_SLEEP_MODE
	int (*set_active)(struct v4l2_subdev *subdev, int enable);
#endif
};

struct fimc_is_aperture_ops {
	int (*set_aperture_value)(struct v4l2_subdev *subdev, int value);
	int (*set_aperture_start_value_step1)(struct v4l2_subdev *subdev, int value);
	int (*set_aperture_start_value_step2)(struct v4l2_subdev *subdev, int value);
	int (*prepare_ois_autotest)(struct v4l2_subdev *subdev);
	int (*aperture_deinit)(struct v4l2_subdev *subdev, int value);
};

/* for SetAlgResFlag API */
struct fimc_is_3a_res_to_sensor {
	u32 hdr_ratio;
	u32 red_gain;
	u32 green_gain;
	u32 blue_gain;
	u32 hdr_enabled;
	u32 hdr_state;
	u32 thermal_mode;
	bool video_mode;
};

/* Flash */
struct fimc_is_flash_ops {
	int (*flash_control)(struct v4l2_subdev *subdev, enum flash_mode mode, u32 intensity);
};

struct fimc_is_preproc_lemode_set{
	u16 lemode;
	bool every_frame;
};

/* Long Term Exposure mode(LTE mode) interface */
struct fimc_is_long_term_expo_mode {
	u32 expo[2];
	u32 tgain[2];
	u32 again[2];
	u32 dgain[2];
	bool sen_strm_off_on_enable;
	u32 sen_strm_off_on_step;
	u32 frm_num_strm_off_on[2];
	u32 frm_num_strm_off_on_interval;
	struct fimc_is_preproc_lemode_set lemode_set;
	/* Back up now line_length_pck when start LTE mode */
	u32 pre_line_length_pck;
	/* Control to frame_interval in sensor driver when LTE mode */
	u32 frame_interval;
};

/* OIS */
struct fimc_is_ois_ops {
	int (*ois_init)(struct v4l2_subdev *subdev);
#ifdef CONFIG_OIS_USE_RUMBA_S6
	int (*ois_deinit)(struct v4l2_subdev *subdev);
#endif
#ifdef USE_OIS_SLEEP_MODE
	int (*ois_start)(struct v4l2_subdev *subdev);
	int (*ois_stop)(struct v4l2_subdev *subdev);
#endif
	int (*ois_set_mode)(struct v4l2_subdev *subdev, int mode);
	int (*ois_shift_compensation)(struct v4l2_subdev *subdev, int position, int resolution);
#ifdef CONFIG_OIS_DIRECT_FW_CONTROL
	int (*ois_fw_update)(struct v4l2_subdev *subdev);
#else
	void (*ois_fw_update)(struct fimc_is_core *core);
#endif
	int (*ois_self_test)(struct fimc_is_core *core);
	bool (*ois_diff_test)(struct fimc_is_core *core, int *x_diff, int *y_diff);
	bool (*ois_auto_test)(struct fimc_is_core *core,
	int threshold, bool *x_result, bool *y_result, int *sin_x, int *sin_y);
#ifdef CAMERA_REAR2_OIS
	bool (*ois_auto_test_rear2)(struct fimc_is_core *core,
	int threshold, bool *x_result, bool *y_result, int *sin_x, int *sin_y,
	bool *x_result_2nd, bool *y_result_2nd, int *sin_x_2nd, int *sin_y_2nd);
#endif
	bool (*ois_check_fw)(struct fimc_is_core *core);
	void (*ois_enable)(struct fimc_is_core *core);
	void (*ois_offset_test)(struct fimc_is_core *core, long *raw_data_x, long *raw_data_y);
	void (*ois_get_offset_data)(struct fimc_is_core *core, long *raw_data_x, long *raw_data_y);
	void (*ois_gyro_sleep)(struct fimc_is_core *core);
	void (*ois_exif_data)(struct fimc_is_core *core);
	u8 (*ois_read_status)(struct fimc_is_core *core);
	u8 (*ois_read_cal_checksum)(struct fimc_is_core *core);
	int (*ois_set_coef)(struct v4l2_subdev *subdev, u8 coef);
	int (*ois_read_fw_ver)(char *name, char *ver);
	int (*ois_center_shift)(struct v4l2_subdev *subdev);
	int (*ois_set_center)(struct v4l2_subdev *subdev);
};

struct fimc_is_sensor_interface;

/* new APIs */
struct fimc_is_cis_interface_ops {
	int (*get_)(struct fimc_is_sensor_interface *itf);

	int (*request_reset_interface)(struct fimc_is_sensor_interface *itf,
					u32 exposure,
					u32 total_gain,
					u32 analog_gain,
					u32 digital_gain);

	int (*get_calibrated_size)(struct fimc_is_sensor_interface *itf,
					u32 *width,
					u32 *height);

	int (*get_bayer_order)(struct fimc_is_sensor_interface *itf,
					u32 *bayer_order);

	u32 (*get_min_exposure_time)(struct fimc_is_sensor_interface *itf);
	u32 (*get_max_exposure_time)(struct fimc_is_sensor_interface *itf);
	u32 (*get_min_analog_gain)(struct fimc_is_sensor_interface *itf);
	u32 (*get_max_analog_gain)(struct fimc_is_sensor_interface *itf);
	u32 (*get_min_digital_gain)(struct fimc_is_sensor_interface *itf);
	u32 (*get_max_digital_gain)(struct fimc_is_sensor_interface *itf);

	u32 (*get_vsync_count)(struct fimc_is_sensor_interface *itf);
	u32 (*get_vblank_count)(struct fimc_is_sensor_interface *itf);
	bool (*is_vvalid_period)(struct fimc_is_sensor_interface *itf);

	int (*request_exposure)(struct fimc_is_sensor_interface *itf,
				u32 long_exposure,
				u32 short_exposure);

	int (*adjust_exposure)(struct fimc_is_sensor_interface *itf,
				u32 long_exposure,
				u32 short_exposure,
				u32 *available_long_exposure,
				u32 *available_short_exposure,
				fimc_is_sensor_adjust_direction adjust_direction);

	int (*get_next_frame_timing)(struct fimc_is_sensor_interface *itf,
					u32 *long_exposure,
					u32 *short_exposure,
					u32 *frame_period,
					u64 *line_period);

	int (*get_frame_timing)(struct fimc_is_sensor_interface *itf,
					u32 *long_exposure,
					u32 *short_exposure,
					u32 *frame_period,
					u64 *line_period);

	int (*request_analog_gain)(struct fimc_is_sensor_interface *itf,
					u32 long_analog_gain,
					u32 short_analog_gain);

	int (*request_gain)(struct fimc_is_sensor_interface *itf,
				u32 long_total_gain,
				u32 long_analog_gain,
				u32 long_digital_gain,
				u32 short_total_gain,
				u32 short_analog_gain,
				u32 short_digital_gain);

	int (*adjust_analog_gain)(struct fimc_is_sensor_interface *itf,
					u32 desired_long_analog_gain,
					u32 desired_short_analog_gain,
					u32 *actual_long_gain,
					u32 *actual_short_gain,
					fimc_is_sensor_adjust_direction adjust_direction);

	int (*get_next_analog_gain)(struct fimc_is_sensor_interface *itf,
				u32 *long_analog_gain,
				u32 *short_analog_gain);

	int (*get_analog_gain)(struct fimc_is_sensor_interface *itf,
				u32 *long_analog_gain,
				u32 *short_analog_gain);

	int (*get_next_digital_gain)(struct fimc_is_sensor_interface *itf,
				u32 *long_digital_gain,
				u32 *short_digital_gain);

	int (*get_digital_gain)(struct fimc_is_sensor_interface *itf,
				u32 *long_digital_gain,
				u32 *short_digital_gain);

	bool (*is_actuator_available)(struct fimc_is_sensor_interface *itf);
	bool (*is_flash_available)(struct fimc_is_sensor_interface *itf);
	bool (*is_companion_available)(struct fimc_is_sensor_interface *itf);
	bool (*is_ois_available)(struct fimc_is_sensor_interface *itf);
	bool (*is_aperture_available)(struct fimc_is_sensor_interface *itf);

	int (*get_sensor_frame_timing)(struct fimc_is_sensor_interface *itf,
				u32 *pclk,
				u32 *line_length_pck,
				u32 *frame_length_lines,
				u32 *max_margin_cit);

	int (*get_sensor_cur_size)(struct fimc_is_sensor_interface *itf,
				u32 *cur_width,
				u32 *cur_height);

	int (*get_sensor_max_fps)(struct fimc_is_sensor_interface *itf,
				u32 *max_fps);

	int (*get_sensor_cur_fps)(struct fimc_is_sensor_interface *itf,
				u32 *cur_fps);

	int (*get_hdr_ratio_ctl_by_again)(struct fimc_is_sensor_interface *itf,
				u32 *ctrl_by_again);

	int (*get_sensor_use_dgain)(struct fimc_is_sensor_interface *itf,
				u32 *use_dgain);

	int (*set_alg_reset_flag)(struct fimc_is_sensor_interface *itf,
				bool executed);

	int (*get_sensor_initial_aperture)(struct fimc_is_sensor_interface *itf,
				u32 *aperture);

	int (*set_initial_exposure_of_setfile)(struct fimc_is_sensor_interface *itf,
				u32 expo);

#ifdef USE_NEW_PER_FRAME_CONTROL
	int (*reserved0)(struct fimc_is_sensor_interface *itf,
				bool reserved0);

	int (*set_num_of_frame_per_one_3aa)(struct fimc_is_sensor_interface *itf,
				u32 *num_of_frame);

	int (*reserved1)(struct fimc_is_sensor_interface *itf,
				u32 *reserved1);
#else
	int (*set_video_mode_of_setfile)(struct fimc_is_sensor_interface *itf,
				bool video_mode);

	int (*get_num_of_frame_per_one_3aa)(struct fimc_is_sensor_interface *itf,
				u32 *num_of_frame);

	int (*get_offset_from_cur_result)(struct fimc_is_sensor_interface *itf,
				u32 *offset);
#endif
	int (*set_cur_uctl_list)(struct fimc_is_sensor_interface *itf);

	int (*apply_sensor_setting)(struct fimc_is_sensor_interface *itf);

	/* reset exposure and gain for Flash */
	int (*request_reset_expo_gain)(struct fimc_is_sensor_interface *itf,
					u32 long_expo,
					u32 long_tgain,
					u32 long_again,
					u32 long_dgain,
					u32 short_expo,
					u32 short_tgain,
					u32 short_again,
					u32 short_dgain);
	int (*set_sensor_info_mode_change)(struct fimc_is_sensor_interface *itf,
					u32 long_expo,
					u32 long_again,
					u32 long_dgain,
					u32 expo,
					u32 again,
					u32 dgain);
	int (*update_sensor_dynamic_meta)(struct fimc_is_sensor_interface *itf,
					u32 frame_count,
					camera2_ctl_t *ctrl,
					camera2_dm_t *dm,
					camera2_udm_t *udm);
	int (*copy_sensor_ctl)(struct fimc_is_sensor_interface *itf,
					u32 frame_count,
					camera2_shot_t *shot);

	/* Get sensor module id */
	int (*get_module_id)(struct fimc_is_sensor_interface *itf,
					u32 *module_id);

	/* Get sensor module position */
	int (*get_module_position)(struct fimc_is_sensor_interface *itf,
					enum exynos_sensor_position *real_module);

	/* Set sensor 3a mode - OTF/M2M */
	int (*set_sensor_3a_mode)(struct fimc_is_sensor_interface *itf,
					u32 mode);
};

struct fimc_is_cis_ext_interface_ops {
	int (*get_sensor_hdr_stat)(struct fimc_is_sensor_interface *itf,
			enum itf_cis_hdr_stat_status *status);

	int (*set_3a_alg_res_to_sens)(struct fimc_is_sensor_interface *itf,
			struct fimc_is_3a_res_to_sensor *sensor_setting);

	/* In order to change a current CIS mode when an user select the WDR (long and short exposure) mode or the normal AE mo */
	int (*change_cis_mode)(struct fimc_is_sensor_interface *itf,
			enum itf_cis_interface cis_mode);
	u32(*set_adjust_sync)(struct fimc_is_sensor_interface *itf, u32 setsync);
	u32(*request_frame_length_line)(struct fimc_is_sensor_interface *itf, u32 framelengthline);
	int (*request_sensitivity)(struct fimc_is_sensor_interface *itf,
								u32 sensitivity);
};

struct fimc_is_cis_ext2_interface_ops {
	int (*set_long_term_expo_mode)(struct fimc_is_sensor_interface *itf,
				struct fimc_is_long_term_expo_mode *long_term_expo_mode);
	int (*set_low_noise_mode)(struct fimc_is_sensor_interface *itf, u32 mode);
	void *reserved[19];
};

struct fimc_is_cis_event_ops {
	int (*start_of_frame)(struct fimc_is_sensor_interface *itf);
	int (*end_of_frame)(struct fimc_is_sensor_interface *itf);
	int (*apply_frame_settings)(struct fimc_is_sensor_interface *itf);
};

/* end of new APIs */

/* Actuator interface */
struct fimc_is_actuator_softlanding_table {
	bool enable;
	u32 step_delay;
	u32 position_num;
	u32 virtual_table[ACTUATOR_MAX_SOFT_LANDING_NUM];
	u32 hw_table[ACTUATOR_MAX_SOFT_LANDING_NUM];
};

struct fimc_is_actuator_position_table {
	bool enable;
	u32 hw_table[ACTUATOR_MAX_FOCUS_POSITIONS];
};

struct fimc_is_actuator_interface {
	/* ToDo: consider M2M scenario */
	u32 virtual_pos;
	u32 hw_pos;

	/*
	 * This values are specific information for AF Tick noise
	 * when turning on a camera.
	 */
	bool initialized;

	struct fimc_is_actuator_position_table position_table;
	struct fimc_is_actuator_softlanding_table soft_landing_table;
};

struct fimc_is_actuator_interface_ops {
	int (*set_actuator_position_table) (struct fimc_is_sensor_interface *itf,
					u32 *position_table);
	int (*set_soft_landing_config) (struct fimc_is_sensor_interface *itf,
					u32 step_delay,
					u32 position_num,
					u32 *position_table);
	int (*set_position) (struct fimc_is_sensor_interface *itf, u32 position);

	int (*get_cur_frame_position) (struct fimc_is_sensor_interface *itf, u32 *position);
	int (*get_applied_actual_position) (struct fimc_is_sensor_interface *itf, u32 *position);
	int (*get_prev_frame_position) (struct fimc_is_sensor_interface *itf,
					u32 *position, u32 frame_diff);

	int (*set_af_window_position) (struct fimc_is_sensor_interface *itf,
					u32 left_x, u32 left_y,
					u32 right_x, u32 right_y);

	int (*get_status) (struct fimc_is_sensor_interface *itf, u32 *status);
	int (*copy_lens_ctl)(struct fimc_is_sensor_interface *itf,
					u32 frame_count,
					camera2_shot_t *shot);
};

struct fimc_is_apature_info_t {
	int	cur_value;
	bool	zoom_running;
};

struct fimc_is_aperture_interface_ops {
	int (*set_aperture_value)(struct fimc_is_sensor_interface *itf, int value);
	int (*get_aperture_value)(struct fimc_is_sensor_interface *itf, struct fimc_is_apature_info_t *param);
};

/* Flash interface */
struct fimc_is_flash_expo_gain {
	u32 expo[2];
	u32 tgain[2];
	u32 again[2];
	u32 dgain[2];
	u32 long_expo[2];
	u32 long_tgain[2];
	u32 long_again[2];
	u32 long_dgain[2];
	u32 short_expo[2];
	u32 short_tgain[2];
	u32 short_again[2];
	u32 short_dgain[2];
	bool pre_fls_ae_reset; /* true: Pre-flash off */
	u32 frm_num_pre_fls; /* If it is set to 100, then Pre-flash is automatically turned off at 100-th frame. */
	bool main_fls_ae_reset; /* true: Main-flash on/off */
	/* If they are set to 200 and 201, then Main-flash sensor settings are applied to a sensor, and the flash is fired at 200-th frame.
	   After the Main-flash, ambient exposure and gains are set to the sensor at 201-th frame. */
	u32 frm_num_main_fls[2];
	u32 main_fls_strm_on_off_step; /* 0: main/pre-flash exposure and gains, 1: ambient exposure and gains */
};

/* PDP structure &  PDP Interface */
enum {
	FILTER_BAND_0,
	FILTER_BAND_1,
	FILTER_BAND_2,
	FILTER_BAND_L,
	MAX_FILTER_BAND
};

struct pdp_main_wininfo {
	bool		update;
	u16		center_x;
	u16		center_y;
	u16		start_x;
	u16		start_y;
	u16		end_x;
	u16		end_y;
};

struct pdp_multi_wininfo {
	bool		update;
	u8		mode_on;
	u16		start_x;
	u16		start_y;
	u16		size_x;
	u16		size_y;
	u16		gap_x;
	u16		gap_y;
	u16		no_x;
	u16		no_y;
};

struct pdp_single_wininfo {
	bool				update;
	u16				sroi;
	struct pdp_main_wininfo		single_win[4];
};

struct pdp_knee_setting_t {
	bool		update;
	u16		knee_on;
	u16		kn_inc_0;
	u16		kn_inc_1;
	u16		kn_inc_2;
	u16		kn_inc_3;
	u16		kn_inc_4;
	u16		kn_inc_5;
	u16		kn_inc_6;
	u16		kn_inc_7;
	u16		kn_offset_1;
	u16		kn_offset_2;
	u16		kn_offset_3;
	u16		kn_offset_4;
	u16		kn_offset_5;
	u16		kn_offset_6;
	u16		kn_offset_7;
};

struct pdp_filterBand_setting_t {
	bool		update;
	u16		gain0;
	u16		k01;
	u16		k02;
	u16		type0;
	u16		gain1;
	u16		k11;
	u16		k12;
	u16		c11;
	u16		c12;
	u16		gain2;
	u16		k21;
	u16		k22;
	u16		c21;
	u16		c22;
	u16		bypass0;
	u16		bypass1;
	u16		bypass2;
	u16		cor_type_b;
	u16		coring_ty_b;
	u16		coring_th_b;
	u16		coring_b;
	u16		bin_first_b;
	u16		binning_num_b;
	u16		bin_skip_b;
};

struct pdp_filterCor_setting_t {
	bool		update;
	u16		coring_ty;
	u16		coring_th;
};

struct pdp_filterBin_setting_t {
	bool		update;
	u16		bin_first;
	u16		binning_num_lmv_h;
	u16		binning_num_lmv_v;
};

struct pdp_wdr_setting_t {
	bool		update;
	u16		wdr_on;
	u16		wdr_coef_long;
	u16		wdr_coef_short;
	u16		wdr_shft_long;
	u16		wdr_shft_short;
};

struct pdp_depth_setting_t {
	bool		update;
	u16		depth_dmode;
	u16		depth_dshift;
	u16		depth_dsize1;
	u16		depth_dsize2;
	u16		depth_dsmoothe_on;
	u16		depth_dscan_on;
	u16		depth_dmedian_on;
	u16		depth_dslope_on;
	u16		depth_darm_edge_h;
	u16		depth_dmedian_size;
	u16		depth_dscan_edge;
	u16		depth_dscan_penalty;
	u16		depth_dbin_up;
	u16		depth_dscale_up;
	u16		depth_dout_stat_mode;
	u16		depth_dinverse_snr;
	u16		depth_dcoef_snr;
	u16		depth_dcoef_edgeh;
	u16		depth_dcoef_edgev;
	u16		depth_dshift_filter;
	u16		depth_dadd_filter;
	u16		depth_dmask_filter_1;
	u16		depth_dmask_filter_2;
	u16		depth_dmask_filter_3;
	u16		depth_dmask_filter_4;
	u16		depth_dmask_filter_5;
};

struct pdp_YextParam_setting_t {
	bool		update;
	u16		max_no_skippxg;
	u16		skip_levelth;
	u16		px_sat_g;
	u16		px_sat_r;
	u16		px_sat_b;
	u16		sat_no_g;
	u16		sat_no_r;
	u16		sat_no_b;
	u16		sat_g;
	u16		sat_r;
	u16		sat_b;
	u16		clip_val_left;
	u16		clip_val_right;
	u16		coef_r_short;
	u16		coef_g_short;
	u16		coef_b_short;
	u16		y_shift;
	u16		coef_r_long;
	u16		coef_g_long;
	u16		coef_b_long;
};

struct pdp_paf_roi_setting_t {
	bool		update;
	u16		roi_start_x;
	u16		roi_start_y;
	u16		roi_end_x;
	u16		roi_end_y;
};

struct pdp_paf_setting_t {
	bool		update;
	u16		xcor_on;
	u16		af_cross;
	u16		mpd_on;
	u16		mpd_hbin;
	u16		mpd_vbin;
	u16		mpd_vsft;
	u16		mpd_dp;
	u16		mpd_dp_th;
	u16		phase_range;
	u16		dpc_on;
	u16		lmv_on;
	u16		lmv_shift;
	u16		alc_on;
	u16		alc_gap;
	u16		alc_clip_on;
	u16		alc_fit_on;
	u16		b2_en;
	u16		crop_on;
	u16		af_debug_mode;
	u16		lf_shift;
	u16		pafsat_on;
	u16		sat_lv;
	u16		sat_lv1;
	u16		sat_lv2;
	u16		sat_lv3;
	u16		sat_src;
	u16		cor_type;
	u16		g_ssd;
	u16		ob_value;
	u16		af_layout;
	u16		af_pattern;
	u16		roi_zg1;
	u16		roi_zg2;
	u16		roi_zg3;
	u16		roi_zg4;
};

struct pdp_total_setting_t {
	struct pdp_paf_setting_t			*paf_setting;
	struct pdp_paf_roi_setting_t			*paf_roi_setting;
	struct pdp_main_wininfo				*paf_main_window;
	struct pdp_single_wininfo			*paf_single_window;
	struct pdp_multi_wininfo			*paf_multi_window;
	struct pdp_knee_setting_t			*paf_knee_setting;
	struct pdp_filterCor_setting_t			*paf_filter_cor;
	struct pdp_filterBin_setting_t			*paf_filter_bin;
	struct pdp_filterBand_setting_t			*paf_filter_band[MAX_FILTER_BAND];

	struct pdp_wdr_setting_t			*wdr_setting;
	struct pdp_depth_setting_t			*depth_setting;
	struct pdp_YextParam_setting_t			*y_ext_param;
};

struct pdp_read_reg_setting_t {
	u32	read_addr_offset;
	u32 *addr_array;
	u32 *buf_array;
	u32 buf_size;
};

struct fimc_is_pdp_ops {
	int (*read_pdp_reg)(struct v4l2_subdev *subdev, struct pdp_read_reg_setting_t *reg_param);
	int (*read_paf_sfr_stat)(struct v4l2_subdev *subdev, u32 *buf);
	int (*get_irq_state)(struct v4l2_subdev *subdev, int *irq_state);
	int (*clear_irq_state)(struct v4l2_subdev *subdev, int irq_state);
	int (*set_pdp_param)(struct v4l2_subdev *subdev, struct pdp_total_setting_t *pdp_param);
};

struct fimc_is_pdp_interface_ops {
	int (*read_pdp_reg)(struct fimc_is_sensor_interface *itf, struct pdp_read_reg_setting_t *reg_param);
	int (*read_paf_sfr_stat)(struct fimc_is_sensor_interface *itf, u32 *buf);
	int (*get_irq_state)(struct fimc_is_sensor_interface *itf, int *irq_state);
	int (*clear_irq_state)(struct fimc_is_sensor_interface *itf, int irq_state);
	int (*set_pdp_param)(struct fimc_is_sensor_interface *itf, struct pdp_total_setting_t *pdp_param);
	int (*reserved[2])(struct fimc_is_sensor_interface *itf);
};

struct fimc_is_flash_interface_ops {
	int (*request_flash)(struct fimc_is_sensor_interface *itf,
				u32 mode,
				bool on,
				u32 intensity,
				u32 time);
	int (*request_flash_expo_gain)(struct fimc_is_sensor_interface *itf,
				struct fimc_is_flash_expo_gain *flash_ae);
	int (*update_flash_dynamic_meta)(struct fimc_is_sensor_interface *itf,
					u32 frame_count,
					camera2_ctl_t *ctrl,
					camera2_dm_t *dm,
					camera2_udm_t *udm);
	int (*copy_flash_ctl)(struct fimc_is_sensor_interface *itf,
					u32 frame_count,
					camera2_shot_t *shot);
};

struct fimc_is_csi_interface_ops {
	int (*get_vc_dma_buf)(struct fimc_is_sensor_interface *itf,
				enum itf_vc_buf_data_type request_data_type,
				u32 *buf_index,
				u64 *buf_addr,
				u32 *frame_count);
	int (*put_vc_dma_buf)(struct fimc_is_sensor_interface *itf,
				enum itf_vc_buf_data_type request_data_type,
				u32 index);
	int (*get_vc_dma_buf_info)(struct fimc_is_sensor_interface *itf,
				enum itf_vc_buf_data_type request_data_type,
				enum itf_vc_stat_type *return_stat_type,
				u32 *width,
				u32 *height,
				u32 *element_size,
				u32 *sensor_shifted_num);
	int (*get_vc_dma_buf_max_size)(struct fimc_is_sensor_interface *itf,
				enum itf_vc_buf_data_type request_data_type,
				u32 *width,
				u32 *height,
				u32 *element_size);
	int (*reserved[4])(struct fimc_is_sensor_interface *itf);
};

struct fimc_is_dual_interface_ops {
	int (*get_sensor_state)(struct fimc_is_sensor_interface *itf);
	int (*get_reuse_3a_state)(struct fimc_is_sensor_interface *itf,
				u32 *position, u32 *ae_exposure, u32 *ae_deltaev, bool is_clear);
	int (*set_reuse_ae_exposure)(struct fimc_is_sensor_interface *itf,
				u32 ae_exposure, u32 ae_deltaev);
	int (*reserved[2])(struct fimc_is_sensor_interface *itf);
};

struct fimc_is_sensor_interface {
	u32					magic;
	struct fimc_is_cis_interface_ops	cis_itf_ops;
	struct fimc_is_cis_event_ops		cis_evt_ops;
	struct fimc_is_actuator_interface	actuator_itf;
	struct fimc_is_actuator_interface_ops	actuator_itf_ops;
	struct fimc_is_flash_interface_ops	flash_itf_ops;
	struct fimc_is_aperture_interface_ops	aperture_itf_ops;
	struct fimc_is_pdp_interface_ops	pdp_itf_ops;

	bool			vsync_flag;
	bool			otf_flag_3aa;
	/* Different frame count between sensor and ISP */
	enum DIFF_BET_SEN_ISP	diff_bet_sen_isp;
	enum itf_cis_interface	cis_mode;

	u32			total_gain[MAX_EXPOSURE_GAIN_PER_FRAME][NUM_FRAMES];
	u32			analog_gain[MAX_EXPOSURE_GAIN_PER_FRAME][NUM_FRAMES];
	u32			digital_gain[MAX_EXPOSURE_GAIN_PER_FRAME][NUM_FRAMES];
	u32			exposure[MAX_EXPOSURE_GAIN_PER_FRAME][NUM_FRAMES];

	u32			flash_mode[NUM_FRAMES];
	u32			flash_intensity[NUM_FRAMES];
	u32			flash_firing_duration[NUM_FRAMES];
	struct fimc_is_cis_ext_interface_ops	cis_ext_itf_ops;
	struct fimc_is_csi_interface_ops	csi_itf_ops;
	/* Add interface for LTE mode */
	struct fimc_is_cis_ext2_interface_ops	cis_ext2_itf_ops;
	/* Add interface for DUAL scenario */
	struct fimc_is_dual_interface_ops	dual_itf_ops;
};

int init_sensor_interface(struct fimc_is_sensor_interface *itf);

/* Sensor interface helper function */
struct fimc_is_module_enum *get_subdev_module_enum(struct fimc_is_sensor_interface *itf);
struct fimc_is_device_csi *get_subdev_csi(struct fimc_is_sensor_interface *itf);
struct fimc_is_actuator *get_subdev_actuator(struct fimc_is_sensor_interface *itf);
u32 get_frame_count(struct fimc_is_sensor_interface *itf);

int sensor_get_ctrl(struct fimc_is_sensor_interface *itf, u32 ctrl_id, u32 *val);

/* Actuator interface function */
int set_actuator_position_table(struct fimc_is_sensor_interface *itf,
			u32 *position_table);
int set_soft_landing_config(struct fimc_is_sensor_interface *itf,
			u32 step_delay, u32 position_num, u32 *hw_pos_table);
int set_position(struct fimc_is_sensor_interface *itf, u32 position);
int get_cur_frame_position(struct fimc_is_sensor_interface *itf, u32 *position);
int get_applied_actual_position(struct fimc_is_sensor_interface *itf, u32 *position);
int get_prev_frame_position(struct fimc_is_sensor_interface *itf,
			u32 *position, u32 frame_diff);
int set_af_window_position(struct fimc_is_sensor_interface *itf,
					u32 left_x, u32 left_y,
					u32 right_x, u32 right_y);
int get_status(struct fimc_is_sensor_interface *itf, u32 *status);

#endif
