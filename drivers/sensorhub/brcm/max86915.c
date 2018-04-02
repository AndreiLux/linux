/*
* Copyright (c) 2014 JY Kim, jy.kim@maximintegrated.com
* Copyright (c) 2013 Maxim Integrated Products, Inc.
*
* This software is licensed under the terms of the GNU General Public
* License, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef CONFIG_SPI_TO_I2C_FPGA
#include <linux/spi/fpga_i2c_expander.h>
#endif

#define VENDOR "MAXIM"
#define MAX86915_CHIP_NAME	"MAX86915"

#define VERSION			"0"
#define SUB_VERSION		"1"
#define VENDOR_VERSION		"m"

#define MODULE_NAME_HRM		"hrm_sensor"
#define DEFAULT_THRESHOLD -4194303
#define SLAVE_ADDR_MAX 0x57

#define MAX86915_I2C_RETRY_DELAY	10
#define MAX86915_I2C_MAX_RETRIES	5

/* Default Register Value */
#define MAX86915_PART_ID			0x2B
#define MAX86915_REV_ID				0x01

#define MAX86915_HRM_DEFAULT_CURRENT1		0x46 /* IR */
#define MAX86915_HRM_DEFAULT_CURRENT2		0x00 /* RED */
#define MAX86915_HRM_DEFAULT_CURRENT3		0x00 /* GREEN */
#define MAX86915_HRM_DEFAULT_CURRENT4		0x00 /* BLUE */

#define MAX86915_HRM_DEFAULT_XTALK_CODE1	0x00 /* IR */
#define MAX86915_HRM_DEFAULT_XTALK_CODE2	0x00 /* RED */
#define MAX86915_HRM_DEFAULT_XTALK_CODE3	0x00 /* GREEN */
#define MAX86915_HRM_DEFAULT_XTALK_CODE4	0x00 /* BLUE */

#define MAX86915_4CH_DEFAULT_CURRENT1	0xFF /* IR */
#define MAX86915_4CH_DEFAULT_CURRENT2	0xFF /* RED */
#define MAX86915_4CH_DEFAULT_CURRENT3	0x87 /* GREEN */
#define MAX86915_4CH_DEFAULT_CURRENT4	0xFF /* BLUE */

#define MAX86915_DEFAULT_XTALK_CODE1	0x00 /* IR */
#define MAX86915_DEFAULT_XTALK_CODE2	0x00 /* RED */
#define MAX86915_DEFAULT_XTALK_CODE3	0x00 /* GREEN */
#define MAX86915_DEFAULT_XTALK_CODE4	0x00 /* BLUE */

#define MAX86915_IR_INIT_CURRENT	0x46 /* IR */
#define MAX86915_RED_INIT_CURRENT	0x41 /* RED */
#define MAX86915_GREEN_INIT_CURRENT	0x28 /* GREEN */
#define MAX86915_BLUE_INIT_CURRENT	0x3C /* BLUE */

#define MAX86915_IR_CURRENT_STEP	0x04 /* IR */
#define MAX86915_RED_CURRENT_STEP	0x04 /* RED */
#define MAX86915_GREEN_CURRENT_STEP	0x03 /* GREEN */
#define MAX86915_BLUE_CURRENT_STEP	0x05 /* BLUE */

#define MAX86915_DEFAULT_LED_RGE	0x02 /* 150mA AGCCONF*/

/* AWB/Flicker Definition */
#define AWB_INTERVAL		20 /* 20 sample(from 17 to 28) */

#define CONFIG_SKIP_CNT		8
#define FLICKER_DATA_CNT	200
#define MAX86915_IOCTL_MAGIC		0xFD
#define MAX86915_IOCTL_READ_FLICKER	_IOR(MAX86915_IOCTL_MAGIC, 0x01, int *)
#define AWB_MAX86915_CONFIG_TH2		480000
#define AWB_MAX86915_CONFIG_TH3		40000

/* Configuration Paramter */
#define AGC_LED_SKIP_CNT	16


/* AGC Configuration */
#define AGC_IR		0
#define AGC_RED		1
#define AGC_GREEN	2
#define AGC_BLUE	3
#define AGC_SKIP_CNT	5

#define MAX86915_MIN_CURRENT	0
#define MAX86915_MAX_CURRENT	((MAX86915_DEFAULT_LED_RGE+1)*51000)
#define MAX86915_CURRENT_FULL_SCALE		\
		(MAX86915_MAX_CURRENT - MAX86915_MIN_CURRENT)

#define MAX86915_MIN_DIODE_VAL	0
#define MAX86915_MAX_DIODE_VAL	((1 << 19) - 1)

#define MAX86915_CURRENT_PER_STEP	((MAX86915_DEFAULT_LED_RGE+1) * 200)

#define MAX86915_AGC_DEFAULT_LED_OUT_RANGE				70
#define MAX86915_AGC_DEFAULT_CORRECTION_COEFF			50
#define MAX86915_AGC_DEFAULT_SENSITIVITY_PERCENT		 14
#define MAX86915_AGC_DEFAULT_MIN_NUM_PERCENT			 (10)

#define ILLEGAL_OUTPUT_POINTER -1
#define CONSTRAINT_VIOLATION -2

#define MAX86915_THRESHOLD_DEFAULT 100000

#define DEFAULT_FIFO_CNT	1

#define CONFIG_4CH_INPUT
#define CONFIG_4CH_800HZ
/* #define CONFIG_HRM_GREEN_BLUE */

/* turnning parameter of dc_step. */
#define EOL_NEW_HRM_ARRY_SIZE			4
#define EOL_NEW_MODE_DC_LOW				0
#define EOL_NEW_MODE_DC_MID				1
#define EOL_NEW_MODE_DC_HIGH			2
#define EOL_NEW_MODE_DC_XTALK			3

/* turnning parameter of skip count */
#define EOL_NEW_START_SKIP_CNT			134

/* turnning parameter of flicker_step. */
#define EOL_NEW_FLICKER_RETRY_CNT		48
#define EOL_NEW_FLICKER_SKIP_CNT		34
#define EOL_NEW_FLICKER_THRESH			8000

#define EOL_NEW_DC_MODE_SKIP_CNT		134
#define EOL_NEW_DC_FREQ_SKIP_CNT		134

#define EOL_NEW_DC_LOW_SKIP_CNT			134
#define EOL_NEW_DC_MIDDLE_SKIP_CNT		134
#define EOL_NEW_DC_HIGH_SKIP_CNT		134
#define EOL_NEW_DC_XTALK_SKIP_CNT		134

/* turnning parameter of frep_step. */
#define EOL_NEW_FREQ_MIN			385
#define EOL_NEW_FREQ_RETRY_CNT			5
#define EOL_NEW_FREQ_SKEW_RETRY_CNT		5
#define EOL_NEW_FREQ_SKIP_CNT			10

/* total buffer size. */
#define EOL_NEW_HRM_SIZE			400 /* <=this size should be lager than below size. */
#define EOL_NEW_FLICKER_SIZE			256
#define EOL_NEW_DC_LOW_SIZE			256
#define EOL_NEW_DC_MIDDLE_SIZE			256
#define EOL_NEW_DC_HIGH_SIZE			256
#define EOL_NEW_DC_XTALK_SIZE			256

/* the led current of DC step */
#define EOL_NEW_DC_LOW_LED_IR_CURRENT		0x53
#define EOL_NEW_DC_LOW_LED_RED_CURRENT		0x53
#define EOL_NEW_DC_LOW_LED_GREEN_CURRENT	0x53
#define EOL_NEW_DC_LOW_LED_BLUE_CURRENT		0x53
#define EOL_NEW_DC_MID_LED_IR_CURRENT		0xA6
#define EOL_NEW_DC_MID_LED_RED_CURRENT		0xA6
#define EOL_NEW_DC_MID_LED_GREEN_CURRENT	0xA6
#define EOL_NEW_DC_MID_LED_BLUE_CURRENT		0xA6
#define EOL_NEW_DC_HIGH_LED_IR_CURRENT		0xFA
#define EOL_NEW_DC_HIGH_LED_RED_CURRENT		0xFA
#define EOL_NEW_DC_HIGH_LED_GREEN_CURRENT	0xE9
#define EOL_NEW_DC_HIGH_LED_BLUE_CURRENT	0xE9
/* the led current of Frequency step */
#define EOL_NEW_FREQUENCY_LED_IR_CURRENT	0xFA
#define EOL_NEW_FREQUENCY_LED_RED_CURRENT	0xFA
#define EOL_NEW_FREQUENCY_LED_GREEN_CURRENT	0xFA
#define EOL_NEW_FREQUENCY_LED_BLUE_CURRENT	0xFA

#define EOL_NEW_HRM_COMPLETE_ALL_STEP		0x0F
#define EOL_SUCCESS				0x1
#define EOL_NEW_TYPE				0x1

#include "max86915.h"

int hrm_debug = 1;
int hrm_info;

module_param(hrm_debug, int, S_IRUGO | S_IWUSR);
module_param(hrm_info, int, S_IRUGO | S_IWUSR);

static void __max869_init_eol_data(struct max86915_eol_data *eol_data);
static void __max869_eol_flicker_data(int ir_data, struct  max86915_eol_data *eol_data);
static void __max869_eol_hrm_data(int ir_data, int red_data,  int green_data, int blue_data,
		struct  max86915_eol_data *eol_data, u32 array_pos, u32 eol_skip_cnt, u32 eol_step_size);
static void __max869_eol_freq_data(struct  max86915_eol_data *eol_data, u8 divid);
static int __max869_eol_check_done(struct max86915_eol_data *eol_data, u8 mode, struct max869_device_data *device);
static void __max869_div_float(struct max86915_div_data *div_data, u64 left_operand, u64 right_operand);
static void __max869_eol_conv2float(struct max86915_eol_data *eol_data, u8 mode, struct max869_device_data *device);
static void __max869_div_str(char *left_integer, char *left_decimal, char *right_integer,
		char *right_decimal, char *result_integer, char *result_decimal);
static void __max869_float_sqrt(char *integer_operand, char *decimal_operand);
static void __max869_plus_str(char *integer_operand, char *decimal_operand, char *result_integer, char *result_decimal);

static struct max869_device_data *max869_data;
static u8 agc_is_enabled = 1;
static u8 fifo_full_cnt = DEFAULT_FIFO_CNT;

int max869_set_current(u8 d1, u8 d2, u8 d3, u8 d4);

static int __max869_write_default_regs(void)
{
	int err = 0;

	return err;
}

static int __max869_write_reg(struct max869_device_data *device,
	u8 reg_addr, u8 data)
{
	int err;
	int tries = 0;
#ifdef CONFIG_SPI_TO_I2C_FPGA
	int num = 0;
#else
	int num = 1;
	u8 buffer[2] = { reg_addr, data };
	struct i2c_msg msgs[] = {
		{
			.addr = device->client->addr,
			.flags = device->client->flags & I2C_M_TEN,
			.len = 2,
			.buf = buffer,
		},
	};
#endif

	mutex_lock(&device->suspendlock);

	if (!device->pm_state) {
		HRM_dbg("%s - write error, pm suspend\n", __func__);
		err = -EFAULT;
		mutex_unlock(&device->suspendlock);
		return err;
	}

	do {
#ifdef CONFIG_SPI_TO_I2C_FPGA
		err = fpga_i2c_exp_write(0x02, device->client->addr, reg_addr, 1, &data, 1);
#else
		err = i2c_transfer(device->client->adapter, msgs, num);
#endif
		if (err != num)
			msleep_interruptible(MAX86915_I2C_RETRY_DELAY);
		if (err < 0)
			HRM_dbg("%s - i2c_transfer error = %d\n", __func__, err);
	} while ((err != num) && (++tries < MAX86915_I2C_MAX_RETRIES));

	mutex_unlock(&device->suspendlock);

	if (err != num) {
		HRM_dbg("%s -write transfer error:%d\n", __func__, err);
		err = -EIO;
		device->i2c_err_cnt++;
		return err;
	}

	return 0;
}

static int __max869_read_reg(struct max869_device_data *device,
	u8 *buffer, int length)
{
	int err = -1;
	int tries = 0; /* # of attempts to read the device */
	u8 addr = buffer[0];
#ifdef CONFIG_SPI_TO_I2C_FPGA
	int num = 0;
#else
	int num = 2;

	struct i2c_msg msgs[] = {
		{
			.addr = device->client->addr,
			.flags = device->client->flags & I2C_M_TEN,
			.len = 1,
			.buf = buffer,
		},
		{
			.addr = device->client->addr,
			.flags = (device->client->flags & I2C_M_TEN) | I2C_M_RD,
			.len = length,
			.buf = buffer,
		},
	};
#endif

	mutex_lock(&device->suspendlock);

	if (!device->pm_state) {
		HRM_dbg("%s - read error, pm suspend\n", __func__);
		err = -EFAULT;
		mutex_unlock(&device->suspendlock);
		return err;
	}

	do {
#ifdef CONFIG_SPI_TO_I2C_FPGA
		err = fpga_i2c_exp_read(0x02, device->client->addr, addr, 1, buffer, length);
#else
		buffer[0] = addr;
		err = i2c_transfer(device->client->adapter, msgs, num);
#endif
		if (err != num)
			msleep_interruptible(MAX86915_I2C_RETRY_DELAY);
		if (err < 0)
			HRM_dbg("%s - i2c_transfer error = %d\n", __func__, err);
	} while ((err != num) && (++tries < MAX86915_I2C_MAX_RETRIES));

	mutex_unlock(&device->suspendlock);

	if (err != num) {
		HRM_dbg("%s -read transfer error:%d\n", __func__, err);
		err = -EIO;
		device->i2c_err_cnt++;
	} else
		err = 0;

	return err;
}
static int __max86915_hrm_enable(struct max869_device_data *data)
{
	int err;
	u8 flex_config[2] = {0, };

	data->sample_cnt = 0;
	data->agc_led_set = 0;

	data->agc_ch_adc[0] = 0;
	data->agc_ch_adc[1] = 0;
	data->agc_ch_adc[2] = 0;
	data->agc_ch_adc[3] = 0;

	flex_config[0] = (RED_LED_CH << MAX86915_LED2_OFFSET) | IR_LED_CH;
	flex_config[1] = 0x00;
#ifdef CONFIG_HRM_GREEN_BLUE
	flex_config[1] = (BLUE_LED_CH << MAX86915_LED4_OFFSET) | GREEN_LED_CH;
#endif

	data->num_samples = 2;
	data->flex_mode = 3;
#ifdef CONFIG_HRM_GREEN_BLUE
	data->num_samples = 4;
	data->flex_mode = 0xf;
#endif

	data->led_current1 = MAX86915_HRM_DEFAULT_CURRENT1;
	data->led_current2 = MAX86915_HRM_DEFAULT_CURRENT2;
#ifdef CONFIG_HRM_GREEN_BLUE
	data->led_current3 = MAX86915_HRM_DEFAULT_CURRENT3;
	data->led_current4 = MAX86915_HRM_DEFAULT_CURRENT4;
#endif
	data->xtalk_code1 = MAX86915_HRM_DEFAULT_XTALK_CODE1;
	data->xtalk_code2 = MAX86915_HRM_DEFAULT_XTALK_CODE2;
#ifdef CONFIG_HRM_GREEN_BLUE
	data->xtalk_code3 = MAX86915_HRM_DEFAULT_XTALK_CODE3;
	data->xtalk_code4 = MAX86915_HRM_DEFAULT_XTALK_CODE4;
#endif

	HRM_info("%s - flexmode : 0x%02x, num_samples : %d\n", __func__,
			data->flex_mode, data->num_samples);

	/*write LED currents ir=1, red=2, violet=4*/
	err = __max869_write_reg(data, MAX86915_LED1_PA,
		data->led_current1);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED1_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED2_PA,
			data->led_current2);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED2_PA!\n",
			__func__);
		return -EIO;
	}

#ifdef CONFIG_HRM_GREEN_BLUE
	err = __max869_write_reg(data, MAX86915_LED3_PA,
			data->led_current3);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED3_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED4_PA,
			data->led_current4);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED4_PA!\n",
			__func__);
		return -EIO;
	}
#endif
	/* LED Range */
	err = __max869_write_reg(data, MAX86915_LED_RANGE,
			  (MAX86915_DEFAULT_LED_RGE << MAX86915_LED1_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED2_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED3_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED4_RGE_OFFSET));
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_RANGE!\n",
			__func__);
		return -EIO;
	}
	/* XTALK */
	err = __max869_write_reg(data, MAX86915_DAC1_XTALK_CODE,
			data->xtalk_code1);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC1_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC2_XTALK_CODE,
			data->xtalk_code2);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC2_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
#ifdef CONFIG_HRM_GREEN_BLUE
	err = __max869_write_reg(data, MAX86915_DAC3_XTALK_CODE,
			data->xtalk_code3);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC3_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC4_XTALK_CODE,
			data->xtalk_code4);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC4_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
#endif

	err = __max869_write_reg(data, MAX86915_INTERRUPT_ENABLE, PPG_RDY_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_INTERRUPT_ENABLE!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_1,
			flex_config[0]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_1!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_2,
			flex_config[1]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_2!\n",
			__func__);
		return -EIO;
	}

	/* 400 Hz Sampling Rate */
	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
			0x0D | (0x03 << MAX86915_ADC_RGE_OFFSET));
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_FIFO_CONFIG,
			(0x02 << MAX86915_SMP_AVE_OFFSET) & MAX86915_SMP_AVE_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x03);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}

	/* AGC control */
	data->reached_thresh[AGC_IR] = 0;
	data->reached_thresh[AGC_RED] = 0;
#ifdef CONFIG_HRM_GREEN_BLUE
	data->reached_thresh[AGC_GREEN] = 0;
	data->reached_thresh[AGC_BLUE] = 0;
#endif
	data->agc_sum[AGC_IR] = 0;
	data->agc_sum[AGC_RED] = 0;
#ifdef CONFIG_HRM_GREEN_BLUE
	data->agc_sum[AGC_GREEN] = 0;
	data->agc_sum[AGC_BLUE] = 0;
#endif
	data->agc_current[AGC_IR] =
		(data->led_current1 * MAX86915_CURRENT_PER_STEP);
	data->agc_current[AGC_RED] =
		(data->led_current2 * MAX86915_CURRENT_PER_STEP);
#ifdef CONFIG_HRM_GREEN_BLUE
	data->agc_current[AGC_GREEN] =
		(data->led_current3 * MAX86915_CURRENT_PER_STEP);
	data->agc_current[AGC_BLUE] =
		(data->led_current4 * MAX86915_CURRENT_PER_STEP);
#endif
	return 0;
}

static int __max86915_sdk_enable(struct max869_device_data *data)
{
	int err;
	u8 flex_config[2] = {0, };

	data->sample_cnt = 0;
	data->agc_led_set = 0;

	data->agc_ch_adc[0] = 0;
	data->agc_ch_adc[1] = 0;
	data->agc_ch_adc[2] = 0;
	data->agc_ch_adc[3] = 0;

	data->num_samples = 4;
	data->flex_mode = 0xf;

	flex_config[0] = (RED_LED_CH << MAX86915_LED2_OFFSET) | IR_LED_CH;
	flex_config[1] = (BLUE_LED_CH << MAX86915_LED4_OFFSET) | GREEN_LED_CH;

	data->led_current1 = MAX86915_MIN_CURRENT;
	data->led_current2 = MAX86915_MIN_CURRENT;
	data->led_current3 = MAX86915_MIN_CURRENT;
	data->led_current4 = MAX86915_MIN_CURRENT;

	/*
	 * data->led_current1 = (data->mode_sdk_enabled & (1<<0)) ? 0xff : 0x00;
	 * data->led_current2 = (data->mode_sdk_enabled & (1<<1)) ? 0xff : 0x00;
	 * data->led_current3 = (data->mode_sdk_enabled & (1<<2)) ? 0xff : 0x00;
	 * data->led_current4 = (data->mode_sdk_enabled & (1<<3)) ? 0xff : 0x00;
	 * data->xtalk_code1 = MAX86915_DEFAULT_XTALK_CODE1;
	 * data->xtalk_code2 = MAX86915_DEFAULT_XTALK_CODE2;
	 * data->xtalk_code3 = MAX86915_DEFAULT_XTALK_CODE3;
	 * data->xtalk_code4 = MAX86915_DEFAULT_XTALK_CODE4;
	 */

	HRM_info("%s - flexmode : 0x%02x, num_samples : %d\n", __func__,
			data->flex_mode, data->num_samples);

	/* write LED currents ir=1, red=2, violet=4 */
	err = __max869_write_reg(data, MAX86915_LED1_PA,
		data->led_current1);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED1_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED2_PA,
			data->led_current2);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED2_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED3_PA,
			data->led_current3);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED3_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED4_PA,
			data->led_current4);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED4_PA!\n",
			__func__);
		return -EIO;
	}

	/* LED Range */
	err = __max869_write_reg(data, MAX86915_LED_RANGE,
			  (MAX86915_DEFAULT_LED_RGE << MAX86915_LED1_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED2_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED3_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED4_RGE_OFFSET));
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_RANGE!\n",
			__func__);
		return -EIO;
	}

	/* XTALK */
	err = __max869_write_reg(data, MAX86915_DAC1_XTALK_CODE,
			data->xtalk_code1);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC1_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC2_XTALK_CODE,
			data->xtalk_code2);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC2_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC3_XTALK_CODE,
			data->xtalk_code3);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC3_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC4_XTALK_CODE,
			data->xtalk_code4);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC4_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_INTERRUPT_ENABLE, A_FULL_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_INTERRUPT_ENABLE!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_1,
			flex_config[0]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_1!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_2,
			flex_config[1]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_2!\n",
			__func__);
		return -EIO;
	}

#ifdef ENABLE_POLL_DELAY
	if (fifo_full_cnt == 8) {
		/* 16uA, 800Hz, 70us */
		err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
				0x50);
		/* 1 Samples avg */
		err = __max869_write_reg(data, MAX86915_FIFO_CONFIG,
				0x1f);
	} else if (fifo_full_cnt == 4) {
		/* 16uA, 400Hz, 120us */
		err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
				0x50);
		/* 2 Samples avg */
		err = __max869_write_reg(data, MAX86915_FIFO_CONFIG,
				0x3f);
	} else if (fifo_full_cnt == 2) {
		/* 16uA, 200Hz, 120us */
		err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
				0x50);
		/* 4 Samples avg */
		err = __max869_write_reg(data, MAX86915_FIFO_CONFIG,
				0x5f);
	} else if (fifo_full_cnt == 1) {
		/* 16uA, 400Hz, 120us */
		err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
				0x4D);
		/* 4 Samples avg */
		err = __max869_write_reg(data, MAX86915_FIFO_CONFIG,
				0x5f);
	}
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
			__func__);
		HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
			__func__);
		return -EIO;
	}
#else

	/* 400 Hz Sampling Rate & 120us Pulse Width */
	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
			0x0D | (0x03 << MAX86915_ADC_RGE_OFFSET));
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_FIFO_CONFIG,
			(0x02 << MAX86915_SMP_AVE_OFFSET) & MAX86915_SMP_AVE_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
			__func__);
		return -EIO;
	}
#endif

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x03);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}

	/* AGC control */
	data->reached_thresh[AGC_IR] = 0;
	data->reached_thresh[AGC_RED] = 0;
	data->reached_thresh[AGC_GREEN] = 0;
	data->reached_thresh[AGC_BLUE] = 0;
	data->agc_sum[AGC_IR] = 0;
	data->agc_sum[AGC_RED] = 0;
	data->agc_sum[AGC_GREEN] = 0;
	data->agc_sum[AGC_BLUE] = 0;
	data->agc_current[AGC_IR] =
		(data->led_current1 * MAX86915_CURRENT_PER_STEP);
	data->agc_current[AGC_RED] =
		(data->led_current2 * MAX86915_CURRENT_PER_STEP);
	data->agc_current[AGC_GREEN] =
		(data->led_current3 * MAX86915_CURRENT_PER_STEP);
	data->agc_current[AGC_BLUE] =
		(data->led_current4 * MAX86915_CURRENT_PER_STEP);
	return 0;
}

static int __max86915_disable(struct max869_device_data *data)
{
	int err;

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, MAX86915_RESET_MASK);
	if (err != 0) {
		HRM_dbg("%s - error init MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		goto i2c_err;
	}
	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, MAX86915_SHDN_MASK);
	if (err != 0) {
		HRM_dbg("%s - error init MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		goto i2c_err;
	}
	data->awb_sample_cnt = 0;
	data->flicker_data_cnt = 0;

	data->led_current1 = 0;
	data->led_current2 = 0;
	data->led_current3 = 0;
	data->led_current4 = 0;

	return 0;

i2c_err:
	return -EIO;
}

static int __max86915_init_device(struct max869_device_data *data)
{
	int err = 0;
	u8 recvData;

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, MAX86915_RESET_MASK);
	if (err != 0) {
		HRM_dbg("%s - error sw shutdown device!\n", __func__);
		return -EIO;
	}

	/* Interrupt Clear */
	recvData = MAX86915_INTERRUPT_STATUS;
	err = __max869_read_reg(data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}

	/* Interrupt2 Clear */
	recvData = MAX86915_INTERRUPT_STATUS_2;
	err = __max869_read_reg(data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}
	HRM_info("%s done, part_type = %u\n", __func__, data->part_type);

	return err;
}

static int __max869_init(struct max869_device_data *data)
{
	int err = 0;

	__max86915_init_device(data);

	if (err < 0)
		HRM_dbg("%s init fail, err  = %d\n", __func__, err);

	err = __max869_write_default_regs();

	HRM_info("%s init done, err  = %d\n", __func__, err);

	data->agc_enabled = 1;

	return err;
}

static int __max869_enable(struct max869_device_data *data)
{
	int err = 0;

	if (data->hrm_mode == MODE_SDK_IR)
		err = __max86915_sdk_enable(data);
	else
		err = __max86915_hrm_enable(data);

	if (err < 0)
		HRM_dbg("%s enable fail, err  = %d\n", __func__, err);

	HRM_info("%s enable done, err  = %d\n", __func__, err);

	return err;
}

static int __max869_disable(struct max869_device_data *data)
{
	int err = 0;

	err = __max86915_disable(data);

	if (err < 0)
		HRM_dbg("%s disable fail, err  = %d\n", __func__, err);

	HRM_info("%s disable done, err  = %d\n", __func__, err);

	return err;
}

static int __max869_set_reg_hrm(struct max869_device_data *data)
{
	int err = 0;

	err = __max869_init(data);
	err = __max869_enable(data);
	data->agc_mode = M_HRM;
	data->agc_enabled = 0;

	return err;
}

static int __max869_set_reg_sdk(struct max869_device_data *data)
{
	int err = 0;

	err = __max869_init(data);
	err = __max869_enable(data);
	data->agc_mode = M_SDK;
	/* data->agc_enabled = 0; */

	return err;
}

static int __max869_set_reg_ambient(struct max869_device_data *data)
{
	int err = 0;
	u8 recvData = 0;

	err = __max869_init(data);
	err = __max869_enable(data);

	/* Mode change to AWB */
	err = __max869_write_reg(data,
		MAX86915_MODE_CONFIGURATION, MAX86915_RESET_MASK);
	if (err != 0) {
		HRM_dbg("%s - error sw shutdown data!\n", __func__);
		return -EIO;
	}

	/* Interrupt Clear */
	recvData = MAX86915_INTERRUPT_STATUS;
	err = __max869_read_reg(data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}

	/* Interrupt2 Clear */
	recvData = MAX86915_INTERRUPT_STATUS_2;
	err = __max869_read_reg(data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}
	/* 400Hz, LED_PW=400us, SPO2_ADC_RANGE=4096nA */
	err = __max869_write_reg(data,
		MAX86915_MODE_CONFIGURATION_2, 0x0F);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data,
		MAX86915_FIFO_CONFIG, ((32 - AWB_INTERVAL) & MAX86915_FIFO_A_FULL_MASK));
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data,
		MAX86915_INTERRUPT_ENABLE, A_FULL_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_INTERRUPT_ENABLE!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_1,
			0x01);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_1!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data,
			MAX86915_MODE_CONFIGURATION, 0x07);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}
	data->awb_sample_cnt = 0;
	data->flicker_data_cnt = 0;
	data->awb_flicker_status = AWB_CONFIG1;
	data->agc_mode = M_NONE;

	return err;
}

static int __max869_set_reg_prox(struct max869_device_data *data)
{
	int err = 0;

	err = __max869_init(data);
	err = __max869_enable(data);

	max869_set_current(0xff, 0, 0, 0);
	data->agc_mode = M_NONE;

	return err;
}

static int max86915_update_led_current(struct max869_device_data *data,
	int new_led_uA,
	int led_num)
{
	int err;
	u8 led_reg_val;
	int led_reg_addr;
	u8 recvData;

	if (new_led_uA > MAX86915_MAX_CURRENT) {
		HRM_dbg("%s - Tried to set LED%d to %duA. Max is %duA\n",
				__func__, led_num, new_led_uA, MAX86915_MAX_CURRENT);
		new_led_uA = MAX86915_MAX_CURRENT;
	} else if (new_led_uA < MAX86915_MIN_CURRENT) {
		HRM_dbg("%s - Tried to set LED%d to %duA. Min is %duA\n",
				__func__, led_num, new_led_uA, MAX86915_MIN_CURRENT);
		new_led_uA = MAX86915_MIN_CURRENT;
	}

	led_reg_val = new_led_uA / MAX86915_CURRENT_PER_STEP;

	led_reg_addr = MAX86915_LED1_PA + led_num;

	HRM_dbg("%s - Setting ADD 0x%x, LED%d to 0x%.2X (%duA)\n",
			__func__, led_reg_addr, led_num, led_reg_val, new_led_uA);

	if (data->part_type < PART_TYPE_MAX86915_CS_211) {
		err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x00);
		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n", __func__);
			return -EIO;
		}
	}

	err = __max869_write_reg(data, led_reg_addr, led_reg_val);

	if (data->part_type < PART_TYPE_MAX86915_CS_211) {
		recvData = MAX86915_FIFO_CONFIG;
		err = __max869_read_reg(data, &recvData, 1);
		data->agc_led_set = AGC_LED_SKIP_CNT >> ((recvData & MAX86915_SMP_AVE_MASK) >> MAX86915_SMP_AVE_OFFSET);

		err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x03);
	}

	if (err != 0) {
		HRM_dbg("%s - error initializing register 0x%.2X!\n",
			__func__, led_reg_addr);
		return -EIO;
	}
	switch (led_num) {
	case AGC_IR:
		data->led_current1 = led_reg_val; /* set ir */
		break;
	case AGC_RED:
		data->led_current2 = led_reg_val; /* set red */
		break;
	case AGC_GREEN:
		data->led_current3 = led_reg_val; /* set green */
		break;
	case AGC_BLUE:
		data->led_current4 = led_reg_val; /* set blue */
		break;
	}

	return 0;
}
#ifdef MAXIM_AGC
static int agc_adj_calculator(struct max869_device_data *data,
			s32 *change_by_percent_of_range,
			s32 *change_by_percent_of_current_setting,
			s32 *change_led_by_absolute_count,
			s32 *set_led_to_absolute_count,
			s32 target_percent_of_range,
			s32 correction_coefficient,
			s32 allowed_error_in_percentage,
			s32 *reached_thresh,
			s32 current_average,
			s32 number_of_samples_averaged,
			s32 led_drive_current_value)
{
	s32 diode_min_val;
	s32 diode_max_val;
	s32 diode_min_current;
	s32 diode_fs_current;

	s32 current_percent_of_range = 0;
	s32 delta = 0;
	s32 desired_delta = 0;
	s32 current_power_percent = 0;

	if (change_by_percent_of_range == 0
			 || change_by_percent_of_current_setting == 0
			 || change_led_by_absolute_count == 0
			 || set_led_to_absolute_count == 0)
		return ILLEGAL_OUTPUT_POINTER;

	if (target_percent_of_range > 90 || target_percent_of_range < 10)
		return CONSTRAINT_VIOLATION;

	if (correction_coefficient > 100 || correction_coefficient < 0)
		return CONSTRAINT_VIOLATION;

	if (allowed_error_in_percentage > 100
			|| allowed_error_in_percentage < 0)
		return CONSTRAINT_VIOLATION;

#if ((MAX86915_MAX_DIODE_VAL-MAX86915_MIN_DIODE_VAL) <= 0 \
			 || (MAX86915_MAX_DIODE_VAL < 0) || (MAX86915_MIN_DIODE_VAL < 0))
	#error "Illegal max86915 diode Min/Max Pair"
#endif

#if ((MAX86915_CURRENT_FULL_SCALE) <= 0 \
		|| (MAX86915_MAX_CURRENT < 0) || (MAX86915_MIN_CURRENT < 0))
	#error "Illegal max86915 LED Min/Max current Pair"
#endif

	diode_min_val = MAX86915_MIN_DIODE_VAL;
	diode_max_val = MAX86915_MAX_DIODE_VAL;
	diode_min_current = MAX86915_MIN_CURRENT;
	diode_fs_current = MAX86915_CURRENT_FULL_SCALE;
	if (led_drive_current_value > MAX86915_MAX_CURRENT
			|| led_drive_current_value < MAX86915_MIN_CURRENT)
		return CONSTRAINT_VIOLATION;

	if (current_average < MAX86915_MIN_DIODE_VAL
				|| current_average > MAX86915_MAX_DIODE_VAL)
		return CONSTRAINT_VIOLATION;

	current_percent_of_range = 100 *
		(current_average - diode_min_val) /
		(diode_max_val - diode_min_val);

	delta = current_percent_of_range - target_percent_of_range;
	delta = delta * correction_coefficient / 100;

	if (!(*reached_thresh))
		allowed_error_in_percentage =
			allowed_error_in_percentage * 2 / 7;

	if (delta > -allowed_error_in_percentage
			&& delta < allowed_error_in_percentage) {
		*change_by_percent_of_range = 0;
		*change_by_percent_of_current_setting = 0;
		*change_led_by_absolute_count = 0;
		*set_led_to_absolute_count = led_drive_current_value;
		*reached_thresh = 1;
		return 0;
	}

	current_power_percent = 100 *
			(led_drive_current_value - diode_min_current) /
			diode_fs_current;

	if (delta < 0) {
		if (current_power_percent > 97
			&& (current_percent_of_range
				< (data->threshold_default * 100) / (diode_max_val - diode_min_val))) {
			desired_delta = -delta * (102 - current_power_percent) /
				(100 - current_percent_of_range);
		} else
			desired_delta = -delta * (100 - current_power_percent) /
				(100 - current_percent_of_range);
	}

	if (delta > 0)
		desired_delta = -delta * (current_power_percent)
				/ (current_percent_of_range);

	*change_by_percent_of_range = desired_delta;

	*change_led_by_absolute_count = (desired_delta
			* diode_fs_current / 100);
	*change_by_percent_of_current_setting =
			(*change_led_by_absolute_count * 100)
			/ (led_drive_current_value);
	*set_led_to_absolute_count = led_drive_current_value
			+ *change_led_by_absolute_count;

	if (*set_led_to_absolute_count > MAX86915_MAX_CURRENT)
		*set_led_to_absolute_count = MAX86915_MAX_CURRENT;

	return 0;
}
#else
int __linear_search_agc(struct max869_device_data *data, int ppg_average, int led_num)
{

	int xtalk_adc = 0;
	int target_adc = MAX86915_MAX_DIODE_VAL * data->agc_led_out_percent / 100;
	long long target_current;
	int agc_range = 8;

	if (ppg_average < MAX86915_MIN_DIODE_VAL
			|| ppg_average > MAX86915_MAX_DIODE_VAL)
		return CONSTRAINT_VIOLATION;

	HRM_info("%s, ppg_average : %d\n", __func__, ppg_average);

	if (ppg_average == MAX86915_MAX_DIODE_VAL) {
		if (data->prev_agc_current[led_num] == MAX86915_MAX_CURRENT)
			target_current = data->agc_current[led_num] / 2;
		else
			target_current = (data->agc_current[led_num] + data->prev_agc_current[led_num]) / 2;

		if (target_current > MAX86915_MAX_CURRENT)
			target_current = MAX86915_MAX_CURRENT;
		else if (target_current < MAX86915_MIN_CURRENT)
			target_current = MAX86915_MIN_CURRENT;

		data->prev_agc_current[led_num] = data->agc_current[led_num];
		data->change_led_by_absolute_count[led_num] = target_current - data->agc_current[led_num];
		data->agc_current[led_num] = target_current;
		return 0;
	} else if (ppg_average == MAX86915_MIN_DIODE_VAL) {
		if (data->prev_agc_current[led_num] < data->agc_current[led_num])
			data->prev_agc_current[led_num] = MAX86915_MAX_CURRENT;
		target_current = (data->agc_current[led_num]
			+ data->prev_agc_current[led_num] + MAX86915_CURRENT_PER_STEP) / 2;

		if (target_current > MAX86915_MAX_CURRENT)
			target_current = MAX86915_MAX_CURRENT;
		else if (target_current < MAX86915_MIN_CURRENT)
			target_current = MAX86915_MIN_CURRENT;

		data->prev_agc_current[led_num] = data->agc_current[led_num];
		data->change_led_by_absolute_count[led_num] = target_current - data->agc_current[led_num];
		data->agc_current[led_num] = target_current;
		return 0;
	}

	if (data->reached_thresh[led_num])
		agc_range = 28;

	HRM_info("%s, target : %d, ppg_average : %d\n", __func__, target_adc, ppg_average);

	if (ppg_average < MAX86915_MAX_DIODE_VAL * (70 + agc_range) / 100
		&& ppg_average > MAX86915_MAX_DIODE_VAL * (70 - agc_range) / 100) {
		data->reached_thresh[led_num] = 1;
		data->change_led_by_absolute_count[led_num] = 0;
		return 0;
	}

	switch (led_num) {
	case AGC_IR:
		xtalk_adc = MAX86915_MAX_DIODE_VAL / 2 * data->xtalk_code1 / 31;
		target_adc += xtalk_adc;
		break;
	case AGC_RED:
		xtalk_adc = MAX86915_MAX_DIODE_VAL / 2 * data->xtalk_code2 / 31;
		target_adc += xtalk_adc;
		break;
	case AGC_GREEN:
		xtalk_adc = MAX86915_MAX_DIODE_VAL / 2 * data->xtalk_code3 / 31;
		target_adc += xtalk_adc;
		break;
	case AGC_BLUE:
		xtalk_adc = MAX86915_MAX_DIODE_VAL / 2 * data->xtalk_code4 / 31;
		target_adc += xtalk_adc;
		break;
	default:
		return -EIO;
	}

	HRM_info("%s, target2 : %d\n", __func__, target_adc);

	target_current = target_adc - (ppg_average + xtalk_adc);

	if (data->agc_current[led_num] > data->prev_current[led_num])
		target_current *= (data->agc_current[led_num] - data->prev_current[led_num]);
	else if (data->agc_current[led_num] < data->prev_current[led_num])
		target_current *= (data->prev_current[led_num] - data->agc_current[led_num]);

	if (ppg_average + xtalk_adc > data->prev_ppg[led_num])
		target_current /= (ppg_average + xtalk_adc - data->prev_ppg[led_num]);
	else if (ppg_average + xtalk_adc < data->prev_ppg[led_num])
		target_current /= (data->prev_ppg[led_num] - (ppg_average + xtalk_adc));

	target_current += data->agc_current[led_num];

	if (target_current > MAX86915_MAX_CURRENT)
		target_current = MAX86915_MAX_CURRENT;
	else if (target_current < MAX86915_MIN_CURRENT)
		target_current = MAX86915_MIN_CURRENT;

	data->change_led_by_absolute_count[led_num] = target_current - data->agc_current[led_num];

	data->prev_current[led_num] = data->agc_current[led_num];
	data->agc_current[led_num] = target_current;
	data->prev_ppg[led_num] = ppg_average + xtalk_adc;

	HRM_info("%s, data->agc_current[%d] : %u, %lld\n",
		__func__, led_num, data->agc_current[led_num], target_current);

	return 0;
}
#endif

int __max869_hrm_agc(struct max869_device_data *data, int counts, int led_num)
{
	int err = 0;
	int avg = 0;

	/* HRM_info("%s - led_num(%d) = %d, %d\n", __func__, led_num, counts, data->agc_sample_cnt[led_num]); */

	if (led_num < AGC_IR || led_num > AGC_BLUE)
		return -EIO;

	data->agc_sum[led_num] += counts;
	data->agc_sample_cnt[led_num]++;

	if (data->agc_sample_cnt[led_num] < data->agc_min_num_samples)
		return 0;

	data->agc_sample_cnt[led_num] = 0;

	avg = data->agc_sum[led_num] / data->agc_min_num_samples;
	data->agc_sum[led_num] = 0;
	/* HRM_info("%s - led_num(%d) = %d, %d\n", __func__, led_num, counts, avg); */

#ifdef MAXIM_AGC
	err = agc_adj_calculator(data,
			&data->change_by_percent_of_range[led_num],
			&data->change_by_percent_of_current_setting[led_num],
			&data->change_led_by_absolute_count[led_num],
			&data->agc_current[led_num],
			data->agc_led_out_percent,
			data->agc_corr_coeff,
			data->agc_sensitivity_percent,
			&data->reached_thresh[led_num],
			avg,
			data->agc_min_num_samples,
			data->agc_current[led_num]);
#else
	err = __linear_search_agc(data, avg, led_num);
#endif

	if (err)
		return err;


	/* HRM_info("%s - %d-a:%d\n", __func__, led_num, data->change_led_by_absolute_count[led_num]); */

	if (data->change_led_by_absolute_count[led_num] == 0) {

		if (led_num == AGC_IR) {
			data->ir_adc = counts;
			data->ir_curr = data->led_current1;
		} else if (led_num == AGC_RED) {
			data->red_adc = counts;
			data->red_curr = data->led_current2;
		} else if (led_num == AGC_GREEN) {
			data->green_adc = counts;
			data->green_curr = data->led_current3;
		} else if (led_num == AGC_BLUE) {
			data->blue_adc = counts;
			data->blue_curr = data->led_current4;
		}


		/*
		 * HRM_info("%s - ir_curr = 0x%2x, red_curr = 0x%2x, ir_adc = %d, red_adc = %d\n",
		 *	__func__, data->ir_curr, data->red_curr, data->ir_adc, data->red_adc);
		 */

		return 0;
	}

	err = data->update_led(data, data->agc_current[led_num], led_num);

	/* skip 2 sample after changing current */
	max869_data->agc_enabled = 0;
	max869_data->sample_cnt = AGC_SKIP_CNT - 2;

	if (err)
		HRM_dbg("%s failed\n", __func__);

	return err;
}

static int __max869_cal_agc(int ir, int red, int green, int blue)
{
	int err = 0;
	int led_num0, led_num1, led_num2, led_num3;

	led_num0 = AGC_IR;
	led_num1 = AGC_RED;
	led_num2 = AGC_GREEN;
	led_num3 = AGC_BLUE;

	if (max869_data->agc_mode == M_HRM) {
		if (ir >= max869_data->threshold_default) { /* Detect */
			err |= __max869_hrm_agc(max869_data, ir, led_num0); /* IR */
			if (max869_data->agc_current[led_num1] != MAX86915_MIN_CURRENT) {
				err |= __max869_hrm_agc(max869_data, red, led_num1); /* RED */
			} else {
				/* init */
				max869_data->agc_current[led_num1]
					= MAX86915_RED_INIT_CURRENT * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num1] = 0;
				max869_data->prev_current[led_num1] = 0;
				max869_data->prev_agc_current[led_num1] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num1] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num1], led_num1);
			}
#ifdef CONFIG_HRM_GREEN_BLUE
			if (max869_data->agc_current[led_num2] != MAX86915_MIN_CURRENT) {
				err |= __max869_hrm_agc(max869_data, green, led_num2); /* GREEN */
			} else {
				/* init */
				max869_data->agc_current[led_num2]
					= MAX86915_GREEN_INIT_CURRENT * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num2] = 0;
				max869_data->prev_current[led_num2] = 0;
				max869_data->prev_agc_current[led_num2] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num2] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num2], led_num2);
			}

			if (max869_data->agc_current[led_num3] != MAX86915_MIN_CURRENT) {
				err |= __max869_hrm_agc(max869_data, blue, led_num3); /* BLUE */
			} else {
				/* init */
				max869_data->agc_current[led_num3]
					= MAX86915_BLUE_INIT_CURRENT * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num3] = 0;
				max869_data->prev_current[led_num3] = 0;
				max869_data->prev_agc_current[led_num3] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num3] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num3], led_num3);
			}
#endif
		} else {
			if (max869_data->agc_current[led_num1] != MAX86915_MIN_CURRENT) { /* Disable led_num1 */
				/* init */
				max869_data->agc_current[led_num1] = MAX86915_MIN_CURRENT;
				max869_data->reached_thresh[led_num1] = 0;
				max869_data->prev_current[led_num1] = 0;
				max869_data->prev_agc_current[led_num1] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num1] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num1], led_num1);
			}
#ifdef CONFIG_HRM_GREEN_BLUE
			if (max869_data->agc_current[led_num2] != MAX86915_MIN_CURRENT) { /* Disable led_num2 */
				/* init */
				max869_data->agc_current[led_num2] = MAX86915_MIN_CURRENT;
				max869_data->reached_thresh[led_num2] = 0;
				max869_data->prev_current[led_num2] = 0;
				max869_data->prev_agc_current[led_num2] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num2] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num2], led_num2);
			}
			if (max869_data->agc_current[led_num3] != MAX86915_MIN_CURRENT) { /* Disable led_num3 */
				/* init */
				max869_data->agc_current[led_num3] = MAX86915_MIN_CURRENT;
				max869_data->reached_thresh[led_num3] = 0;
				max869_data->prev_current[led_num3] = 0;
				max869_data->prev_agc_current[led_num3] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num3] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num3], led_num3);
			}
#endif
			if (max869_data->agc_current[led_num0]
					!= MAX86915_IR_INIT_CURRENT * MAX86915_CURRENT_PER_STEP) {
				/* init */
				max869_data->agc_current[led_num0]
					= MAX86915_IR_INIT_CURRENT * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num0] = 0;
				max869_data->prev_current[led_num0] = 0;
				max869_data->prev_agc_current[led_num0] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num0] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num0], led_num0);
			}
		}
	} else if (max869_data->agc_mode == M_HRMLED_IR) {
		if (max869_data->agc_current[led_num0] != MAX86915_MIN_CURRENT) {
			err |= __max869_hrm_agc(max869_data, ir, led_num0); /* IR */
		} else {
			max869_data->agc_current[led_num0] = MAX86915_MAX_CURRENT;
			max869_data->agc_enabled = 0;
			max869_data->sample_cnt = 0;
			max869_data->reached_thresh[led_num0] = 0;
			max869_data->prev_current[led_num0] = 0;
			max869_data->prev_agc_current[led_num0] = MAX86915_MAX_CURRENT;
			max869_data->prev_ppg[led_num0] = 0;
			max869_data->update_led(max869_data,
					max869_data->agc_current[led_num0], led_num0);
		}
		if (max869_data->agc_current[led_num1] != 0) { /* Disable led_num1 */
			/* init */
			max869_data->agc_current[led_num1] = 0;
			max869_data->reached_thresh[led_num1] = 0;
			max869_data->prev_current[led_num1] = 0;
			max869_data->prev_agc_current[led_num1] = MAX86915_MAX_CURRENT;
			max869_data->prev_ppg[led_num1] = 0;
			max869_data->update_led(max869_data,
					max869_data->agc_current[led_num1], led_num1);
		}
	} else if (max869_data->agc_mode == M_HRMLED_RED) {
		if (max869_data->agc_current[led_num1] != MAX86915_MIN_CURRENT) {
			err |= __max869_hrm_agc(max869_data, red, led_num1); /* RED */
		} else {
			max869_data->agc_current[led_num1] = MAX86915_MAX_CURRENT;
			max869_data->agc_enabled = 0;
			max869_data->sample_cnt = 0;
			max869_data->reached_thresh[led_num1] = 0;
			max869_data->prev_current[led_num1] = 0;
			max869_data->prev_agc_current[led_num1] = MAX86915_MAX_CURRENT;
			max869_data->prev_ppg[led_num1] = 0;
			max869_data->update_led(max869_data,
					max869_data->agc_current[led_num1], led_num1);
		}
		if (max869_data->agc_current[led_num0] != 0) { /* Disable led_num0 */
			/* init */
			max869_data->agc_current[led_num0] = 0;
			max869_data->reached_thresh[led_num0] = 0;
			max869_data->prev_current[led_num0] = 0;
			max869_data->prev_agc_current[led_num0] = MAX86915_MAX_CURRENT;
			max869_data->prev_ppg[led_num0] = 0;
			max869_data->update_led(max869_data,
					max869_data->agc_current[led_num0], led_num0);
		}
	} else if (max869_data->agc_mode == M_HRMLED_BOTH) {
		if (max869_data->agc_current[led_num0] != MAX86915_MIN_CURRENT) {
			err |= __max869_hrm_agc(max869_data, ir, led_num0); /* IR */
		} else {
			max869_data->agc_current[led_num0] = MAX86915_MAX_CURRENT;
			max869_data->agc_enabled = 0;
			max869_data->sample_cnt = 0;
			max869_data->reached_thresh[led_num0] = 0;
			max869_data->prev_current[led_num0] = 0;
			max869_data->prev_agc_current[led_num0] = MAX86915_MAX_CURRENT;
			max869_data->prev_ppg[led_num0] = 0;
			max869_data->update_led(max869_data,
					max869_data->agc_current[led_num0], led_num0);
		}
		if (max869_data->agc_current[led_num1] != MAX86915_MIN_CURRENT) {
			err |= __max869_hrm_agc(max869_data, red, led_num1); /* RED */
		} else {
			max869_data->agc_current[led_num1] = MAX86915_MAX_CURRENT;
			max869_data->agc_enabled = 0;
			max869_data->sample_cnt = 0;
			max869_data->reached_thresh[led_num1] = 0;
			max869_data->prev_current[led_num1] = 0;
			max869_data->prev_agc_current[led_num1] = MAX86915_MAX_CURRENT;
			max869_data->prev_ppg[led_num1] = 0;
			max869_data->update_led(max869_data,
					max869_data->agc_current[led_num1], led_num1);
		}
	} else if (max869_data->agc_mode == M_SDK) {
		if (max869_data->mode_sdk_enabled & 0x01) { /* IR channel */
			if (max869_data->agc_current[led_num0] != MAX86915_MIN_CURRENT) {
				err |= __max869_hrm_agc(max869_data, ir, led_num0);
			} else {
				max869_data->agc_current[led_num0]
					= max869_data->default_current1 * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num0] = 0;
				max869_data->prev_current[led_num0] = 0;
				max869_data->prev_agc_current[led_num0] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num0] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num0], led_num0);
			}
		}
		if (max869_data->mode_sdk_enabled & 0x02) { /* RED channel */
			if (max869_data->agc_current[led_num1] != MAX86915_MIN_CURRENT) {
				err |= __max869_hrm_agc(max869_data, red, led_num1);
			} else {
				max869_data->agc_current[led_num1]
					= max869_data->default_current2 * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num1] = 0;
				max869_data->prev_current[led_num1] = 0;
				max869_data->prev_agc_current[led_num1] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num1] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num1], led_num1);
			}
		}
		if (max869_data->mode_sdk_enabled & 0x04) { /* Green channel */
			if (max869_data->agc_current[led_num2] != MAX86915_MIN_CURRENT) {
				err |= __max869_hrm_agc(max869_data, green, led_num2);
			} else {
				max869_data->agc_current[led_num2]
					= max869_data->default_current3 * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num2] = 0;
				max869_data->prev_current[led_num2] = 0;
				max869_data->prev_agc_current[led_num2] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num2] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num2], led_num2);
			}
		}
		if (max869_data->mode_sdk_enabled & 0x08) { /* BLUE channel */
			if (max869_data->agc_current[led_num3] != MAX86915_MIN_CURRENT) {
				err |= __max869_hrm_agc(max869_data, blue, led_num3);
			} else {
				max869_data->agc_current[led_num3]
					= max869_data->default_current4 * MAX86915_CURRENT_PER_STEP;
				max869_data->agc_enabled = 0;
				max869_data->sample_cnt = 0;
				max869_data->reached_thresh[led_num3] = 0;
				max869_data->prev_current[led_num3] = 0;
				max869_data->prev_agc_current[led_num3] = MAX86915_MAX_CURRENT;
				max869_data->prev_ppg[led_num3] = 0;
				max869_data->update_led(max869_data,
						max869_data->agc_current[led_num3], led_num3);
			}
		}
	}

	return err;
}

static int __max869_write_reg_to_file(void)
{
	int reg, err;
	u8 recvData;

	for (reg = 0; reg < 0x55; reg++) {
		recvData = reg;

		err = __max869_read_reg(max869_data, &recvData, 1);
		if (err != 0) {
			HRM_dbg("%s - error reading 0x%02x err:%d\n",
				__func__, reg, err);
		} else {
			HRM_dbg("%s - 0x%02x = 0x%02x\n",
				__func__, reg, recvData);
		}
	}
	return 0;
}


static int __max869_write_file_to_reg(void)
{
	return -EIO;
}

static void __max869_print_mode(int mode)
{
	HRM_info("%s - ===== mode(%d) =====\n", __func__, mode);
	if (mode == MODE_HRM)
		HRM_info("%s - %s(%d)\n", __func__, "HRM", MODE_HRM);
	if (mode == MODE_AMBIENT)
		HRM_info("%s - %s(%d)\n", __func__, "AMBIENT", MODE_AMBIENT);
	if (mode == MODE_PROX)
		HRM_info("%s - %s(%d)\n", __func__, "PROX", MODE_PROX);
	if (mode == MODE_SDK_IR)
		HRM_info("%s - %s(%d)\n", __func__, "SDK", MODE_SDK_IR);
	if (mode == MODE_UNKNOWN)
		HRM_info("%s - %s(%d)\n", __func__, "UNKNOWN", MODE_UNKNOWN);
	HRM_info("%s - ====================\n", __func__);
}


static int __max869_get_part_id(struct max869_device_data *data)
{
	u8 recvData;
	u8 recvDataSub;
	int err;

	err = __max869_write_reg(data, 0xFF, 0x54);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_TEST0!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, 0xFF, 0x4d);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_TEST1!\n",
			__func__);
		return -EIO;
	}

	recvData = 0xC1;
	err = __max869_read_reg(data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}

	err = __max869_write_reg(data, 0xFF, 0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86900_MODE_TEST0!\n",
			__func__);
		return -EIO;
	}

	HRM_dbg("%s - 0xC1 :%x\n", __func__, recvData);

	if (recvData == 0x83)
		return PART_TYPE_MAX86915_ES;
	else if (recvData == 0x86)
		return PART_TYPE_MAX86915_CS_15;
	else if (recvData == 0x88) {
		recvDataSub = 0xFE;
		err = __max869_read_reg(data, &recvDataSub, 1);
		if (err != 0) {
			HRM_dbg("%s - __max869_read_reg err:%d, address:0x%02x\n",
				__func__, err, recvDataSub);
			return -EIO;
		}
		if (recvDataSub == 0x41)
			return PART_TYPE_MAX86915_CS_21;
		else if (recvDataSub == 0xC1)
			return PART_TYPE_MAX86915_CS_22;
		else if (recvDataSub == 0x42)
			return PART_TYPE_MAX86915_CS_211;
		else if (recvDataSub == 0xC2)
			return PART_TYPE_MAX86915_CS_221;

		HRM_info("%s - recvDataSub:0x%02x\n", __func__, recvDataSub);

		return PART_TYPE_NONE;
	}

	HRM_info("%s - recvData:0x%02x\n", __func__, recvData);

	recvDataSub = 0xFE;

	err = __max869_read_reg(data, &recvDataSub, 1);
	if (err != 0) {
		HRM_dbg("%s - __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvDataSub);
		return -EIO;
	}

	if (recvDataSub == 0x41)
		return PART_TYPE_MAX86915_CS_21;
	else if (recvDataSub == 0xC1)
		return PART_TYPE_MAX86915_CS_22;
	else if (recvDataSub == 0x42)
		return PART_TYPE_MAX86915_CS_211;
	else if (recvDataSub == 0xC2)
		return PART_TYPE_MAX86915_CS_221;

	HRM_info("%s - recvDataSub:0x%02x\n", __func__, recvDataSub);

	return PART_TYPE_NONE;
}

int __max869_get_num_samples_in_fifo(struct max869_device_data *data)
{
	int fifo_rd_ptr;
	int fifo_wr_ptr;
	int fifo_ovf_cnt;
	char fifo_data[3];
	int num_samples = 0;
	int ret;

	fifo_data[0] = MAX86915_FIFO_WRITE_POINTER;
	ret = __max869_read_reg(data, fifo_data, 3);
	if (ret < 0) {
		HRM_dbg("%s failed. ret: %d\n", __func__, ret);
		return ret;
	}

	fifo_wr_ptr = fifo_data[0] & 0x1F;
	fifo_ovf_cnt = fifo_data[1] & 0x1F;
	fifo_rd_ptr = fifo_data[2] & 0x1F;

	if (fifo_ovf_cnt || fifo_rd_ptr == fifo_wr_ptr) {
		if (fifo_ovf_cnt > 0)
			HRM_dbg("###FALTAL### FIFO is Full. OVF: %d RD:%d WR:%d\n",
					fifo_ovf_cnt, fifo_rd_ptr, fifo_wr_ptr);
		num_samples = MAX86915_FIFO_SIZE;
	} else {
		if (fifo_wr_ptr > fifo_rd_ptr)
			num_samples = fifo_wr_ptr - fifo_rd_ptr;
		else if (fifo_wr_ptr < fifo_rd_ptr)
			num_samples = MAX86915_FIFO_SIZE +
						fifo_wr_ptr - fifo_rd_ptr;
	}
	return num_samples;
}

int __max869_get_fifo_settings(struct max869_device_data *data, u16 *ch)
{
	int num_item = 0;
	int i;
	u16 tmp;
	int ret;
	u8 buf[2];

	/*
	 * TODO: Somehow when the data corrupted in the bus,
	 * and i2c functions don't return error messages.
	 */

	buf[0] = MAX86915_LED_SEQ_REG_1;
	ret = __max869_read_reg(data, buf, 2);
	if (ret < 0)
		return ret;

	tmp = ((u16)buf[1] << 8) | (u16)buf[0];
	*ch = tmp;
	for (i = 0; i < 4; i++) {
		if (tmp & 0x000F)
			num_item++;
		tmp >>= 4;
	}

	return num_item;
}

int __max869_read_fifo(struct max869_device_data *sd, char *fifo_buf, int num_bytes)
{
	int ret = 0;
	int data_offset = 0;
	int to_read = 0;

	while (num_bytes > 0) {
		if (num_bytes > MAX_I2C_BLOCK_SIZE) {
			to_read = MAX_I2C_BLOCK_SIZE;
			num_bytes -= to_read;
		} else {
			to_read = num_bytes;
			num_bytes = 0;
		}
		fifo_buf[data_offset] = MAX86915_FIFO_DATA;
		ret = __max869_read_reg(sd, &fifo_buf[data_offset], to_read);

		if (ret < 0)
			break;

		data_offset += to_read;
	}
	return ret;
}

int __max869_fifo_read_data(struct max869_device_data *data)
{
	int ret = 0;
	int num_samples;
	int num_bytes;
	int num_channel = 0;
	u16 fd_settings = 0;
	int i;
	int j;
	int offset1;
	int offset2;
	int index;
	int samples[16] = {0, };
	u8 fifo_buf[NUM_BYTES_PER_SAMPLE*32*MAX_LED_NUM];
	int fifo_mode = -1;

	if (data->hrm_mode == MODE_SDK_IR) {
		num_samples = fifo_full_cnt;
		num_channel = 4;
	} else if (data->eol_test_is_enable == 1) {
		num_samples = 20;
		num_channel = data->num_samples;
	} else {
		num_samples = __max869_get_num_samples_in_fifo(data);
		if (num_samples <= 0 || num_samples > MAX86915_FIFO_SIZE) {
			ret = num_samples;
			goto fail;
		}
		num_channel = __max869_get_fifo_settings(data, &fd_settings);
		if (num_channel < 0) {
			ret = num_channel;
			goto fail;
		}
	}

	num_bytes = num_channel * num_samples * NUM_BYTES_PER_SAMPLE;
	ret = __max869_read_fifo(data, fifo_buf, num_bytes);
	if (ret < 0)
		goto fail;

	/* clear the fifo_data buffer */
	for (i = 0; i < MAX86915_FIFO_SIZE; i++)
		for (j = 0; j < MAX_LED_NUM; j++)
			data->fifo_data[j][i] = 0;

	data->fifo_samples = 0;

	for (i = 0; i < num_samples; i++) {
		offset1 = i * NUM_BYTES_PER_SAMPLE * num_channel;
		offset2 = 0;

		for (j = 0; j < MAX_LED_NUM; j++) {
			if (data->flex_mode & (1 << j)) {
				index = offset1 + offset2;
				samples[j] = ((int)fifo_buf[index + 0] << 16)
						| ((int)fifo_buf[index + 1] << 8)
						| ((int)fifo_buf[index + 2]);
				samples[j] &= 0x7ffff;
				data->fifo_data[j][i] = samples[j];
				offset2 += NUM_BYTES_PER_SAMPLE;
			}
		}
	}
	data->fifo_samples = num_samples;

	return ret;
fail:
	HRM_dbg("%s failed. ret: %d, fifo_mode: %d\n", __func__, ret, fifo_mode);
	return ret;
}

int __max869_fifo_irq_handler(struct max869_device_data *data)
{
	int ret;

	ret = __max869_fifo_read_data(data);

	if (ret < 0) {
		HRM_dbg("%s fifo. ret: %d\n", __func__, ret);
		return ret;
	}

	return ret;
}

static void __max869_div_float(struct max86915_div_data *div_data, u64 left_operand, u64 right_operand)
{
	snprintf(div_data->left_integer, INTEGER_LEN, "%llu", left_operand);
	memset(div_data->left_decimal, '0', sizeof(div_data->left_decimal));
	div_data->left_decimal[DECIMAL_LEN - 1] = 0;

	snprintf(div_data->right_integer, INTEGER_LEN, "%llu", right_operand);
	memset(div_data->right_decimal, '0', sizeof(div_data->right_decimal));
	div_data->right_decimal[DECIMAL_LEN - 1] = 0;

	__max869_div_str(div_data->left_integer, div_data->left_decimal, div_data->right_integer,
			div_data->right_decimal, div_data->result_integer, div_data->result_decimal);
}

static void __max869_plus_str(char *integer_operand, char *decimal_operand, char *result_integer, char *result_decimal)
{
	unsigned long long a, b;
	int i;

	for (i = 0; i < DECIMAL_LEN - 1; i++) {
		decimal_operand[i] -= '0';
		result_decimal[i] -= '0';
	}

	for (i = 0; i < DECIMAL_LEN - 1; i++)
		result_decimal[i] += decimal_operand[i];

	sscanf(integer_operand, "%19llu", &a);
	sscanf(result_integer, "%19llu", &b);

	for (i = DECIMAL_LEN - 1; i >= 0; i--) {
		if (result_decimal[i] >= 10) {
			if (i > 0)
				result_decimal[i - 1] += 1;
			else
				a += 1;
			result_decimal[i] -= 10;
		}
	}

	a += b;

	for (i = 0; i < DECIMAL_LEN - 1; i++)
		result_decimal[i] += '0';

	snprintf(result_integer, INTEGER_LEN, "%llu", a);
}

static void __max869_float_sqrt(char *integer_operand, char *decimal_operand)
{
	int i;
	char left_integer[INTEGER_LEN] = { '\0', }, left_decimal[DECIMAL_LEN] = { '\0', };
	char result_integer[INTEGER_LEN] = { '\0', }, result_decimal[DECIMAL_LEN] = { '\0', };
	char temp_integer[INTEGER_LEN] = { '\0', }, temp_decimal[DECIMAL_LEN] = { '\0', };

	strncpy(left_integer, integer_operand, INTEGER_LEN - 1);
	strncpy(left_decimal, decimal_operand, DECIMAL_LEN - 1);

	for (i = 0; i < 100; i++) {
		__max869_div_str(left_integer, left_decimal, integer_operand,
			decimal_operand, result_integer, result_decimal);
		__max869_plus_str(integer_operand, decimal_operand, result_integer, result_decimal);

		snprintf(temp_integer, INTEGER_LEN, "%d", 2);
		memset(temp_decimal, '0', sizeof(temp_decimal));
		temp_decimal[DECIMAL_LEN - 1] = 0;

		__max869_div_str(result_integer, result_decimal, temp_integer,
			temp_decimal, integer_operand, decimal_operand);
	}
}

static void __max869_div_str(char *left_integer, char *left_decimal, char *right_integer,
				char *right_decimal, char *result_integer, char *result_decimal)
{
	int i;
	unsigned long long j;
	unsigned long long loperand, roperand;
	unsigned long long ldigit, rdigit, temp;
	char str[10] = { 0, };

	ldigit = 0;
	rdigit = 0;

	sscanf(left_integer, "%19llu", &loperand);
	for (i = DECIMAL_LEN - 2; i >= 0; i--) {
		if (left_decimal[i] != '0') {
			ldigit = (unsigned long long)i + 1;
			break;
		}
	}

	sscanf(right_integer, "%19llu", &roperand);
	for (i = DECIMAL_LEN - 2; i >= 0; i--) {
		if (right_decimal[i] != '0') {
			rdigit = (unsigned long long)i + 1;
			break;
		}
	}

	if (ldigit < rdigit)
		ldigit = rdigit;

	for (j = 0; j < ldigit; j++) {
		loperand *= 10;
		roperand *= 10;
	}
	if (ldigit != 0) {
		snprintf(str, sizeof(str), "%%%llullu", ldigit);
		sscanf(left_decimal, str, &temp);
		loperand += temp;

		snprintf(str, sizeof(str), "%%%llullu", ldigit);
		sscanf(right_decimal, str, &temp);
		roperand += temp;
	}
	if (roperand == 0)
		roperand = 1;
	temp = loperand / roperand;
	snprintf(result_integer, INTEGER_LEN, "%llu", temp);
	loperand -= roperand * temp;
	loperand *= 10;
	for (i = 0; i < DECIMAL_LEN; i++) {
		temp = loperand / roperand;
		if (temp != 0) {
			loperand -= roperand * temp;
			loperand *= 10;
		} else
			loperand *= 10;
		result_decimal[i] = '0' + temp;
	}
	result_decimal[DECIMAL_LEN - 1] = 0;
}

static void __max869_init_eol_data(struct max86915_eol_data *eol_data)
{
	memset(eol_data, 0, sizeof(struct max86915_eol_data));
	do_gettimeofday(&eol_data->start_time);
	eol_data->state = _EOL_STATE_TYPE_NEW_INIT;
}
static void __max869_eol_flicker_data(int ir_data, struct  max86915_eol_data *eol_data)
{
		int i = 0;
		int eol_flicker_retry = eol_data->eol_flicker_data.retry;
		int eol_flicker_count = eol_data->eol_flicker_data.count;
		int eol_flicker_index = eol_data->eol_flicker_data.index;
		u64 data[2];
		u64 average[2];

		if (!eol_flicker_count)
			HRM_dbg("%s - EOL FLICKER STEP 1 STARTED !!\n", __func__);

		if (eol_flicker_count < EOL_NEW_FLICKER_SKIP_CNT)
			goto eol_flicker_exit;

		if (!eol_data->eol_flicker_data.state) {
			if (ir_data < EOL_NEW_FLICKER_THRESH)
				eol_data->eol_flicker_data.state = 1;
			else {
				if (eol_flicker_retry < EOL_NEW_FLICKER_RETRY_CNT) {
					HRM_dbg("%s - EOL FLICKER STEP 1 Retry : %d\n",
						__func__, eol_data->eol_flicker_data.retry);
					eol_data->eol_flicker_data.retry++;
					goto eol_flicker_exit;
				} else
					eol_data->eol_flicker_data.state = 1;
			}
		}

		if (eol_flicker_index < EOL_NEW_FLICKER_SIZE) {
			eol_data->eol_flicker_data.buf[SELF_IR_CH][eol_flicker_index] = (u64)ir_data * CONVER_FLOAT;
			eol_data->eol_flicker_data.sum[SELF_IR_CH]
				+= eol_data->eol_flicker_data.buf[SELF_IR_CH][eol_flicker_index];
			if (eol_data->eol_flicker_data.max
					< eol_data->eol_flicker_data.buf[SELF_IR_CH][eol_flicker_index])
				eol_data->eol_flicker_data.max
					= eol_data->eol_flicker_data.buf[SELF_IR_CH][eol_flicker_index];
			eol_data->eol_flicker_data.index++;
		} else if (eol_flicker_index == EOL_NEW_FLICKER_SIZE && eol_data->eol_flicker_data.done != 1) {
			do_div(eol_data->eol_flicker_data.sum[SELF_IR_CH], EOL_NEW_FLICKER_SIZE);
			eol_data->eol_flicker_data.average[SELF_IR_CH] = eol_data->eol_flicker_data.sum[SELF_IR_CH];
			average[SELF_IR_CH] = eol_data->eol_flicker_data.average[SELF_IR_CH];
			do_div(average[SELF_IR_CH], CONVER_FLOAT);

			for (i = 0; i <	EOL_NEW_FLICKER_SIZE; i++) {
				do_div(eol_data->eol_flicker_data.buf[SELF_IR_CH][i], CONVER_FLOAT);
				/*
				 * HRM_dbg("EOL FLICKER STEP 1 ir_data %d, %llu",
				 * i, eol_data->eol_flicker_data.buf[SELF_IR_CH][i]);
				 */
				data[SELF_IR_CH] = eol_data->eol_flicker_data.buf[SELF_IR_CH][i];
				eol_data->eol_flicker_data.std_sum[SELF_IR_CH]
					+= (data[SELF_IR_CH] - average[SELF_IR_CH])
						* (data[SELF_IR_CH] - average[SELF_IR_CH]);
			}
			eol_data->eol_flicker_data.done = 1;
			HRM_dbg("%s - EOL FLICKER STEP 1 Done\n", __func__);
		}
eol_flicker_exit:
		eol_data->eol_flicker_data.count++;

}

static void __max869_eol_hrm_data(int ir_data, int red_data, int green_data, int blue_data,
	struct  max86915_eol_data *eol_data, u32 array_pos, u32 eol_skip_cnt, u32 eol_step_size)
{
	int i = 0;
	int hrm_eol_count = eol_data->eol_hrm_data[array_pos].count;
	int hrm_eol_index = eol_data->eol_hrm_data[array_pos].index;
	int ir_current = eol_data->eol_hrm_data[array_pos].ir_current;
	int red_current = eol_data->eol_hrm_data[array_pos].red_current;
	int green_current = eol_data->eol_hrm_data[array_pos].green_current;
	int blue_current = eol_data->eol_hrm_data[array_pos].blue_current;
	u64 data[4];
	u64 average[4];
	u32 hrm_eol_skip_cnt = eol_skip_cnt;
	u32 hrm_eol_size = eol_step_size;

	if (!hrm_eol_count) {
		HRM_dbg("%s - STEP [%d], [%d] started IR : %d , RED : %d, GREEN : %d, BLUE : %d,eol_skip_cnt : %d, eol_step_size : %d\n",
			__func__, array_pos, array_pos + 2, ir_current, red_current,
			green_current, blue_current, eol_skip_cnt, eol_step_size);
	}
	if (eol_step_size > EOL_NEW_HRM_SIZE) {
		HRM_dbg("%s - FATAL ERROR !!!! the buffer size should be smaller than EOL_NEW_HRM_SIZE\n", __func__);
		goto hrm_eol_exit;
	}
	if (hrm_eol_count < hrm_eol_skip_cnt)
		goto hrm_eol_exit;

	if (hrm_eol_index < hrm_eol_size) {
		eol_data->eol_hrm_data[array_pos].buf[SELF_IR_CH][hrm_eol_index] = (u64)ir_data * CONVER_FLOAT;
		eol_data->eol_hrm_data[array_pos].buf[SELF_RED_CH][hrm_eol_index] = (u64)red_data * CONVER_FLOAT;
		eol_data->eol_hrm_data[array_pos].buf[SELF_GREEN_CH][hrm_eol_index] = (u64)green_data * CONVER_FLOAT;
		eol_data->eol_hrm_data[array_pos].buf[SELF_BLUE_CH][hrm_eol_index] = (u64)blue_data * CONVER_FLOAT;
		eol_data->eol_hrm_data[array_pos].sum[SELF_IR_CH]
			+= eol_data->eol_hrm_data[array_pos].buf[SELF_IR_CH][hrm_eol_index];
		eol_data->eol_hrm_data[array_pos].sum[SELF_RED_CH]
			+= eol_data->eol_hrm_data[array_pos].buf[SELF_RED_CH][hrm_eol_index];
		eol_data->eol_hrm_data[array_pos].sum[SELF_GREEN_CH]
			+= eol_data->eol_hrm_data[array_pos].buf[SELF_GREEN_CH][hrm_eol_index];
		eol_data->eol_hrm_data[array_pos].sum[SELF_BLUE_CH]
			+= eol_data->eol_hrm_data[array_pos].buf[SELF_BLUE_CH][hrm_eol_index];
		eol_data->eol_hrm_data[array_pos].index++;
	} else if (hrm_eol_index == hrm_eol_size && eol_data->eol_hrm_data[array_pos].done != 1) {
		do_div(eol_data->eol_hrm_data[array_pos].sum[SELF_IR_CH], hrm_eol_size);
		eol_data->eol_hrm_data[array_pos].average[SELF_IR_CH]
			= eol_data->eol_hrm_data[array_pos].sum[SELF_IR_CH];
		do_div(eol_data->eol_hrm_data[array_pos].sum[SELF_RED_CH], hrm_eol_size);
		eol_data->eol_hrm_data[array_pos].average[SELF_RED_CH]
			= eol_data->eol_hrm_data[array_pos].sum[SELF_RED_CH];
		do_div(eol_data->eol_hrm_data[array_pos].sum[SELF_GREEN_CH], hrm_eol_size);
		eol_data->eol_hrm_data[array_pos].average[SELF_GREEN_CH]
			= eol_data->eol_hrm_data[array_pos].sum[SELF_GREEN_CH];
		do_div(eol_data->eol_hrm_data[array_pos].sum[SELF_BLUE_CH], hrm_eol_size);
		eol_data->eol_hrm_data[array_pos].average[SELF_BLUE_CH]
			= eol_data->eol_hrm_data[array_pos].sum[SELF_BLUE_CH];

		average[SELF_IR_CH] = eol_data->eol_hrm_data[array_pos].average[SELF_IR_CH];
		do_div(average[SELF_IR_CH], CONVER_FLOAT);
		average[SELF_RED_CH] = eol_data->eol_hrm_data[array_pos].average[SELF_RED_CH];
		do_div(average[SELF_RED_CH], CONVER_FLOAT);
		average[SELF_GREEN_CH] = eol_data->eol_hrm_data[array_pos].average[SELF_GREEN_CH];
		do_div(average[SELF_GREEN_CH], CONVER_FLOAT);
		average[SELF_BLUE_CH] = eol_data->eol_hrm_data[array_pos].average[SELF_BLUE_CH];
		do_div(average[SELF_BLUE_CH], CONVER_FLOAT);

		for (i = 0; i <	hrm_eol_size; i++) {
			do_div(eol_data->eol_hrm_data[array_pos].buf[SELF_IR_CH][i], CONVER_FLOAT);
			data[SELF_IR_CH] = eol_data->eol_hrm_data[array_pos].buf[SELF_IR_CH][i];
			do_div(eol_data->eol_hrm_data[array_pos].buf[SELF_RED_CH][i], CONVER_FLOAT);
			data[SELF_RED_CH] = eol_data->eol_hrm_data[array_pos].buf[SELF_RED_CH][i];
			do_div(eol_data->eol_hrm_data[array_pos].buf[SELF_GREEN_CH][i], CONVER_FLOAT);
			data[SELF_GREEN_CH] = eol_data->eol_hrm_data[array_pos].buf[SELF_GREEN_CH][i];
			do_div(eol_data->eol_hrm_data[array_pos].buf[SELF_BLUE_CH][i], CONVER_FLOAT);
			data[SELF_BLUE_CH] = eol_data->eol_hrm_data[array_pos].buf[SELF_BLUE_CH][i];

			eol_data->eol_hrm_data[array_pos].std_sum[SELF_IR_CH]
				+= (data[SELF_IR_CH] - average[SELF_IR_CH])
					* (data[SELF_IR_CH] - average[SELF_IR_CH]);
			eol_data->eol_hrm_data[array_pos].std_sum[SELF_RED_CH]
				+= (data[SELF_RED_CH] - average[SELF_RED_CH])
					* (data[SELF_RED_CH] - average[SELF_RED_CH]);
			eol_data->eol_hrm_data[array_pos].std_sum[SELF_GREEN_CH]
				+= (data[SELF_GREEN_CH] - average[SELF_GREEN_CH])
					* (data[SELF_GREEN_CH] - average[SELF_GREEN_CH]);
			eol_data->eol_hrm_data[array_pos].std_sum[SELF_BLUE_CH]
				+= (data[SELF_BLUE_CH] - average[SELF_BLUE_CH])
					* (data[SELF_BLUE_CH] - average[SELF_BLUE_CH]);
		}
		eol_data->eol_hrm_data[array_pos].done = 1;
		eol_data->eol_hrm_flag |= 1 << array_pos;
		HRM_dbg("%s - STEP [%d], [%d], [%x] Done\n",
			__func__, array_pos, array_pos + 2, eol_data->eol_hrm_flag);
	}
hrm_eol_exit:
	eol_data->eol_hrm_data[array_pos].count++;
}

static void __max869_eol_freq_data(struct  max86915_eol_data *eol_data, u8 divid)
{
	int freq_state = eol_data->eol_freq_data.state;
	unsigned long freq_end_time = eol_data->eol_freq_data.end_time;
	unsigned long freq_prev_time = eol_data->eol_freq_data.prev_time;
	unsigned long freq_current_time = jiffies;
	int freq_count = eol_data->eol_freq_data.skip_count;
	unsigned long freq_interval;
	int freq_start_time;
	int freq_work_time;
	u32 freq_skip_cnt = EOL_NEW_FREQ_SKIP_CNT / divid;
	u32 freq_interrupt_min = EOL_NEW_FREQ_MIN / divid;
	u32 freq_timeout = (HZ / (HZ / divid));

	if (freq_count < freq_skip_cnt)
		goto seq3_exit;

	switch (freq_state) {
	case 0:
		HRM_dbg("%s - EOL FREQ 7 stated ###\n", __func__);
		eol_data->eol_freq_data.start_time = jiffies;
		do_gettimeofday(&eol_data->eol_freq_data.start_time_t);
		eol_data->eol_freq_data.end_time = jiffies + HZ;        /* timeout in 1s */
		eol_data->eol_freq_data.state = 1;
		eol_data->eol_freq_data.count++;
		break;
	case 1:
		if (time_is_after_eq_jiffies(freq_end_time)) {
			if (time_is_after_eq_jiffies(freq_prev_time)) {
				eol_data->eol_freq_data.count++;
			} else {
				if (eol_data->eol_freq_data.skew_retry < EOL_NEW_FREQ_RETRY_CNT) {
					eol_data->eol_freq_data.skew_retry++;
					freq_interval = freq_current_time-freq_prev_time;
					HRM_dbg("%s - !!!!!! EOL FREQ 7 Clock skew too large retry : %d ,interval : %lu ms, count : %d\n", __func__,
						eol_data->eol_freq_data.skew_retry, freq_interval*10,
						eol_data->eol_freq_data.count);
					eol_data->eol_freq_data.state = 0;
					eol_data->eol_freq_data.count = 0;
					eol_data->eol_freq_data.prev_time = 0;
				}
			}
			do_gettimeofday(&eol_data->eol_freq_data.work_time_t);
		} else {
			if (eol_data->eol_freq_data.count < freq_interrupt_min
				&& eol_data->eol_freq_data.retry < EOL_NEW_FREQ_SKEW_RETRY_CNT) {
				HRM_dbg("%s - !!!!!! EOL FREQ 7 CNT Gap too large : %d, retry : %d\n", __func__,
					eol_data->eol_freq_data.count, eol_data->eol_freq_data.retry);
				eol_data->eol_freq_data.retry++;
				eol_data->eol_freq_data.state = 0;
				eol_data->eol_freq_data.count = 0;
				eol_data->eol_freq_data.prev_time = 0;
			} else {
				eol_data->eol_freq_data.count *= divid;
				eol_data->eol_freq_data.done = 1;
				freq_start_time = (eol_data->eol_freq_data.start_time_t.tv_sec * 1000)
							+ (eol_data->eol_freq_data.start_time_t.tv_usec / 1000);
				freq_work_time = (eol_data->eol_freq_data.work_time_t.tv_sec * 1000)
							+ (eol_data->eol_freq_data.work_time_t.tv_usec / 1000);
				HRM_dbg("%s - EOL FREQ 7 Done###\n", __func__);
				HRM_dbg("%s - EOL FREQ 7 sample_no_cnt : %d, time : %d ms, divid : %d\n", __func__,
					eol_data->eol_freq_data.count, freq_work_time - freq_start_time, divid);
			}
		}
		break;
	default:
		HRM_dbg("%s - EOL FREQ 7 Should not call this routine !!!###\n", __func__);
		break;
	}
seq3_exit:
	eol_data->eol_freq_data.prev_time = freq_current_time + freq_timeout;  /* timeout in 10 or 40 ms or 100 ms */
	eol_data->eol_freq_data.skip_count++;
}

static int __max869_eol_check_done(struct max86915_eol_data *eol_data, u8 mode, struct max869_device_data *device)
{
	int i = 0;
	int start_time;
	int end_time;

	if ((eol_data->eol_flicker_data.done || (device->pre_eol_test_is_enable == 2))
		&& ((eol_data->eol_hrm_flag == EOL_NEW_HRM_COMPLETE_ALL_STEP) || (device->pre_eol_test_is_enable))
		&& (eol_data->eol_freq_data.done || (device->pre_eol_test_is_enable == 1))) {
		do_gettimeofday(&eol_data->end_time);
		start_time = (eol_data->start_time.tv_sec * 1000) + (eol_data->start_time.tv_usec / 1000);
		end_time = (eol_data->end_time.tv_sec * 1000) + (eol_data->end_time.tv_usec / 1000);
		HRM_dbg("%s - EOL Tested Time :  %d ms Tested Count : %d\n",
			__func__, end_time - start_time, device->sample_cnt);

		HRM_dbg("%s - SETTING CUREENT %x, %x\n", __func__,
			EOL_NEW_DC_HIGH_LED_IR_CURRENT, EOL_NEW_DC_HIGH_LED_RED_CURRENT);

		for (i = 0; i < EOL_NEW_HRM_ARRY_SIZE; i++)
			HRM_dbg("%s - DOUBLE CHECK STEP[%d], done : %d\n",
				__func__, i + 2, eol_data->eol_hrm_data[i].done);

		if (device->pre_eol_test_is_enable != 2) {
			for (i = 0; i < EOL_NEW_FLICKER_SIZE; i++)
				HRM_dbg("%s - EOL_NEW_FLICKER_MODE EOLRAW,%llu,%d\n",
					__func__, eol_data->eol_flicker_data.buf[SELF_IR_CH][i], 0);
		}

		if (device->pre_eol_test_is_enable == 0) {
			for (i = 0; i < EOL_NEW_DC_LOW_SIZE; i++)
				HRM_dbg("%s - EOL_NEW_DC_LOW_MODE EOLRAW,%llu,%llu,%llu,%llu\n", __func__,
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].buf[SELF_IR_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].buf[SELF_RED_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].buf[SELF_GREEN_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].buf[SELF_BLUE_CH][i]);

			for (i = 0; i < EOL_NEW_DC_MIDDLE_SIZE; i++)
				HRM_dbg("%s - EOL_NEW_DC_MIDDLE_MODE EOLRAW,%llu,%llu,%llu,%llu\n", __func__,
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].buf[SELF_IR_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].buf[SELF_RED_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].buf[SELF_GREEN_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].buf[SELF_BLUE_CH][i]);

			for (i = 0; i < EOL_NEW_DC_HIGH_SIZE; i++)
				HRM_dbg("%s - EOL_NEW_DC_HIGH_MODE EOLRAW,%llu,%llu,%llu,%llu\n", __func__,
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].buf[SELF_IR_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].buf[SELF_RED_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].buf[SELF_GREEN_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].buf[SELF_BLUE_CH][i]);

			for (i = 0; i < EOL_NEW_DC_XTALK_SIZE; i++)
				HRM_dbg("%s - EOL_NEW_DC_XTALK EOLRAW,%llu,%llu,%llu,%llu\n", __func__,
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].buf[SELF_IR_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].buf[SELF_RED_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].buf[SELF_GREEN_CH][i],
					eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].buf[SELF_BLUE_CH][i]);

			HRM_dbg("%s - EOL_NEW_FLICKER_MODE RESULT,%llu,%llu\n", __func__,
				eol_data->eol_flicker_data.average[SELF_IR_CH],
				eol_data->eol_flicker_data.std_sum[SELF_IR_CH]);
			HRM_dbg("%s - EOL_NEW_DC_LOW_MODE RESULT,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu\n", __func__,
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].std_sum[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].std_sum[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].std_sum[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_BLUE_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].std_sum[SELF_BLUE_CH]);
			HRM_dbg("%s - EOL_NEW_DC_MIDDLE_MODE RESULT,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu\n",	__func__,
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].std_sum[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].std_sum[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].std_sum[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_BLUE_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].std_sum[SELF_BLUE_CH]);
			HRM_dbg("%s - EOL_NEW_DC_HIGH_MODE RESULT,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu\n", __func__,
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_BLUE_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_BLUE_CH]);
			HRM_dbg("%s - EOL_NEW_DC_XTALK RESULT,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu\n", __func__,
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].std_sum[SELF_IR_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].std_sum[SELF_RED_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].std_sum[SELF_GREEN_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_BLUE_CH],
				eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].std_sum[SELF_BLUE_CH]);
		}

		if (device->pre_eol_test_is_enable != 1) {
			HRM_dbg("%s - EOL FREQ RESULT,%d\n", __func__,
				eol_data->eol_freq_data.count);
		}

		__max869_eol_conv2float(eol_data, mode, device);
		device->eol_test_status = 1;
		return 1;
	} else {
		return 0;
	}
}

static void __max869_eol_conv2float(struct max86915_eol_data *eol_data, u8 mode, struct max869_device_data *device)
{
	struct max86915_div_data div_data;
	char flicker_max[2][INTEGER_LEN] = { {'\0', }, };
	char ir_ldc_avg[2][INTEGER_LEN] = { {'\0', }, }, red_ldc_avg[2][INTEGER_LEN] = { {'\0', }, };
	char green_ldc_avg[2][INTEGER_LEN] = { {'\0', }, }, blue_ldc_avg[2][INTEGER_LEN] = { {'\0', }, };
	char ir_mdc_avg[2][INTEGER_LEN] = { {'\0', }, }, red_mdc_avg[2][INTEGER_LEN] = { {'\0', }, };
	char green_mdc_avg[2][INTEGER_LEN] = { {'\0', }, }, blue_mdc_avg[2][INTEGER_LEN] = { {'\0', }, };
	char ir_hdc_avg[2][INTEGER_LEN] = { {'\0', }, }, ir_hdc_std[2][INTEGER_LEN] = { {'\0', }, };
	char red_hdc_avg[2][INTEGER_LEN] = { {'\0', }, }, red_hdc_std[2][INTEGER_LEN] = { {'\0', }, };
	char green_hdc_avg[2][INTEGER_LEN] = { {'\0', }, }, green_hdc_std[2][INTEGER_LEN] = { {'\0', }, };
	char blue_hdc_avg[2][INTEGER_LEN] = { {'\0', }, }, blue_hdc_std[2][INTEGER_LEN] = { {'\0', }, };
	char ir_xtalk_avg[2][INTEGER_LEN] = { {'\0', }, }, red_xtalk_avg[2][INTEGER_LEN] = { {'\0', }, };
	char green_xtalk_avg[2][INTEGER_LEN] = { {'\0', }, }, blue_xtalk_avg[2][INTEGER_LEN] = { {'\0', }, };

	HRM_info("%s - EOL_TEST __max869_eol_conv2float\n", __func__);

	if (device->pre_eol_test_is_enable != 2) {
		/* flicker */
		__max869_div_float(&div_data, eol_data->eol_flicker_data.max, CONVER_FLOAT);
		strncpy(flicker_max[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(flicker_max[1], div_data.result_decimal, INTEGER_LEN - 1);
	}

	if (device->pre_eol_test_is_enable == 0) {
		/* dc low */
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_IR_CH], CONVER_FLOAT);
		strncpy(ir_ldc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(ir_ldc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_RED_CH], CONVER_FLOAT);
		strncpy(red_ldc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(red_ldc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_GREEN_CH], CONVER_FLOAT);
		strncpy(green_ldc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(green_ldc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_LOW].average[SELF_BLUE_CH], CONVER_FLOAT);
		strncpy(blue_ldc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(blue_ldc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);

		/* dc mid */
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_IR_CH], CONVER_FLOAT);
		strncpy(ir_mdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(ir_mdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_RED_CH], CONVER_FLOAT);
		strncpy(red_mdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(red_mdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_GREEN_CH], CONVER_FLOAT);
		strncpy(green_mdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(green_mdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data, eol_data->eol_hrm_data[EOL_NEW_MODE_DC_MID].average[SELF_BLUE_CH], CONVER_FLOAT);
		strncpy(blue_mdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(blue_mdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);

		/* dc high */
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_IR_CH], CONVER_FLOAT);
		strncpy(ir_hdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(ir_hdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_IR_CH], EOL_NEW_DC_HIGH_SIZE);
		strncpy(ir_hdc_std[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(ir_hdc_std[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_RED_CH], CONVER_FLOAT);
		strncpy(red_hdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(red_hdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_RED_CH], EOL_NEW_DC_HIGH_SIZE);
		strncpy(red_hdc_std[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(red_hdc_std[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_GREEN_CH], CONVER_FLOAT);
		strncpy(green_hdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(green_hdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_GREEN_CH], EOL_NEW_DC_HIGH_SIZE);
		strncpy(green_hdc_std[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(green_hdc_std[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].average[SELF_BLUE_CH], CONVER_FLOAT);
		strncpy(blue_hdc_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(blue_hdc_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_HIGH].std_sum[SELF_BLUE_CH], EOL_NEW_DC_HIGH_SIZE);
		strncpy(blue_hdc_std[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(blue_hdc_std[1], div_data.result_decimal, INTEGER_LEN - 1);

		/* dc xtalk */
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_IR_CH], CONVER_FLOAT);
		strncpy(ir_xtalk_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(ir_xtalk_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_RED_CH], CONVER_FLOAT);
		strncpy(red_xtalk_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(red_xtalk_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_GREEN_CH], CONVER_FLOAT);
		strncpy(green_xtalk_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(green_xtalk_avg[1], div_data.result_decimal, INTEGER_LEN - 1);
		__max869_div_float(&div_data,
			eol_data->eol_hrm_data[EOL_NEW_MODE_DC_XTALK].average[SELF_BLUE_CH], CONVER_FLOAT);
		strncpy(blue_xtalk_avg[0], div_data.result_integer, INTEGER_LEN - 1);
		strncpy(blue_xtalk_avg[1], div_data.result_decimal, INTEGER_LEN - 1);

		HRM_info("%s - hdc_std : %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c\n", __func__,
				ir_hdc_std[0], ir_hdc_std[1][0], ir_hdc_std[1][1],
				red_hdc_std[0], red_hdc_std[1][0], red_hdc_std[1][1],
				green_hdc_std[0], green_hdc_std[1][0], green_hdc_std[1][1],
				blue_hdc_std[0], blue_hdc_std[1][0], blue_hdc_std[1][1]);

		__max869_float_sqrt(ir_hdc_std[0], ir_hdc_std[1]);
		__max869_float_sqrt(red_hdc_std[0], red_hdc_std[1]);
		__max869_float_sqrt(green_hdc_std[0], green_hdc_std[1]);
		__max869_float_sqrt(blue_hdc_std[0], blue_hdc_std[1]);
	}

	if (device->eol_test_result != NULL)
		kfree(device->eol_test_result);

	device->eol_test_result = kzalloc(sizeof(char) * MAX_EOL_RESULT, GFP_KERNEL);
	if (device->eol_test_result == NULL) {
		HRM_dbg("max86915_%s - couldn't allocate memory\n",
			__func__);
		return;
	}

	if (device->pre_eol_test_is_enable == 1) { /* PRE EOL System Noise */
		snprintf(device->eol_test_result, MAX_EOL_RESULT, "%s.%c%c\n",
			flicker_max[0], flicker_max[1][0], flicker_max[1][1]);
	} else if (device->pre_eol_test_is_enable == 2) { /* PRE EOL FREQ */
		snprintf(device->eol_test_result, MAX_EOL_RESULT, "%d\n",
			eol_data->eol_freq_data.count);
	} else if (device->pre_eol_test_is_enable == 3) { /* PRE EOL BOTH */
		snprintf(device->eol_test_result, MAX_EOL_RESULT, "%s.%c%c, %d\n",
			flicker_max[0], flicker_max[1][0], flicker_max[1][1],
			eol_data->eol_freq_data.count);
	} else {
		snprintf(device->eol_test_result, MAX_EOL_RESULT, "%s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %s.%c%c, %d\n",
			flicker_max[0], flicker_max[1][0], flicker_max[1][1],
			ir_ldc_avg[0], ir_ldc_avg[1][0], ir_ldc_avg[1][1],
			red_ldc_avg[0], red_ldc_avg[1][0], red_ldc_avg[1][1],
			green_ldc_avg[0], green_ldc_avg[1][0], green_ldc_avg[1][1],
			blue_ldc_avg[0], blue_ldc_avg[1][0], blue_ldc_avg[1][1],
			ir_mdc_avg[0], ir_mdc_avg[1][0], ir_mdc_avg[1][1],
			red_mdc_avg[0], red_mdc_avg[1][0], red_mdc_avg[1][1],
			green_mdc_avg[0], green_mdc_avg[1][0], green_mdc_avg[1][1],
			blue_mdc_avg[0], blue_mdc_avg[1][0], blue_mdc_avg[1][1],
			ir_hdc_avg[0], ir_hdc_avg[1][0], ir_hdc_avg[1][1],
			red_hdc_avg[0], red_hdc_avg[1][0], red_hdc_avg[1][1],
			green_hdc_avg[0], green_hdc_avg[1][0], green_hdc_avg[1][1],
			blue_hdc_avg[0], blue_hdc_avg[1][0], blue_hdc_avg[1][1],
			ir_xtalk_avg[0], ir_xtalk_avg[1][0], ir_xtalk_avg[1][1],
			red_xtalk_avg[0], red_xtalk_avg[1][0], red_xtalk_avg[1][1],
			green_xtalk_avg[0], green_xtalk_avg[1][0], green_xtalk_avg[1][1],
			blue_xtalk_avg[0], blue_xtalk_avg[1][0], blue_xtalk_avg[1][1],
			ir_hdc_std[0], ir_hdc_std[1][0], ir_hdc_std[1][1],
			red_hdc_std[0], red_hdc_std[1][0], red_hdc_std[1][1],
			green_hdc_std[0], green_hdc_std[1][0], green_hdc_std[1][1],
			blue_hdc_std[0], blue_hdc_std[1][0], blue_hdc_std[1][1],
			eol_data->eol_freq_data.count);
	}

	HRM_dbg("%s result - %s\n", __func__, device->eol_test_result);

}

static int __max86915_enable_eol_flicker(struct max869_device_data *data)
{
	int err;

	data->sample_cnt = 0;
	data->num_samples = 1;
	data->flex_mode = 1;

	err = __max869_init(data);

	/* 400Hz, LED_PW=400us, SPO2_ADC_RANGE=4096nA */
	err = __max869_write_reg(data,
		MAX86915_MODE_CONFIGURATION_2, 0x0F);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data,
		MAX86915_FIFO_CONFIG, 0x0C | MAX86915_FIFO_ROLLS_ON_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data,
		MAX86915_INTERRUPT_ENABLE, A_FULL_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_INTERRUPT_ENABLE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_1,
			0x01);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_1!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data,
			MAX86915_MODE_CONFIGURATION, 0x07);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}

	data->eol_data.state = _EOL_STATE_TYPE_NEW_FLICKER_INIT;
	return err;
}


static int __max86915_enable_eol_dc(struct max869_device_data *data)
{
	int err;
	u8 flex_config[2] = {0, };

	/* data->led = 1; */ /* Prevent resetting MAX86915_LED_CONFIGURATION */
	data->num_samples = 0;
	data->flex_mode = 0;

	flex_config[0] = (RED_LED_CH << MAX86915_LED2_OFFSET) | IR_LED_CH;
	flex_config[1] = (BLUE_LED_CH << MAX86915_LED4_OFFSET) | GREEN_LED_CH;

	if (flex_config[0] & MAX86915_LED1_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 0);
	}
	if (flex_config[0] & MAX86915_LED2_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 1);
	}
	if (flex_config[1] & MAX86915_LED3_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 2);
	}
	if (flex_config[1] & MAX86915_LED4_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 3);
	}

	HRM_info("%s - flexmode : 0x%02x, num_samples : %d\n", __func__,
			data->flex_mode, data->num_samples);

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_LED1_PA,
			EOL_NEW_DC_LOW_LED_IR_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED1_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED2_PA,
			EOL_NEW_DC_LOW_LED_RED_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED2_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED3_PA,
			EOL_NEW_DC_LOW_LED_GREEN_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED3_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED4_PA,
			EOL_NEW_DC_LOW_LED_BLUE_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED4_PA!\n",
			__func__);
		return -EIO;
	}

	/* LED Range */
	err = __max869_write_reg(data, MAX86915_LED_RANGE,
			  (MAX86915_DEFAULT_LED_RGE << MAX86915_LED1_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED2_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED3_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED4_RGE_OFFSET));

	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_RANGE!\n",
			__func__);
		return -EIO;
	}

	/* XTALK */
	err = __max869_write_reg(data, MAX86915_DAC1_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC1_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC2_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC2_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC3_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC3_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC4_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC4_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_INTERRUPT_ENABLE, A_FULL_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_INTERRUPT_ENABLE!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_1,
			flex_config[0]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_1!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_2,
			flex_config[1]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_2!\n",
			__func__);
		return -EIO;
	}

	/* 32uA, 400Hz, 120us */
	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
			0x0D | (0x03 << MAX86915_ADC_RGE_OFFSET));
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_FIFO_CONFIG, 0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x03);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}

	data->eol_data.state = _EOL_STATE_TYPE_NEW_DC_MODE_LOW;
	return 0;
}

static int __max86915_enable_eol_freq(struct max869_device_data *data)
{
	int err;
	u8 flex_config[2] = {0, };

	data->num_samples = 0;
	data->flex_mode = 0;

	flex_config[0] = (RED_LED_CH << MAX86915_LED2_OFFSET) | IR_LED_CH;
	flex_config[1] = (BLUE_LED_CH << MAX86915_LED4_OFFSET) | GREEN_LED_CH;

	if (flex_config[0] & MAX86915_LED1_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 0);
	}
	if (flex_config[0] & MAX86915_LED2_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 1);
	}
	if (flex_config[1] & MAX86915_LED3_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 2);
	}
	if (flex_config[1] & MAX86915_LED4_MASK) {
		data->num_samples++;
		data->flex_mode |= (1 << 3);
	}

	HRM_info("%s - flexmode : 0x%02x, num_samples : %d\n", __func__,
			data->flex_mode, data->num_samples);

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_1,
			flex_config[0]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_1!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED_SEQ_REG_2,
			flex_config[1]);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_SEQ_REG_2!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_LED1_PA,
			EOL_NEW_FREQUENCY_LED_IR_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED1_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED2_PA,
			EOL_NEW_FREQUENCY_LED_RED_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED2_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED3_PA,
			EOL_NEW_FREQUENCY_LED_GREEN_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED3_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_LED4_PA,
			EOL_NEW_FREQUENCY_LED_BLUE_CURRENT);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED4_PA!\n",
			__func__);
		return -EIO;
	}

	/* LED Range */
	err = __max869_write_reg(data, MAX86915_LED_RANGE,
			  (MAX86915_DEFAULT_LED_RGE << MAX86915_LED1_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED2_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED3_RGE_OFFSET)
			| (MAX86915_DEFAULT_LED_RGE << MAX86915_LED4_RGE_OFFSET));

	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED_RANGE!\n",
			__func__);
		return -EIO;
	}

	/* XTALK */
	err = __max869_write_reg(data, MAX86915_DAC1_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC1_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC2_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC2_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC3_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC3_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(data, MAX86915_DAC4_XTALK_CODE,
			0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_DAC4_XTALK_CODE!\n",
			__func__);
		return -EIO;
	}

	/* 32uA, 400Hz, 120us */
	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION_2,
			0x0D | (0x03 << MAX86915_ADC_RGE_OFFSET)); /* Maximum PW is 100us for 4ch/400Hz */
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
			__func__);
		return -EIO;
	}

	/* AVERAGE 4 */
	err = __max869_write_reg(data, MAX86915_FIFO_CONFIG,
			(0x02 << MAX86915_SMP_AVE_OFFSET) & MAX86915_SMP_AVE_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_INTERRUPT_ENABLE, PPG_RDY_MASK);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_INTERRUPT_ENABLE!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(data, MAX86915_MODE_CONFIGURATION, 0x03);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
			__func__);
		return -EIO;
	}

	/* set the mode */
	data->eol_data.state = _EOL_STATE_TYPE_NEW_FREQ_MODE;
	return err;
}

static void __max86915_eol_test_onoff(struct max869_device_data *data, int onoff)
{
	int err;

	if (onoff) {
		data->agc_mode = M_NONE;
		__max869_init_eol_data(&data->eol_data);

		if (data->pre_eol_test_is_enable == 2) /* Pre Test Only for Frequency */
			err = __max86915_enable_eol_freq(data);
		else
			err = __max86915_enable_eol_flicker(data);

		if (err != 0)
			HRM_dbg("__max86915_eol_test_onoff err : %d\n", err);
	} else {
		HRM_info("%s - eol test off\n", __func__);
		err = __max86915_disable(data);
		if (err != 0)
			HRM_dbg("__max86915_disable err : %d\n", err);

		/*
		 * data->led_current1 = data->default_current1;
		 * data->led_current2 = data->default_current2;
		 * data->led_current3 = data->default_current3;
		 * data->led_current4 = data->default_current4;
		 */

		err = __max86915_init_device(data);
		if (err)
			HRM_dbg("%s __max86915_init_devicedevice fail err = %d\n",
				__func__, err);

		err = __max86915_hrm_enable(data);
		if (err != 0)
			HRM_dbg("__max86915_hrm_enable err : %d\n", err);
	}
	HRM_info("%s - onoff = %d\n", __func__, onoff);
}

static int __max86915_eol_read_data(struct hrm_output_data *data, struct max869_device_data *device, int *raw_data)
{
	int err = 0;
	int i = 0, j = 0;
	int total_eol_cnt = (330 + (400 * 20)); /* maximnum count */
	u8 recvData[MAX_LED_NUM * NUM_BYTES_PER_SAMPLE] = { 0x00, };

	switch (device->eol_data.state) {
	case _EOL_STATE_TYPE_NEW_FREQ_MODE:
	case _EOL_STATE_TYPE_NEW_END:
	case _EOL_STATE_TYPE_NEW_STOP:
		recvData[0] = MAX86915_FIFO_DATA;
		err = __max869_read_reg(device, recvData,
				device->num_samples * NUM_BYTES_PER_SAMPLE);
		if (err != 0) {
			HRM_dbg("%s __max869_read_reg err:%d, address:0x%02x\n",
				__func__, err, recvData[0]);
			return -EIO;
		}
		for (i = 0; i < MAX_LED_NUM; i++) {
			if (device->flex_mode & (1 << i)) {
				raw_data[i] =  recvData[j++] << 16 & 0x30000;
				raw_data[i] += recvData[j++] << 8;
				raw_data[i] += recvData[j++] << 0;
			} else
				raw_data[i] = 0;
		}
		device->sample_cnt++;
		device->eol_data.eol_count++;
		break;
	default:
		err = __max869_fifo_irq_handler(device);
		if (err != 0) {
			HRM_dbg("%s __max869_fifo_irq_handler err:%d\n",
				__func__, err);
			return -EIO;
		}
		data->fifo_num = device->fifo_samples;
		device->sample_cnt += device->fifo_samples;
		device->eol_data.eol_count += device->fifo_samples;
		break;
	}

	switch (device->eol_data.state) {
	case _EOL_STATE_TYPE_NEW_INIT:
		HRM_dbg("%s _EOL_STATE_TYPE_NEW_INIT\n", __func__);
		data->fifo_num = 0; /* doesn't report the data */
		err = 1;
		if (device->eol_data.eol_count >= EOL_NEW_START_SKIP_CNT) {
			device->eol_data.state = _EOL_STATE_TYPE_NEW_FLICKER_INIT;
			device->eol_data.eol_count = 0;
		}
		break;
	case _EOL_STATE_TYPE_NEW_FLICKER_INIT:
		for (i = 0; i < device->fifo_samples; i++) {
			data->fifo_main[AGC_IR][i] = device->fifo_data[AGC_IR][i];
			data->fifo_main[AGC_RED][i] = 0;
			device->eol_data.eol_flicker_data.max = 0;
		}

		if (device->eol_data.eol_count >= EOL_NEW_START_SKIP_CNT) {
			HRM_dbg("%s FINISH : _EOL_STATE_TYPE_NEW_FLICKER_INIT\n", __func__);
			device->eol_data.state = _EOL_STATE_TYPE_NEW_FLICKER_MODE;
			device->eol_data.eol_count = 0;
		}
		break;
	case _EOL_STATE_TYPE_NEW_FLICKER_MODE:
		for (i = 0; i < device->fifo_samples; i++) {
			device->fifo_data[AGC_IR][i] = device->fifo_data[AGC_IR][i] >> 3;
			data->fifo_main[AGC_IR][i] = device->fifo_data[AGC_IR][i];
			data->fifo_main[AGC_RED][i] = 0;
			__max869_eol_flicker_data(data->fifo_main[AGC_IR][i], &device->eol_data);
		}
		if (device->eol_data.eol_flicker_data.done == EOL_SUCCESS) {
			HRM_dbg("%s FINISH : _EOL_STATE_TYPE_NEW_FLICKER_MODE : %d, %d\n", __func__,
				device->eol_data.eol_flicker_data.index, device->eol_data.eol_flicker_data.done);

			if (device->pre_eol_test_is_enable == 3) /* PRE EOL for both */
				__max86915_enable_eol_freq(device);
			else if (device->pre_eol_test_is_enable == 1) /* Finish PRE EOL */
				__max869_eol_check_done(&device->eol_data, SELF_MODE_400HZ, device);
			else
				__max86915_enable_eol_dc(device);

			device->eol_data.eol_count = 0;

			/* clear the remained datasheet after changing mode (interrupt TRIGGER mode). */
			err = __max869_fifo_irq_handler(device);
			if (err != 0) {
				HRM_dbg("%s __max869_fifo_irq_handler err:%d\n",
					__func__, err);
				return -EIO;
			}
		}
		break;
	case _EOL_STATE_TYPE_NEW_DC_MODE_LOW:
		for (i = 0; i < device->fifo_samples; i++) {
			data->fifo_main[AGC_IR][i] = device->fifo_data[AGC_IR][i];
			data->fifo_main[AGC_RED][i] = device->fifo_data[AGC_RED][i];
			data->fifo_main[AGC_GREEN][i] = device->fifo_data[AGC_GREEN][i];
			data->fifo_main[AGC_BLUE][i] = device->fifo_data[AGC_BLUE][i];
			__max869_eol_hrm_data(data->fifo_main[AGC_IR][i], data->fifo_main[AGC_RED][i],
				data->fifo_main[AGC_GREEN][i], data->fifo_main[AGC_BLUE][i], &device->eol_data,
				EOL_NEW_MODE_DC_LOW, EOL_NEW_DC_LOW_SKIP_CNT, EOL_NEW_DC_LOW_SIZE);
		}

		if (device->eol_data.eol_hrm_data[EOL_NEW_MODE_DC_LOW].done == EOL_SUCCESS) {
			HRM_dbg("%s FINISH : _EOL_STATE_TYPE_NEW_DC_MODE_LOW\n", __func__);

			err = max869_set_current(EOL_NEW_DC_MID_LED_IR_CURRENT, EOL_NEW_DC_MID_LED_RED_CURRENT,
				EOL_NEW_DC_MID_LED_GREEN_CURRENT, EOL_NEW_DC_MID_LED_BLUE_CURRENT);

			device->eol_data.state = _EOL_STATE_TYPE_NEW_DC_MODE_MID;
			device->eol_data.eol_count = 0;
		}
		break;
	case _EOL_STATE_TYPE_NEW_DC_MODE_MID:
		for (i = 0; i < device->fifo_samples; i++) {
			data->fifo_main[AGC_IR][i] = device->fifo_data[AGC_IR][i];
			data->fifo_main[AGC_RED][i] = device->fifo_data[AGC_RED][i];
			data->fifo_main[AGC_GREEN][i] = device->fifo_data[AGC_GREEN][i];
			data->fifo_main[AGC_BLUE][i] = device->fifo_data[AGC_BLUE][i];
			__max869_eol_hrm_data(data->fifo_main[AGC_IR][i], data->fifo_main[AGC_RED][i],
				data->fifo_main[AGC_GREEN][i], data->fifo_main[AGC_BLUE][i], &device->eol_data,
				EOL_NEW_MODE_DC_MID, EOL_NEW_DC_MIDDLE_SKIP_CNT, EOL_NEW_DC_MIDDLE_SIZE);
		}
		if (device->eol_data.eol_hrm_data[EOL_NEW_MODE_DC_MID].done == EOL_SUCCESS) {
			HRM_dbg("%s FINISH : _EOL_STATE_TYPE_NEW_DC_MODE_MID\n", __func__);

			err = max869_set_current(EOL_NEW_DC_HIGH_LED_IR_CURRENT, EOL_NEW_DC_HIGH_LED_RED_CURRENT,
				EOL_NEW_DC_HIGH_LED_GREEN_CURRENT, EOL_NEW_DC_HIGH_LED_BLUE_CURRENT);

			device->eol_data.state = _EOL_STATE_TYPE_NEW_DC_MODE_HIGH;
			device->eol_data.eol_count = 0;
		}
		break;
	case _EOL_STATE_TYPE_NEW_DC_MODE_HIGH:
		for (i = 0; i < device->fifo_samples; i++) {
			data->fifo_main[AGC_IR][i] = device->fifo_data[AGC_IR][i];
			data->fifo_main[AGC_RED][i] = device->fifo_data[AGC_RED][i];
			data->fifo_main[AGC_GREEN][i] = device->fifo_data[AGC_GREEN][i];
			data->fifo_main[AGC_BLUE][i] = device->fifo_data[AGC_BLUE][i];
			__max869_eol_hrm_data(data->fifo_main[AGC_IR][i], data->fifo_main[AGC_RED][i],
				data->fifo_main[AGC_GREEN][i], data->fifo_main[AGC_BLUE][i], &device->eol_data,
				EOL_NEW_MODE_DC_HIGH, EOL_NEW_DC_HIGH_SKIP_CNT, EOL_NEW_DC_HIGH_SIZE);
		}

		if (device->eol_data.eol_hrm_data[EOL_NEW_MODE_DC_HIGH].done == EOL_SUCCESS) {
			HRM_dbg("%s FINISH : _EOL_STATE_TYPE_NEW_DC_MODE_HIGH\n", __func__);

			err = __max869_write_reg(device, MAX86915_DAC1_XTALK_CODE,
					0x1f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_DAC1_XTALK_CODE!\n",
					__func__);
				return -EIO;
			}
			err = __max869_write_reg(device, MAX86915_DAC2_XTALK_CODE,
					0x1f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_DAC2_XTALK_CODE!\n",
					__func__);
				return -EIO;
			}
			err = __max869_write_reg(device, MAX86915_DAC3_XTALK_CODE,
					0x1f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_DAC3_XTALK_CODE!\n",
					__func__);
				return -EIO;
			}
			err = __max869_write_reg(device, MAX86915_DAC4_XTALK_CODE,
					0x1f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_DAC4_XTALK_CODE!\n",
					__func__);
				return -EIO;
			}

			device->eol_data.state = _EOL_STATE_TYPE_NEW_DC_XTALK;
			device->eol_data.eol_count = 0;
		}
		break;
	case _EOL_STATE_TYPE_NEW_DC_XTALK:
		for (i = 0; i < device->fifo_samples; i++) {
			data->fifo_main[AGC_IR][i] = device->fifo_data[AGC_IR][i];
			data->fifo_main[AGC_RED][i] = device->fifo_data[AGC_RED][i];
			data->fifo_main[AGC_GREEN][i] = device->fifo_data[AGC_GREEN][i];
			data->fifo_main[AGC_BLUE][i] = device->fifo_data[AGC_BLUE][i];
			__max869_eol_hrm_data(data->fifo_main[AGC_IR][i], data->fifo_main[AGC_RED][i],
				data->fifo_main[AGC_GREEN][i], data->fifo_main[AGC_BLUE][i], &device->eol_data,
				EOL_NEW_MODE_DC_XTALK, EOL_NEW_DC_XTALK_SKIP_CNT, EOL_NEW_DC_XTALK_SIZE);
		}

		if (device->eol_data.eol_hrm_data[EOL_NEW_MODE_DC_XTALK].done == EOL_SUCCESS) {
			HRM_dbg("%s FINISH : _EOL_STATE_TYPE_NEW_DC_XTALK\n", __func__);

			__max86915_enable_eol_freq(device);
			device->eol_data.eol_count = 0;
		}
		break;

	case _EOL_STATE_TYPE_NEW_FREQ_MODE:
		if (device->eol_data.eol_freq_data.done != EOL_SUCCESS) {
			__max869_eol_freq_data(&device->eol_data, SELF_DIVID_100HZ);
			__max869_eol_check_done(&device->eol_data, SELF_MODE_400HZ, device);
		}
		if (!(device->eol_test_status) && device->sample_cnt >= total_eol_cnt)
			device->eol_data.state = _EOL_STATE_TYPE_NEW_END;
		break;
	case _EOL_STATE_TYPE_NEW_END:
		HRM_dbg("@@@@@NEVER CALL !!!!FAIL EOL TEST  : HR_STATE: _EOL_STATE_TYPE_NEW_END !!!!!!!!!!!!!!!!!!\n");
		__max869_eol_check_done(&device->eol_data, SELF_MODE_400HZ, device);
		device->eol_data.state = _EOL_STATE_TYPE_NEW_STOP;
		break;
	case _EOL_STATE_TYPE_NEW_STOP:
		break;
	default:
		break;
	}
	return err;
}

static int __max86915_hrm_read_data(struct max869_device_data *device, int *data)
{
	int err;
	u8 recvData[MAX_LED_NUM * NUM_BYTES_PER_SAMPLE] = { 0x00, };
	int i, j = 0;
	int ret = 0;

	if (device->sample_cnt == MAX86915_COUNT_MAX)
		device->sample_cnt = 0;

	recvData[0] = MAX86915_FIFO_DATA;
	err = __max869_read_reg(device, recvData,
			device->num_samples * NUM_BYTES_PER_SAMPLE);
	if (err != 0) {
		HRM_dbg("%s __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData[0]);
		return -EIO;
	}

	for (i = 0; i < MAX_LED_NUM; i++)	{
		if (device->flex_mode & (1 << i)) {
			data[i] =  recvData[j++] << 16 & 0x70000;
			data[i] += recvData[j++] << 8;
			data[i] += recvData[j++] << 0;
		} else
			data[i] = 0;
	}

	if ((device->sample_cnt % 1000) == 0)
		HRM_info("%s - %u, %u, %u, %u\n", __func__,
			data[0], data[1], data[2], data[3]);

	if (device->agc_led_set == 0) {
		device->agc_ch_adc[0] = data[0];
		device->agc_ch_adc[1] = data[1];
		device->agc_ch_adc[2] = data[2];
		device->agc_ch_adc[3] = data[3];
		device->sample_cnt++;
	} else {
		data[0] = device->agc_ch_adc[0];
		data[1] = device->agc_ch_adc[1];
		data[2] = device->agc_ch_adc[2];
		data[3] = device->agc_ch_adc[3];
	}

	return ret;
}

static int __max86915_awb_flicker_read_data(struct max869_device_data *device, int *data)
{
	int err;
	u8 recvData[AWB_INTERVAL * NUM_BYTES_PER_SAMPLE] = { 0x00, };
	int ret = 0;
	int mode_changed = 0;
	int i;
	u8 irq_status = 0;
	int previous_awb_data = 0;

	recvData[0] = MAX86915_INTERRUPT_STATUS;
	err = __max869_read_reg(device, recvData, 1);
	if (err != 0) {
		HRM_dbg("%s __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData[0]);
		return -EIO;
	}
	irq_status = recvData[0];
	if (irq_status != 0x80) {
		HRM_dbg("%s - irq_status 0x%x, return 1\n", __func__, irq_status);
		return 1;
	}

	recvData[0] = MAX86915_FIFO_DATA;

	err = __max869_read_reg(device, recvData, AWB_INTERVAL * NUM_BYTES_PER_SAMPLE);
	if (err != 0) {
		HRM_dbg("%s __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData[0]);
		return -EIO;
	}

	*data = (recvData[0 + (AWB_INTERVAL - 1)*NUM_BYTES_PER_SAMPLE] << 16 & 0x70000)
		+ (recvData[1 + (AWB_INTERVAL - 1)*NUM_BYTES_PER_SAMPLE] << 8)
		+ (recvData[2 + (AWB_INTERVAL - 1)*NUM_BYTES_PER_SAMPLE] << 0);

	previous_awb_data = (recvData[0 + (AWB_INTERVAL - 2)*NUM_BYTES_PER_SAMPLE] << 16 & 0x70000)
		+ (recvData[1 + (AWB_INTERVAL - 2)*NUM_BYTES_PER_SAMPLE] << 8)
		+ (recvData[2 + (AWB_INTERVAL - 2)*NUM_BYTES_PER_SAMPLE] << 0);

	if (device->awb_sample_cnt > CONFIG_SKIP_CNT) {
		mutex_lock(&device->flickerdatalock);
		for (i = 0; i < AWB_INTERVAL; i++) {
			if (device->flicker_data_cnt < FLICKER_DATA_CNT) {
				device->flicker_data[device->flicker_data_cnt++] =
					(recvData[0 + i*NUM_BYTES_PER_SAMPLE] << 16 & 0x70000)
					+ (recvData[1 + i*NUM_BYTES_PER_SAMPLE] << 8)
					+ (recvData[2 + i*NUM_BYTES_PER_SAMPLE] << 0);
			}
		}
		mutex_unlock(&device->flickerdatalock);
	}

	/* Change Configuation */
	if (device->awb_flicker_status == AWB_CONFIG1) {
		if (*data > AWB_MAX86915_CONFIG_TH2
			&& previous_awb_data > AWB_MAX86915_CONFIG_TH2) { /* Change to AWB_CONFIG2 */
			err = __max869_write_reg(device, MAX86915_MODE_CONFIGURATION, 0x00);
			/* 16384 uA setting */
			err = __max869_write_reg(device,
				MAX86915_MODE_CONFIGURATION_2, 0x6F);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
					__func__);
				return -EIO;
			}
			device->awb_flicker_status = AWB_CONFIG2;
			mode_changed = 1;
		}
	} else if (device->awb_flicker_status == AWB_CONFIG2) {
		if (*data < AWB_MAX86915_CONFIG_TH3
			&& previous_awb_data < AWB_MAX86915_CONFIG_TH3) { /* Change to AWB_CONFIG1 */
			err = __max869_write_reg(device, MAX86915_MODE_CONFIGURATION, 0x00);
			/* 4096 uA setting */
			err = __max869_write_reg(device,
				MAX86915_MODE_CONFIGURATION_2, 0x0F);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
					__func__);
				return -EIO;
			}
			device->awb_flicker_status = AWB_CONFIG1;
			mode_changed = 1;
		}
	}

	if (device->awb_sample_cnt > CONFIG_SKIP_CNT) {
		if (device->awb_flicker_status < AWB_CONFIG2)
			*data = *data >> 3; /* 2uA setting should devided by 8 */
		ret = 0;
	} else {
		ret = 1;
		HRM_dbg("%s - awb sample cnt %d, less than CONFIG SKIP CNT\n", __func__, device->awb_sample_cnt);
	}

	if (mode_changed == 1) {
		/* Flush Buffer */
		err = __max869_write_reg(device,
			MAX86915_MODE_CONFIGURATION, 0x07);
		if (err != 0) {
			HRM_dbg("%s - error init MAX86915_MODE_CONFIGURATION!\n",
				__func__);
			return -EIO;
		}
		device->flicker_data_cnt = 0;
		device->awb_sample_cnt = 0;
		ret = 1;
		HRM_dbg("%s - mode changed to : %d\n", __func__, device->awb_flicker_status);
	} else
		device->awb_sample_cnt += AWB_INTERVAL;

	return ret;
}

int max869_i2c_read(u32 reg, u32 *value, u32 *size)
{
	int err;

	*value = reg;
	err = __max869_read_reg(max869_data, (u8 *)value, 1);

	*size = 1;

	return err;
}

int max869_i2c_write(u32 reg, u32 value)
{
	int err;

	err = __max869_write_reg(max869_data, (u8)reg, (u8)value);

	return err;
}

static long __max869_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	int ret = 0;

	struct max869_device_data *data = container_of(file->private_data,
	struct max869_device_data, miscdev);

	/* HRM_info("%s - ioctl start\n", __func__); */
	mutex_lock(&data->flickerdatalock);

	switch (cmd) {
	case MAX86915_IOCTL_READ_FLICKER:
		ret = copy_to_user(argp,
			data->flicker_data,
			sizeof(int)*FLICKER_DATA_CNT);

		if (unlikely(ret))
			goto ioctl_error;

		break;

	default:
		HRM_dbg("%s - invalid cmd\n", __func__);
		break;
	}

	mutex_unlock(&data->flickerdatalock);
	return ret;

ioctl_error:
	mutex_unlock(&data->flickerdatalock);
	HRM_dbg("%s - read flicker data err(%d)\n", __func__, ret);
	return -ret;
}

static const struct file_operations __max869_fops = {
	.owner = THIS_MODULE,
	.open = nonseekable_open,
	.unlocked_ioctl = __max869_ioctl,
};


static void __max869_init_agc_settings(struct max869_device_data *data)
{
	data->agc_is_enable = true;
	data->update_led = max86915_update_led_current;
	data->agc_led_out_percent = MAX86915_AGC_DEFAULT_LED_OUT_RANGE;
	data->agc_corr_coeff = MAX86915_AGC_DEFAULT_CORRECTION_COEFF;
	data->agc_min_num_samples = MAX86915_AGC_DEFAULT_MIN_NUM_PERCENT;
	data->agc_sensitivity_percent = MAX86915_AGC_DEFAULT_SENSITIVITY_PERCENT;
	data->threshold_default = MAX86915_THRESHOLD_DEFAULT;
}

static int __max869_trim_check(void)
{
	u8 recvData;
	u8 reg93_val;
	int err;

	max869_data->isTrimmed = 0;

	err = __max869_write_reg(max869_data, 0xFF, 0x54);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_TEST0!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(max869_data, 0xFF, 0x4d);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_TEST1!\n",
			__func__);
		return -EIO;
	}

	recvData = 0x93;
	err = __max869_read_reg(max869_data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - max86915_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}
	reg93_val = recvData;

	err = __max869_write_reg(max869_data, 0xFF, 0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_TEST3!\n",
			__func__);
		return -EIO;
	}

	max869_data->isTrimmed = (reg93_val & 0x20);
	HRM_dbg("%s isTrimmed :%d, reg93_val : %d\n", __func__, max869_data->isTrimmed, reg93_val);

	return err;
}

static int __max869_init_var(struct max869_device_data *data)
{
	int err;
	u8 buffer[2] = {0, };

	buffer[0] = MAX86915_REV_ID_REG;
	err = __max869_read_reg(data, buffer, 2);
	if (err) {
		HRM_dbg("%s MAX86915 WHOAMI read fail0\n", __func__);
		err = -ENODEV;
		goto err_of_read_chipid;
	}
	data->ir_led = 0;
	data->red_led = 1;
	data->green_led = 2;
	data->blue_led = 3;
	data->i2c_err_cnt = 0;
	data->ir_curr = 0;
	data->red_curr = 0;
	data->green_curr = 0;
	data->blue_curr = 0;
	data->ir_adc = 0;
	data->red_adc = 0;
	data->green_adc = 0;
	data->blue_adc = 0;

	if (buffer[1] == MAX86915_PART_ID) {
		data->default_current1 = MAX86915_IR_INIT_CURRENT;
		data->default_current2 = MAX86915_RED_INIT_CURRENT;
		data->default_current3 = MAX86915_GREEN_INIT_CURRENT;
		data->default_current4 = MAX86915_BLUE_INIT_CURRENT;

		data->default_xtalk_code1 = MAX86915_DEFAULT_XTALK_CODE1;
		data->default_xtalk_code2 = MAX86915_DEFAULT_XTALK_CODE2;
		data->default_xtalk_code3 = MAX86915_DEFAULT_XTALK_CODE3;
		data->default_xtalk_code4 = MAX86915_DEFAULT_XTALK_CODE4;

		data->part_type = __max869_get_part_id(data);
	} else {
		HRM_dbg("%s MAX86915 WHOAMI read fail\n", __func__);
		err = -ENODEV;
		goto err_of_read_chipid;
	}

	/* AGC setting */
	__max869_init_agc_settings(data);

	return 0;

err_of_read_chipid:
	return -EINVAL;
}

int max869_init_device(struct max869_device_data *data)
{
	int err = 0;

	data->miscdev.minor = MISC_DYNAMIC_MINOR;
	data->miscdev.name = "max_hrm";
	data->miscdev.fops = &__max869_fops;
	data->miscdev.mode = S_IRUGO;
	err = misc_register(&data->miscdev);
	if (err < 0) {
		HRM_dbg("%s - failed to register Device\n", __func__);
		goto err_register_fail;
	}

	data->flicker_data = kzalloc(sizeof(int)*FLICKER_DATA_CNT, GFP_KERNEL);
	if (data->flicker_data == NULL) {
		HRM_dbg("%s - couldn't allocate flicker memory\n", __func__);
		goto err_flicker_alloc_fail;
	}

	mutex_init(&data->flickerdatalock);

	data->threshold_default = MAX86915_THRESHOLD_DEFAULT;

	err = __max869_init_var(data);
	if (err < 0) {
		err = -EIO;
		goto err_init_fail;
	}

	if (__max869_init(data) < 0) {
		err = -EIO;
		HRM_dbg("%s __max869_init failed\n", __func__);
		goto err_init_fail;
	}

	__max869_trim_check();

	goto done;

err_init_fail:
	mutex_destroy(&data->flickerdatalock);
	kfree(data->flicker_data);
err_flicker_alloc_fail:
	misc_deregister(&data->miscdev);
err_register_fail:
	kfree(data);
	HRM_dbg("%s failed\n", __func__);

done:
	return err;
}


int max869_deinit_device(struct max869_device_data *data)
{
	mutex_destroy(&data->flickerdatalock);
	misc_deregister(&data->miscdev);

	kfree(data->flicker_data);
	kfree(data);
	data = NULL;

	return 0;
}


int max869_enable(enum hrm_mode mode)
{
	int err = 0;

	max869_data->hrm_mode = mode;

	HRM_dbg("%s - enable_m : 0x%x\t cur_m : 0x%x\t cur_p : 0x%x\n",
		__func__, mode, max869_data->hrm_mode, max869_data->part_type);

	__max869_print_mode(max869_data->hrm_mode);

	if (mode == MODE_HRM)
		err = __max869_set_reg_hrm(max869_data);
	else if (mode == MODE_AMBIENT)
		err = __max869_set_reg_ambient(max869_data);
	else if (mode == MODE_PROX)
		err = __max869_set_reg_prox(max869_data);
	else if (mode == MODE_SDK_IR)
		err = __max869_set_reg_sdk(max869_data);
	else
		HRM_dbg("%s - MODE_UNKNOWN\n", __func__);

	if (max869_data->agc_mode != M_NONE) {
		agc_pre_s = S_UNKNOWN;
		agc_cur_s = S_INIT;
	}

	max869_data->agc_sample_cnt[0] = 0;
	max869_data->agc_sample_cnt[1] = 0;
	max869_data->agc_sample_cnt[2] = 0;
	max869_data->agc_sample_cnt[3] = 0;

	return err;
}

int max869_disable(enum hrm_mode mode)
{
	int err = 0;

	max869_data->hrm_mode = mode;
	HRM_info("%s - disable_m : 0x%x\t cur_m : 0x%x\n", __func__, mode, max869_data->hrm_mode);
	__max869_print_mode(max869_data->hrm_mode);

	if (max869_data->hrm_mode == 0) {
		__max869_disable(max869_data);
		return err;
	}

	if (max869_data->agc_mode != M_NONE) {
		agc_pre_s = S_UNKNOWN;
		agc_cur_s = S_INIT;
	}

	return err;
}

int max869_get_current(u8 *d1, u8 *d2, u8 *d3, u8 *d4)
{
	*d1 = max869_data->led_current1; /* IR */
	*d2 = max869_data->led_current2; /* RED */
	*d3 = max869_data->led_current3; /* GEEN */
	*d4 = max869_data->led_current4; /* BLUE */

	return 0;
}

int max869_set_current(u8 d1, u8 d2, u8 d3, u8 d4)
{
	int err = 0;
	u8 recvData;

	max869_data->led_current1 = d1; /* set ir; */
	max869_data->led_current2 = d2; /* set red; */
	max869_data->led_current3 = d3; /* set ir; */
	max869_data->led_current4 = d4; /* set red; */
	HRM_info("%s - 0x%x 0x%x 0x%x 0x%x\n", __func__,
			max869_data->led_current1, max869_data->led_current2,
			max869_data->led_current3, max869_data->led_current4);

	if (max869_data->part_type < PART_TYPE_MAX86915_CS_211) {
		err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION, 0x00);

		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
				__func__);
			return -EIO;
		}
	}

	err = __max869_write_reg(max869_data, MAX86915_LED1_PA,
			max869_data->led_current1);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED1_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(max869_data, MAX86915_LED2_PA,
			max869_data->led_current2);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED2_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(max869_data, MAX86915_LED3_PA,
			max869_data->led_current3);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED3_PA!\n",
			__func__);
		return -EIO;
	}
	err = __max869_write_reg(max869_data, MAX86915_LED4_PA,
			max869_data->led_current4);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_LED4_PA!\n",
			__func__);
		return -EIO;
	}

	if (max869_data->part_type < PART_TYPE_MAX86915_CS_211) {
		recvData = MAX86915_FIFO_CONFIG;
		err = __max869_read_reg(max869_data, &recvData, 1);
		max869_data->agc_led_set = AGC_LED_SKIP_CNT >>
			((recvData & MAX86915_SMP_AVE_MASK) >> MAX86915_SMP_AVE_OFFSET);
		err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION, 0x03);
	}

	return err;
}

int max869_get_led_test(u8 *result)
{
	int err = 0;
	u8 recvData = 0;
	int i = 0;
	int retry = 0;

	*result = 0;

	err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION,
		0x1 << MAX86915_RESET_OFFSET);
	if (err != 0) {
		HRM_dbg("%s - error writing MAX86915_MODE_CONFIGURATION err:%d\n",
			__func__, err);
		return -EIO;
	}

	usleep_range(100000, 110000);

	err = __max869_write_reg(max869_data, MAX86915_LED1_PA, 0xFF);
	if (err != 0) {
		HRM_dbg("%s - error writing MAX86915_LED1_PA err:%d\n",
			__func__, err);
		return -EIO;
	}

	err = __max869_write_reg(max869_data, MAX86915_LED2_PA, 0xFF);
	if (err != 0) {
		HRM_dbg("%s - error writing MAX86915_LED2_PA err:%d\n",
			__func__, err);
		return -EIO;
	}

	err = __max869_write_reg(max869_data, MAX86915_LED3_PA, 0x08);
	if (err != 0) {
		HRM_dbg("%s - error writing MAX86915_LED3_PA err:%d\n",
			__func__, err);
		return -EIO;
	}

	err = __max869_write_reg(max869_data, MAX86915_LED4_PA, 0x08);
	if (err != 0) {
		HRM_dbg("%s - error writing MAX86915_LED4_PA err:%d\n",
			__func__, err);
		return -EIO;
	}

	err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION, 0x01);
	if (err != 0) {
		HRM_dbg("%s - error writing MAX86915_MODE_CONFIGURATION err:%d\n",
			__func__, err);
		return -EIO;
	}

	usleep_range(1200, 1210);

	for (retry = 0 ; retry < 3; retry++) {
		*result = 0;

		for (i = 0; i < MAX_LED_NUM; i++) {
			err = __max869_write_reg(max869_data, 0x31, 0x10 << i);
			if (err != 0) {
				HRM_dbg("%s - error writing MAX86915_LED_TEST_EN%d err:%d\n",
					__func__, i, err);
				return -EIO;
			}

			usleep_range(100, 110);

			err = __max869_write_reg(max869_data, 0x31, (0x10 << i) | 0x01);
			if (err != 0) {
				HRM_dbg("%s - error writing MAX86915_LED_TEST_EN%d err:%d\n",
					__func__, i, err);
				return -EIO;
			}

			usleep_range(1000, 1100);

			recvData = 0x32;

			err = __max869_read_reg(max869_data, &recvData, 1);
			if (err != 0) {
				HRM_dbg("%s - error reading MAX86915_COMP_OUT%d err:%d\n",
					__func__, i, err);
				return -EIO;
			}

			*result |= (recvData & (1 << i));
		}

		if (*result == 0)
			break;

		HRM_dbg("%s - fail retry %d = 0x%02x\n", __func__, retry, *result);
	}

	err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION,
		0x1 << MAX86915_SHDN_OFFSET);
	if (err != 0) {
		HRM_dbg("%s - error writing MAX86915_MODE_CONFIGURATION err:%d\n",
			__func__, err);
		return -EIO;
	}

	return 0;
}

int max869_get_xtalk(u8 *d1, u8 *d2, u8 *d3, u8 *d4)
{
	*d1 = max869_data->xtalk_code1; /* IR */
	*d2 = max869_data->xtalk_code2; /* RED */
	*d3 = max869_data->xtalk_code3; /* GEEN */
	*d4 = max869_data->xtalk_code4; /* BLUE */

	return 0;
}

int max869_set_xtalk(u8 d1, u8 d2, u8 d3, u8 d4)
{
	int err = 0;

	max869_data->xtalk_code1 = d1; /* set ir; */
	max869_data->xtalk_code2 = d2; /* set red; */
	max869_data->xtalk_code3 = d3; /* set ir; */
	max869_data->xtalk_code4 = d4; /* set red; */
	HRM_info("%s - 0x%x 0x%x 0x%x 0x%x\n", __func__,
			max869_data->xtalk_code1, max869_data->xtalk_code2,
			max869_data->xtalk_code3, max869_data->xtalk_code4);

	if (max869_data->led_current1 == MAX86915_MIN_CURRENT)
		max869_data->default_current1 = MAX86915_IR_INIT_CURRENT
					+ MAX86915_IR_CURRENT_STEP * max869_data->xtalk_code1;

	if (max869_data->led_current2 == MAX86915_MIN_CURRENT)
		max869_data->default_current2 = MAX86915_RED_INIT_CURRENT
					+ MAX86915_RED_CURRENT_STEP * max869_data->xtalk_code2;

	if (max869_data->led_current3 == MAX86915_MIN_CURRENT)
		max869_data->default_current3 = MAX86915_GREEN_INIT_CURRENT
					+ MAX86915_GREEN_CURRENT_STEP * max869_data->xtalk_code3;

	if (max869_data->led_current4 == MAX86915_MIN_CURRENT)
		max869_data->default_current4 = MAX86915_BLUE_INIT_CURRENT
					+ MAX86915_BLUE_CURRENT_STEP * max869_data->xtalk_code4;

	HRM_info("%s - current 0x%x 0x%x 0x%x 0x%x\n", __func__,
			max869_data->default_current1, max869_data->default_current2,
			max869_data->default_current3, max869_data->default_current4);

	if (max869_data->hrm_mode) {
		err = __max869_write_reg(max869_data, MAX86915_DAC1_XTALK_CODE,
				max869_data->xtalk_code1);
		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_DAC1_XTALK_CODE!\n",
				__func__);
			return -EIO;
		}
		err = __max869_write_reg(max869_data, MAX86915_DAC2_XTALK_CODE,
				max869_data->xtalk_code2);
		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_DAC2_XTALK_CODE!\n",
				__func__);
			return -EIO;
		}
		err = __max869_write_reg(max869_data, MAX86915_DAC3_XTALK_CODE,
				max869_data->xtalk_code3);
		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_DAC3_XTALK_CODE!\n",
				__func__);
			return -EIO;
		}
		err = __max869_write_reg(max869_data, MAX86915_DAC4_XTALK_CODE,
				max869_data->xtalk_code4);
		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_DAC4_XTALK_CODE!\n",
				__func__);
			return -EIO;
		}
	}

	return err;
}

int max869_get_sdk_enabled(u8 *enabled)
{
	*enabled = max869_data->mode_sdk_enabled;
	return 0;
}

int max869_set_sdk_enabled(int channel, int enable)
{
	HRM_dbg("%s - channel enable %d, %d\n", __func__, channel, enable);

	if (!enable) {
		/* LED PA to 0 */
		switch (channel) {
		case 0:
			max869_data->led_current1 = 0x0;
			max869_data->reached_thresh[AGC_IR] = 0;
			max869_data->agc_sum[AGC_IR] = 0;
			max869_data->agc_sample_cnt[AGC_IR] = 0;
			max869_data->agc_current[AGC_IR] =
				(max869_data->led_current1 * MAX86915_CURRENT_PER_STEP);
			break;
		case 1:
			max869_data->led_current2 = 0x0;
			max869_data->reached_thresh[AGC_RED] = 0;
			max869_data->agc_sum[AGC_RED] = 0;
			max869_data->agc_sample_cnt[AGC_RED] = 0;
			max869_data->agc_current[AGC_RED] =
				(max869_data->led_current2 * MAX86915_CURRENT_PER_STEP);
			break;
		case 2:
			max869_data->led_current3 = 0x0;
			max869_data->reached_thresh[AGC_GREEN] = 0;
			max869_data->agc_sum[AGC_GREEN] = 0;
			max869_data->agc_sample_cnt[AGC_GREEN] = 0;
			max869_data->agc_current[AGC_GREEN] =
				(max869_data->led_current3 * MAX86915_CURRENT_PER_STEP);
			break;
		case 3:
			max869_data->led_current4 = 0x0;
			max869_data->reached_thresh[AGC_BLUE] = 0;
			max869_data->agc_sum[AGC_BLUE] = 0;
			max869_data->agc_sample_cnt[AGC_BLUE] = 0;
			max869_data->agc_current[AGC_BLUE] =
				(max869_data->led_current4 * MAX86915_CURRENT_PER_STEP);
			break;

		}
		max869_set_current(max869_data->led_current1,
			max869_data->led_current2,
			max869_data->led_current3,
			max869_data->led_current4);

	}
	return 0;
}

int max869_read_data(struct hrm_output_data *data)
{
	int err = 0;
	int ret;
	int raw_data[4] = {0x00, };
	u8 recvData;
	int i = 0;
	int j = 0;
	int data_fifo[MAX_LED_NUM][MAX86915_FIFO_SIZE];

	data->sub_num = 0;
	data->fifo_num = 0;

	for (i = 0; i < MAX86915_FIFO_SIZE; i++) {
		for (j = 0; j < MAX_LED_NUM; j++) {
			data_fifo[j][i] = 0;
			data->fifo_main[j][i] = 0;
		}
	}

	switch (max869_data->hrm_mode) {
	case  MODE_HRM:
#ifndef ENABLE_POLL_DELAY
	case MODE_SDK_IR:
#endif
	case  MODE_PROX:
		if (max869_data->eol_test_is_enable) {
			err = __max86915_eol_read_data(data, max869_data, raw_data);
		} else {
			err = __max86915_hrm_read_data(max869_data, raw_data);
		}

		break;
	case MODE_AMBIENT:
		err = __max86915_awb_flicker_read_data(max869_data, raw_data);
		break;
#ifdef ENABLE_POLL_DELAY
	case MODE_SDK_IR:
		err = __max869_fifo_irq_handler(max869_data);
#ifdef CONFIG_4CH_IOCTL
		mutex_lock(&max869_data->flickerdatalock);

		for (i = 0; i < max869_data->fifo_samples; i++) {
			if (max869_data->agc_led_set > 0) {
				max869_data->flicker_data[i*4+0] = max869_data->agc_ch_adc[0];
				max869_data->flicker_data[i*4+1] = max869_data->agc_ch_adc[1];
				max869_data->flicker_data[i*4+2] = max869_data->agc_ch_adc[2];
				max869_data->flicker_data[i*4+3] = max869_data->agc_ch_adc[3];
			} else {
				max869_data->flicker_data[i*4+0] = max869_data->fifo_data[0][i];
				max869_data->flicker_data[i*4+1] = max869_data->fifo_data[1][i];
				max869_data->flicker_data[i*4+2] = max869_data->fifo_data[2][i];
				max869_data->flicker_data[i*4+3] = max869_data->fifo_data[3][i];

				max869_data->agc_ch_adc[0] = max869_data->fifo_data[0][max869_data->fifo_samples-1];
				max869_data->agc_ch_adc[1] = max869_data->fifo_data[1][max869_data->fifo_samples-1];
				max869_data->agc_ch_adc[2] = max869_data->fifo_data[2][max869_data->fifo_samples-1];
				max869_data->agc_ch_adc[3] = max869_data->fifo_data[3][max869_data->fifo_samples-1];
			}
		}

		mutex_unlock(&max869_data->flickerdatalock);
#endif
#ifdef CONFIG_4CH_INPUT
		for (i = 0; i < max869_data->fifo_samples; i++) {
			if (max869_data->agc_led_set > 0) {
				data->fifo_main[AGC_IR][i] = max869_data->agc_ch_adc[0];
				data->fifo_main[AGC_RED][i] = max869_data->agc_ch_adc[1];
				data->fifo_main[AGC_GREEN][i] = max869_data->agc_ch_adc[2];
				data->fifo_main[AGC_BLUE][i] = max869_data->agc_ch_adc[3];
			} else {
				data->fifo_main[AGC_IR][i] = max869_data->fifo_data[0][i];
				data->fifo_main[AGC_RED][i] = max869_data->fifo_data[1][i];
				data->fifo_main[AGC_GREEN][i] = max869_data->fifo_data[2][i];
				data->fifo_main[AGC_BLUE][i] = max869_data->fifo_data[3][i];

				max869_data->agc_ch_adc[0] = max869_data->fifo_data[0][max869_data->fifo_samples-1];
				max869_data->agc_ch_adc[1] = max869_data->fifo_data[1][max869_data->fifo_samples-1];
				max869_data->agc_ch_adc[2] = max869_data->fifo_data[2][max869_data->fifo_samples-1];
				max869_data->agc_ch_adc[3] = max869_data->fifo_data[3][max869_data->fifo_samples-1];
			}
		}
		data->fifo_num = fifo_full_cnt;
#endif
		if (max869_data->agc_led_set == 0) /* for AGC */
			max869_data->sample_cnt += max869_data->fifo_samples;
		break;
#endif

	default:
		err = 1; /* error case */
		break;
	}

	if (err < 0)
		HRM_dbg("__max86915_hrm_read_data err : %d\n", err);

	if (err == 0) {
		data->main_num = 2;
		if (max869_data->hrm_mode == MODE_AMBIENT) {
			data->data_main[0] = raw_data[0];
			if (max869_data->flicker_data_cnt == FLICKER_DATA_CNT) {
				data->data_main[1] = -2;
				max869_data->flicker_data_cnt = 0;
			} else {
				data->data_main[1] = 0;
			}
#ifndef ENABLE_POLL_DELAY
		} else if (max869_data->hrm_mode == MODE_SDK_IR) {
			data->main_num = 4;
			data->data_main[0] = raw_data[0];
			data->data_main[1] = raw_data[1];
			data->data_main[2] = raw_data[2];
			data->data_main[3] = raw_data[3];
#else
		} else if (max869_data->hrm_mode == MODE_SDK_IR) {
#ifdef CONFIG_4CH_IOCTL
			data->data_main[1] = -3;
#endif
#ifdef CONFIG_4CH_INPUT
			data->data_main[1] = -3;
			raw_data[0] = data->fifo_main[AGC_IR][0];
			raw_data[1] = data->fifo_main[AGC_RED][0];
			raw_data[2] = data->fifo_main[AGC_GREEN][0];
			raw_data[3] = data->fifo_main[AGC_BLUE][0];
#endif
#endif
		} else if (max869_data->hrm_mode == MODE_HRM) {
			data->main_num = 2;
			data->data_main[0] = raw_data[0];
			data->data_main[1] = raw_data[1];
#ifdef CONFIG_HRM_GREEN_BLUE
			data->main_num = 4;
			data->data_main[2] = raw_data[2];
			data->data_main[3] = raw_data[3];
#endif
		} else {
			data->main_num = 2;
			data->data_main[0] = raw_data[0];
			data->data_main[1] = raw_data[1];
		}

		data->mode = max869_data->hrm_mode;
		ret = 0;
	} else
		ret = 1;

	/* Interrupt Clear */
	recvData = MAX86915_INTERRUPT_STATUS;
	err = __max869_read_reg(max869_data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - __max869_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}

	if (!max869_data->agc_enabled || max869_data->hrm_mode == MODE_PROX) {
		if (max869_data->sample_cnt > AGC_SKIP_CNT)
			max869_data->agc_enabled = 1;
		else {
			data->main_num = 0;
			data->fifo_num = 0;
		}
	}

	if (max869_data->agc_mode != M_NONE
		&& agc_is_enabled
		&& max869_data->agc_enabled
		&& (max869_data->agc_led_set == 0)) {
		__max869_cal_agc(raw_data[0], raw_data[1], raw_data[2], raw_data[3]);
	}

	if (max869_data->agc_led_set > 0) {
		if (max869_data->hrm_mode == MODE_SDK_IR) {
			if (max869_data->agc_led_set < fifo_full_cnt)
				max869_data->agc_led_set = 0;
			else
				max869_data->agc_led_set -= fifo_full_cnt;
		} else {
			max869_data->agc_led_set--;
		}
	}

	return ret;
}

int max869_get_chipid(u64 *chip_id)
{
	u8 recvData;
	int err;
	u32 c1_code = 0;
	u32 c2_code = 0;
	u32 c3_code = 0;
	u32 c4_code = 0;

	*chip_id = 0;

	err = __max869_write_reg(max869_data, 0xFF, 0x54);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_TEST0!\n",
			__func__);
		return -EIO;
	}

	err = __max869_write_reg(max869_data, 0xFF, 0x4d);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86915_MODE_TEST1!\n",
			__func__);
		return -EIO;
	}

	recvData = 0xC1;
	err = __max869_read_reg(max869_data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - max86915_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}
	c1_code = recvData;

	recvData = 0xC2;
	err = __max869_read_reg(max869_data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - max86915_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}
	c2_code = recvData;

	recvData = 0xC3;
	err = __max869_read_reg(max869_data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - max86915_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}
	c3_code = recvData;

	recvData = 0xC4;
	err = __max869_read_reg(max869_data, &recvData, 1);
	if (err != 0) {
		HRM_dbg("%s - max86915_read_reg err:%d, address:0x%02x\n",
			__func__, err, recvData);
		return -EIO;
	}
	c4_code = recvData;

	err = __max869_write_reg(max869_data, 0xFF, 0x00);
	if (err != 0) {
		HRM_dbg("%s - error initializing MAX86900_MODE_TEST0!\n",
			__func__);
		return -EIO;
	}

	*chip_id = (((u64)c1_code) << 24) + (((u64)c2_code) << 16)
		+ (((u64)c3_code) << 8) + (c4_code);

	HRM_info("%s - Device ID = %lld\n", __func__, *chip_id);

	return 0;
}

int max869_get_part_type(u16 *part_type)
{
	int err;
	u8 buffer[2] = {0, };

	buffer[0] = MAX86915_REV_ID_REG;
	err = __max869_read_reg(max869_data, buffer, 2);
	if (err) {
		HRM_dbg("%s Max86915 WHOAMI read fail0\n", __func__);
		err = -ENODEV;
		goto err_of_read_part_type;
	}

	if (buffer[1] == MAX86915_PART_ID) {
		max869_data->part_type = __max869_get_part_id(max869_data);
	} else {
		HRM_dbg("%s Max86915 WHOAMI read fail2\n", __func__);
		err = -ENODEV;
		goto err_of_read_part_type;
	}

	*part_type = max869_data->part_type;

	return err;

err_of_read_part_type:
	return -EINVAL;
}

int max869_get_i2c_err_cnt(u32 *err_cnt)
{
	int err = 0;

	*err_cnt = max869_data->i2c_err_cnt;

	return err;
}

int max869_set_i2c_err_cnt(void)
{
	int err = 0;

	max869_data->i2c_err_cnt = 0;

	return err;
}

int max869_get_curr_adc(u16 *ir_curr, u16 *red_curr, u32 *ir_adc, u32 *red_adc)
{
	int err = 0;

	*ir_curr = max869_data->ir_curr;
	*red_curr = max869_data->red_curr;
	/*
	 * *green_curr = max869_data->green_curr;
	 * *blue_curr = max869_data->blue_curr;
	 */
	*ir_adc = max869_data->ir_adc;
	*red_adc = max869_data->red_adc;
	/*
	 * *green_adc = max869_data->green_adc;
	 * *blue_adc = max869_data->blue_adc;
	 */

	return err;
}

int max869_set_curr_adc(void)
{
	int err = 0;

	max869_data->ir_curr = 0;
	max869_data->red_curr = 0;
	max869_data->ir_adc = 0;
	max869_data->red_adc = 0;

	return err;
}

int max869_get_name_chipset(char *name)
{
	if (max869_data->part_type >= PART_TYPE_MAX86915_ES)
		strlcpy(name, MAX86915_CHIP_NAME, strlen(MAX86915_CHIP_NAME) + 1);
	else
		strlcpy(name, "NONE", 5);

	return 0;
}

int max869_get_name_vendor(char *name)
{
	strlcpy(name, VENDOR, strlen(VENDOR) + 1);
	if (strncmp(name, VENDOR, strlen(VENDOR) + 1))
		return -EINVAL;
	else
		return 0;
}

int max869_get_threshold(s32 *threshold)
{
	*threshold = max869_data->threshold_default;
	if (*threshold != max869_data->threshold_default)
		return -EINVAL;
	else
		return 0;
}

int max869_set_threshold(s32 threshold)
{
	max869_data->threshold_default = threshold;
	if (threshold != max869_data->threshold_default)
		return -EINVAL;
	else
		return 0;
}

int max869_set_eol_enable(u8 enable)
{
	int err = 0;

	HRM_dbg("%s - CONFIG_SENSORS_HRM_MAX869_NEW_EOL\n", __func__);
	__max86915_eol_test_onoff(max869_data, enable);

	return err;
}

int max869_debug_set(u8 mode)
{
	HRM_info("%s - mode = %d\n", __func__, mode);

	switch (mode) {
	case DEBUG_WRITE_REG_TO_FILE:
		__max869_write_reg_to_file();
		break;
	case DEBUG_WRITE_FILE_TO_REG:
		__max869_write_file_to_reg();
		__max869_write_default_regs();
		break;
	case DEBUG_SHOW_DEBUG_INFO:
		break;
	case DEBUG_ENABLE_AGC:
		agc_is_enabled = 1;
		break;
	case DEBUG_DISABLE_AGC:
		agc_is_enabled = 0;
		break;
	default:
		break;
	}
	return 0;
}

int max869_set_sampling_rate(u32 sampling_period_ns)
{
	int err = 0;
	u8 recvData;

	switch (sampling_period_ns) {
	case 10000000:
		fifo_full_cnt = 1;
		break;
	case 5000000:
		fifo_full_cnt = 2;
		break;
	case 2500000:
		fifo_full_cnt = 4;
		break;
	case 1250000:
		fifo_full_cnt = 8;
		break;
	default:
		HRM_dbg("%s - %dns is not supported.\n",	__func__, sampling_period_ns);
		return -EIO;
	}

	if (max869_data->hrm_mode == MODE_SDK_IR) {

		err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION, 0x00);
		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION!\n",
				__func__);
			return -EIO;
		}

		if (fifo_full_cnt == 8) {
			/* 16uA, 800Hz, 70us */
			err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION_2,
					0x50);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
					__func__);
				return -EIO;
			}
			/* 1 Samples avg */
			err = __max869_write_reg(max869_data, MAX86915_FIFO_CONFIG,
					0x1f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
					__func__);
				return -EIO;
			}
		} else if (fifo_full_cnt == 4) {
			/* 16uA, 400Hz, 120us */
			err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION_2,
					0x50);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
					__func__);
				return -EIO;
			}
			/* 2 Samples avg */
			err = __max869_write_reg(max869_data, MAX86915_FIFO_CONFIG,
					0x3f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
					__func__);
				return -EIO;
			}
		} else if (fifo_full_cnt == 2) {
			/* 16uA, 200Hz, 120us */
			err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION_2,
					0x50);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
					__func__);
				return -EIO;
			}
			/* 4 Samples avg */
			err = __max869_write_reg(max869_data, MAX86915_FIFO_CONFIG,
					0x5f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
					__func__);
				return -EIO;
			}
		} else if (fifo_full_cnt == 1) {
			/* 32uA, 400Hz, 120us */
			err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION_2,
					0x6D);
			/* 4 Samples avg */
			err = __max869_write_reg(max869_data, MAX86915_FIFO_CONFIG,
					0x5f);
			if (err != 0) {
				HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
					__func__);
				return -EIO;
			}
		}
		/* SDK mode only use FLEX MODE 0x03 After changing FIFO CONFIG skip initial sample. */
		recvData = MAX86915_FIFO_CONFIG;
		err = __max869_read_reg(max869_data, &recvData, 1);

		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_FIFO_CONFIG!\n",
				__func__);
			return -EIO;
		}
		max869_data->agc_led_set = AGC_LED_SKIP_CNT >>
			((recvData & MAX86915_SMP_AVE_MASK) >> MAX86915_SMP_AVE_OFFSET);
		err = __max869_write_reg(max869_data, MAX86915_MODE_CONFIGURATION, 0x03);

		if (err != 0) {
			HRM_dbg("%s - error initializing MAX86915_MODE_CONFIGURATION_2!\n",
				__func__);
			return -EIO;
		}

		max869_data->sample_cnt = 0;
		err = sampling_period_ns;
	}

	return err;
}

int max869_get_fac_cmd(char *cmd_result)
{
	if (max869_data->isTrimmed)
		return snprintf(cmd_result, MAX_BUF_LEN, "%d", 1);
	else
		return snprintf(cmd_result, MAX_BUF_LEN, "%d", 0);
}

/* #define DEBUG_HRMSENSOR */
#ifdef DEBUG_HRMSENSOR
static void __hrm_debug_device_data(struct max869_device_data *data)
{
	HRM_dbg("===== %s =====\n", __func__);
	HRM_dbg("%s client %p slave_addr 0x%x\n", __func__,
		data->client, data->client->addr);
	HRM_dbg("%s dev %p\n", __func__, data->dev);
	HRM_dbg("%s hrm_input_dev %p\n", __func__, data->hrm_input_dev);
	HRM_dbg("%s hrm_pinctrl %p\n", __func__, data->hrm_pinctrl);
	HRM_dbg("%s pins_sleep %p\n", __func__, data->pins_sleep);
	HRM_dbg("%s pins_idle %p\n", __func__, data->pins_idle);
	HRM_dbg("%s led_3p3 %s\n", __func__, data->led_3p3);
	HRM_dbg("%s vdd_1p8 %s\n", __func__, data->vdd_1p8);
	HRM_dbg("%s i2c_1p8 %s\n", __func__, data->i2c_1p8);
	HRM_dbg("%s hrm_enabled_mode %d\n", __func__, data->hrm_enabled_mode);
	HRM_dbg("%s hrm_sampling_rate %d\n", __func__, data->hrm_sampling_rate);
	HRM_dbg("%s regulator_state %d\n", __func__, data->regulator_state);
	HRM_dbg("%s hrm_int %d\n", __func__, data->hrm_int);
	HRM_dbg("%s hrm_en %d\n", __func__, data->hrm_en);
	HRM_dbg("%s hrm_irq %d\n", __func__, data->hrm_irq);
	HRM_dbg("%s irq_state %d\n", __func__, data->irq_state);
	HRM_dbg("%s led_current %d\n", __func__, data->led_current);
	HRM_dbg("%s xtalk_code %d\n", __func__, data->xtalk_code);
	HRM_dbg("%s hrm_threshold %d\n", __func__, data->hrm_threshold);
	HRM_dbg("%s eol_test_is_enable %d\n", __func__,
		data->eol_test_is_enable);
	HRM_dbg("%s eol_test_status %d\n", __func__,
		data->eol_test_status);
	HRM_dbg("%s pre_eol_test_is_enable %d\n", __func__,
		data->pre_eol_test_is_enable);
	HRM_dbg("%s lib_ver %s\n", __func__, data->lib_ver);
	HRM_dbg("===== %s =====\n", __func__);
}
#endif

static void max86915_init_device_data(struct max869_device_data *data)
{
	data->client = NULL;
	data->dev = NULL;
	data->hrm_input_dev = NULL;
	data->hrm_pinctrl = NULL;
	data->pins_sleep = NULL;
	data->pins_idle = NULL;
	data->led_3p3 = NULL;
	data->vdd_1p8 = NULL;
	data->i2c_1p8 = NULL;
	data->hrm_enabled_mode = 0;
	data->sampling_period_ns = 0;
	data->mode_sdk_enabled = 0;
	data->regulator_state = 0;
	data->irq_state = 0;
	data->hrm_threshold = DEFAULT_THRESHOLD;
	data->eol_test_is_enable = 0;
	data->eol_test_status = 0;
	data->pre_eol_test_is_enable = 0;
	data->reg_read_buf = 0;
	data->lib_ver = NULL;
	data->pm_state = PM_RESUME;
	data->hrm_prev_mode = 0;
	data->mode_sdk_prev = 0;
}

static void max86915_irq_set_state(struct max869_device_data *data, int irq_enable)
{
	HRM_info("%s - irq_enable : %d, irq_state : %d\n",
		__func__, irq_enable, data->irq_state);

	if (irq_enable) {
		if (data->irq_state++ == 0)
			enable_irq(data->hrm_irq);
	} else {
		if (data->irq_state == 0)
			return;
		if (--data->irq_state <= 0) {
			disable_irq(data->hrm_irq);
			data->irq_state = 0;
		}
	}
}

static int max86915_power_ctrl(struct max869_device_data *data, int onoff)
{
	int rc = 0;
	static int i2c_1p8_enable;

	struct regulator *regulator_vdd_1p8;
	struct regulator *regulator_i2c_1p8 = 0;

	HRM_dbg("%s - onoff : %d, state : %d\n",
		__func__, onoff, data->regulator_state);

	if (onoff == PWR_ON) {
		if (data->regulator_state != 0) {
			HRM_dbg("%s - duplicate regulator : %d\n",
				__func__, onoff);
			data->regulator_state++;
			return 0;
		}
		data->regulator_state++;
		data->pm_state = PM_RESUME;
	} else {
		if (data->regulator_state == 0) {
			HRM_dbg("%s - already off the regulator : %d\n",
				__func__, onoff);
			return 0;
		} else if (data->regulator_state != 1) {
			HRM_dbg("%s - duplicate regulator : %d\n",
				__func__, onoff);
			data->regulator_state--;
			return 0;
		}
		data->regulator_state--;
	}

	if (data->i2c_1p8 != NULL) {
		regulator_i2c_1p8 = regulator_get(NULL, data->i2c_1p8);
		if (IS_ERR(regulator_i2c_1p8) || regulator_i2c_1p8 == NULL) {
			HRM_dbg("%s - i2c_1p8 regulator_get fail\n", __func__);
			rc = PTR_ERR(regulator_i2c_1p8);
			regulator_i2c_1p8 = NULL;
			goto get_i2c_1p8_failed;
		}
	}

#ifdef CONFIG_ARCH_QCOM
#ifdef CONFIG_SPI_TO_I2C_FPGA
	regulator_vdd_1p8 =
		regulator_get(&data->pdev->dev, "hrmsensor_1p8");
#else
	regulator_vdd_1p8 =
		regulator_get(&data->client->dev, "hrmsensor_1p8");
#endif
#else /* EXYNOS */
	regulator_vdd_1p8 = regulator_get(NULL, data->vdd_1p8);
#endif
	if (IS_ERR(regulator_vdd_1p8) || regulator_vdd_1p8 == NULL) {
		HRM_dbg("%s - vdd_1p8 regulator_get fail\n", __func__);
		rc = PTR_ERR(regulator_vdd_1p8);
		regulator_vdd_1p8 = NULL;
		goto get_vdd_1p8_failed;
	}

	HRM_dbg("%s - onoff = %d\n", __func__, onoff);

	if (onoff == PWR_ON) {
		if (data->i2c_1p8 != NULL && i2c_1p8_enable == 0) {
			rc = regulator_enable(regulator_i2c_1p8);
			i2c_1p8_enable = 1;
			if (rc) {
				HRM_dbg("enable i2c_1p8 failed, rc=%d\n", rc);
				goto enable_i2c_1p8_failed;
			}
		}
#if !defined(CONFIG_ARCH_QCOM) /* EXYNOS */
		rc = regulator_set_voltage(regulator_vdd_1p8,
			1800000, 1800000);
		if (rc < 0) {
			HRM_dbg("%s - set vdd_1p8 failed, rc=%d\n",
				__func__, rc);
			goto enable_vdd_1p8_failed;
		}
#endif
		rc = regulator_enable(regulator_vdd_1p8);
		if (rc) {
			HRM_dbg("%s - enable vdd_1p8 failed, rc=%d\n",
				__func__, rc);
			goto enable_vdd_1p8_failed;
		}

		rc = gpio_direction_output(data->hrm_en, 1);
		if (rc) {
			HRM_dbg("%s - gpio direction output failed, rc=%d\n",
				__func__, rc);
			goto gpio_direction_output_failed;
		}

		usleep_range(1000, 1100);
	} else {
		rc = regulator_disable(regulator_vdd_1p8);
		if (rc) {
			HRM_dbg("%s - disable vdd_1p8 failed, rc=%d\n",
				__func__, rc);
			goto done;
		}

		gpio_set_value(data->hrm_en, 0);
		rc = gpio_direction_input(data->hrm_en);
		if (rc) {
			HRM_dbg("%s - gpio direction input failed, rc=%d\n",
				__func__, rc);
		}

#ifdef I2C_1P8_DISABLE
		if (data->i2c_1p8 != NULL) {
			rc = regulator_disable(regulator_i2c_1p8);
			i2c_1p8_enable = 0;
			if (rc) {
				HRM_dbg("disable i2c_1p8 failed, rc=%d\n", rc);
				goto done;
			}
		}
#endif
	}

	goto done;

gpio_direction_output_failed:
	regulator_disable(regulator_vdd_1p8);
enable_vdd_1p8_failed:
#ifdef I2C_1P8_DISABLE
	if (data->i2c_1p8 != NULL) {
		regulator_disable(regulator_i2c_1p8);
		i2c_1p8_enable = 0;
	}
#endif
enable_i2c_1p8_failed:
done:
	regulator_put(regulator_vdd_1p8);
get_vdd_1p8_failed:
	if (data->i2c_1p8 != NULL)
		regulator_put(regulator_i2c_1p8);
get_i2c_1p8_failed:
	return rc;

}

static int max86915_enable(struct max869_device_data *data, enum hrm_mode mode)
{
	int err;
	s32 threshold;

	HRM_dbg("%s - debug = %d, info = %d\n", __func__, hrm_debug, hrm_info);

	err = max869_get_threshold(&threshold);
	if (err < 0) {
		HRM_dbg("%s get_threshold fail err = %d\n",
			__func__, err);
		return err;
	}
	if (data->hrm_threshold != DEFAULT_THRESHOLD) {
		if (threshold != data->hrm_threshold) {
			err = max869_set_threshold(data->hrm_threshold);
			if (err < 0) {
				HRM_dbg("%s set_threshold fail err = %d\n",
					__func__, err);
				return err;
			}
		}
	} else
		data->hrm_threshold = threshold;

	err = max869_enable(mode);
	if (err < 0) {
		HRM_dbg("%s fail err = %d\n", __func__, err);
		return err;
	}

	max86915_irq_set_state(data, PWR_ON);

	return 0;
}

static int max86915_disable(struct max869_device_data *data, enum hrm_mode mode)
{
	int err;

	HRM_dbg("%s\n", __func__);

	max86915_irq_set_state(data, PWR_OFF);

	err = max869_disable(mode);
	if (err < 0) {
		HRM_dbg("%s fail err = %d\n", __func__, err);
		return err;
	}

	return 0;
}

static int max86915_read_data(struct max869_device_data *data,
	struct hrm_output_data *read_data)
{
	int err = 0;

	err = max869_read_data(read_data);
	if (err < 0) {
		if (err != -ENODATA)
			HRM_dbg("%s read_data fail err = %d\n", __func__, err);

		return err;
	}

	return err;
}

static int max86915_eol_test_onoff(struct max869_device_data *data, int onoff)
{
	int err;

	HRM_dbg("%s: %d\n", __func__, onoff);

	if (onoff == PWR_ON) {
		data->eol_test_is_enable = 1;

		err = max869_set_eol_enable(data->eol_test_is_enable);
		if (err < 0) {
			HRM_dbg("%s fail err = %d\n", __func__, err);
			return err;
		}
	} else {
		data->eol_test_is_enable = 0;

		err = max869_set_eol_enable(data->eol_test_is_enable);
		if (err < 0) {
			HRM_dbg("%s fail err = %d\n", __func__, err);
			return err;
		}
	}
	return err;
}

static int max86915_pre_eol_test_onoff(struct max869_device_data *data, int onoff)
{
	HRM_dbg("%s: %d\n", __func__, onoff);

	if (onoff == PWR_OFF)
		data->pre_eol_test_is_enable = 0;
	else
		data->pre_eol_test_is_enable = onoff;

	return 0;
}

void max86915_mode_enable(struct max869_device_data *data,
	int onoff, enum hrm_mode mode)
{
	int err;

	if (onoff == PWR_ON) {
		err = max86915_power_ctrl(data, PWR_ON);
		if (err < 0)
			HRM_dbg("%s hrm_regulator_on fail err = %d\n",
				__func__, err);
		data->hrm_enabled_mode = mode;
		err = max86915_enable(data, mode);
		if (err != 0)
			HRM_dbg("enable err : %d\n", err);
		if (err < 0 && mode == MODE_AMBIENT) {
			input_report_rel(data->hrm_input_dev,
				REL_Y, -5 + 1); /* F_ERR_I2C -5 detected i2c error */
			input_sync(data->hrm_input_dev);
			HRM_dbg("%s - awb mode enable error\n",
				__func__);
		}
	} else {
		if (data->regulator_state == 0) {
			HRM_dbg("%s - already power off - disable skip\n",
				__func__);
			return;
		}
		if (data->eol_test_is_enable)
			max86915_eol_test_onoff(data, PWR_OFF);

		data->hrm_enabled_mode = 0;
		data->mode_sdk_enabled = 0;
		err = max86915_disable(data, mode);
		if (err != 0)
			HRM_dbg("disable err : %d\n", err);
		err = max86915_power_ctrl(data, PWR_OFF);
		if (err < 0)
			HRM_dbg("%s hrm_regulator_off fail err = %d\n",
				__func__, err);
	}
	HRM_dbg("%s - onoff = %d m : %d c : %d\n",
		__func__, onoff, mode, data->hrm_enabled_mode);

}

/* hrm input enable/disable sysfs */
static ssize_t max86915_enable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	return snprintf(buf, strlen(buf), "%d\n", data->hrm_enabled_mode);
}

static ssize_t max86915_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int on_off;
	enum hrm_mode mode;

	mutex_lock(&data->activelock);
	if (sysfs_streq(buf, "0")) {
		on_off = PWR_OFF;
		mode = MODE_NONE;
	} else if (sysfs_streq(buf, "1")) {
		on_off = PWR_ON;
		mode = MODE_HRM;
		data->mode_cnt.hrm_cnt++;
	} else if (sysfs_streq(buf, "2")) {
		on_off = PWR_ON;
		mode = MODE_AMBIENT;
		data->mode_cnt.amb_cnt++;
	} else if (sysfs_streq(buf, "3")) {
		on_off = PWR_ON;
		mode = MODE_PROX;
		data->mode_cnt.prox_cnt++;
	} else if (sysfs_streq(buf, "4")) {
		on_off = PWR_ON;
		mode = MODE_HRMLED_IR;
	} else if (sysfs_streq(buf, "5")) {
		on_off = PWR_ON;
		mode = MODE_HRMLED_RED;
	} else if (sysfs_streq(buf, "6")) {
		on_off = PWR_ON;
		mode = MODE_HRMLED_BOTH;
	} else if (sysfs_streq(buf, "10")) {
		on_off = PWR_ON;
		mode = MODE_SDK_IR;
		data->mode_cnt.sdk_cnt++;
	} else if (sysfs_streq(buf, "11")) {
		on_off = PWR_ON;
		mode = MODE_SDK_RED;
	} else if (sysfs_streq(buf, "12")) {
		on_off = PWR_ON;
		mode = MODE_SDK_GREEN;
	} else if (sysfs_streq(buf, "13")) {
		on_off = PWR_ON;
		mode = MODE_SDK_BLUE;
	} else if (sysfs_streq(buf, "-10")) {
		on_off = PWR_OFF;
		mode = MODE_SDK_IR;
	} else if (sysfs_streq(buf, "-11")) {
		on_off = PWR_OFF;
		mode = MODE_SDK_RED;
	} else if (sysfs_streq(buf, "-12")) {
		on_off = PWR_OFF;
		mode = MODE_SDK_GREEN;
	} else if (sysfs_streq(buf, "-13")) {
		on_off = PWR_OFF;
		mode = MODE_SDK_BLUE;
	} else {
		HRM_dbg("%s - invalid value %d\n", __func__, *buf);
		data->mode_cnt.unkn_cnt++;
		mutex_unlock(&data->activelock);
		return -EINVAL;
	}

	if (mode == MODE_SDK_IR || mode == MODE_SDK_RED
		|| mode == MODE_SDK_GREEN || mode == MODE_SDK_BLUE) {
		HRM_dbg("%s - Check SDK enable %d, %d, %d\n", __func__, on_off, mode, data->mode_sdk_enabled);
		if (on_off == PWR_ON) {
			if (data->mode_sdk_enabled & (1<<(mode - MODE_SDK_IR))) { /* already enabled */
				/* Do Nothing */
				HRM_dbg("%s - SDK mode already enabled %d\n", __func__, mode);
				mutex_unlock(&data->activelock);
				return count;
			}

			/* Oring enable mode */
			max869_set_sdk_enabled((mode - MODE_SDK_IR), PWR_ON);

			if (data->mode_sdk_enabled == 0) {
				data->mode_sdk_enabled |= (1<<(mode - MODE_SDK_IR));
				mode = MODE_SDK_IR;
				data->mode_cnt.sdk_cnt++;
			} else {
				data->mode_sdk_enabled |= (1<<(mode - MODE_SDK_IR));
				mutex_unlock(&data->activelock);
				return count;
			}
		} else {
			if ((data->mode_sdk_enabled & (1<<(mode - MODE_SDK_IR))) == 0) { /* Not Enabled */
				/* Do Nothing */
				HRM_dbg("%s - SDK mode not enabled %d\n", __func__, mode);
				mutex_unlock(&data->activelock);
				return count;
			}

			/* Oring disable mode */
			data->mode_sdk_enabled &= ~(1<<(mode - MODE_SDK_IR));

			max869_set_sdk_enabled((mode - MODE_SDK_IR), PWR_OFF);

			if (data->mode_sdk_enabled == 0) {
				mode = MODE_NONE;
			} else {
				mutex_unlock(&data->activelock);
				return count;
			}
		}
		HRM_dbg("%s - Current SDK Mode %02x\n", __func__, data->mode_sdk_enabled);
	}

	HRM_dbg("%s en : %d m : %d c : %d\n",
		__func__, on_off, mode, data->hrm_enabled_mode);
	max86915_mode_enable(data, on_off, mode);
	mutex_unlock(&data->activelock);

	return count;
}

static ssize_t max86915_poll_delay_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->sampling_period_ns);
}

static ssize_t max86915_poll_delay_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	u32 sampling_period_ns = 0;
	int err = 0;

	mutex_lock(&data->activelock);

	err = kstrtoint(buf, 10, &sampling_period_ns);

	if (err < 0) {
		HRM_dbg("%s - kstrtoint failed.(%d)\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}

	err = max869_set_sampling_rate(sampling_period_ns);

	if (err > 0)
		data->sampling_period_ns = err;

	HRM_dbg("%s - hrm sensor sampling rate is setted as %dns\n", __func__, sampling_period_ns);

	mutex_unlock(&data->activelock);

	return size;
}

static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP,
	max86915_enable_show, max86915_enable_store);
static DEVICE_ATTR(poll_delay, S_IRUGO|S_IWUSR|S_IWGRP,
	max86915_poll_delay_show, max86915_poll_delay_store);

static struct attribute *hrm_sysfs_attrs[] = {
	&dev_attr_enable.attr,
	&dev_attr_poll_delay.attr,
	NULL
};

static struct attribute_group hrm_attribute_group = {
	.attrs = hrm_sysfs_attrs,
};

/* hrm_sensor sysfs */
static ssize_t max86915_name_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	char chip_name[NAME_LEN];

	max869_get_name_chipset(chip_name);
	return snprintf(buf, PAGE_SIZE, "%s\n", chip_name);
}

static ssize_t max86915_vendor_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	char vendor[NAME_LEN];

	max869_get_name_vendor(vendor);
	return snprintf(buf, PAGE_SIZE, "%s\n", vendor);
}

static ssize_t max86915_led_current_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	int err;
	int led1, led2, led3, led4;
	struct max869_device_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->activelock);
	err = sscanf(buf, "%8x", &data->led_current);
	if (err < 0) {
		HRM_dbg("%s - failed, err = %x\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	led1 = 0x000000ff & data->led_current;
	led2 = (0x0000ff00 & data->led_current) >> 8;
	led3 = (0x00ff0000 & data->led_current) >> 16;
	led4 = (0xff000000 & data->led_current) >> 24;
	HRM_info("%s 0x%02x, 0x%02x, 0x%02x, 0x%02x\n", __func__, led1, led2, led3, led4);

	err = max869_set_current(led1, led2, led3, led4);
	if (err < 0) {
		HRM_dbg("%s - failed, err = %x\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	mutex_unlock(&data->activelock);

	return size;
}

static ssize_t max86915_led_current_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int err;
	u8 led1, led2, led3, led4;
	struct max869_device_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->activelock);

	err = max869_get_current(&led1, &led2, &led3, &led4);
	if (err < 0) {
		HRM_dbg("%s - failed, err = %x\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	data->led_current = (led1 & 0xff) | ((led2 & 0xff) << 8)
		| ((led3 & 0xff) << 16) | ((led4 & 0xff) << 24);

	mutex_unlock(&data->activelock);
	HRM_info("%s led1 0x%02x, led2 0x%02x, led3 0x%02x, led4 0x%02x\n",
		__func__, led1, led2, led3, led4);

	return snprintf(buf, PAGE_SIZE, "%08X\n", data->led_current);
}

static ssize_t max86915_led_test_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int err;
	u8 result;
	struct max869_device_data *data = dev_get_drvdata(dev);

	err = max86915_power_ctrl(data, PWR_ON);
	if (err < 0)
		HRM_dbg("%s hrm_regulator_on fail err = %d\n",
		__func__, err);

	mutex_lock(&data->activelock);

	err = max869_get_led_test(&result);
	if (err < 0) {
		HRM_dbg("%s - failed, err = %x\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}

	mutex_unlock(&data->activelock);

	err = max86915_power_ctrl(data, PWR_OFF);
	if (err < 0)
		HRM_dbg("%s hrm_regulator_off fail err = %d\n",
		__func__, err);

	HRM_info("%s result = 0x%02x\n", __func__, result);

	return snprintf(buf, PAGE_SIZE, "0x%02x\n", result);
}

static ssize_t max86915_flush_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int ret = 0;
	u8 handle = 0;

	mutex_lock(&data->activelock);
	ret = kstrtou8(buf, 10, &handle);
	if (ret < 0) {
		HRM_dbg("%s - kstrtou8 failed.(%d)\n", __func__, ret);
		mutex_unlock(&data->activelock);
		return ret;
	}
	HRM_dbg("%s - handle = %d\n", __func__, handle);
	mutex_unlock(&data->activelock);

	input_report_rel(data->hrm_input_dev, REL_MISC, handle);

	return size;
}

static ssize_t max86915_int_pin_check(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	/* need to check if this should be implemented */
	HRM_dbg("%s\n", __func__);
	return snprintf(buf, PAGE_SIZE, "%d\n", 0);
}

static ssize_t max86915_lib_ver_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	size_t buf_len;

	mutex_lock(&data->activelock);
	buf_len = strlen(buf) + 1;
	if (buf_len > NAME_LEN)
		buf_len = NAME_LEN;

	if (data->lib_ver != NULL)
		kfree(data->lib_ver);

	data->lib_ver = kzalloc(sizeof(char) * buf_len, GFP_KERNEL);
	if (data->lib_ver == NULL) {
		HRM_dbg("%s - couldn't allocate memory\n", __func__);
		mutex_unlock(&data->activelock);
		return -ENOMEM;
	}
	strlcpy(data->lib_ver, buf, buf_len);

	HRM_info("%s - lib_ver = %s\n", __func__, data->lib_ver);
	mutex_unlock(&data->activelock);

	return size;
}

static ssize_t max86915_lib_ver_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	if (data->lib_ver == NULL) {
		HRM_dbg("%s - data->lib_ver is NULL\n", __func__);
		return snprintf(buf, PAGE_SIZE, "%s\n", "NULL");
	}
	HRM_info("%s - lib_ver = %s\n", __func__, data->lib_ver);
	return snprintf(buf, PAGE_SIZE, "%s\n", data->lib_ver);
}

static ssize_t max86915_threshold_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int err = 0;

	mutex_lock(&data->activelock);
	err = kstrtoint(buf, 10, &data->hrm_threshold);
	if (err < 0) {
		HRM_dbg("%s - kstrtoint failed.(%d)\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	HRM_info("%s - threshold = %d\n",
		__func__, data->hrm_threshold);

	mutex_unlock(&data->activelock);
	return size;
}

static ssize_t max86915_threshold_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	if (data->hrm_threshold) {
		HRM_info("%s - threshold = %d\n",
			__func__, data->hrm_threshold);
		return snprintf(buf, PAGE_SIZE, "%d\n", data->hrm_threshold);
	}

	HRM_info("%s - threshold = 0\n", __func__);
	return snprintf(buf, PAGE_SIZE, "%d\n", 0);
}

static ssize_t max86915_eol_test_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	int test_onoff;
	struct max869_device_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->activelock);
	if (sysfs_streq(buf, "1")) { /* eol_test start */
		test_onoff = 1;
	} else if (sysfs_streq(buf, "0")) { /* eol_test stop */
		test_onoff = 0;
		max86915_pre_eol_test_onoff(data, test_onoff);
	} else {
		HRM_dbg("%s: invalid value %d\n", __func__, *buf);
		mutex_unlock(&data->activelock);
		return -EINVAL;
	}
	HRM_dbg("%s: %d\n", __func__, test_onoff);
	if (data->eol_test_is_enable == test_onoff) {
		HRM_dbg("%s: invalid eol status Pre: %d, AF : %d\n", __func__,
			data->eol_test_is_enable, test_onoff);
		mutex_unlock(&data->activelock);
		return -EINVAL;
	}
	if (max86915_eol_test_onoff(data, test_onoff) < 0)
		data->eol_test_is_enable = 0;

	mutex_unlock(&data->activelock);
	return size;
}

static ssize_t max86915_eol_test_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", data->eol_test_is_enable);
}

static ssize_t max86915_eol_test_result_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->activelock);

	if (data->eol_test_status == 0) {
		HRM_dbg("%s - data->eol_test_status is NULL\n",
		__func__);
		data->eol_test_status = 0;
		mutex_unlock(&data->activelock);
		return snprintf(buf, PAGE_SIZE, "%s\n", "NO_EOL_TEST");
	}
	HRM_dbg("%s - result = %d\n", __func__, data->eol_test_status);
	data->eol_test_status = 0;

	mutex_unlock(&data->activelock);

	return snprintf(buf, PAGE_SIZE, "%s\n", data->eol_test_result);
}

static ssize_t max86915_eol_test_status_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", data->eol_test_status);
}

static ssize_t max86915_pre_eol_test_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	int test_onoff = 0;
	struct max869_device_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->activelock);
	if (sysfs_streq(buf, "1")) { /* PRE EOL SYSTEM NOISE */
		test_onoff = 1;
	} else if (sysfs_streq(buf, "2")) { /* PRE EOL FREQ */
		test_onoff = 2;
	} else if (sysfs_streq(buf, "3")) { /* PRE EOL BOTH */
		test_onoff = 3;
	} else if (sysfs_streq(buf, "0")) { /* pre_eol_test stop */
		test_onoff = 0;
	} else {
		HRM_dbg("%s: invalid value %d\n", __func__, *buf);
		mutex_unlock(&data->activelock);
		return -EINVAL;
	}
	HRM_dbg("%s: %d\n", __func__, test_onoff);

	max86915_pre_eol_test_onoff(data, test_onoff);

	mutex_unlock(&data->activelock);
	return size;
}

static ssize_t max86915_pre_eol_test_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", data->pre_eol_test_is_enable);
}


static ssize_t max86915_read_reg_get(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	HRM_info("%s - val=0x%06x\n", __func__, data->reg_read_buf);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->reg_read_buf);
}

static ssize_t max86915_read_reg_set(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	int err = -1;
	unsigned int cmd = 0;
	u32 val = 0;
	u32 sz = 0;

	mutex_lock(&data->i2clock);
	if (data->regulator_state == 0) {
		HRM_dbg("%s - need to power on\n", __func__);
		mutex_unlock(&data->i2clock);
		return size;
	}
	err = sscanf(buf, "%8x", &cmd);
	if (err == 0) {
		HRM_dbg("%s - sscanf fail\n", __func__);
		mutex_unlock(&data->i2clock);
		return size;
	}

	err = max869_i2c_read((u32)cmd, &val, &sz);
	if (err != 0) {
		HRM_dbg("%s err=%d, val=0x%06x\n",
			__func__, err, val);
		mutex_unlock(&data->i2clock);
		return size;
	}
	data->reg_read_buf = val;
	mutex_unlock(&data->i2clock);

	return size;
}
static ssize_t max86915_write_reg_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	int err = -1;
	unsigned int cmd = 0;
	unsigned int val = 0;

	mutex_lock(&data->i2clock);
	if (data->regulator_state == 0) {
		HRM_dbg("%s - need to power on.\n", __func__);
		mutex_unlock(&data->i2clock);
		return size;
	}
	err = sscanf(buf, "%8x, %8x", &cmd, &val);
	if (err == 0) {
		HRM_dbg("%s - sscanf fail %s\n", __func__, buf);
		mutex_unlock(&data->i2clock);
		return size;
	}

	err = max869_i2c_write((u32)cmd, (u32)val);
	if (err < 0) {
		HRM_dbg("%s fail err = %d\n", __func__, err);
		mutex_unlock(&data->i2clock);
		return err;
	}
	mutex_unlock(&data->i2clock);

	return size;
}

static ssize_t max86915_debug_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	HRM_info("%s - debug mode = %u\n", __func__, data->debug_mode);

	return snprintf(buf, PAGE_SIZE, "%u\n", data->debug_mode);
}

static ssize_t max86915_debug_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int err;
	s32 mode;

	mutex_lock(&data->activelock);
	err = kstrtoint(buf, 10, &mode);
	if (err < 0) {
		HRM_dbg("%s - kstrtoint failed.(%d)\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	HRM_info("%s - mode = %d\n", __func__, mode);

	data->debug_mode = (u8)mode;
	max869_debug_set((u8)mode);
	mutex_unlock(&data->activelock);

	return size;
}

static ssize_t device_id_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int err;
	u64 device_id = 0;

	err = max86915_power_ctrl(data, PWR_ON);
	if (err < 0)
		HRM_dbg("%s hrm_regulator_on fail err = %d\n",
		__func__, err);

	mutex_lock(&data->activelock);

	max869_get_chipid(&device_id);

	mutex_unlock(&data->activelock);

	err = max86915_power_ctrl(data, PWR_OFF);
	if (err < 0)
		HRM_dbg("%s hrm_regulator_off fail err = %d\n",
		__func__, err);

	return snprintf(buf, PAGE_SIZE, "%lld\n", device_id);
}

static ssize_t part_type_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int err;
	u16 part_type = 0;

	err = max86915_power_ctrl(data, PWR_ON);
	if (err < 0)
		HRM_dbg("%s hrm_regulator_on fail err = %d\n",
		__func__, err);

	mutex_lock(&data->activelock);

	max869_get_part_type(&part_type);

	mutex_unlock(&data->activelock);

	err = max86915_power_ctrl(data, PWR_OFF);
	if (err < 0)
		HRM_dbg("%s hrm_regulator_off fail err = %d\n",
		__func__, err);

	return snprintf(buf, PAGE_SIZE, "%d\n", part_type);
}

static ssize_t i2c_err_cnt_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	u32 err_cnt = 0;

	max869_get_i2c_err_cnt(&err_cnt);

	return snprintf(buf, PAGE_SIZE, "%d\n", err_cnt);
}

static ssize_t i2c_err_cnt_store(struct device *dev,
struct device_attribute *attr, const char *buf, size_t size)
{
	max869_set_i2c_err_cnt();

	return size;
}

static ssize_t curr_adc_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	u16 ir_curr = 0;
	u16 red_curr = 0;
	u32 ir_adc = 0;
	u32 red_adc = 0;

	max869_get_curr_adc(&ir_curr, &red_curr, &ir_adc, &red_adc);

	return snprintf(buf, PAGE_SIZE,
		"\"HRIC\":\"%d\",\"HRRC\":\"%d\",\"HRIA\":\"%d\",\"HRRA\":\"%d\"\n",
		ir_curr, red_curr, ir_adc, red_adc);
}

static ssize_t curr_adc_store(struct device *dev,
struct device_attribute *attr, const char *buf, size_t size)
{
	max869_set_curr_adc();

	return size;
}

static ssize_t mode_cnt_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE,
		"\"CNT_HRM\":\"%d\",\"CNT_AMB\":\"%d\",\"CNT_PROX\":\"%d\",\"CNT_SDK\":\"%d\",\"CNT_CGM\":\"%d\",\"CNT_UNKN\":\"%d\"\n",
		data->mode_cnt.hrm_cnt, data->mode_cnt.amb_cnt, data->mode_cnt.prox_cnt,
		data->mode_cnt.sdk_cnt, data->mode_cnt.cgm_cnt, data->mode_cnt.unkn_cnt);
}

static ssize_t mode_cnt_store(struct device *dev,
struct device_attribute *attr, const char *buf, size_t size)
{
	struct max869_device_data *data = dev_get_drvdata(dev);

	data->mode_cnt.hrm_cnt = 0;
	data->mode_cnt.amb_cnt = 0;
	data->mode_cnt.prox_cnt = 0;
	data->mode_cnt.sdk_cnt = 0;
	data->mode_cnt.cgm_cnt = 0;
	data->mode_cnt.unkn_cnt = 0;

	return size;
}

static ssize_t max86915_factory_cmd_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	static char cmd_result[MAX_BUF_LEN];

	mutex_lock(&data->activelock);

	max869_get_fac_cmd(cmd_result);

	HRM_dbg("%s cmd_result = %s\n", __func__, cmd_result);

	mutex_unlock(&data->activelock);

	return snprintf(buf, PAGE_SIZE, "%s\n", cmd_result);
}

static ssize_t max86915_version_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	HRM_info("%s cmd_result = %s.%s%s\n", __func__, VERSION, SUB_VERSION, VENDOR_VERSION);

	return snprintf(buf, PAGE_SIZE, "%s.%s%s\n", VERSION, SUB_VERSION, VENDOR_VERSION);
}

static ssize_t max86915_sensor_info_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	HRM_dbg("%s sensor_info_data not support\n", __func__);

	return snprintf(buf, PAGE_SIZE, "NOT SUPPORT\n");
}

static ssize_t max86915_xtalk_code_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	int err;
	int xtalk1, xtalk2, xtalk3, xtalk4;
	struct max869_device_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->activelock);
	err = sscanf(buf, "%8x", &data->xtalk_code);
	if (err < 0) {
		HRM_dbg("%s - failed, err = %x\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	xtalk1 = 0x0000001f & data->xtalk_code;
	xtalk2 = (0x00001f00 & data->xtalk_code) >> 8;
	xtalk3 = (0x001f0000 & data->xtalk_code) >> 16;
	xtalk4 = (0x1f000000 & data->xtalk_code) >> 24;
	HRM_info("%s 0x%02x, 0x%02x, 0x%02x, 0x%02x\n", __func__, xtalk1, xtalk2, xtalk3, xtalk4);

	err = max869_set_xtalk(xtalk1, xtalk2, xtalk3, xtalk4);
	if (err < 0) {
		HRM_dbg("%s - failed, err = %x\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	mutex_unlock(&data->activelock);

	return size;
}

static ssize_t max86915_xtalk_code_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int err;
	u8 xtalk1, xtalk2, xtalk3, xtalk4;
	struct max869_device_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->activelock);

	err = max869_get_xtalk(&xtalk1, &xtalk2, &xtalk3, &xtalk4);
	if (err < 0) {
		HRM_dbg("%s - failed, err = %x\n", __func__, err);
		mutex_unlock(&data->activelock);
		return err;
	}
	data->xtalk_code = (xtalk1 & 0x1f) | ((xtalk2 & 0x1f) << 8)
		| ((xtalk3 & 0x1f) << 16) | ((xtalk4 & 0x1f) << 24);

	mutex_unlock(&data->activelock);
	HRM_info("%s xtalk1 0x%02x, xtalk2 0x%02x, xtalk3 0x%02x, xtalk4 0x%02x\n",
		__func__, xtalk1, xtalk2, xtalk3, xtalk4);

	return snprintf(buf, PAGE_SIZE, "%08X\n", data->xtalk_code);
}

static DEVICE_ATTR(name, S_IRUGO, max86915_name_show, NULL);
static DEVICE_ATTR(vendor, S_IRUGO, max86915_vendor_show, NULL);
static DEVICE_ATTR(led_current, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_led_current_show, max86915_led_current_store);
static DEVICE_ATTR(led_test, S_IRUGO, max86915_led_test_show, NULL);
static DEVICE_ATTR(hrm_flush, S_IWUSR | S_IWGRP, NULL, max86915_flush_store);
static DEVICE_ATTR(int_pin_check, S_IRUGO, max86915_int_pin_check, NULL);
static DEVICE_ATTR(lib_ver, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_lib_ver_show, max86915_lib_ver_store);
static DEVICE_ATTR(threshold, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_threshold_show, max86915_threshold_store);
static DEVICE_ATTR(eol_test, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_eol_test_show, max86915_eol_test_store);
static DEVICE_ATTR(eol_test_result, S_IRUGO, max86915_eol_test_result_show, NULL);
static DEVICE_ATTR(eol_test_status, S_IRUGO, max86915_eol_test_status_show, NULL);
static DEVICE_ATTR(pre_eol_test, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_pre_eol_test_show, max86915_pre_eol_test_store);
static DEVICE_ATTR(read_reg, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_read_reg_get, max86915_read_reg_set);
static DEVICE_ATTR(write_reg, S_IWUSR | S_IWGRP, NULL, max86915_write_reg_store);
static DEVICE_ATTR(hrm_debug, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_debug_show, max86915_debug_store);
static DEVICE_ATTR(device_id, S_IRUGO, device_id_show, NULL);
static DEVICE_ATTR(part_type, S_IRUGO, part_type_show, NULL);
static DEVICE_ATTR(i2c_err_cnt, S_IRUGO | S_IWUSR | S_IWGRP, i2c_err_cnt_show, i2c_err_cnt_store);
static DEVICE_ATTR(curr_adc, S_IRUGO | S_IWUSR | S_IWGRP, curr_adc_show, curr_adc_store);
static DEVICE_ATTR(mode_cnt, S_IRUGO | S_IWUSR | S_IWGRP, mode_cnt_show, mode_cnt_store);
static DEVICE_ATTR(hrm_factory_cmd, S_IRUGO, max86915_factory_cmd_show, NULL);
static DEVICE_ATTR(hrm_version, S_IRUGO, max86915_version_show, NULL);
static DEVICE_ATTR(sensor_info, S_IRUGO, max86915_sensor_info_show, NULL);
static DEVICE_ATTR(xtalk_code, S_IRUGO | S_IWUSR | S_IWGRP,
	max86915_xtalk_code_show, max86915_xtalk_code_store);

static struct device_attribute *max86915_sensor_attrs[] = {
	&dev_attr_name,
	&dev_attr_vendor,
	&dev_attr_led_current,
	&dev_attr_led_test,
	&dev_attr_hrm_flush,
	&dev_attr_int_pin_check,
	&dev_attr_lib_ver,
	&dev_attr_threshold,
	&dev_attr_eol_test,
	&dev_attr_eol_test_result,
	&dev_attr_eol_test_status,
	&dev_attr_pre_eol_test,
	&dev_attr_read_reg,
	&dev_attr_write_reg,
	&dev_attr_device_id,
	&dev_attr_part_type,
	&dev_attr_i2c_err_cnt,
	&dev_attr_curr_adc,
	&dev_attr_mode_cnt,
	&dev_attr_hrm_debug,
	&dev_attr_hrm_factory_cmd,
	&dev_attr_hrm_version,
	&dev_attr_sensor_info,
	&dev_attr_xtalk_code,
	NULL,
};

void max86915_pin_control(struct max869_device_data *data, bool pin_set)
{
	int status = 0;

	if (!data->hrm_pinctrl) {
		HRM_dbg("%s hrm_pinctrl is null\n", __func__);
		return;
	}
	if (pin_set) {
		if (!IS_ERR_OR_NULL(data->pins_idle)) {
			status = pinctrl_select_state(data->hrm_pinctrl,
				data->pins_idle);
			if (status)
				HRM_dbg("%s: can't set pin default state\n",
					__func__);
			HRM_info("%s idle\n", __func__);
		}
		if (data->hrm_prev_mode != 0) {
			status = gpio_direction_output(data->hrm_en, 1);
			if (status) {
				HRM_dbg("%s - gpio direction output failed, rc=%d\n",
					__func__, status);
			}
		}
	} else {
		if (!IS_ERR_OR_NULL(data->pins_sleep)) {
			status = pinctrl_select_state(data->hrm_pinctrl,
				data->pins_sleep);
			if (status)
				HRM_dbg("%s: can't set pin sleep state\n",
					__func__);
			HRM_info("%s sleep\n", __func__);
		}
	}
}

irqreturn_t max86915_irq_handler(int hrm_irq, void *device)
{
	int err;
	struct max869_device_data *data = device;
	struct hrm_output_data read_data;
	int i;
	static unsigned int sample_cnt;

	memset(&read_data, 0, sizeof(struct hrm_output_data));

	if (data->regulator_state == 0)
		return IRQ_HANDLED;

#ifdef CONFIG_ARCH_QCOM
	pm_qos_add_request(&data->pm_qos_req_fpm, PM_QOS_CPU_DMA_LATENCY,
		PM_QOS_DEFAULT_VALUE);
#endif

	err = max86915_read_data(data, &read_data);

	if (err == 0) {
		if (data->hrm_input_dev == NULL) {
			HRM_dbg("%s - hrm_input_dev is NULL\n", __func__);
		} else if (data->hrm_enabled_mode == 0) {
			HRM_dbg("%s - hrm_enabled_mode %d\n", __func__, data->hrm_enabled_mode);
		} else {
			if (read_data.fifo_num) {
				for (i = 0; i < read_data.fifo_num; i++) {
					input_report_rel(data->hrm_input_dev,
						REL_X, read_data.fifo_main[0][i] + 1);
					input_report_rel(data->hrm_input_dev,
						REL_Y,  read_data.fifo_main[1][i] + 1);
					input_report_rel(data->hrm_input_dev,
						REL_Z, read_data.fifo_main[2][i] + 1);
					input_report_rel(data->hrm_input_dev,
						REL_RX,  read_data.fifo_main[3][i] + 1);
					input_sync(data->hrm_input_dev);
				}
			} else {
				for (i = 0; i < read_data.main_num; i++)
					input_report_rel(data->hrm_input_dev,
						REL_X + i, read_data.data_main[i] + 1);

				for (i = 0; i < read_data.sub_num; i++)
					input_report_abs(data->hrm_input_dev,
						ABS_X + i, read_data.data_sub[i] + 1);

				if (read_data.main_num || read_data.sub_num)
					input_sync(data->hrm_input_dev);
			}
			if (sample_cnt++ > 100) {
				HRM_dbg("%s mode:0x%x main:%d,%d,%d,%d sub:%d,%d,%d,%d,%d,%d,%d,%d\n", __func__,
						read_data.mode, read_data.data_main[0], read_data.data_main[1],
						read_data.data_main[2], read_data.data_main[3], read_data.data_sub[0],
						read_data.data_sub[1], read_data.data_sub[2], read_data.data_sub[3], read_data.data_sub[4],
						read_data.data_sub[5], read_data.data_sub[6], read_data.data_sub[7]);
				sample_cnt = 0;
			} else {
				HRM_info("%s mode:0x%x main:%d,%d,%d,%d sub:%d,%d,%d,%d,%d,%d,%d,%d\n", __func__,
					read_data.mode, read_data.data_main[0], read_data.data_main[1],
					read_data.data_main[2], read_data.data_main[3], read_data.data_sub[0],
					read_data.data_sub[1], read_data.data_sub[2], read_data.data_sub[3], read_data.data_sub[4],
					read_data.data_sub[5], read_data.data_sub[6], read_data.data_sub[7]);
			}
		}
	}
#ifdef CONFIG_ARCH_QCOM
	pm_qos_remove_request(&data->pm_qos_req_fpm);
#endif

	return IRQ_HANDLED;
}


static int max86915_parse_dt(struct max869_device_data *data)
{
#ifdef CONFIG_SPI_TO_I2C_FPGA
	struct device *dev = &data->pdev->dev;
#else
	struct device *dev = &data->client->dev;
#endif
	struct device_node *dNode = dev->of_node;
	enum of_gpio_flags flags;

	if (dNode == NULL)
		return -ENODEV;

	data->hrm_int = of_get_named_gpio_flags(dNode,
		"hrmsensor,hrm_int-gpio", 0, &flags);
	if (data->hrm_int < 0) {
		HRM_dbg("%s - get hrm_int error\n", __func__);
		return -ENODEV;
	}

	data->hrm_en = of_get_named_gpio_flags(dNode,
	"hrmsensor,hrm_boost_en-gpio", 0, &flags);
	if (data->hrm_en < 0) {
		HRM_dbg("%s - get hrm_en error\n", __func__);
		return -ENODEV;
	}

#ifdef CONFIG_ARCH_QCOM
	data->hrm_pinctrl = devm_pinctrl_get(dev);
#else
	if (of_property_read_string(dNode, "hrmsensor,vdd_1p8",
		(char const **)&data->vdd_1p8) < 0)
		HRM_dbg("%s - get vdd_1p8 error\n", __func__);

	if (of_property_read_string(dNode, "hrmsensor,i2c_1p8",
		(char const **)&data->i2c_1p8) < 0)
		HRM_dbg("%s - don't use i2c_1p8\n", __func__);

	data->hrm_pinctrl = pinctrl_get_select_default(dev);
#endif

	if (IS_ERR_OR_NULL(data->hrm_pinctrl)) {
		HRM_dbg("%s: failed pinctrl_get (%li)\n",
			__func__, PTR_ERR(data->hrm_pinctrl));
		data->hrm_pinctrl = NULL;
		return -EINVAL;
	}
	HRM_dbg("%s: success pinctrl_get (%p)\n", __func__, data->hrm_pinctrl);

	data->pins_sleep =
		pinctrl_lookup_state(data->hrm_pinctrl, "sleep");
	if (IS_ERR_OR_NULL(data->pins_sleep)) {
		HRM_dbg("%s : could not get pins sleep_state (%li)\n",
			__func__, PTR_ERR(data->pins_sleep));
#ifdef CONFIG_ARCH_QCOM
		devm_pinctrl_put(data->hrm_pinctrl);
#else
		pinctrl_put(data->hrm_pinctrl);
#endif
		data->pins_sleep = NULL;
		return -EINVAL;
	}

	data->pins_idle =
		pinctrl_lookup_state(data->hrm_pinctrl, "idle");
	if (IS_ERR_OR_NULL(data->pins_idle)) {
		HRM_dbg("%s : could not get pins idle_state (%li)\n",
			__func__, PTR_ERR(data->pins_idle));

#ifdef CONFIG_ARCH_QCOM
		devm_pinctrl_put(data->hrm_pinctrl);
#else
		pinctrl_put(data->hrm_pinctrl);
#endif
		data->pins_idle = NULL;
		return -EINVAL;
	}

	return 0;
}
static int max86915_setup_irq(struct max869_device_data *data)
{
	int errorno = -EIO;

	errorno = request_threaded_irq(data->hrm_irq, NULL,
		max86915_irq_handler, IRQF_TRIGGER_FALLING|IRQF_ONESHOT,
		"hrm_sensor_irq", data);

	if (errorno < 0) {
		HRM_dbg("%s - failed for setup hrm_irq errono= %d\n",
			   __func__, errorno);
		errorno = -ENODEV;
		return errorno;
	}

	disable_irq(data->hrm_irq);

	return errorno;
}

static int max86915_setup_gpio(struct max869_device_data *data)
{
	int errorno = -EIO;

	errorno = gpio_request(data->hrm_int, "hrm_int");
	if (errorno) {
		HRM_dbg("%s - failed to request hrm_int\n", __func__);
		return errorno;
	}

	errorno = gpio_direction_input(data->hrm_int);
	if (errorno) {
		HRM_dbg("%s - failed to set hrm_int as input\n", __func__);
		goto err_gpio_direction_input;
	}
	data->hrm_irq = gpio_to_irq(data->hrm_int);

	errorno = gpio_request(data->hrm_en, "hrm_en");
	if (errorno) {
		HRM_dbg("%s - failed to request hrm_en\n", __func__);
		return errorno;
	}
	goto done;

err_gpio_direction_input:
	gpio_free(data->hrm_int);
done:
	return errorno;
}

int max86915_init_device(struct max869_device_data *data)
{
	int err = 0;
	char chip_name[NAME_LEN];

	if (data->client->addr == SLAVE_ADDR_MAX) {
		err = max869_init_device(data);
		if (err == 0) {
			err = max869_get_name_chipset(chip_name);
			HRM_info("%s - use %s\n", __func__, chip_name);
			return err;
		}
	} else
		HRM_dbg("%s - slave address error, 0x%02x\n", __func__, data->client->addr);

	return err;
}

#ifdef CONFIG_SPI_TO_I2C_FPGA
static int max86915_probe(struct platform_device *pdev)
#else
int max86915_probe(struct i2c_client *client, const struct i2c_device_id *id)
#endif
{
	int err = -ENODEV;

	struct max869_device_data *data;

#ifdef CONFIG_SPI_TO_I2C_FPGA
	u32 addr = 0;
#else
	/* check to make sure that the adapter supports I2C */
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		HRM_dbg("%s - I2C_FUNC_I2C not supported\n", __func__);
		return -ENODEV;
	}
#endif
	/* allocate some memory for the device */
	data = kzalloc(sizeof(struct max869_device_data), GFP_KERNEL);
	if (data == NULL) {
		HRM_dbg("%s - couldn't allocate memory\n", __func__);
		return -ENOMEM;
	}

	max869_data = data;

	max86915_init_device_data(data);

#ifdef CONFIG_SPI_TO_I2C_FPGA
	if (!pdev->dev.of_node) {
		HRM_dbg("%s - failed, DT is NULL\n", __func__);
		goto err_get_dt;
	}
	data->client = kzalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (data->client == NULL) {
		HRM_dbg("%s - couldn't allocate memory\n", __func__);
		goto err_allocate_memory;
	}

	data->pdev = pdev;
	err = of_property_read_u32(data->pdev->dev.of_node, "reg", &addr);
	if (err < 0) {
		HRM_dbg("%s - Unable to read hrmsensor slave addr\n", __func__);
		goto err_read_reg;
	}
	HRM_dbg("%s - hrmsensor slave addr(0x%p) = 0x%x\n", __func__, data->pdev->dev.of_node, addr);
	data->client->addr = addr;

#else
	data->client = client;
	i2c_set_clientdata(client, data);
#endif
	mutex_init(&data->i2clock);
	mutex_init(&data->activelock);
	mutex_init(&data->suspendlock);

	HRM_dbg("%s - start\n", __func__);

	HRM_info("%s client  = %p\n", __func__, client);

	err = max86915_parse_dt(data);
	if (err < 0) {
		HRM_dbg("[SENSOR] %s - of_node error\n", __func__);
		err = -ENODEV;
		goto err_parse_dt;
	}
	err = max86915_setup_gpio(data);
	if (err) {
		HRM_dbg("%s - could not initialize resources\n", __func__);
		goto err_setup_gpio;
	}

	err = max86915_power_ctrl(data, PWR_ON);
	if (err < 0) {
		HRM_dbg("%s max86915_power_ctrl fail(%d, %d)\n", __func__,
			err, PWR_ON);
		goto err_power_on;
	}

	err = max86915_init_device(data);
	if (err < 0) {
		HRM_dbg("%s max86915_init_device fail(%d)\n", __func__,
			err);
		goto hrm_init_device_failed;
	}

	data->hrm_input_dev = input_allocate_device();
	if (!data->hrm_input_dev) {
		HRM_dbg("%s - could not allocate input device\n",
			__func__);
		goto err_input_allocate_device;
	}
	data->hrm_input_dev->name = MODULE_NAME_HRM;
	input_set_drvdata(data->hrm_input_dev, data);
	input_set_capability(data->hrm_input_dev, EV_REL, REL_X);
	input_set_capability(data->hrm_input_dev, EV_REL, REL_Y);
	input_set_capability(data->hrm_input_dev, EV_REL, REL_Z);
	input_set_capability(data->hrm_input_dev, EV_REL, REL_RX);
	input_set_capability(data->hrm_input_dev, EV_REL, REL_MISC);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_X);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_Y);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_Z);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_RX);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_RY);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_RZ);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_THROTTLE);
	input_set_capability(data->hrm_input_dev, EV_ABS, ABS_RUDDER);

	err = input_register_device(data->hrm_input_dev);
	if (err < 0) {
		input_free_device(data->hrm_input_dev);
		HRM_dbg("%s - could not register input device\n", __func__);
		goto err_input_register_device;
	}
	err = sensors_create_symlink(data->hrm_input_dev);
	if (err < 0) {
		HRM_dbg("%s - create_symlink error\n", __func__);
		goto err_sensors_create_symlink;
	}
	err = sysfs_create_group(&data->hrm_input_dev->dev.kobj,
				 &hrm_attribute_group);
	if (err) {
		HRM_dbg("%s - could not create sysfs group\n",
			__func__);
		goto err_sysfs_create_group;
	}
#ifdef CONFIG_ARCH_QCOM
	err = sensors_register(&data->dev, data, max86915_sensor_attrs,
			MODULE_NAME_HRM);
#else
	err = sensors_register(data->dev, data, max86915_sensor_attrs,
			MODULE_NAME_HRM);
#endif
	if (err) {
		HRM_dbg("%s - cound not register hrm_sensor(%d).\n",
			__func__, err);
		goto hrm_sensor_register_failed;
	}

	err = max86915_setup_irq(data);
	if (err) {
		HRM_dbg("%s - could not setup hrm_irq\n", __func__);
		goto err_setup_irq;
	}

	err = max86915_power_ctrl(data, PWR_OFF);
	if (err < 0) {
		HRM_dbg("%s max86915_power_ctrl fail(%d, %d)\n", __func__,
			err, PWR_OFF);
		goto dev_set_drvdata_failed;
	}

	HRM_dbg("%s success\n", __func__);
	goto done;

dev_set_drvdata_failed:
	free_irq(data->hrm_irq, data);
err_setup_irq:
	sensors_unregister(data->dev, max86915_sensor_attrs);
hrm_sensor_register_failed:
	sysfs_remove_group(&data->hrm_input_dev->dev.kobj,
			   &hrm_attribute_group);
err_sysfs_create_group:
	sensors_remove_symlink(data->hrm_input_dev);
err_sensors_create_symlink:
	input_unregister_device(data->hrm_input_dev);
err_input_register_device:
err_input_allocate_device:
hrm_init_device_failed:
	max86915_power_ctrl(data, PWR_OFF);
err_power_on:
	gpio_free(data->hrm_int);
	gpio_free(data->hrm_en);
err_setup_gpio:
err_parse_dt:
	if (data->hrm_pinctrl) {
#ifdef CONFIG_ARCH_QCOM
		devm_pinctrl_put(data->hrm_pinctrl);
#else
		pinctrl_put(data->hrm_pinctrl);
#endif
		data->hrm_pinctrl = NULL;
	}
	if (data->pins_idle)
		data->pins_idle = NULL;
	if (data->pins_sleep)
		data->pins_sleep = NULL;

	mutex_destroy(&data->i2clock);
	mutex_destroy(&data->activelock);
	mutex_destroy(&data->suspendlock);
#ifdef CONFIG_SPI_TO_I2C_FPGA
err_read_reg:
	kfree(data->client);
err_allocate_memory:
err_get_dt:
#endif
	kfree(data);
	HRM_dbg("%s failed\n", __func__);
done:
	return err;
}


#ifdef CONFIG_SPI_TO_I2C_FPGA
static int max86915_remove(struct platform_device *pdev)
#else
int max86915_remove(struct i2c_client *client)
#endif
{
#ifdef CONFIG_SPI_TO_I2C_FPGA
	struct max869_device_data *data = dev_get_drvdata(&pdev->dev);
#else
	struct max869_device_data *data = i2c_get_clientdata(client);
#endif
	int err;

	HRM_dbg("%s\n", __func__);
	max86915_power_ctrl(data, PWR_OFF);
	err = max869_deinit_device(data);
	if (err)
		HRM_dbg("%s hrm_deinit device fail err = %d\n",
			__func__, err);

	sensors_unregister(data->dev, max86915_sensor_attrs);
	sysfs_remove_group(&data->hrm_input_dev->dev.kobj,
			   &hrm_attribute_group);
	sensors_remove_symlink(data->hrm_input_dev);
	input_unregister_device(data->hrm_input_dev);

	if (data->hrm_pinctrl) {
#ifdef CONFIG_ARCH_QCOM
		devm_pinctrl_put(data->hrm_pinctrl);
#else
		pinctrl_put(data->hrm_pinctrl);
#endif
		data->hrm_pinctrl = NULL;
	}
	if (data->pins_idle)
		data->pins_idle = NULL;
	if (data->pins_sleep)
		data->pins_sleep = NULL;
	disable_irq(data->hrm_irq);
	free_irq(data->hrm_irq, data);
	gpio_free(data->hrm_int);
	gpio_free(data->hrm_en);
	mutex_destroy(&data->i2clock);
	mutex_destroy(&data->activelock);
	mutex_destroy(&data->suspendlock);

	kfree(data->lib_ver);
	kfree(data);
#ifndef CONFIG_SPI_TO_I2C_FPGA
	i2c_set_clientdata(client, NULL);
#endif
	return 0;
}

#ifndef CONFIG_SPI_TO_I2C_FPGA
static void max86915_shutdown(struct i2c_client *client)
{
	HRM_dbg("%s\n", __func__);
}
#endif
#ifdef CONFIG_PM
static int max86915_pm_suspend(struct device *dev)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int err = 0;

	HRM_dbg("%s %d\n", __func__, data->hrm_enabled_mode);

	data->hrm_prev_mode = data->hrm_enabled_mode;
	data->mode_sdk_prev = data->mode_sdk_enabled;

	if (data->hrm_enabled_mode != 0)
		max86915_enable_store(dev, &dev_attr_enable, "0", 2);

	mutex_lock(&data->suspendlock);

	data->pm_state = PM_SUSPEND;

	max86915_pin_control(data, false);

	mutex_unlock(&data->suspendlock);

	return err;
}

static int max86915_pm_resume(struct device *dev)
{
	struct max869_device_data *data = dev_get_drvdata(dev);
	int err = 0;
	char buf[3];

	HRM_dbg("%s %d\n", __func__, data->hrm_prev_mode);

	mutex_lock(&data->suspendlock);

	max86915_pin_control(data, true);

	data->pm_state = PM_RESUME;

	mutex_unlock(&data->suspendlock);

	if (data->hrm_prev_mode != 0) {
		if (data->hrm_prev_mode == MODE_SDK_IR) {
			if (data->mode_sdk_prev & 1)
				max86915_enable_store(dev, &dev_attr_enable, "10", 2);
			if (data->mode_sdk_prev & 2)
				max86915_enable_store(dev, &dev_attr_enable, "11", 2);
			if (data->mode_sdk_prev & 4)
				max86915_enable_store(dev, &dev_attr_enable, "12", 2);
			if (data->mode_sdk_prev & 8)
				max86915_enable_store(dev, &dev_attr_enable, "13", 2);
		} else {
			sprintf(buf, "%d", data->hrm_prev_mode);
			max86915_enable_store(dev, &dev_attr_enable, buf, 2);
		}
	}

	return err;
}

static const struct dev_pm_ops max86915_pm_ops = {
	.suspend = max86915_pm_suspend,
	.resume = max86915_pm_resume
};
#endif

static struct of_device_id max86915_match_table[] = {
	{ .compatible = "hrmsensor",},
	{},
};

static const struct i2c_device_id max86915_device_id[] = {
	{ "hrmsensor", 0 },
	{ }
};
/* descriptor of the hrmsensor I2C driver */
#ifdef CONFIG_SPI_TO_I2C_FPGA
static struct platform_driver max86915_i2c_driver = {
	.driver = {
	    .name = "hrmsensor",
	    .owner = THIS_MODULE,
		.of_match_table = max86915_match_table,
	},
	.probe = max86915_probe,
	.remove = max86915_remove,
};
#else
static struct i2c_driver max86915_i2c_driver = {
	.driver = {
	    .name = "hrmsensor",
	    .owner = THIS_MODULE,
#if defined(CONFIG_PM)
	    .pm = &max86915_pm_ops,
#endif
	    .of_match_table = max86915_match_table,
	},
	.probe = max86915_probe,
	.remove = max86915_remove,
	.shutdown = max86915_shutdown,
	.id_table = max86915_device_id,
};
#endif

/* initialization and exit functions */
static int __init max86915_init(void)
{
	if (!lpcharge)
#ifdef CONFIG_SPI_TO_I2C_FPGA
		return platform_driver_register(&max86915_i2c_driver);
#else
		return i2c_add_driver(&max86915_i2c_driver);
#endif
	else
		return 0;
}

static void __exit max86915_exit(void)
{
#ifdef CONFIG_SPI_TO_I2C_FPGA
	platform_driver_unregister(&max86915_i2c_driver);
#else
	i2c_del_driver(&max86915_i2c_driver);
#endif
}

module_init(max86915_init);
module_exit(max86915_exit);

MODULE_DESCRIPTION("max86915 Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");

