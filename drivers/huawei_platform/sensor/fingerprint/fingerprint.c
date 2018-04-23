
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/wakelock.h>
#include <linux/hisi/hw_cmdline_parse.h>
#include <media/huawei/hw_extern_pmic.h>
#include <linux/slab.h>
#include "fingerprint.h"
#if defined (CONFIG_TEE_TUI)
#include "tui.h"
#endif

#if defined (CONFIG_HUAWEI_DSM)
#include <dsm/dsm_pub.h>
struct dsm_dev dsm_fingerprint =
{
    .name = "dsm_fingerprint",
    .device_name = "fingerprint",
    .ic_name = "NNN",
    .module_name = "NNN",
    .fops = NULL,
    .buff_size = 1024,
};
struct dsm_client *fingerprint_dclient = NULL;

static fp_sensor_info g_fp_sensor_info[] = {
        {0x021b, "FPC1021B"},
        {0x021f, "FPC1021F"},
        {0x0111, "FPC10221"},
        {0x0121, "FPC10222"},
        {0x1401, "FPC11401"},
        {0x140a, "FPC1140A"},
        {0x140b, "FPC1140B"},
        {0x140c, "FPC1140C"},
        {0x0321, "FPC1268X"},
        {0x0311, "FPC1265X"},
        {0x0711, "FPC1075S"},
        {0x0721, "FPC1075T"},
        {0x0611, "FPC1023SMA"},
        {0x0612, "FPC1023SMB"},
        {0x0621, "FPC1023TSC"},
        {0x0341, "FPC1266Z120"},
        {0x0074, "SYNA109A0"},
        {0x3266, "GOODIX3266"},
        {0x3288, "GOODIX3288"},
        {0x8206, "GOODIX8206"},
        {0x5266, "GOODIX5266"},
        {0x5296, "GOODIX5296"},
        {0x6185, "SILEAD6185"},
        {0x6165, "SILEAD6165"},
        {0x6175, "SILEAD6275"},
};
#endif
#define HWLOG_TAG fingerprint
HWLOG_REGIST();

/* tui_flg will be set to 1 when tui init(which mean tui is displaying)
 * set to 0 when tui exit
 * This will let fp hal do not read from rst gpio,just return 1
 */
static int tui_flg = 0;
/*lint -save -e* */
static DEFINE_MUTEX(tui_flg_lock);
/*lint -restore*/
extern unsigned int runmode_is_factory(void);
/* fp_ready_flg will be set to 1 when fp hal has init fp sensor succeed
 * This will tell tui_fp_notify when to send msg to fp hal
 */
static int fp_ready_flg = 0;

/*
 * When tui is displaying, the fingerprint sensor will response as a fiq
 * in this situation fp_fiq_flg will be set to 1
 * When hal check gpio, if fp_fiq_flg is 1 then just return irq=1, else return irq=0
 */
static int fp_fiq_flg = 0;

/*extern ldo power supply for fingerprint */
struct regulator *fp_ex_regulator = NULL;
#define FINGERPRINT_EXTERN_LDO_NUM "fingreprint_ldo"
#define FINGERPRINT_EXTERN_LDO_NAME "EXTERN_LDO"
extern int tp_gpio_num;
/**
 * sysf node to check the interrupt status of the sensor, the interrupt
 * handler should perform sysf_notify to allow userland to poll the node.
 */
static ssize_t irq_get(struct device* device,
                       struct device_attribute* attribute,
                       char* buffer)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    int irq;
    printk("tui_flg is %d, fp_ready_flg is %d, fp_fiq_flg is %d\n", tui_flg, fp_ready_flg, fp_fiq_flg);
    mutex_lock(&tui_flg_lock);
    if (tui_flg == 0)
        irq = gpio_get_value(fingerprint->irq_gpio);
    else {
        //when fp fiq comes, it should return 1 else return 0
        if (!fp_fiq_flg) {
            irq = 0;
        }
        else {
            irq = 1;
            fp_fiq_flg = 0;
        }
    }
    mutex_unlock(&tui_flg_lock);
    return scnprintf(buffer, PAGE_SIZE, "%i\n", irq);
}


/**
 * writing to the irq node will just drop a printk message
 * and return success, used for latency measurement.
 */
static ssize_t irq_ack(struct device* device,
                       struct device_attribute* attribute,
                       const char* buffer, size_t count)
{
    //hwlog_info("%s\n", __func__);
    return count;
}

static ssize_t result_show(struct device* device,
                       struct device_attribute* attribute,
                       char* buffer)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    return scnprintf(buffer, PAGE_SIZE, "%lu", fingerprint->finger_num);
}

static ssize_t result_store(struct device* device,
                       struct device_attribute* attribute,
                       const char* buffer, size_t count)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    fingerprint->finger_num = simple_strtoul(buffer, NULL, 10);
    sysfs_notify(&fingerprint->pf_dev->dev.kobj, NULL, "result");
    return count;
}

static ssize_t read_image_flag_show(struct device* device,
                       struct device_attribute* attribute,
                       char* buffer)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    return scnprintf(buffer, PAGE_SIZE, "%d", fingerprint->read_image_flag ? 1 : 0);
}
static ssize_t read_image_flag_store(struct device* device,
                       struct device_attribute* attribute,
                       const char* buffer, size_t count)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    fingerprint->read_image_flag = simple_strtoul(buffer, NULL, 10);
    return count;
}

static ssize_t irq_enabled_show(struct device* device,
                       struct device_attribute* attribute,
                       char* buffer)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    return scnprintf(buffer, PAGE_SIZE, "%d", fingerprint->irq_enabled ? 1 : 0);
}
static ssize_t irq_enabled_store(struct device* device,
                       struct device_attribute* attribute,
                       const char* buffer, size_t count)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}
    fingerprint->irq_enabled = simple_strtoul(buffer, NULL, 10);
    hwlog_info("%s irq_enabled = %d\n", __func__, (int)fingerprint->irq_enabled);
    return count;
}
static ssize_t test_power_en_show(struct device* device,
                       struct device_attribute* attribute,
                       char* buffer)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    int status = 0;
    if (NULL == fingerprint){return -EINVAL;}
    if ((-EINVAL) == fingerprint->power_en_gpio)
    {
        return scnprintf(buffer, PAGE_SIZE, "%d", (-EIO));
    }
    status = gpio_get_value_cansleep(fingerprint->power_en_gpio);
    return scnprintf(buffer, PAGE_SIZE, "%d", status);
}
static ssize_t test_power_en_store(struct device* device,
                       struct device_attribute* attribute,
                       const char* buffer, size_t count)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    int error = 0;
    unsigned long power_en_value = 0;
    if ((NULL == fingerprint) || ((-EINVAL) == fingerprint->power_en_gpio))
    {
        return -EIO;
    }
    if (!runmode_is_factory())// if normal version just go out
    {
        return -EIO;
    }
    power_en_value = simple_strtoul(buffer, NULL, 10);
    hwlog_err("%s power_en_value = %lu\n", __func__, power_en_value);
    if ((0 == power_en_value) || (1 == power_en_value))
    {
        if (0 == power_en_value)
        {
            msleep(50); // wait for TA actions finished then power off.
        }
        error = gpio_direction_output(fingerprint->power_en_gpio, power_en_value);
        if (error)
        {
            hwlog_err("%s gpio_direction_output failed\n", __func__);
            return error;
        }
    }
    else
    {
        return -EINVAL;
    }

    return count;
}

static ssize_t nav_show(struct device* device,
                       struct device_attribute* attribute,
                       char* buffer)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    return scnprintf(buffer, PAGE_SIZE, "%d", fingerprint->nav_stat);
}

static ssize_t nav_store(struct device* device,
                       struct device_attribute* attribute,
                       const char* buffer, size_t count)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}

    fingerprint->nav_stat = simple_strtoul(buffer, NULL, 10);
    return count;
}
static ssize_t module_id_show(struct device* device,
                       struct device_attribute* attribute,
                       char* buffer)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}
    return scnprintf(buffer,64, "%s", fingerprint->module_id);
}

static ssize_t module_id_store(struct device* device,
                       struct device_attribute* attribute,
                       const char* buffer, size_t count)
{
    struct fp_data* fingerprint = dev_get_drvdata(device);
    if (NULL == fingerprint)
    {return -EINVAL;}
    strncpy( fingerprint->module_id,buffer ,63);
    fingerprint->module_id[63] = '\0';// write the tail
    return count;
}

static DEVICE_ATTR(irq, S_IRUSR | S_IWUSR, irq_get, irq_ack);
static DEVICE_ATTR(result, S_IRUSR | S_IWUSR, result_show, result_store);
static DEVICE_ATTR(read_image_flag, S_IRUSR | S_IWUSR, read_image_flag_show, read_image_flag_store);
static DEVICE_ATTR(test_power_en, S_IRUSR | S_IWUSR, test_power_en_show, test_power_en_store);
static DEVICE_ATTR(nav, S_IRUSR | S_IWUSR |S_IRGRP |S_IWGRP, nav_show, nav_store);
static DEVICE_ATTR(module_id, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, module_id_show, module_id_store);
static DEVICE_ATTR(irq_enabled, S_IRUSR | S_IWUSR, irq_enabled_show, irq_enabled_store);

/*
*FUNCTION: fingerprint_get_module_name
*
*DESCRIPTION:
*	This API is used to get module name based on module id
*@fingerprint:the fingerprint info
*@module:the buf to store the module name
*@len:the buf length
*
*RETRUN:
*	SUCCESS:	0
*	FAILED:		not 0
*/
static int fingerprint_get_module_name(struct fp_data* fingerprint, char *module, unsigned int len)
{
    int ret = -1;
    const char* module_info = NULL;
    struct device* dev = NULL;
    struct device_node* np = NULL;

    if (NULL == fingerprint || NULL == fingerprint->dev || NULL == module)
    {
        hwlog_err("%s failed,the pointer is null\n", __func__);
        goto exit;
    }

    if(len <= FP_DEFAULT_INFO_LEN || len > FP_MAX_MODULE_INFO_LEN)
    {
        hwlog_err("%s failed,the buf length is error!\n", __func__);
        goto exit;
    }
    dev = fingerprint->dev;

    if(NULL == dev->of_node)
    {
        hwlog_err("%s failed,the pointer dev->of_node is null\n", __func__);
        goto exit;
    }
    np = dev->of_node;

    switch (fingerprint->module_vendor_info)
    {
       case MODULEID_LOW:
            ret = of_property_read_string(np, "fingerprint,moduleid_low", &module_info);
           if (ret)
           {
               hwlog_err("%s failed to get moduleid_low from device tree\n", __func__);
               break;
            }
            strncpy(module, module_info, len-1);
            break;
      case MODULEID_HIGT:
            ret = of_property_read_string(np, "fingerprint,moduleid_high", &module_info);
            if (ret)
            {
                hwlog_err("%s failed to get moduleid_high from device tree\n", __func__);
                break;
            }
            strncpy(module, module_info, len-1);
            break;
      case MODULEID_FLOATING:
            ret = of_property_read_string(np, "fingerprint,moduleid_float", &module_info);
            if (ret)
            {
                 hwlog_err("%s failed to get fingerprint,moduleid_float from device tree\n", __func__);
                 break;
            }
            strncpy(module, module_info, len-1);
            break;
      default:
            ret = 0;
            strncpy(module, "NN", FP_DEFAULT_INFO_LEN);
            break;
    }
    *(module+len-1) = '\0';
exit:
    return ret;
}

/*
*FUNCTION: fingerprint_get_sensor_name
*
*DESCRIPTION:
*	This API is used to get sensor info based on hardware id
*@sensor_id:the hardware id
*
*RETRUN:
*	SUCCESS:	pointer to sensor info
*	FAILED:		NULL
*/
static char* fingerprint_get_sensor_name(unsigned int sensor_id)
{
    unsigned int i = 0;
    for(i = 0; i < sizeof(g_fp_sensor_info)/sizeof(g_fp_sensor_info[0]); i++)
    {
        if(sensor_id == g_fp_sensor_info[i].sensor_id)
        {
            hwlog_info("sensor_name:%s\n", g_fp_sensor_info[i].sensor_name);
            return g_fp_sensor_info[i].sensor_name;
        }
    }
    return NULL;
}
#if defined (CONFIG_HUAWEI_DSM)
/*
*FUNCTION: fingerprint_update_vendor_info
*
*DESCRIPTION:
*	This API is used to upate vendor info based on sensor_id & module_name for DMD
*@fingerprint:pointer to the structer that contained the sensor_id and module_name
*
*RETRUN:
*
*/
static void fingerprint_update_vendor_info(struct fp_data* fingerprint)
{
    char *ic_name = NULL;
    char module_name[FP_MAX_MODULE_INFO_LEN] = {0};

    if(NULL == fingerprint)
    {
        hwlog_err("the pointer is null");
        return;
    }
    if(FP_RETURN_SUCCESS == fingerprint_get_module_name(fingerprint, module_name, FP_MAX_MODULE_INFO_LEN))
    {
        dsm_fingerprint.module_name = module_name;
    }

    ic_name = fingerprint_get_sensor_name(fingerprint->sensor_id);
    if(ic_name)
    {
        dsm_fingerprint.ic_name = ic_name;
        dsm_update_client_vendor_info(&dsm_fingerprint);
    }
    else
    {
        ic_name = (char*)kzalloc(FP_MAX_SENSOR_ID_LEN, GFP_KERNEL);
        if(NULL == ic_name)
        {
            hwlog_err("malloc failed!\n");
            return;
        }
        snprintf(ic_name, FP_MAX_SENSOR_ID_LEN, "%x", fingerprint->sensor_id);
        dsm_fingerprint.ic_name = ic_name;
        dsm_update_client_vendor_info(&dsm_fingerprint);
        kfree(ic_name);
        dsm_fingerprint.ic_name = NULL;
    }
}
#endif

static ssize_t fingerprint_chip_info_show(struct device *device, struct device_attribute *attribute, char *buf)
{
    int retval = 0;
    char module[FP_MAX_MODULE_INFO_LEN] = {0};
    char sensor_id[FP_MAX_SENSOR_ID_LEN]={0};

    if(NULL == device || NULL == buf)
    {
        hwlog_err("%s failed,the pointer is null\n", __func__);
        return -EINVAL;
    }

    struct fp_data* fingerprint= dev_get_drvdata(device);
    if (NULL == fingerprint)
    {
        hwlog_err("%s failed,the parameters is null\n", __func__);
        return -EINVAL;
    }

    retval = fingerprint_get_module_name(fingerprint, module, FP_MAX_MODULE_INFO_LEN);
    if(retval != FP_RETURN_SUCCESS)
    {
        strncpy(module, "NN", FP_DEFAULT_INFO_LEN);
    }

    snprintf(sensor_id, FP_MAX_SENSOR_ID_LEN, "%x", fingerprint->sensor_id);
    return scnprintf(buf,FP_MAX_CHIP_INFO_LEN,"%s--%s\n", sensor_id, module);
}
static DEVICE_ATTR(fingerprint_chip_info, S_IRUSR  | S_IRGRP, fingerprint_chip_info_show, NULL);
static struct attribute* attributes[] =
{
    &dev_attr_irq.attr,
    &dev_attr_fingerprint_chip_info.attr,
    &dev_attr_result.attr,
    &dev_attr_read_image_flag.attr,
    &dev_attr_test_power_en.attr,
    &dev_attr_nav.attr,
    &dev_attr_module_id.attr,
    &dev_attr_irq_enabled.attr,
    NULL
};

static const struct attribute_group attribute_group =
{
    .attrs = attributes,
};
static struct fp_data* g_fingerprint = NULL;
int tui_fp_notify(void)
{
    struct fp_data* fingerprint = g_fingerprint;

    if (!fingerprint)
    {
        hwlog_err("%s failed g_fingerprint is NULL\n", __func__);
        return -EINVAL;
    }
    if (!fp_ready_flg) {
        //fp not ready
        return 0;
    }
    printk("%s: fp is ready\n", __func__);
    fp_fiq_flg = 1;
    smp_rmb();

    if (fingerprint->wakeup_enabled) {
        wake_lock_timeout(&fingerprint->ttw_wl, msecs_to_jiffies(FPC_TTW_HOLD_TIME));
    }

    sysfs_notify(&fingerprint->pf_dev->dev.kobj, NULL, dev_attr_irq.attr.name);
    return 0;
}
static irqreturn_t fingerprint_irq_handler(int irq, void* handle)
{
    struct fp_data* fingerprint = handle;

    smp_rmb();

    if (fingerprint->wakeup_enabled)
    {
        wake_lock_timeout(&fingerprint->ttw_wl, msecs_to_jiffies(FPC_TTW_HOLD_TIME));
    }

    sysfs_notify(&fingerprint->pf_dev->dev.kobj, NULL, dev_attr_irq.attr.name);
    //hwlog_info("%s \n", __func__);
    return IRQ_HANDLED;
}

void fingerprint_get_navigation_adjustvalue(struct device* dev, struct fp_data* fp_data)
{
    struct device_node* np;
    int adjust1 = NAVIGATION_ADJUST_NOREVERSE;
    int adjust2 = NAVIGATION_ADJUST_NOTURN;

    if (!dev || !dev->of_node)
    {
        hwlog_err("%s failed dev or dev node is NULL\n", __func__);
        return;
    }

    np = dev->of_node;

    (void)of_property_read_u32(np, "fingerprint,navigation_adjust1", (unsigned int *)(&adjust1));

    if(adjust1 != NAVIGATION_ADJUST_NOREVERSE && adjust1 != NAVIGATION_ADJUST_REVERSE)
    {
        adjust1 = NAVIGATION_ADJUST_NOREVERSE;
        hwlog_err("%s navigation_adjust1 set err only support 0 and 1.\n", __func__);
    }

    (void)of_property_read_u32(np, "fingerprint,navigation_adjust2", (unsigned int *)(&adjust2));

    if(adjust2 != NAVIGATION_ADJUST_NOTURN && adjust2 != NAVIGATION_ADJUST_TURN90 &&
            adjust2 != NAVIGATION_ADJUST_TURN180 && adjust2 != NAVIGATION_ADJUST_TURN270)
    {
        adjust2 = NAVIGATION_ADJUST_NOTURN;
        hwlog_err("%s navigation_adjust2 set err only support 0 90 180 and 270.\n", __func__);
    }

    fp_data->navigation_adjust1 = adjust1;
    fp_data->navigation_adjust2 = adjust2;

    hwlog_info("%s get navigation_adjust1 = %d, navigation_adjust2 = %d.\n", __func__,
                  fp_data->navigation_adjust1, fp_data->navigation_adjust2);
    return;
}

int fingerprint_get_dts_data(struct device* dev, struct fp_data* fp_data)
{
    struct device_node* np;
    int ret = 0;
    const char* extern_ldo_info;

    if (!dev || !dev->of_node)
    {
        hwlog_err("%s failed dev or dev node is NULL\n", __func__);
        return -EINVAL;
    }

    np = dev->of_node;


    ret = of_property_read_u32(np, "fingerprint,reset_gpio",
                               (unsigned int *)(&fp_data->rst_gpio));

    if (ret)
    {
        fp_data->rst_gpio = -EINVAL;
        hwlog_err("%s failed to get reset gpio from device tree\n", __func__);
        goto exit;
    }

    ret = of_property_read_u32(np, "fingerprint,irq_gpio",
                               (unsigned int *)(&fp_data->irq_gpio));

    if (ret)
    {
        fp_data->irq_gpio = -EINVAL;
        hwlog_err("%s failed to get irq gpio from device tree\n", __func__);
        goto exit;
    }

    ret = of_property_read_u32(np, "fingerprint,power_en_gpio", (unsigned int *)(&fp_data->power_en_gpio));
    if (ret)
    {
        fp_data->power_en_gpio = -EINVAL;
        ret = 0;
        hwlog_info("%s failed to get power_en_gpio gpio from device tree, just go on\n", __func__);
    }

   ret = of_property_read_string(np, "fingerprint,extern_ldo_name", &extern_ldo_info);
   if (ret)
    {
        strncpy(fp_data->extern_ldo_name, "EINVAL", 8);
        ret = 0;
        hwlog_info("%s failed to get extern_ldo_name gpio from device tree, just go on\n", __func__);
    }
   else
   {
       strncpy(fp_data->extern_ldo_name, extern_ldo_info, sizeof(fp_data->extern_ldo_name));
       fp_data->extern_ldo_name[(sizeof(fp_data->extern_ldo_name)-1)] = '\0';

       ret = of_property_read_u32(np, "fingerprint,extern_ldo_num", (unsigned int *)(&fp_data->extern_ldo_num));
       if (ret)
       {
          fp_data->extern_ldo_num = -EINVAL;
          ret = 0;
          hwlog_info("%s failed to get extern_ldo_num gpio from device tree, just go on\n", __func__);
       }

        ret = of_property_read_u32(np, "fingerprint,extern_vol", (unsigned int *)(&fp_data->extern_vol));
       if (ret)
       {
          fp_data->extern_vol = -EINVAL;
          ret = 0;
          hwlog_info("%s failed to get extern_vol gpio from device tree, just go on\n", __func__);
       }
   }

    fp_data->moduleID_gpio = of_get_named_gpio(np, "fingerprint,moduleid_gpio", 0);

    if ((int)(fp_data->moduleID_gpio) < 0)
    {
        ret = of_property_read_u32(np, "fingerprint,moduleid_gpio",
                                   (unsigned int *)(&fp_data->moduleID_gpio));

        if (ret)
        {
            fp_data->moduleID_gpio = -EINVAL;
            hwlog_info("%s failed to moduleID_gpio gpio from device tree\n", __func__);
            //goto exit;  /* some products didn't support module ID gpio */
        }

        hwlog_info("%s moduleID_gpio=%u\n", __func__, fp_data->moduleID_gpio);
    }
    else
    {
        hwlog_info("%s moduleID_gpio=%u\n", __func__, fp_data->moduleID_gpio);
    }

    return 0;

exit:
    return  -EINVAL;
}

static int fingerprint_key_remap_reverse(int key)
{
    switch(key)
    {
        case EVENT_LEFT:
            key = EVENT_RIGHT;
            break;
        case EVENT_RIGHT:
            key = EVENT_LEFT;
            break;
        default:
            break;
    }

    return key;
}

static int fingerprint_key_remap_turn90(int key)
{
    switch(key)
    {
        case EVENT_LEFT:
            key = EVENT_UP;
            break;
        case EVENT_RIGHT:
            key = EVENT_DOWN;
            break;
        case EVENT_UP:
            key = EVENT_RIGHT;
            break;
        case EVENT_DOWN:
            key = EVENT_LEFT;
            break;
        default:
            break;
    }

    return key;
}

static int fingerprint_key_remap_turn180(int key)
{
    switch(key)
    {
        case EVENT_LEFT:
            key = EVENT_RIGHT;
            break;
        case EVENT_RIGHT:
            key = EVENT_LEFT;
            break;
        case EVENT_UP:
            key = EVENT_DOWN;
            break;
        case EVENT_DOWN:
            key = EVENT_UP;
            break;
        default:
            break;
    }

    return key;
}

static int fingerprint_key_remap_turn270(int key)
{
    switch(key)
    {
        case EVENT_LEFT:
            key = EVENT_DOWN;
            break;
        case EVENT_RIGHT:
            key = EVENT_UP;
            break;
        case EVENT_UP:
            key = EVENT_LEFT;
            break;
        case EVENT_DOWN:
            key = EVENT_RIGHT;
            break;
        default:
            break;
    }

    return key;
}

static int fingerprint_key_remap(struct fp_data* fingerprint, int key)
{
    if(key != EVENT_RIGHT && key != EVENT_LEFT && key != EVENT_UP && key != EVENT_DOWN)
    {
        return key;
    }

    if(fingerprint->navigation_adjust1 == NAVIGATION_ADJUST_REVERSE)
    {
        key = fingerprint_key_remap_reverse(key);
    }

    switch (fingerprint->navigation_adjust2)
    {
        case NAVIGATION_ADJUST_TURN90:
            key = fingerprint_key_remap_turn90(key);
            break;
        case NAVIGATION_ADJUST_TURN180:
            key = fingerprint_key_remap_turn180(key);
            break;
        case NAVIGATION_ADJUST_TURN270:
            key = fingerprint_key_remap_turn270(key);
            break;
        default:
            break;
    }

    return key;
}


static void fingerprint_input_report(struct fp_data* fingerprint, int key)
{
    key = fingerprint_key_remap(fingerprint, key);
    hwlog_info("%s key=%d\n", __func__, key);
    input_report_key(fingerprint->input_dev, key, 1);
    input_sync(fingerprint->input_dev);
    input_report_key(fingerprint->input_dev, key, 0);
    input_sync(fingerprint->input_dev);
}

static int fingerprint_open(struct inode* inode, struct file* file)
{
    struct fp_data* fingerprint;
    hwlog_info("%s\n", __func__);
    fingerprint = container_of(inode->i_cdev, struct fp_data, cdev);
    file->private_data = fingerprint;
    return 0;
}

static ssize_t fingerprint_write(struct file* file, const char* buff, size_t count, loff_t* ppos)
{
    hwlog_info("%s\n", __func__);
    return count;
}

static ssize_t fingerprint_read(struct file* file, char* buff, size_t count, loff_t* ppos)
{
    hwlog_info("%s\n", __func__);
    return 0;
}

static int fingerprint_get_irq_status(struct fp_data* fingerprint )
{
    int status = 0;
    status = gpio_get_value_cansleep(fingerprint->irq_gpio);
    return status;
}



static long fingerprint_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{

    int error = 0;

    struct fp_data* fingerprint;
    void __user* argp = (void __user*)arg;
    int key;
    int status;
    unsigned int sensor_id;

    fingerprint = (struct fp_data*)file->private_data;
    if (NULL == fingerprint)
    {
        hwlog_err("%s fingerprint is NULL\n", __func__);
        return -EFAULT;
    }

    if (_IOC_TYPE(cmd) != FP_IOC_MAGIC)
    { return -ENOTTY; }

    switch (cmd)
    {
        case FP_IOC_CMD_ENABLE_IRQ:
            hwlog_info("%s FP_IOC_CMD_ENABLE_IRQ \n", __func__);
            fp_ready_flg  = 1;
            if (wake_lock_active(&fingerprint->ttw_wl)) 
            {
                wake_unlock(&fingerprint->ttw_wl);
            }
            enable_irq(gpio_to_irq(fingerprint->irq_gpio ));
            fingerprint->irq_enabled = true;
            break;

        case FP_IOC_CMD_DISABLE_IRQ:
            hwlog_info("%s FP_IOC_CMD_DISABLE_IRQ \n", __func__);
            fp_ready_flg  = 0;
            disable_irq(gpio_to_irq(fingerprint->irq_gpio ));
            fingerprint->irq_enabled = false;
            break;

        case FP_IOC_CMD_SEND_UEVENT:
            if (copy_from_user(&key, argp, sizeof(key)))
            {
                hwlog_err("%s copy_from_user failed.\n", __func__);
                return -EFAULT;
            }
            if(key < KEY_MIN ||key > KEY_MAX)
            {
                hwlog_err("%s nav key is %d.\n", __func__ , key);
                return -EFAULT;
            }

            fingerprint_input_report(fingerprint, key);
            hwlog_info("%s FP_IOC_CMD_SEND_UEVENT\n", __func__);
            break;

        case FP_IOC_CMD_GET_IRQ_STATUS:

            status = fingerprint_get_irq_status(fingerprint);

            if(status != 0 && status != 1)
            {
                hwlog_err("%s fingerprint irq is %d.\n", __func__ , status);
                return -EFAULT;
            }

            error = copy_to_user(argp, &status, sizeof(status));

            if (error)
            {
                hwlog_err("%s copy_to_user failed error=%d.\n", __func__, error);
                return -EFAULT;
            }

            hwlog_info("%s FP_IOC_CMD_GET_IRQ_STATUS status=%d\n", __func__, status);
            break;

        case FP_IOC_CMD_SET_WAKELOCK_STATUS:
            if (copy_from_user(&key, argp, sizeof(key)))
            {
                hwlog_err("%s copy_from_user failed.\n", __func__);
                return -EFAULT;
            }

            if (key == 1)
            { fingerprint->wakeup_enabled = true; }
            else
            { fingerprint->wakeup_enabled = false; }

            hwlog_info("%s FP_IOC_CMD_SET_WAKELOCK_STATUS key =%d\n", __func__, key);

            break;
        case FP_IOC_CMD_SEND_SENSORID:
            if (copy_from_user(&sensor_id, argp, sizeof(sensor_id)))
            {
                hwlog_err("%s copy_from_user failed.\n", __func__);
                return -EFAULT;
            }

            fingerprint->sensor_id=sensor_id;
#if defined (CONFIG_HUAWEI_DSM)
            fingerprint_update_vendor_info(fingerprint);
#endif
            hwlog_info("%s FP_IOC_CMD_SEND_SENSORID =%x\n", __func__,sensor_id);
            break;
        default:
            hwlog_err("%s error = -EFAULT.\n", __func__);
            error = -EFAULT;
            break;
    }

    return error;
}

static int fingerprint_release(struct inode* inode, struct file* file)
{
    hwlog_info("%s\n", __func__);
    return 0;
}

static const struct file_operations fingerprint_fops =
{
    .owner          = THIS_MODULE,
    .open            = fingerprint_open,
    .write            = fingerprint_write,
    .read             = fingerprint_read,
    .release         = fingerprint_release,
    .unlocked_ioctl = fingerprint_ioctl,
};

static int fingerprint_reset_gpio_init(struct fp_data* fingerprint)
{
    int error = 0;

    error = gpio_request(fingerprint->rst_gpio, "fingerprint_reset_gpio");

    if (error)
    {
        hwlog_err(" %s gpio_request (reset_gpio) failed.\n",__func__);
        error = -EINVAL;
        return error;
    }


    fingerprint->pctrl = devm_pinctrl_get(&fingerprint->pf_dev->dev);

    if (IS_ERR(fingerprint->pctrl))
    {
        hwlog_err("%s devm_pinctrl_get failed\n", __func__);
        error = -EINVAL;
        return error;
    }

    fingerprint->pins_default = pinctrl_lookup_state(fingerprint->pctrl, "default");

    if (IS_ERR(fingerprint->pins_default))
    {
        hwlog_err("%s pinctrl_lookup_state failed\n", __func__);
        error = -EINVAL;
        goto err_pinctrl_put;
    }

    error = pinctrl_select_state(fingerprint->pctrl, fingerprint->pins_default);

    if (error < 0)
    {
        hwlog_err("%s pinctrl_select_state failed error=%d\n", __func__, error);
        error = -EINVAL;
        goto err_pinctrl_put;
    }

    error = gpio_direction_output(fingerprint->rst_gpio, 0);

    if (error)
    {
        hwlog_err("%s gpio_direction_output failed\n", __func__);
        return error;
    }

    return error;

err_pinctrl_put:
    devm_pinctrl_put(fingerprint->pctrl);
    return error;
}
static int fingerprint_power_en_gpio_init(struct fp_data* fingerprint)
{
    int error = 0;

    error = gpio_request(fingerprint->power_en_gpio, "fingerprint_power_en_gpio");

    if (error)
    {
        hwlog_err(" %s gpio_request (power_en_gpio) failed.\n",__func__);
        error = -EINVAL;
        return error;
    }

    error = gpio_direction_output(fingerprint->power_en_gpio, 1);

    if (error)
    {
        hwlog_err("%s gpio_direction_output power_en_gpio failed\n", __func__);
        return error;
    }
    return error;

}

static int fingerprint_extern_power_en(struct fp_data* fingerprint)
{
    int error = 0;
    error = strncmp(fingerprint->extern_ldo_name, "CAM_LDO", 7);

    if (error == 0)
    {
        if ((-EINVAL == fingerprint->extern_ldo_num) || (-EINVAL == fingerprint->extern_vol))
        {
            hwlog_err("%s something wrong with dts confing extern ldo\n", __func__);
            return -EINVAL;
        }

        hw_extern_pmic_config(fingerprint->extern_ldo_num,  fingerprint->extern_vol, 1);

        //hwlog_info("ldo_num = %d, ldo_vol = %d", fingerprint->extern_ldo_num, fingerprint->extern_vol );
        return 0;
    }
    else
    {
        error = strncmp(fingerprint->extern_ldo_name, FINGERPRINT_EXTERN_LDO_NAME, strlen(FINGERPRINT_EXTERN_LDO_NAME));
        if (error == 0)
        {
            if (-EINVAL == fingerprint->extern_vol)
            {
                hwlog_err("%s something wrong with dts confing extern ldo\n", __func__);
                return -EINVAL;
            }
            fp_ex_regulator = devm_regulator_get(fingerprint->dev, FINGERPRINT_EXTERN_LDO_NUM);

            if (IS_ERR(fp_ex_regulator))
            {
                hwlog_err("%s:No extern ldo found for fingerprint\n", __func__);
                return -EINVAL;
            }

            error= regulator_set_voltage(fp_ex_regulator, fingerprint->extern_vol, fingerprint->extern_vol);
            if(error!=0)
            {
                hwlog_err("%s:regulator_set_voltage fail,ret = %d \n", __func__, error);
            }

            error = regulator_set_mode(fp_ex_regulator, REGULATOR_MODE_NORMAL);
            if(error!=0)
            {
                hwlog_err("%s:regulator_set_mode fail,ret = %d \n", __func__, error);
            }

            error = regulator_enable(fp_ex_regulator);
            if(error!=0)
            {
                hwlog_err("%s:regulator_enable,ret = %d \n", __func__, error);
            }

            return 0;
        }
        else
        {
            hwlog_err("%s fingerprint_extern_power_en other ldo \n", __func__);
            return 0;
        }
    }
}
/*
 * This Function will be called when tui init or exit
 * It will set tui_flg, when it is set to 1 this mean tui is displaying
 * Fingerprint driver cannot read rst gpio because gpio is in secure status.
 */
#if defined (CONFIG_TEE_TUI_HI3650)
static int tui_fp_init(void *data, int secure)
{
    struct fp_data* fingerprint = g_fingerprint;
    if(!fingerprint){
        pr_err("%s:error:g_fingerprint is NULL\n", __func__);
        return -EINVAL;
    }
    //if fp rst gpio is in the same group with tui tp gpio then register the tui_fp_init
    if (fingerprint->irq_gpio/8 == tp_gpio_num/8) {
        if (secure) {
            mutex_lock(&tui_flg_lock);
            tui_flg = 1;
            mutex_unlock(&tui_flg_lock);
            disable_irq(gpio_to_irq(fingerprint->irq_gpio ));
            fingerprint->irq_enabled = false;
        }
        else {
            mutex_lock(&tui_flg_lock);
            tui_flg = 0;
            mutex_unlock(&tui_flg_lock);
            enable_irq(gpio_to_irq(fingerprint->irq_gpio ));
            fingerprint->irq_enabled = true;
        }
        printk("%s: tui_flg is %d\n", __func__, tui_flg);
    }
    return 0;
}
#endif
static int finerprint_get_module_info(struct fp_data* fingerprint)
{
    int error = 0;
    int pd_value = 0, pu_value = 0;

    error = gpio_request(fingerprint->moduleID_gpio, "fingerprint_moduleID_gpio");

    if (error)
    {
        hwlog_err("%s gpio_request failed\n", __func__);
        return error;
    }

    fingerprint->pctrl = devm_pinctrl_get(&fingerprint->pf_dev->dev);

    if (IS_ERR(fingerprint->pctrl))
    {
        hwlog_err("%s devm_pinctrl_get failed\n", __func__);
        error = -EINVAL;
        return error;
    }

    fingerprint->pins_default = pinctrl_lookup_state(fingerprint->pctrl, "default");

    if (IS_ERR(fingerprint->pins_default))
    {
        hwlog_err("%s pinctrl_lookup_state pins_default failed\n", __func__);
        error = -EINVAL;
        goto error_pinctrl_put;
    }

    fingerprint->pins_idle = pinctrl_lookup_state(fingerprint->pctrl, "idle");

    if (IS_ERR(fingerprint->pins_idle))
    {
        hwlog_err("%s pinctrl_lookup_state pins_idle failed.\n", __func__);
        error = -EINVAL;
        goto error_pinctrl_put;
    }

    error = gpio_direction_input(fingerprint->moduleID_gpio);

    if (error < 0)
    {
        hwlog_err("%s set input mode error, error=%d.\n", __func__, error);
        error = -EINVAL;
        return error;
    }

    error = pinctrl_select_state(fingerprint->pctrl, fingerprint->pins_default);

    if (error < 0)
    {
        hwlog_err("%s set iomux normal error, error=%d.\n", __func__, error);
        error = -EINVAL;
        goto error_pinctrl_put;
    }
/*lint -save -e* */
    mdelay(10);
/*lint -restore*/
    pu_value = gpio_get_value_cansleep(fingerprint->moduleID_gpio);
    hwlog_info("%s PU module id gpio = %d.\n", __func__, pu_value);

    error = gpio_direction_input(fingerprint->moduleID_gpio);

    if (error < 0)
    {
        hwlog_err("%s set input mode error, error=%d.\n", __func__, error);
        error = -EINVAL;
        return error;
    }

    error = pinctrl_select_state(fingerprint->pctrl, fingerprint->pins_idle);

    if (error < 0)
    {
        hwlog_err("%s set iomux normal error, error=%d.\n", __func__, error);
        error = -EINVAL;
        return error;
    }
/*lint -save -e* */
    mdelay(10);
/*lint -restore*/
    pd_value = gpio_get_value_cansleep(fingerprint->moduleID_gpio);
    hwlog_info("%s PD module id gpio=%d.\n", __func__, pd_value);

    if (pu_value == pd_value)
    {
        if (pu_value == 1)
        {
            fingerprint->module_vendor_info = MODULEID_HIGT;
            hwlog_info("%s moduleID pin is HIGT.\n", __func__);
        }

        if (pd_value == 0)
        {
            fingerprint->module_vendor_info = MODULEID_LOW;
            hwlog_info("%s moduleID pin is LOW.\n", __func__);
        }
    }
    else
    {
        fingerprint->module_vendor_info = MODULEID_FLOATING;
        hwlog_info("%s moduleID pin is FLOATING.\n", __func__);
    }

    return error;

error_pinctrl_put:
    devm_pinctrl_put(fingerprint->pctrl);
    return error;
}

static int fingerprint_probe(struct platform_device* pdev)
{
    struct device* dev = &pdev->dev;
    int rc = 0;
    int irqf;
    struct device_node* np = dev->of_node;
    struct fp_data* fingerprint = devm_kzalloc(dev, sizeof(*fingerprint),
                                  GFP_KERNEL);
#if defined (CONFIG_HUAWEI_DSM)
    if(!fingerprint_dclient)
    {
        fingerprint_dclient = dsm_register_client(&dsm_fingerprint);
    }
#endif

    if (!fingerprint)
    {
        hwlog_err("%s failed to allocate memory for struct fp_data.\n", __func__);
        rc = -ENOMEM;
        goto exit;
    }

    hwlog_info("fingerprint driver v4.0 for Android N.");

    fingerprint->dev = dev;
    dev_set_drvdata(dev, fingerprint);
    fingerprint->pf_dev = pdev;

    if (!np)
    {
        hwlog_err("%s dev->of_node not found.\n", __func__);
        rc = -EINVAL;
        goto exit;
    }

    rc = fingerprint_get_dts_data(&pdev->dev, fingerprint);

    if (rc)
    {
        hwlog_err("something wrong in fingerprint_get_dts_data error=%d\n", rc);
        rc = -EINVAL;
        goto exit;
    }

    fingerprint_get_navigation_adjustvalue(&pdev->dev, fingerprint);

#if defined (CONFIG_HISI_PARTITION_KIRIN970)
    hwlog_info("fingerprint driver KIRIN970.");
#else
    rc = fingerprint_reset_gpio_init(fingerprint);

    if (rc)
    {
        hwlog_err("something wrong in reset gpio init error=%d\n", rc);
        rc = -EINVAL;
        goto exit;
    }
#endif

    if ((-EINVAL) != fingerprint->power_en_gpio) // Check power_en_gpio exsit.
    {
        rc = fingerprint_power_en_gpio_init(fingerprint);
        if (rc)
        {
            hwlog_err("something wrong in power_en_gpio gpio init error=%d\n", rc);
            rc = 0;
            //goto exit;
        }
    }

    if (strncmp(fingerprint->extern_ldo_name, "EINVAL", 6) != 0)
    {
        rc = fingerprint_extern_power_en(fingerprint);

        if (rc)
        {
            hwlog_err("something wrong in extern_power_en  init error=%d\n", rc);
            rc = 0;
        }
    }

#if defined (CONFIG_TEE_TUI_HI3650)
    //when tui init, the fingerprint should know, then the fingerprint driver will not get gpio value
    //Because the gpio is set to secure status
    register_tui_driver(tui_fp_init, "fp", NULL);
#endif

    if (fingerprint->moduleID_gpio >= 0)
    {
        rc = finerprint_get_module_info(fingerprint);

        if (rc < 0)
        {
            hwlog_err("unknow vendor info error=%d\n", rc);
            goto exit;
        }
    }

    fingerprint->class = class_create(THIS_MODULE, FP_CLASS_NAME);


    rc = alloc_chrdev_region(&fingerprint->devno, 0, 1, FP_DEV_NAME);

    if (rc)
    {
        hwlog_err("%s alloc_chrdev_region failed rc=%d.\n", __func__, rc);
        goto exit;
    }

    fingerprint->device = device_create(fingerprint->class, NULL, fingerprint->devno,
                                        NULL, "%s", FP_DEV_NAME);

    cdev_init(&fingerprint->cdev, &fingerprint_fops);
    fingerprint->cdev.owner = THIS_MODULE;

    rc = cdev_add(&fingerprint->cdev, fingerprint->devno, 1);

    if (rc)
    {
        hwlog_err("%s cdev_add failed rc=%d.\n", __func__, rc);
        rc = -ENOMEM;
        goto exit;
    }

    fingerprint->input_dev = devm_input_allocate_device(dev);

    if (!fingerprint->input_dev)
    {
        hwlog_err("%s failed to allocate input device.\n", __func__);
        rc = -ENOMEM;
        goto exit;
    }


    fingerprint->input_dev->name = "fingerprint";
    /* Also register the key for wake up */
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_UP);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_DOWN);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_LEFT);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_RIGHT);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_HOLD);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_CLICK);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_HOLD);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_DCLICK);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_FINGER_UP);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_FINGER_DOWN);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_FINGER_IDENTIFY);
    input_set_capability(fingerprint->input_dev, EV_KEY, EVENT_IDENTIFY_END);
    set_bit(EV_KEY, fingerprint->input_dev->evbit);
    set_bit(EVENT_UP, fingerprint->input_dev->evbit);
    set_bit(EVENT_DOWN, fingerprint->input_dev->evbit);
    set_bit(EVENT_LEFT, fingerprint->input_dev->evbit);
    set_bit(EVENT_RIGHT, fingerprint->input_dev->evbit);
    set_bit(EVENT_CLICK, fingerprint->input_dev->evbit);
    set_bit(EVENT_HOLD, fingerprint->input_dev->evbit);
    set_bit(EVENT_DCLICK, fingerprint->input_dev->evbit);
    set_bit(EVENT_FINGER_UP, fingerprint->input_dev->evbit);
    set_bit(EVENT_FINGER_DOWN, fingerprint->input_dev->evbit);
    set_bit(EVENT_FINGER_IDENTIFY, fingerprint->input_dev->evbit);
    set_bit(EVENT_IDENTIFY_END, fingerprint->input_dev->evbit);

    rc = input_register_device(fingerprint->input_dev);

    if (rc)
    {
        hwlog_err("%s input_register_device failed rc=%d.\n", __func__, rc);
        goto exit;
    }


    fingerprint->wakeup_enabled = false;

    rc = sysfs_create_group(&fingerprint->pf_dev->dev.kobj, &attribute_group);
    if (rc)
     {
         hwlog_err("%s sysfs_create_group failed rc=%d.\n", __func__, rc);
         goto exit;
     }

    irqf = IRQF_TRIGGER_RISING | IRQF_ONESHOT | IRQF_NO_SUSPEND;

    device_init_wakeup(dev, 1);
    wake_lock_init(&fingerprint->ttw_wl, WAKE_LOCK_SUSPEND, "fpc_ttw_wl");

    mutex_init(&fingerprint->lock);
    rc = devm_request_threaded_irq(dev, gpio_to_irq(fingerprint->irq_gpio),
                                   NULL, fingerprint_irq_handler, irqf,
                                   dev_name(dev), fingerprint);

    if (rc)
    {
        hwlog_err("%s failed could not request irq %d.\n", __func__, gpio_to_irq(fingerprint->irq_gpio));
        goto exit;
    }

    //dev_dbg(dev, "requested irq %d\n", gpio_to_irq(fingerprint->irq_gpio));

    /* Request that the interrupt should be wakeable */
    enable_irq_wake(gpio_to_irq( fingerprint->irq_gpio ) );
    fingerprint->wakeup_enabled = true;
    fingerprint->nav_stat = 0;
    fingerprint->sensor_id = 0;

    g_fingerprint = fingerprint;
    hwlog_info("%s OK.\n", __func__);

exit:
   #if defined (CONFIG_HUAWEI_DSM)
    if (rc && !dsm_client_ocuppy(fingerprint_dclient))
    {
        dsm_client_record(fingerprint_dclient,"fingerprint_probe failed rc=%d!\n",rc);
        dsm_client_notify(fingerprint_dclient, DSM_FINGERPRINT_PROBE_FAIL_ERROR_NO);
    }
   #endif
    return rc;
}

static int fingerprint_remove(struct platform_device* pdev)
{
    int error = 0;
    struct  fp_data* fingerprint = dev_get_drvdata(&pdev->dev);
    if (NULL == fingerprint)
    {return -EINVAL;}
    error = strncmp(fingerprint->extern_ldo_name, FINGERPRINT_EXTERN_LDO_NAME, strlen(FINGERPRINT_EXTERN_LDO_NAME));
    if(error == 0)
    {
        error = regulator_disable(fp_ex_regulator);
        if(error < 0){
            pr_err("%s:regulator_disable fail, ret = %d \n", __func__, error);
        }
    }
    sysfs_remove_group(&fingerprint->pf_dev->dev.kobj, &attribute_group);
    cdev_del(&fingerprint->cdev);
    unregister_chrdev_region(fingerprint->devno, 1);
    input_free_device(fingerprint->input_dev);
    mutex_destroy(&fingerprint->lock);
    wake_lock_destroy(&fingerprint->ttw_wl);
    hwlog_info("%s\n", __func__);
    return 0;
}


/* -------------------------------------------------------------------- */
static int fingerprint_suspend(struct device* dev)
{
#if defined (CONFIG_HUAWEI_DSM)
    struct  fp_data* fingerprint = dev_get_drvdata(dev);
    if (NULL == fingerprint)
    {
        return -EINVAL;
    }
    if (!runmode_is_factory() && (fingerprint->navigation_adjust1 == NAVIGATION_ADJUST_REVERSE) && (0 != fingerprint->sensor_id)) // �����汾����ǰ��ָ�ƣ�ָ��hal����OK
    {
        if (!fingerprint->irq_enabled)
        {
            if (!dsm_client_ocuppy(fingerprint_dclient))
            {
                dsm_client_record(fingerprint_dclient,"irq not enable before suspend, adjust1=%d, irq_enabled=%d",fingerprint->navigation_adjust1, (int)fingerprint->irq_enabled);
                dsm_client_notify(fingerprint_dclient, DSM_FINGERPRINT_WAIT_FOR_FINGER_ERROR_NO);
            }
        }
    }
#endif
    hwlog_info("%s\n", __func__);
    return 0;
}
/* -------------------------------------------------------------------- */
static int fingerprint_resume(struct device* dev)
{
    hwlog_info("%s\n", __func__);
    return 0;
}

static const struct dev_pm_ops fingerprint_pm =
{
    .suspend = fingerprint_suspend,
    .resume = fingerprint_resume
};

static struct of_device_id fingerprint_of_match[] =
{
    { .compatible = "fpc,fingerprint", },
    {}
};
MODULE_DEVICE_TABLE(of, fingerprint_of_match);

static struct platform_driver fingerprint_driver =
{
    .driver = {
        .name = "fingerprint",
        .owner = THIS_MODULE,
        .of_match_table = fingerprint_of_match,
        .pm = &fingerprint_pm
    },
    .probe = fingerprint_probe,
    .remove = fingerprint_remove
};

static int __init fingerprint_init(void)
{
    int rc = platform_driver_register(&fingerprint_driver);

    if (!rc)
    { hwlog_info("%s OK\n", __func__); }
    else
    { hwlog_err("%s %d\n", __func__, rc); }

    return rc;
}

static void __exit fingerprint_exit(void)
{
    hwlog_info("%s\n", __func__);
    platform_driver_unregister(&fingerprint_driver);
#if defined (CONFIG_TEE_TUI)
    unregister_tui_driver("fp");
#endif
}

#ifdef CONFIG_LLT_TEST
struct LLT_fingprint_ops LLT_fingerprint = {
    .irq_get = irq_get,
    .fingerprint_irq_handler = fingerprint_irq_handler,
    .fingerprint_open = fingerprint_open,
    .fingerprint_get_irq_status = fingerprint_get_irq_status,
    .fingerprint_ioctl = fingerprint_ioctl,
    .fingerprint_reset_gpio_init = fingerprint_reset_gpio_init,
    .finerprint_get_module_info = finerprint_get_module_info,
    .fingerprint_probe = fingerprint_probe,
    .fingerprint_remove = fingerprint_remove,
};
#endif
/*lint -save -e* */
module_init(fingerprint_init);
module_exit(fingerprint_exit);
/*lint -restore*/
EXPORT_SYMBOL(tui_fp_notify);

MODULE_LICENSE("GPL v2");
