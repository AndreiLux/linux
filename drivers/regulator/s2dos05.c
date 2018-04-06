/*
 * s2dos05.c - Regulator driver for the Samsung s2dos05
 *
 * Copyright (C) 2016 Samsung Electronics
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

#include <linux/module.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/i2c.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/s2dos05.h>
#include <linux/regulator/of_regulator.h>
#include <linux/debugfs.h>

struct s2dos05_data {
	struct s2dos05_dev *iodev;
	int num_regulators;
	struct regulator_dev *rdev[S2DOS05_REGULATOR_MAX];
	int opmode[S2DOS05_REGULATOR_MAX];
};

int s2dos05_read_reg(struct i2c_client *i2c, u8 reg, u8 *dest)
{
	struct s2dos05_dev *s2dos05 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&s2dos05->i2c_lock);
	ret = i2c_smbus_read_byte_data(i2c, reg);
	mutex_unlock(&s2dos05->i2c_lock);
	if (ret < 0) {
		pr_info("%s:%s reg(0x%x), ret(%d)\n",
			 MFD_DEV_NAME, __func__, reg, ret);
		return ret;
	}

	ret &= 0xff;
	*dest = ret;
	return 0;
}

int s2dos05_bulk_read(struct i2c_client *i2c, u8 reg, int count, u8 *buf)
{
	struct s2dos05_dev *s2dos05 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&s2dos05->i2c_lock);
	ret = i2c_smbus_read_i2c_block_data(i2c, reg, count, buf);
	mutex_unlock(&s2dos05->i2c_lock);
	if (ret < 0)
		return ret;

	return 0;
}

int s2dos05_read_word(struct i2c_client *i2c, u8 reg)
{
	struct s2dos05_dev *s2dos05 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&s2dos05->i2c_lock);
	ret = i2c_smbus_read_word_data(i2c, reg);
	mutex_unlock(&s2dos05->i2c_lock);
	if (ret < 0)
		return ret;

	return ret;
}

int s2dos05_write_reg(struct i2c_client *i2c, u8 reg, u8 value)
{
	struct s2dos05_dev *s2dos05 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&s2dos05->i2c_lock);
	ret = i2c_smbus_write_byte_data(i2c, reg, value);
	mutex_unlock(&s2dos05->i2c_lock);
	if (ret < 0)
		pr_info("%s:%s reg(0x%x), ret(%d)\n",
				MFD_DEV_NAME, __func__, reg, ret);

	return ret;
}

int s2dos05_bulk_write(struct i2c_client *i2c, u8 reg, int count, u8 *buf)
{
	struct s2dos05_dev *s2dos05 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&s2dos05->i2c_lock);
	ret = i2c_smbus_write_i2c_block_data(i2c, reg, count, buf);
	mutex_unlock(&s2dos05->i2c_lock);
	if (ret < 0)
		return ret;

	return 0;
}

int s2dos05_update_reg(struct i2c_client *i2c, u8 reg, u8 val, u8 mask)
{
	struct s2dos05_dev *s2dos05 = i2c_get_clientdata(i2c);
	int ret;
	u8 old_val, new_val;

	mutex_lock(&s2dos05->i2c_lock);
	ret = i2c_smbus_read_byte_data(i2c, reg);
	if (ret >= 0) {
		old_val = ret & 0xff;
		new_val = (val & mask) | (old_val & (~mask));
		ret = i2c_smbus_write_byte_data(i2c, reg, new_val);
	}
	mutex_unlock(&s2dos05->i2c_lock);
	return ret;
}

#ifdef CONFIG_DEBUG_FS
static int s2dos05_regdump_show(struct seq_file *s, void *unused)
{
	struct s2dos05_dev *s2dos05_dev = s->private;
	u8 i, val = 0;

	for (i = S2DOS05_REG_STAT; i <= S2DOS05_REG_IRQ; i++) {
		s2dos05_read_reg(s2dos05_dev->i2c, i, &val);
		seq_printf(s, "0x%x: 0x%x\n", i, val);
	}

	return 0;
}

static int s2dos05_regdump_open(struct inode *inode, struct file *file)
{
	return single_open(file, s2dos05_regdump_show, inode->i_private);
}

static const struct file_operations s2dos05_regdump_operations = {
	.open           = s2dos05_regdump_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static void s2dos05_debugfs_init(struct s2dos05_dev *s2dos05)
{
	debugfs_create_file("s2dos05_regdump", 0440,
			NULL, s2dos05, &s2dos05_regdump_operations);
}
#endif

static int s2m_enable(struct regulator_dev *rdev)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;

	return s2dos05_update_reg(i2c, rdev->desc->enable_reg,
				  info->opmode[rdev_get_id(rdev)],
					rdev->desc->enable_mask);
}

static int s2m_disable_regmap(struct regulator_dev *rdev)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;
	u8 val;

	if (rdev->desc->enable_is_inverted)
		val = rdev->desc->enable_mask;
	else
		val = 0;

	return s2dos05_update_reg(i2c, rdev->desc->enable_reg,
				   val, rdev->desc->enable_mask);
}

static int s2m_is_enabled_regmap(struct regulator_dev *rdev)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;
	int ret;
	u8 val;

	ret = s2dos05_read_reg(i2c, rdev->desc->enable_reg, &val);
	if (ret < 0)
		return ret;

	if (rdev->desc->enable_is_inverted)
		return (val & rdev->desc->enable_mask) == 0;
	else
		return (val & rdev->desc->enable_mask) != 0;
}

static int s2m_get_voltage_sel_regmap(struct regulator_dev *rdev)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;
	int ret;
	u8 val;

	ret = s2dos05_read_reg(i2c, rdev->desc->vsel_reg, &val);
	if (ret < 0)
		return ret;

	val &= rdev->desc->vsel_mask;

	return val;
}

static int s2m_set_voltage_sel_regmap(struct regulator_dev *rdev,
					unsigned int sel)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;
	int ret;

	ret = s2dos05_update_reg(i2c, rdev->desc->vsel_reg,
				sel, rdev->desc->vsel_mask);
	if (ret < 0)
		goto out;

	if (rdev->desc->apply_bit)
		ret = s2dos05_update_reg(i2c, rdev->desc->apply_reg,
					 rdev->desc->apply_bit,
					 rdev->desc->apply_bit);
	return ret;
out:
	pr_warn("%s: failed to set voltage_sel_regmap\n", rdev->desc->name);
	return ret;
}

static int s2m_set_voltage_sel_regmap_buck(struct regulator_dev *rdev,
					unsigned int sel)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;
	int ret;

	ret = s2dos05_write_reg(i2c, rdev->desc->vsel_reg, sel);
	if (ret < 0)
		goto out;

	if (rdev->desc->apply_bit)
		ret = s2dos05_update_reg(i2c, rdev->desc->apply_reg,
					 rdev->desc->apply_bit,
					 rdev->desc->apply_bit);
	return ret;
out:
	pr_warn("%s: failed to set voltage_sel_regmap\n", rdev->desc->name);
	return ret;
}

static int s2m_set_voltage_time_sel(struct regulator_dev *rdev,
				   unsigned int old_selector,
				   unsigned int new_selector)
{
	int old_volt, new_volt;

	/* sanity check */
	if (!rdev->desc->ops->list_voltage)
		return -EINVAL;

	old_volt = rdev->desc->ops->list_voltage(rdev, old_selector);
	new_volt = rdev->desc->ops->list_voltage(rdev, new_selector);

	if (old_selector < new_selector)
		return DIV_ROUND_UP(new_volt - old_volt, S2DOS05_RAMP_DELAY);

	return 0;
}

static int s2m_set_active_discharge(struct regulator_dev *rdev,
					bool enable)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;
	int ret;
	u8 val;

	if (enable)
		val = rdev->desc->active_discharge_on;
	else
		val = rdev->desc->active_discharge_off;

	ret = s2dos05_update_reg(i2c, rdev->desc->active_discharge_reg,
				val, rdev->desc->active_discharge_mask);
	return ret;
}

#ifdef CONFIG_SEC_PM
static int s2m_set_elvss_short_detection(struct regulator_dev *rdev,
					 bool enable, int lv_uA)
{
	struct s2dos05_data *info = rdev_get_drvdata(rdev);
	struct i2c_client *i2c = info->iodev->i2c;
	int ret = -ENODEV;
	u8 lv = 0, val, mask;

	if (enable) {
		lv = lv_uA / 2000 - 1;

		if (lv > 3)
			return -EINVAL;
	}

	val = (!enable << 3) | (lv << 5);
	mask = S2DOS05_ELVSS_SSD_EN_MASK | S2DOS05_ELVSS_SEL_SSD_MASK;

	ret = s2dos05_update_reg(i2c, S2DOS05_REG_SSD_TSD, val, mask);
	return ret;
}
#endif /* CONFIG_SEC_PM */

static struct regulator_ops s2dos05_ldo_ops = {
	.list_voltage		= regulator_list_voltage_linear,
	.map_voltage		= regulator_map_voltage_linear,
	.is_enabled		= s2m_is_enabled_regmap,
	.enable			= s2m_enable,
	.disable		= s2m_disable_regmap,
	.get_voltage_sel	= s2m_get_voltage_sel_regmap,
	.set_voltage_sel	= s2m_set_voltage_sel_regmap,
	.set_voltage_time_sel	= s2m_set_voltage_time_sel,
	.set_active_discharge	= s2m_set_active_discharge,
};

static struct regulator_ops s2dos05_buck_ops = {
	.list_voltage		= regulator_list_voltage_linear,
	.map_voltage		= regulator_map_voltage_linear,
	.is_enabled		= s2m_is_enabled_regmap,
	.enable			= s2m_enable,
	.disable		= s2m_disable_regmap,
	.get_voltage_sel	= s2m_get_voltage_sel_regmap,
	.set_voltage_sel	= s2m_set_voltage_sel_regmap_buck,
	.set_voltage_time_sel	= s2m_set_voltage_time_sel,
	.set_active_discharge	= s2m_set_active_discharge,
};

#ifdef CONFIG_SEC_PM
static struct regulator_ops s2dos05_elvss_ops = {
	.set_short_detection	= s2m_set_elvss_short_detection,
};
#endif

#define _BUCK(macro)	S2DOS05_BUCK##macro
#define _buck_ops(num)	s2dos05_buck_ops##num

#define _LDO(macro)	S2DOS05_LDO##macro
#define _REG(ctrl)	S2DOS05_REG##ctrl
#define _ldo_ops(num)	s2dos05_ldo_ops##num
#define _MASK(macro)	S2DOS05_ENABLE_MASK##macro
#define _TIME(macro)	S2DOS05_ENABLE_TIME##macro

#define BUCK_DESC(_name, _id, _ops, m, s, v, e, em, t, a) {	\
	.name		= _name,				\
	.id		= _id,					\
	.ops		= _ops,					\
	.type		= REGULATOR_VOLTAGE,			\
	.owner		= THIS_MODULE,				\
	.min_uV		= m,					\
	.uV_step	= s,					\
	.n_voltages	= S2DOS05_BUCK_N_VOLTAGES,		\
	.vsel_reg	= v,					\
	.vsel_mask	= S2DOS05_BUCK_VSEL_MASK,		\
	.enable_reg	= e,					\
	.enable_mask	= em,					\
	.enable_time	= t,					\
	.active_discharge_off = 0,				\
	.active_discharge_on = S2DOS05_BUCK_FD_MASK,		\
	.active_discharge_reg	= a,				\
	.active_discharge_mask	= S2DOS05_BUCK_FD_MASK		\
}

#define LDO_DESC(_name, _id, _ops, m, s, v, e, em, t, a) {	\
	.name		= _name,				\
	.id		= _id,					\
	.ops		= _ops,					\
	.type		= REGULATOR_VOLTAGE,			\
	.owner		= THIS_MODULE,				\
	.min_uV		= m,					\
	.uV_step	= s,					\
	.n_voltages	= S2DOS05_LDO_N_VOLTAGES,		\
	.vsel_reg	= v,					\
	.vsel_mask	= S2DOS05_LDO_VSEL_MASK,		\
	.enable_reg	= e,					\
	.enable_mask	= em,					\
	.enable_time	= t,					\
	.active_discharge_off = 0,				\
	.active_discharge_on = S2DOS05_LDO_FD_MASK,		\
	.active_discharge_reg	= a,				\
	.active_discharge_mask	= S2DOS05_LDO_FD_MASK		\
}

#ifdef CONFIG_SEC_PM
#define ELVSS_DESC(_name, _id)				{	\
	.name		= _name,				\
	.id		= _id,					\
	.ops		= &s2dos05_elvss_ops,			\
	.type		= REGULATOR_CURRENT,			\
	.owner		= THIS_MODULE,				\
}
#endif

static struct regulator_desc regulators[S2DOS05_REGULATOR_MAX] = {
		/* name, id, ops, min_uv, uV_step, vsel_reg, enable_reg */
		LDO_DESC("s2dos05-ldo1", _LDO(1), &_ldo_ops(), _LDO(_MIN1),
			_LDO(_STEP1), _REG(_LDO1_CFG),
			_REG(_EN), _MASK(_L1), _TIME(_LDO), _REG(_LDO1_CFG)),
		LDO_DESC("s2dos05-ldo2", _LDO(2), &_ldo_ops(), _LDO(_MIN1),
			_LDO(_STEP1), _REG(_LDO2_CFG),
			_REG(_EN), _MASK(_L2), _TIME(_LDO), _REG(_LDO2_CFG)),
		LDO_DESC("s2dos05-ldo3", _LDO(3), &_ldo_ops(), _LDO(_MIN2),
			_LDO(_STEP1), _REG(_LDO3_CFG),
			_REG(_EN), _MASK(_L3), _TIME(_LDO), _REG(_LDO3_CFG)),
		LDO_DESC("s2dos05-ldo4", _LDO(4), &_ldo_ops(), _LDO(_MIN2),
			_LDO(_STEP1), _REG(_LDO4_CFG),
			_REG(_EN), _MASK(_L4), _TIME(_LDO), _REG(_LDO4_CFG)),
		BUCK_DESC("s2dos05-buck1", _BUCK(1), &_buck_ops(), _BUCK(_MIN1),
			_BUCK(_STEP1), _REG(_BUCK_VOUT),
			_REG(_EN), _MASK(_B1), _TIME(_BUCK), _REG(_BUCK_CFG)),
#ifdef CONFIG_SEC_PM
		ELVSS_DESC("s2dos05-elvss", S2DOS05_ELVSS),
#endif
};

static irqreturn_t s2dos05_irq_thread(int irq, void *irq_data)
{
	struct s2dos05_data *s2dos05 = irq_data;
	u8 val = 0;
#ifdef CONFIG_SEC_PM_DEBUG
	const char *irq_bit[] = { "OCD", "UVLO", "SCP", "SSD", "TSD", "PWRMT" };
	char irq_name[32];
	ssize_t ret = 0;
	size_t i;
#endif /* CONFIG_SEC_PM_DEBUG */


	s2dos05_read_reg(s2dos05->iodev->i2c, S2DOS05_REG_IRQ, &val);
	pr_info("%s:irq(%d) S2DOS05_REG_IRQ : 0x%x\n", __func__, irq, val);
#ifdef CONFIG_SEC_PM_DEBUG
	for (i = 0; i < ARRAY_SIZE(irq_bit); i++)
		if (val & (1 << i))
			ret += sprintf(irq_name + ret, " %s", irq_bit[i]);

	pr_info("%s: IRQ:%s\n", __func__, irq_name);
#endif /* CONFIG_SEC_PM_DEBUG */

	return IRQ_HANDLED;
}

#ifdef CONFIG_OF
static int s2dos05_pmic_dt_parse_pdata(struct device *dev,
					struct s2dos05_platform_data *pdata)
{
	struct device_node *pmic_np, *regulators_np, *reg_np;
	struct s2dos05_regulator_data *rdata;
	unsigned int i;
	int ret;
	u32 val;

	pmic_np = dev->of_node;
	if (!pmic_np) {
		dev_err(dev, "could not find pmic sub-node\n");
		return -ENODEV;
	}

	pdata->dp_irq = of_get_named_gpio(pmic_np, "s2dos05,s2dos05_int", 0);
	if (pdata->dp_irq < 0)
		pr_err("%s error reading s2dos05_irq = %d\n",
			__func__, pdata->dp_irq);

	pdata->wakeup = of_property_read_bool(pmic_np, "s2dos05,wakeup");

	pdata->adc_mode = 0;
	ret = of_property_read_u32(pmic_np, "adc_mode", &val);
	if (ret)
		return -EINVAL;
	pdata->adc_mode = val;

	pdata->adc_sync_mode = 0;
	ret = of_property_read_u32(pmic_np, "adc_sync_mode", &val);
	if (ret)
		return -EINVAL;
	pdata->adc_sync_mode = val;

	regulators_np = of_find_node_by_name(pmic_np, "regulators");
	if (!regulators_np) {
		dev_err(dev, "could not find regulators sub-node\n");
		return -EINVAL;
	}

	/* count the number of regulators to be supported in pmic */
	pdata->num_regulators = 0;
	for_each_child_of_node(regulators_np, reg_np) {
		pdata->num_regulators++;
	}

	rdata = devm_kzalloc(dev, sizeof(*rdata) *
				pdata->num_regulators, GFP_KERNEL);
	if (!rdata) {
		dev_err(dev,
			"could not allocate memory for regulator data\n");
		return -ENOMEM;
	}

	pdata->regulators = rdata;
	for_each_child_of_node(regulators_np, reg_np) {
		for (i = 0; i < ARRAY_SIZE(regulators); i++)
			if (!of_node_cmp(reg_np->name,
					regulators[i].name))
				break;

		if (i == ARRAY_SIZE(regulators)) {
			dev_warn(dev,
			"don't know how to configure regulator %s\n",
			reg_np->name);
			continue;
		}

		rdata->id = i;
		rdata->initdata = of_get_regulator_init_data(
						dev, reg_np,
						&regulators[i]);
		rdata->reg_node = reg_np;
		rdata++;
	}
	of_node_put(regulators_np);

	return 0;
}
#else
static int s2dos05_pmic_dt_parse_pdata(struct s2dos05_dev *iodev,
					struct s2dos05_platform_data *pdata)
{
	return 0;
}
#endif /* CONFIG_OF */

static int s2dos05_pmic_probe(struct i2c_client *i2c,
				const struct i2c_device_id *dev_id)
{
	struct s2dos05_dev *iodev;
	struct s2dos05_platform_data *pdata = i2c->dev.platform_data;
	struct regulator_config config = { };
	struct s2dos05_data *s2dos05;
	int i;
	int ret = 0;
	u8 val = 0, mask = 0;

	pr_info("%s:%s\n", MFD_DEV_NAME, __func__);

	iodev = kzalloc(sizeof(struct s2dos05_dev), GFP_KERNEL);
	if (!iodev) {
		dev_err(&i2c->dev, "%s: Failed to alloc mem for s2dos05\n",
							__func__);
		return -ENOMEM;
	}

	if (i2c->dev.of_node) {
		pdata = devm_kzalloc(&i2c->dev,
			sizeof(struct s2dos05_platform_data), GFP_KERNEL);
		if (!pdata) {
			dev_err(&i2c->dev, "Failed to allocate memory\n");
			ret = -ENOMEM;
			goto err_pdata;
		}
		ret = s2dos05_pmic_dt_parse_pdata(&i2c->dev, pdata);
		if (ret < 0) {
			dev_err(&i2c->dev, "Failed to get device of_node\n");
			goto err_dt;
		}

		i2c->dev.platform_data = pdata;
	} else
		pdata = i2c->dev.platform_data;

	iodev->dev = &i2c->dev;
	iodev->i2c = i2c;

	if (pdata) {
		iodev->pdata = pdata;
		iodev->wakeup = pdata->wakeup;
	} else {
		ret = -EINVAL;
		goto err_dt;
	}
	mutex_init(&iodev->i2c_lock);
	i2c_set_clientdata(i2c, iodev);

	s2dos05 = devm_kzalloc(&i2c->dev, sizeof(struct s2dos05_data),
				GFP_KERNEL);
	if (!s2dos05) {
		pr_info("[%s:%d] if (!s2dos05)\n", __FILE__, __LINE__);
		ret = -EINVAL;
		goto err_data;
	}

	s2dos05->iodev = iodev;
	s2dos05->num_regulators = pdata->num_regulators;

	for (i = 0; i < pdata->num_regulators; i++) {
		int id = pdata->regulators[i].id;

		config.dev = &i2c->dev;
		config.init_data = pdata->regulators[i].initdata;
		config.driver_data = s2dos05;
		config.of_node = pdata->regulators[i].reg_node;
		s2dos05->opmode[id] = regulators[id].enable_mask;
		s2dos05->rdev[i] = regulator_register(&regulators[id], &config);
		if (IS_ERR(s2dos05->rdev[i])) {
			ret = PTR_ERR(s2dos05->rdev[i]);
			dev_err(&i2c->dev, "regulator init failed for %d\n",
				id);
			s2dos05->rdev[i] = NULL;
			goto err_rdata;
		}
	}

	iodev->adc_mode = pdata->adc_mode;
	iodev->adc_sync_mode = pdata->adc_sync_mode;
	if (iodev->adc_mode > 0)
		s2dos05_powermeter_init(iodev);

	/* Enable SSD, SCP interrupt */
	val = (S2DOS05_IRQ_PWRMT_MASK | S2DOS05_IRQ_TSD_MASK
			| S2DOS05_IRQ_UVLO_MASK | S2DOS05_IRQ_OCD_MASK);
	mask = (S2DOS05_IRQ_PWRMT_MASK | S2DOS05_IRQ_TSD_MASK | S2DOS05_IRQ_SSD_MASK
		| S2DOS05_IRQ_SCP_MASK | S2DOS05_IRQ_UVLO_MASK | S2DOS05_IRQ_OCD_MASK);
	ret = s2dos05_update_reg(iodev->i2c, S2DOS05_REG_IRQ_MASK, val, mask);
	if (ret < 0) {
		dev_err(&i2c->dev, "Failed to mask IRQ MASK address\n");
		return ret;
	}

	if (pdata->dp_irq > 0) {
		iodev->dp_irq = gpio_to_irq(pdata->dp_irq);
		pr_info("%s : dp_irq = %d\n", __func__, iodev->dp_irq);
		if (iodev->dp_irq > 0) {
			ret = request_threaded_irq(iodev->dp_irq,
					NULL, s2dos05_irq_thread,
					IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
					"dp-pmic-irq", s2dos05);
			if (ret) {
				dev_err(&i2c->dev,
						"%s: Failed to Request IRQ\n", __func__);
				goto err_rdata;
			}

			ret = enable_irq_wake(iodev->dp_irq);
			if (ret < 0)
				dev_err(&i2c->dev,
						"%s: Failed to Enable Wakeup Source(%d)\n",
						__func__, ret);
		} else {
			dev_err(&i2c->dev, "%s: Failed gpio_to_irq(%d)\n",
					__func__, iodev->dp_irq);
			goto err_rdata;
		}
	}

#ifdef CONFIG_DEBUG_FS
	s2dos05_debugfs_init(iodev);
#endif

	return ret;

err_rdata:
	pr_info("[%s:%d] err:\n", __FILE__, __LINE__);
	for (i = 0; i < s2dos05->num_regulators; i++)
		if (s2dos05->rdev[i])
			regulator_unregister(s2dos05->rdev[i]);
err_data:
	mutex_destroy(&iodev->i2c_lock);
	devm_kfree(&i2c->dev, (void *)s2dos05);
err_dt:
	devm_kfree(&i2c->dev, (void *)pdata);
err_pdata:
	kfree(iodev);

	return ret;
}

#if defined(CONFIG_OF)
static const struct of_device_id s2dos05_i2c_dt_ids[] = {
	{ .compatible = "samsung,s2dos05pmic" },
	{ },
};
#endif /* CONFIG_OF */

static int s2dos05_pmic_remove(struct i2c_client *i2c)
{
	struct s2dos05_data *s2dos05 = i2c_get_clientdata(i2c);
	int i;

	dev_info(&i2c->dev, "%s\n", __func__);
	for (i = 0; i < s2dos05->num_regulators; i++)
		if (s2dos05->rdev[i])
			regulator_unregister(s2dos05->rdev[i]);

	s2dos05_powermeter_deinit(s2dos05->iodev);

	return 0;
}

#if defined(CONFIG_PM)
static int s2dos05_pmic_suspend(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct s2dos05_dev *s2dos05 = platform_get_drvdata(pdev);

	pr_info("%s adc_mode : %d\n", __func__, s2dos05->adc_mode);
	if (s2dos05->adc_mode > 0) {
		s2dos05_read_reg(s2dos05->i2c, S2DOS05_REG_PWRMT_CTRL2, &s2dos05->adc_en_val);
		if (s2dos05->adc_en_val & 0x80)
			s2dos05_update_reg(s2dos05->i2c, S2DOS05_REG_PWRMT_CTRL2, 0, ADC_EN_MASK);
	}
	return 0;
}

static int s2dos05_pmic_resume(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct s2dos05_dev *s2dos05 = platform_get_drvdata(pdev);
	int ret = 0;

	pr_info("%s adc_mode : %d\n", __func__, s2dos05->adc_mode);

	if (s2dos05->adc_mode > 0) {
		ret = s2dos05_update_reg(s2dos05->i2c, S2DOS05_REG_PWRMT_CTRL2,
				s2dos05->adc_en_val & 0x80, ADC_EN_MASK);
		if (ret < 0)
			pr_err("%s s2dos05_update_reg is fail(%d)!!\n", __func__, ret);
		else
			pr_info("%s s2dos05_update_reg\n", __func__);
	}
	return 0;
}
#else
#define s2dos05_pmic_suspend	NULL
#define s2dos05_pmic_resume	NULL
#endif /* CONFIG_PM */

const struct dev_pm_ops s2dos05_pmic_pm = {
	.suspend = s2dos05_pmic_suspend,
	.resume = s2dos05_pmic_resume,
};

#if defined(CONFIG_OF)
static const struct i2c_device_id s2dos05_pmic_id[] = {
	{"s2dos05-regulator", 0},
	{},
};
#endif

static struct i2c_driver s2dos05_i2c_driver = {
	.driver = {
		.name = "s2dos05-regulator",
		.owner = THIS_MODULE,
		.pm = &s2dos05_pmic_pm,
#if defined(CONFIG_OF)
		.of_match_table	= s2dos05_i2c_dt_ids,
#endif /* CONFIG_OF */
		.suppress_bind_attrs = true,
	},
	.probe = s2dos05_pmic_probe,
	.remove = s2dos05_pmic_remove,
	.id_table = s2dos05_pmic_id,
};

static int __init s2dos05_i2c_init(void)
{
	pr_info("%s:%s\n", MFD_DEV_NAME, __func__);
	return i2c_add_driver(&s2dos05_i2c_driver);
}
subsys_initcall(s2dos05_i2c_init);

static void __exit s2dos05_i2c_exit(void)
{
	i2c_del_driver(&s2dos05_i2c_driver);
}
module_exit(s2dos05_i2c_exit);

MODULE_DESCRIPTION("SAMSUNG s2dos05 Regulator Driver");
MODULE_LICENSE("GPL");
