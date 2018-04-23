#include "psam_balong.h"

extern struct psam_device *g_psam_device;

void psam_help(void)
{
	printk(KERN_ALERT "psam_ad_info 		��ӡ����ad��������Ϣ\n");
	printk(KERN_ALERT "psam_dump_ad_info 	��ӡ����ad��������Ϣ\n\n");

	printk(KERN_ALERT "********ad���еĻ�����Ϣ\n");
	printk(KERN_ALERT "ad0 virtual addr  0x%x\n",g_psam_device->adq0_info.vir_addr);

	printk(KERN_ALERT "ad0 physical addr 0x%llx\n",g_psam_device->hal->get_ad0_base());
	printk(KERN_ALERT "ad0 lenth is %d\n",  	 g_psam_device->desc_num[0]);
	printk(KERN_ALERT "ad0 read index is %d\n",  readl(g_psam_device->regs + HI_PSAM_ADQ0_RPTR_OFFSET));
	printk(KERN_ALERT "ad0 write index is %d\n", readl(g_psam_device->regs + HI_PSAM_ADQ0_WPTR_OFFSET));

	printk(KERN_ALERT "ad1 virtual addr  0x%x\n",g_psam_device->adq1_info.vir_addr);

	printk(KERN_ALERT "ad1 physical addr 0x%llx\n",g_psam_device->hal->get_ad1_base());
	printk(KERN_ALERT "ad1 lenth is %d\n",  	 g_psam_device->desc_num[1]);
	printk(KERN_ALERT "ad1 read index is %d\n",  readl(g_psam_device->regs + HI_PSAM_ADQ1_RPTR_OFFSET));
	printk(KERN_ALERT "ad1 write index is %d\n\n", readl(g_psam_device->regs + HI_PSAM_ADQ1_WPTR_OFFSET));

	printk(KERN_ALERT "********ipf rd status\n");
	printk(KERN_ALERT "get_rd_num_times		%d\n", ((struct ipf_debug*)(g_psam_device->ipf_deb))->get_rd_num_times);
	printk(KERN_ALERT "get_rd_times			%d\n", ((struct ipf_debug*)(g_psam_device->ipf_deb))->get_rd_times);
	printk(KERN_ALERT "dlrd short cnt %d\n",((struct ipf_debug*)(g_psam_device->ipf_deb))->get_rd_cnt[0]);
	printk(KERN_ALERT "dlrd long cnt  %d\n",((struct ipf_debug*)(g_psam_device->ipf_deb))->get_rd_cnt[1]);
    printk(KERN_ALERT "free ad numbers:  %d\n", g_psam_device->debug.adq_free_cnt);

	printk(KERN_ALERT "********ad���п���ͳ��\n");
	printk(KERN_ALERT "adq0 �յĴ����� %d\n",g_psam_device->debug.adq0_epty_int1);
	printk(KERN_ALERT "adq1 �յĴ����� %d\n",g_psam_device->debug.adq1_epty_int1);
	printk(KERN_ALERT "upoverflow ������ %d\n",g_psam_device->debug.lbdq_upoverflow);
	printk(KERN_ALERT "lbdq_full ������ %d\n",g_psam_device->debug.lbdq_full);
	printk(KERN_ALERT "lbdq_empty ������ %d\n\n",g_psam_device->debug.lbdq_empty);

	printk(KERN_ALERT "********�ӿ���Ϣͳ��\n");
	printk(KERN_ALERT "call get_dlad_num cnt is: %d\n\n",g_psam_device->debug.get_dlad_num);

	printk(KERN_ALERT "adq0 �ɹ������� %d\n",g_psam_device->debug.cfg_dl_ad_succ[0]);
	printk(KERN_ALERT "���� get_dlad0_num ΪNULL %d\n",g_psam_device->debug.cfg_get_ad_para_null[0]);
	printk(KERN_ALERT "���� ad0 desc ΪNULL %d\n",g_psam_device->debug.dlad_desc_null[0]);
	printk(KERN_ALERT "adq0 outpointerΪ0 %d\n\n",g_psam_device->debug.dlad_outptr_err[0]);

	printk(KERN_ALERT "adq1 �ɹ������� %d\n",g_psam_device->debug.cfg_dl_ad_succ[1]);
	printk(KERN_ALERT "���� get_dlad0_num ΪNULL %d\n",g_psam_device->debug.cfg_get_ad_para_null[1]);
	printk(KERN_ALERT "���� ad1 desc ΪNULL %d\n",g_psam_device->debug.dlad_desc_null[1]);
	printk(KERN_ALERT "adq1 outpointerΪ0 %d\n\n",g_psam_device->debug.dlad_outptr_err[1]);

	printk(KERN_ALERT "ad���ʹ��� %d\n",g_psam_device->debug.dlad_type_err);
	printk(KERN_ALERT "ad�������� %d\n",g_psam_device->debug.cfg_dlad_num_err);
	printk(KERN_ALERT "scr ad���� %d\n",g_psam_device->debug.scr_ad_addr_exc);
}

int psam_ad_info( unsigned int ad_type, unsigned int adq_ptr)
{
    if(adq_ptr >= PSAM_DLAD1_DESC_SIZE)
    {
        return -1;
    }

 	printk(KERN_ALERT "===========DL AD%d Info==========\n",ad_type);
 	printk(KERN_ALERT "ADλ��:             %d\n",adq_ptr);
// 	printk(KERN_ALERT "OutPtr0(phy_addr, use by hardware): 0x%llx\n",(long long unsigned int)g_psam_device->desc[ad_type][adq_ptr].u32OutPtr0);
// 	printk(KERN_ALERT "OutPtr1(usrfield skb_addr default): 0x%llx\n",(long long unsigned int)g_psam_device->desc[ad_type][adq_ptr].u32OutPtr1);
    printk(KERN_ALERT "************************\n");
    return 0;
}

int psam_dump_ad_info(unsigned int ad_type)
{
    unsigned int i;

    for(i = 0;i < PSAM_DLAD1_DESC_SIZE;i++)
    {
        psam_ad_info(i, ad_type);
    }

    return 0;
}

EXPORT_SYMBOL(psam_help);

