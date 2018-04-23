#ifndef __WIFI_FILTER_H__
#define __WIFI_FILTER_H__

/******************************************************************************

				  ��Ȩ���� (C), 2017, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : wifi_filter.h
  �� �� ��   : ����
  ��	��   : z00220931
  ��������   : 2017��04��26��
  ����޸�   :
  ��������   : dozeģʽ��wifiоƬ���������
  �����б�   :
  �޸���ʷ   :
  1.��	��   : 2017��04��26��
	��	��   : z00220931
	�޸�����   : �����ļ�
******************************************************************************/
#include <net/ip.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
typedef struct {
    unsigned short protocol;    //Э������
    unsigned short port;  //Ŀ�Ķ˿ں�
    unsigned int filter_cnt;    //���˱�����
//    unsigned int uid;
} hw_wifi_filter_item;

struct hw_wlan_filter_ops{
    int (*set_filter_enable)(int);
    int (*add_filter_items)(hw_wifi_filter_item*, int);
    int (*clear_filters)(void);
    int (*get_filter_pkg_stat)(hw_wifi_filter_item*, int, int*);
};


/******************************************************************************
   2 ��������
******************************************************************************/
/*---------driver----------*/
int hw_register_wlan_filter(struct hw_wlan_filter_ops *ops);

int hw_unregister_wlan_filter(void);

/*---------wl_android----------*/

int hw_set_net_filter_enable(int enable);
/*-----kernel----*/
void get_filter_info(
    struct sk_buff *skb,
    const struct nf_hook_state *state,
    unsigned int hook,
    const struct xt_table_info *private,
    const struct ipt_entry *e);

#endif

