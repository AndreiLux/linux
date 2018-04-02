#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <soc/samsung/cal-if.h>

#include "fvmap.h"
#include "cmucal.h"
#include "vclk.h"
#include "ra.h"

#define FVMAP_SIZE		(SZ_8K)
#define STEP_UV			(6250)

void __iomem *fvmap_base;
void __iomem *sram_fvmap_base;

static int init_margin_table[MAX_MARGIN_ID];
static int volt_offset_percent = 0;
#if defined(CONFIG_SAMSUNG_VST_CAL)
static int volt_vst_cal;
int volt_vst_cal_bdata;
EXPORT_SYMBOL(volt_vst_cal_bdata);
#endif
static int percent_margin_table[MAX_MARGIN_ID];

static int __init get_mif_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_MIF] = volt;

	return 0;
}
early_param("mif", get_mif_volt);

static int __init get_int_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_INT] = volt;

	return 0;
}
early_param("int", get_int_volt);

static int __init get_big_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_BIG] = volt;

	return 0;
}
early_param("big", get_big_volt);

static int __init get_lit_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_LIT] = volt;

	return 0;
}
early_param("lit", get_lit_volt);

static int __init get_g3d_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_G3D] = volt;

	return 0;
}
early_param("g3d", get_g3d_volt);

static int __init get_intcam_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_INTCAM] = volt;

	return 0;
}
early_param("intcam", get_intcam_volt);

static int __init get_cam_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_CAM] = volt;

	return 0;
}
early_param("cam", get_cam_volt);

static int __init get_disp_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_DISP] = volt;

	return 0;
}
early_param("disp", get_disp_volt);

static int __init get_g3dm_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_G3DM] = volt;

	return 0;
}
early_param("g3dm", get_g3dm_volt);

static int __init get_cp_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_CP] = volt;

	return 0;
}
early_param("cp", get_cp_volt);

static int __init get_fsys0_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_FSYS0] = volt;

	return 0;
}
early_param("fsys0", get_fsys0_volt);

static int __init get_aud_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_AUD] = volt;

	return 0;
}
early_param("aud", get_aud_volt);

static int __init get_iva_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_IVA] = volt;

	return 0;
}
early_param("iva", get_iva_volt);

static int __init get_score_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_SCORE] = volt;

	return 0;
}
early_param("score", get_score_volt);

static int __init get_percent_margin_volt(char *str)
{
	int percent;

	get_option(&str, &percent);
	volt_offset_percent = percent;

	return 0;
}
early_param("volt_offset_percent", get_percent_margin_volt);

#if defined(CONFIG_SAMSUNG_VST_CAL)
static int __init get_vst_margin_volt(char *str)
{
	unsigned int vst_volt;

	get_option(&str, &vst_volt);
	volt_vst_cal = vst_volt;

	return 0;
}
early_param("vst_adjust", get_vst_margin_volt);
#endif

int fvmap_set_raw_voltage_table(unsigned int id, int uV)
{
	struct fvmap_header *fvmap_header;
	struct rate_volt_header *fv_table;
	int num_of_lv;
	int idx, i;

	idx = GET_IDX(id);

	fvmap_header = sram_fvmap_base;
	fv_table = sram_fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		fv_table->table[i].volt += uV;

	return 0;
}

int fvmap_get_voltage_table(unsigned int id, unsigned int *table)
{
	struct fvmap_header *fvmap_header = fvmap_base;
	struct rate_volt_header *fv_table;
	int idx, i;
	int num_of_lv;

	if (!IS_ACPM_VCLK(id))
		return 0;

	idx = GET_IDX(id);

	fvmap_header = fvmap_base;
	fv_table = fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		table[i] = fv_table->table[i].volt;

	return num_of_lv;

}

int fvmap_get_raw_voltage_table(unsigned int id)
{
	struct fvmap_header *fvmap_header;
	struct rate_volt_header *fv_table;
	int idx, i;
	int num_of_lv;
	unsigned int table[20];

	idx = GET_IDX(id);

	fvmap_header = sram_fvmap_base;
	fv_table = sram_fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		table[i] = fv_table->table[i].volt;

	for (i = 0; i < num_of_lv; i++)
		printk("dvfs id : %d  %d Khz : %d uv\n", ACPM_VCLK_TYPE | id, fv_table->table[i].rate, table[i]);

	return 0;
}

static void check_percent_margin(struct rate_volt_header *head, unsigned int num_of_lv)
{
	int org_volt;
	int percent_volt;
	int i;

	if (!volt_offset_percent)
		return;

	for (i = 0; i < num_of_lv; i++) {
		org_volt = head->table[i].volt;
		percent_volt = org_volt * volt_offset_percent / 100;
		head->table[i].volt = org_volt + rounddown(percent_volt, STEP_UV);
	}
}

#if defined(CONFIG_SAMSUNG_VST_CAL)
static void check_vst_margin(void)
{
	int i;
	int bit = 1;
	int tmp = 0;
	int crc = 0;
	int magic = 0;
	int margin = volt_vst_cal;

	/* initialize vst cal bdata before using it */
	volt_vst_cal_bdata = 0;

	if (!volt_vst_cal) {
		pr_info("[svst] no vst cal\n");
		return;
	}

	/* check magic */
	magic = margin & 0xffffffc0;
	if ((magic >> 6) != NAD_CRC_MAGIC) {
		pr_info("[svst] invalid magic = %d\n", magic);
		return;
	}

	/* check crc */
	crc = margin & 0x3;
	margin = margin >> 2;

	for (i = 0; i < 30; i++) {
		if (bit & margin)
			tmp++;
		bit = bit << 1;
	}
	tmp = (tmp % 4);
	if (tmp != 0)
		tmp = 4 - tmp;

	pr_info("[svst] crc = %d, tmp = %d\n", crc, tmp);

	if (crc != tmp) {
		pr_info("[svst] broken crc\n");
		return;
	}

	margin = margin & 0xf;
	volt_vst_cal_bdata = margin;
	pr_info("[svst] result = %d, bdata = %d\n", margin, volt_vst_cal_bdata);

	bit = 1;
	for (i = 0; i < 4; i++) {
		if (bit & margin) {
			if (i == 0) {
				pr_info("[svst] big[%d]\n", NAD_VST_CAL_VOLT);
				init_margin_table[MARGIN_BIG] += NAD_VST_CAL_VOLT;
			} else if (i == 1) {
				pr_info("[svst] lit[%d]\n", NAD_VST_CAL_VOLT);
				init_margin_table[MARGIN_LIT] += NAD_VST_CAL_VOLT;
			} else if (i == 2) {
				pr_info("[svst] g3d[%d]\n", NAD_VST_CAL_VOLT);
				init_margin_table[MARGIN_G3D] += NAD_VST_CAL_VOLT;
			} else {
				pr_info("[svst] mif[%d]\n", NAD_VST_CAL_VOLT);
				init_margin_table[MARGIN_MIF] += NAD_VST_CAL_VOLT;
			}
		}
		bit = bit << 1;
	}
}
#endif
static int get_vclk_id_from_margin_id(int margin_id)
{
	int size = cmucal_get_list_size(ACPM_VCLK_TYPE);
	int i;
	struct vclk *vclk;

	for (i = 0; i < size; i++) {
		vclk = cmucal_get_node(ACPM_VCLK_TYPE | i);

		if (vclk->margin_id == margin_id)
			return i;
	}

	return -EINVAL;
}

#define attr_percent(margin_id, type)								\
static ssize_t show_##type##_percent								\
(struct kobject *kobj, struct kobj_attribute *attr, char *buf)					\
{												\
	return snprintf(buf, PAGE_SIZE, "%d\n", percent_margin_table[margin_id]);		\
}												\
												\
static ssize_t store_##type##_percent								\
(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)		\
{												\
	int input, vclk_id;									\
												\
	if (!sscanf(buf, "%d", &input))								\
		return -EINVAL;									\
												\
	if (input < -100 || input > 100)							\
		return -EINVAL;									\
												\
	vclk_id = get_vclk_id_from_margin_id(margin_id);					\
	if (vclk_id == -EINVAL)									\
		return vclk_id;									\
	percent_margin_table[margin_id] = input;						\
	cal_dfs_set_volt_margin(vclk_id | ACPM_VCLK_TYPE, input);				\
												\
	return count;										\
}												\
												\
static struct kobj_attribute type##_percent =							\
__ATTR(type##_percent, 0600,									\
	show_##type##_percent, store_##type##_percent)

attr_percent(MARGIN_MIF, mif_margin);
attr_percent(MARGIN_INT, int_margin);
attr_percent(MARGIN_BIG, big_margin);
attr_percent(MARGIN_LIT, lit_margin);
attr_percent(MARGIN_G3D, g3d_margin);
attr_percent(MARGIN_INTCAM, intcam_margin);
attr_percent(MARGIN_CAM, cam_margin);
attr_percent(MARGIN_DISP, disp_margin);
attr_percent(MARGIN_CP, cp_margin);
attr_percent(MARGIN_FSYS0, fsys0_margin);
attr_percent(MARGIN_AUD, aud_margin);
attr_percent(MARGIN_IVA, iva_margin);
attr_percent(MARGIN_SCORE, score_margin);

static struct attribute *percent_margin_attrs[] = {
	&mif_margin_percent.attr,
	&int_margin_percent.attr,
	&big_margin_percent.attr,
	&lit_margin_percent.attr,
	&g3d_margin_percent.attr,
	&intcam_margin_percent.attr,
	&cam_margin_percent.attr,
	&disp_margin_percent.attr,
	&cp_margin_percent.attr,
	&fsys0_margin_percent.attr,
	&aud_margin_percent.attr,
	&iva_margin_percent.attr,
	&score_margin_percent.attr,
	NULL,
};

static const struct attribute_group percent_margin_group = {
	.attrs = percent_margin_attrs,
};

static void fvmap_copy_from_sram(void __iomem *map_base, void __iomem *sram_base)
{
	struct fvmap_header *fvmap_header, *header;
	struct rate_volt_header *old, *new;
	struct clocks *clks;
	struct pll_header *plls;
	struct vclk *vclk;
	struct cmucal_clk *clk_node;
	unsigned int paddr_offset, fvaddr_offset;
	int size, margin;
	int i, j;

	fvmap_header = map_base;
	header = sram_base;

	size = cmucal_get_list_size(ACPM_VCLK_TYPE);

#if defined(CONFIG_SAMSUNG_VST_CAL)
	check_vst_margin();
#endif

	for (i = 0; i < size; i++) {
		/* load fvmap info */
		fvmap_header[i].dvfs_type = header[i].dvfs_type;
		fvmap_header[i].num_of_lv = header[i].num_of_lv;
		fvmap_header[i].num_of_members = header[i].num_of_members;
		fvmap_header[i].num_of_pll = header[i].num_of_pll;
		fvmap_header[i].num_of_mux = header[i].num_of_mux;
		fvmap_header[i].num_of_div = header[i].num_of_div;
		fvmap_header[i].gearratio = header[i].gearratio;
		fvmap_header[i].init_lv = header[i].init_lv;
		fvmap_header[i].num_of_gate = header[i].num_of_gate;
		fvmap_header[i].reserved[0] = header[i].reserved[0];
		fvmap_header[i].reserved[1] = header[i].reserved[1];
		fvmap_header[i].block_addr[0] = header[i].block_addr[0];
		fvmap_header[i].block_addr[1] = header[i].block_addr[1];
		fvmap_header[i].block_addr[2] = header[i].block_addr[2];
		fvmap_header[i].o_members = header[i].o_members;
		fvmap_header[i].o_ratevolt = header[i].o_ratevolt;
		fvmap_header[i].o_tables = header[i].o_tables;

		vclk = cmucal_get_node(ACPM_VCLK_TYPE | i);
		if (vclk == NULL)
			continue;
		pr_info("dvfs_type : %s - id : %x\n",
			vclk->name, fvmap_header[i].dvfs_type);
		pr_info("  num_of_lv      : %d\n", fvmap_header[i].num_of_lv);
		pr_info("  num_of_members : %d\n", fvmap_header[i].num_of_members);

		old = sram_base + fvmap_header[i].o_ratevolt;
		new = map_base + fvmap_header[i].o_ratevolt;

		check_percent_margin(old, fvmap_header[i].num_of_lv);

		margin = init_margin_table[vclk->margin_id];
		if (margin)
			cal_dfs_set_volt_margin(i | ACPM_VCLK_TYPE, margin);

		for (j = 0; j < fvmap_header[i].num_of_lv; j++) {
			new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) \n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
		}

		for (j = 0; j < fvmap_header[i].num_of_pll; j++) {
			clks = sram_base + fvmap_header[i].o_members;
			plls = sram_base + clks->addr[j];
			clk_node = cmucal_get_node(vclk->list[j]);
			if (clk_node == NULL)
				continue;
			paddr_offset = clk_node->paddr & 0xFFFF;
			fvaddr_offset = plls->addr & 0xFFFF;
			if (paddr_offset == fvaddr_offset)
				continue;

			clk_node->paddr += fvaddr_offset - paddr_offset;
			clk_node->pll_con0 += fvaddr_offset - paddr_offset;
			if (clk_node->pll_con1)
				clk_node->pll_con1 += (unsigned long long)(fvaddr_offset - paddr_offset);
		}
	}
}

int fvmap_init(void __iomem *sram_base)
{
	void __iomem *map_base;
	struct kobject *kobj;

	map_base = kzalloc(FVMAP_SIZE, GFP_KERNEL);

	fvmap_base = map_base;
	sram_fvmap_base = sram_base;
	pr_info("%s:fvmap initialize %p\n", __func__, sram_base);
	fvmap_copy_from_sram(map_base, sram_base);

	/* percent margin for each doamin at runtime */
	kobj = kobject_create_and_add("percent_margin", power_kobj);
	if (!kobj)
		pr_err("Fail to create percent_margin kboject\n");

	if (sysfs_create_group(kobj, &percent_margin_group))
		pr_err("Fail to create percent_margin group\n");

	return 0;
}
