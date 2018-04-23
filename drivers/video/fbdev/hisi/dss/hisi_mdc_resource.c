/* Copyright (c) 2014-2017, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include "hisi_fb.h"

//lint -save -e768
static int mdc_chn_request_handle(struct hisi_fb_data_type *hisifd,
	mdc_ch_info_t *chn_info)
{
	unsigned int i;
	mdc_chn_info_t *mdc_chn = NULL;
	mdc_func_ops_t *mdc_ops;

	if (!hisifd || !chn_info) {
		HISI_FB_ERR("hisifd or chn_info is null.\n");
		return -EINVAL;
	}
	mdc_ops = &(hisifd->mdc_ops);

	for (i = 0; i < mdc_ops->chan_num; i++) {
		mdc_chn = &(mdc_ops->mdc_channel[i]);
		/* specified chn request */
		if ((chn_info->rch_idx != 0)
			&& (chn_info->rch_idx != mdc_chn->rch_idx)) {
			/* Not suitable */
			HISI_FB_DEBUG("need_chn(%d) not my specified chn(%d).\n",
				mdc_chn->rch_idx, chn_info->rch_idx);
			continue;
		}

		/* specified cap request */
		if ((chn_info->rch_need_cap != 0)
			&& ((chn_info->rch_need_cap & mdc_chn->cap_available)
				!= chn_info->rch_need_cap)) {
			/* Not suitable */
			HISI_FB_DEBUG("need_cap(0x%x) is not available, mdc_chn->cap(0x%x).\n",
				chn_info->rch_need_cap, mdc_chn->cap_available);
			continue;
		}

		if (mdc_chn->status != FREE) {
			continue;
		}

		/* chn available */
		if ((chn_info->rch_need_cap & CAP_HFBCD) == CAP_HFBCD) {
			if ((chn_info->mmbuf_size == 0)
				|| (chn_info->mmbuf_size > MMBUF_SIZE_MDC_MAX)
				|| (chn_info->mmbuf_size & (MMBUF_ADDR_ALIGN - 1))) {

				HISI_FB_ERR("fb%d, mmbuf size is invalid, size = %d!\n",
					hisifd->index, chn_info->mmbuf_size);
				return -EINVAL;
			}

			chn_info->mmbuf_addr = MMBUF_SIZE_MAX + MMBUF_BASE;//hisi_dss_mmbuf_alloc(hisifd->mmbuf_gen_pool, chn_info->mmbuf_size);
		}

		chn_info->rch_idx = mdc_chn->rch_idx;
		chn_info->wch_idx = mdc_chn->wch_idx;
		chn_info->ovl_idx = mdc_chn->ovl_idx;
		chn_info->wb_composer_type = mdc_chn->wb_composer_type;
		mdc_chn->status = !!chn_info->hold_flag ? HOLD : USED;

		HISI_FB_DEBUG("request mdc channel(%d) seccess.\n", mdc_chn->rch_idx);
		return 0;
	}

	for (i = 0; i < mdc_ops->chan_num; i++) {
		mdc_chn = &(mdc_ops->mdc_channel[i]);
		/* specified chn request */
		if ((chn_info->rch_idx != 0)
			&& (chn_info->rch_idx != mdc_chn->rch_idx)) {
			/* Not suitable */
			HISI_FB_DEBUG("need_chn(%d) not my specified chn(%d).\n",
				mdc_chn->rch_idx, chn_info->rch_idx);
			continue;
		}

		/* specified cap request */
		if ((chn_info->rch_need_cap != 0)
			&& ((chn_info->rch_need_cap & mdc_chn->cap_available)
				!= chn_info->rch_need_cap)) {
			/* Not suitable */
			HISI_FB_DEBUG("need_cap(0x%x) is not available, mdc_chn->cap(0x%x).\n",
				chn_info->rch_need_cap, mdc_chn->cap_available);
			continue;
		}

		if (mdc_chn->status != USED) {
			continue;
		}

		/* chn available */
		if ((chn_info->rch_need_cap & CAP_HFBCD) == CAP_HFBCD) {
			if ((chn_info->mmbuf_size == 0)
				|| (chn_info->mmbuf_size > MMBUF_SIZE_MDC_MAX)
				|| (chn_info->mmbuf_size & (MMBUF_ADDR_ALIGN - 1))) {

				HISI_FB_ERR("fb%d, mmbuf size is invalid, size = %d!\n",
					hisifd->index, chn_info->mmbuf_size);
				return -EINVAL;
			}

			chn_info->mmbuf_addr = MMBUF_SIZE_MAX + MMBUF_BASE;//hisi_dss_mmbuf_alloc(hisifd->mmbuf_gen_pool, chn_info->mmbuf_size);
		}

		chn_info->rch_idx = mdc_chn->rch_idx;
		chn_info->wch_idx = mdc_chn->wch_idx;
		chn_info->ovl_idx = mdc_chn->ovl_idx;
		chn_info->wb_composer_type = mdc_chn->wb_composer_type;
		mdc_chn->status = !!chn_info->hold_flag ? HOLD : USED;

		HISI_FB_DEBUG("Request mdc channel(%d) success.\n", mdc_chn->rch_idx);
		return 0;
	}

	HISI_FB_DEBUG("request channel failed, have no available channel.\n");
	return -1;
}

int mdc_chn_release_handle(struct hisi_fb_data_type *hisifd,
	mdc_ch_info_t *chn_info)
{
	unsigned int i;
	mdc_chn_info_t *mdc_chn = NULL;
	mdc_func_ops_t *mdc_ops;

	if (!hisifd || !chn_info) {
		HISI_FB_ERR("hisifd or chn_info is null.\n");
		return -EINVAL;
	}
	mdc_ops = &(hisifd->mdc_ops);

	for (i = 0; i < mdc_ops->chan_num; i++) {
		mdc_chn = &(mdc_ops->mdc_channel[i]);

		if (chn_info->rch_idx != mdc_chn->rch_idx) {
			continue;
		}

		if ((chn_info->rch_need_cap & CAP_HFBCD) == CAP_HFBCD) {
			if ((chn_info->mmbuf_addr < MMBUF_BASE) || (chn_info->mmbuf_size <= 0)) {
				HISI_FB_ERR("mdc(%d) release failed, hfbc(addr=0x%x, size=%d) is invalid!\n",
						chn_info->rch_idx, chn_info->mmbuf_addr, chn_info->mmbuf_size);
				return -EINVAL;
			}
		}

		HISI_FB_DEBUG("Release mdc channel(%d) success.\n", mdc_chn->rch_idx);
		mdc_chn->status = FREE;
	}

	return 0;
}
//lint -restore

int hisi_mdc_chn_request(struct fb_info *info, void __user *argp)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	mdc_func_ops_t *mdc_ops = NULL;
	mdc_ch_info_t chn_info;

	if (NULL == info) {
		HISI_FB_ERR("info null pointer!\n");
		return -EINVAL;
	}

	hisifd = (struct hisi_fb_data_type *)info->par;
	if (NULL == hisifd) {
		HISI_FB_ERR("hisifd null pointer!\n");
		return -EINVAL;
	}

	if (hisifd->index != AUXILIARY_PANEL_IDX) {
		HISI_FB_INFO("fb(%d) don't support.", hisifd->index);
		return -EINVAL;
	}

	if (NULL == argp) {
		HISI_FB_ERR("argp null pointer!\n");
		return -EINVAL;
	}

	mdc_ops = &(hisifd->mdc_ops);
	down(&mdc_ops->mdc_sem);

	ret = copy_from_user(&chn_info, argp, sizeof(mdc_ch_info_t));
	if (ret) {
		HISI_FB_ERR("fb%d, copy for user failed! ret=%d.\n", hisifd->index, ret);
		up(&mdc_ops->mdc_sem);
		return -EINVAL;
	}

	if (mdc_ops->chn_request_handle) {
		if (mdc_ops->chn_request_handle(hisifd, &chn_info)) {
			HISI_FB_ERR("fb%d, request chn failed! ret=%d.\n", hisifd->index, ret);
			up(&mdc_ops->mdc_sem);
			return -EINVAL;
		}
	}

	ret = copy_to_user(argp, &chn_info, sizeof(mdc_ch_info_t));
	if (ret) {
		HISI_FB_ERR("fb%d, copy to user failed! ret=%d.", hisifd->index, ret);
		if (mdc_ops->chn_release_handle) {
			mdc_ops->chn_release_handle(hisifd, &chn_info);
		}
	}
	up(&mdc_ops->mdc_sem);

	return ret;
}

int hisi_mdc_chn_release(struct fb_info *info, void __user *argp)
{
	int ret = 0;
	struct hisi_fb_data_type *hisifd = NULL;
	mdc_func_ops_t *mdc_ops = NULL;
	mdc_ch_info_t chn_info;

	if (NULL == info) {
		HISI_FB_ERR("info null pointer!\n");
		return -EINVAL;
	}

	hisifd = (struct hisi_fb_data_type *)info->par;
	if (NULL == hisifd) {
		HISI_FB_ERR("hisifd null pointer!\n");
		return -EINVAL;
	}

	if (hisifd->index != AUXILIARY_PANEL_IDX) {
		HISI_FB_INFO("fb(%d) don't support.", hisifd->index);
		return -EINVAL;
	}

	if (NULL == argp) {
		HISI_FB_ERR("argp null pointer!\n");
		return -EINVAL;
	}
	mdc_ops = &(hisifd->mdc_ops);
	down(&mdc_ops->mdc_sem);

	ret = copy_from_user(&chn_info, argp, sizeof(mdc_ch_info_t));
	if (ret) {
		HISI_FB_ERR("fb%d, copy for user failed! ret=%d.", hisifd->index, ret);
		up(&mdc_ops->mdc_sem);
		return -EINVAL;
	}

	if (mdc_ops->chn_release_handle) {
		ret = mdc_ops->chn_release_handle(hisifd, &chn_info);
	}

	up(&mdc_ops->mdc_sem);
	return ret;
}

int hisi_mdc_resource_init(struct hisi_fb_data_type *hisifd, unsigned int platform)
{
	int ret = 0;
	mdc_func_ops_t *mdc_ops;

	if (!hisifd) {
		HISI_FB_ERR("hisifd is null pointer!\n");
		return -EINVAL;
	}

	mdc_ops = &(hisifd->mdc_ops);
	mdc_ops->chn_request_handle = mdc_chn_request_handle;
	mdc_ops->chn_release_handle = mdc_chn_release_handle;
	sema_init(&mdc_ops->mdc_sem, 1);

	switch (platform) {
		case FB_ACCEL_HI365x:
			mdc_ops->chan_num = 1;
			mdc_ops->mdc_channel[0].cap_available = CAP_BASE | CAP_DIM \
				| CAP_SCL | CAP_YUV_PACKAGE \
				| CAP_YUV_SEMI_PLANAR | CAP_YUV_PLANAR \
				| CAP_YUV_DEINTERLACE;
			mdc_ops->mdc_channel[0].rch_idx = DSS_RCHN_V1;
			mdc_ops->mdc_channel[0].wch_idx = DSS_WCHN_W1;
			mdc_ops->mdc_channel[0].ovl_idx = DSS_OVL3;
			mdc_ops->mdc_channel[0].wb_composer_type = DSS_WB_COMPOSE_COPYBIT;
			mdc_ops->mdc_channel[0].status = FREE;
			break;

		case FB_ACCEL_HI625x:
			mdc_ops->chan_num = 1;
			mdc_ops->mdc_channel[0].cap_available = CAP_BASE | CAP_DIM \
				| CAP_SCL | CAP_YUV_PACKAGE \
				| CAP_YUV_SEMI_PLANAR | CAP_YUV_PLANAR \
				| CAP_YUV_DEINTERLACE;
			mdc_ops->mdc_channel[0].rch_idx = DSS_RCHN_V1;
			mdc_ops->mdc_channel[0].wch_idx = DSS_WCHN_W0;
			mdc_ops->mdc_channel[0].ovl_idx = DSS_OVL2;
			mdc_ops->mdc_channel[0].wb_composer_type = DSS_WB_COMPOSE_COPYBIT;
			mdc_ops->mdc_channel[0].status = FREE;
			break;

		case FB_ACCEL_HI366x:
			mdc_ops->chan_num = 1;
			mdc_ops->mdc_channel[0].cap_available = CAP_BASE | CAP_DIM \
				| CAP_SCL | CAP_YUV_PACKAGE \
				| CAP_YUV_SEMI_PLANAR | CAP_YUV_PLANAR \
				| CAP_YUV_DEINTERLACE;
			mdc_ops->mdc_channel[0].rch_idx = DSS_RCHN_V2;
			mdc_ops->mdc_channel[0].wch_idx = DSS_WCHN_W2;
			mdc_ops->mdc_channel[0].ovl_idx = DSS_OVL3;
			mdc_ops->mdc_channel[0].wb_composer_type = DSS_WB_COMPOSE_COPYBIT;
			mdc_ops->mdc_channel[0].status = FREE;
			break;

		case FB_ACCEL_KIRIN970:
			mdc_ops->chan_num = 2;
			mdc_ops->mdc_channel[0].cap_available = CAP_BASE | CAP_DIM \
				| CAP_YUV_PACKAGE | CAP_YUV_SEMI_PLANAR \
				| CAP_YUV_PLANAR | CAP_YUV_DEINTERLACE;
			mdc_ops->mdc_channel[0].rch_idx = DSS_RCHN_V2;
			mdc_ops->mdc_channel[0].wch_idx = DSS_WCHN_W1;
			mdc_ops->mdc_channel[0].ovl_idx = DSS_OVL3;
			mdc_ops->mdc_channel[0].wb_composer_type = DSS_WB_COMPOSE_COPYBIT;
			mdc_ops->mdc_channel[0].status = FREE;

			mdc_ops->mdc_channel[1].cap_available = CAP_BASE | CAP_DIM \
				| CAP_SCL | CAP_YUV_PACKAGE \
				| CAP_YUV_SEMI_PLANAR | CAP_YUV_PLANAR \
				| CAP_YUV_DEINTERLACE | CAP_HFBCD;
			mdc_ops->mdc_channel[1].rch_idx = DSS_RCHN_V1;
			mdc_ops->mdc_channel[1].wch_idx = DSS_WCHN_W1;
			mdc_ops->mdc_channel[1].ovl_idx = DSS_OVL3;
			mdc_ops->mdc_channel[1].wb_composer_type = DSS_WB_COMPOSE_COPYBIT;
			mdc_ops->mdc_channel[1].status = FREE;

			// cppcheck-suppress *
			break;

		default:
			HISI_FB_ERR("Not support mdc copybit func!\n");
			ret = -1;
			break;
	}

	HISI_FB_INFO("Init complete!\n");
	return ret;
}
