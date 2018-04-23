#ifndef __VDEC_REGULATOR_H__
#define __VDEC_REGULATOR_H__

#include "hi_type.h"
#include "vfmw_dts.h"

typedef enum {
	VDEC_CLK_RATE_LOW = 0,
	VDEC_CLK_RATE_NORMAL,
	VDEC_CLK_RATE_HIGH,
	VDEC_CLK_RATE_BUTT,
}CLK_RATE_E;

#ifdef PLATFORM_KIRIN970
static const struct of_device_id Hisi_Vdec_Match_Table[] = {
	{.compatible = "hisi,kirin970-vdec",},
	{ }
};
#endif

HI_S32  VDEC_Regulator_Probe(struct device *dev);
HI_S32  VDEC_Regulator_Remove(struct device *dev);
HI_S32  VDEC_Regulator_Enable(HI_VOID);
HI_S32  VDEC_Regulator_Disable(HI_VOID);
HI_S32  VDEC_Regulator_GetClkRate(CLK_RATE_E *pClkRate);
HI_S32  VDEC_Regulator_SetClkRate(CLK_RATE_E eClkRate);

#endif

