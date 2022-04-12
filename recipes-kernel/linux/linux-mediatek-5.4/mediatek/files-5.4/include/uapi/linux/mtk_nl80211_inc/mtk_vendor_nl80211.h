/*
 * Copyright (c) [2020], MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary to
 * MediaTek Inc. and/or its licensors.
 * Except as otherwise provided in the applicable licensing terms with
 * MediaTek Inc. and/or its licensors, any reproduction, modification, use or
 * disclosure of MediaTek Software, and information contained herein, in whole
 * or in part, shall be strictly prohibited.
*/

#ifndef __MTK_VENDOR_NL80211_H
#define __MTK_VENDOR_NL80211_H
/*
 * This header file defines the userspace API to the wireless stack. Please
 * be careful not to break things - i.e. don't move anything around or so
 * unless you can demonstrate that it breaks neither API nor ABI.
 *
 */

#include <linux/types.h>

#ifndef GNU_PACKED
#define GNU_PACKED  __attribute__ ((packed))
#endif /* GNU_PACKED */

#define MTK_NL80211_VENDOR_ID	0x0ce7

/**
 * enum mtk_nl80211_vendor_commands - supported mtk nl80211 vendor commands
 *
 * @MTK_NL80211_VENDOR_SUBCMD_UNSPEC: Reserved value 0
 * @MTK_NL80211_VENDOR_SUBCMD_TEST: Test for nl80211command/event
 * @MTK_NL80211_VENDOR_SUBCMD_AMNT_CTRL = 0x000000ae:
 * @MTK_NL80211_VENDOR_SUBCMD_CSI_CTRL = 0x000000c2:
 * @MTK_NL80211_VENDOR_SUBCMD_RFEATURE_CTRL:
 * @MTK_NL80211_VENDOR_SUBCMD_WIRELESS_CTRL:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_DOT11V_WNM:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_WAPP:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_CH_MONITOR:
 * @MTK_NL80211_VENDOR_SUBCMD_BSS_INFO:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_MAP_R3:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_HS_ANQP_RSP:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_HS:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_OFFCH_SCAN:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_DFS_INFO:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_WSC:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_MBO_MSG:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_RRM_COMMAND:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_QOS:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_CAP: command to get capability information
 *  from wifi driver, it requres mtk_nl80211_vendor_attr_get_cap attributes.
 * @MTK_NL80211_VENDOR_SUBCMD_GET_RUNTIME_INFO: command to get run time information
 *  from wifi driver, it requres mtk_nl80211_vendor_attr_get_runtime_info attributes.
 * @MTK_NL80211_VENDOR_SUBCMD_GET_STATISTIC: command to get statistic information
 *  from wifi driver, it requres mtk_nl80211_vendor_attr_get_static_info attributes.
 * @MTK_NL80211_VENDOR_SUBCMD_GET_SRG_BITMAP:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_STATIC_INFO:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_WNM:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_HS:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_WSC:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_DFS_ZERO_WAIT:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_FRAME:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_APCLI_INFO:
 * @MTK_NL80211_VENDOR_SUBCMD_BBP:
 * @MTK_NL80211_VENDOR_SUBCMD_MAC: command to set or get mac register
 *  information to/from wifi driver, require mtk_nl80211_vendor_attrs_mac
 *  attributes.
 * @MTK_NL80211_VENDOR_SUBCMD_E2P:
 * @MTK_NL80211_VENDOR_SUBCMD_ATE:
 * @MTK_NL80211_VENDOR_SUBCMD_STATISTICS: command to get statistic information
 *  in string from wifi driver, this command is used to be compatible with
 *  old iwpriv stat command, it requres mtk_nl80211_vendor_attrs_statistics
 *  attributes.
 * @MTK_NL80211_VENDOR_SUBCMD_ADD_PMKID_CACHE:
 * @MTK_NL80211_VENDOR_SUBCMD_RADIUS_DATA:
 * @MTK_NL80211_VENDOR_SUBCMD_GSITESURVEY:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_MAC_TABLE:
 * @MTK_NL80211_VENDOR_SUBCMD_STATIC_WEP_COPY:
 * @MTK_NL80211_VENDOR_SUBCMD_WSC_PROFILE:
 * @MTK_NL80211_VENDOR_SUBCMD_RF:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_WSC_PROFILE_U32_ITEM:
 * @MTK_NL80211_VENDOR_SUBCMD_QUERY_BATABLE:
 * @MTK_NL80211_VENDOR_SUBCMD_RD:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_FT_PARAM:
 * @MTK_NL80211_VENDOR_SUBCMD_SET_WSCOOB:
 * @MTK_NL80211_VENDOR_SUBCMD_WSC_CALLBACK:
 * @MTK_NL80211_VENDOR_SUBCMD_RX_STATISTICS:
 * @MTK_NL80211_VENDOR_SUBCMD_GET_DRIVER_INFO:
 * @MTK_NL80211_VENDOR_SUBCMD_STA_VLAN:
 * @MTK_NL80211_VENDOR_SUBCMD_PHY_STATE:
 * @MTK_NL80211_VENDOR_SUBCMD_VENDOR_SET: command to set old iwpriv set command
 *  string to wifi driver, it requires mtk_nl80211_vendor_attrs_vendor_set attributes,
 *  please note that this command is just used to be compatible with old iwpriv
 *  set command, and it will be discarded in some time.
 *
 * @MTK_NL80211_VENDOR_SUBCMD_VENDOR_SHOW: command to set old iwpriv show command
 *  string to wifi driver, require mtk_nl80211_vendor_attrs_vendor_show attributes,
 *  please note that this command is just used to be compatible with old iwpriv
 *  show command, and it will be discarded in some time.
 *
 * @MTK_NL80211_VENDOR_SUBCMD_HS:
 * @MTK_NL80211_VENDOR_SUBCMD_WNM:
 * @MTK_NL80211_VENDOR_SUBCMD_MBO_MSG:
 * @MTK_NL80211_VENDOR_SUBCMD_NEIGHBOR_REPORT:
 * @MTK_NL80211_VENDOR_SUBCMD_OFFCHANNEL_INFO:
 * @MTK_NL80211_VENDOR_SUBCMD_OCE_MSG:
 * @MTK_NL80211_VENDOR_SUBCMD_WAPP_REQ: command to set or get wapp requred
 *  information from wifi driver, require mtk_nl80211_vendor_attr_wapp_req attributes.
 * @MTK_NL80211_VENDOR_SUBCMD_SET_AP_SECURITY: command to set ap security configurations
 *  to a specific bss in wifi driver, it requires mtk_nl80211_vendor_attrs_ap_security
 *  attributes.
 * @MTK_NL80211_VENDOR_SUBCMD_SET_AP_VOW:  command to set ap vow configurations
 *  it requires mtk_nl80211_vendor_attrs_ap_vow attributes.
 * @MTK_NL80211_VENDOR_CMD_MAX: highest used command number
 * @__MTK_NL80211_VENDOR_CMD_AFTER_LAST: internal use
 */
enum mtk_nl80211_vendor_commands {
	MTK_NL80211_VENDOR_SUBCMD_UNSPEC = 0,
	MTK_NL80211_VENDOR_SUBCMD_TEST = 1,

/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_SUBCMD_AMNT_CTRL = 0x000000ae,
	MTK_NL80211_VENDOR_SUBCMD_CSI_CTRL = 0x000000c2,
	MTK_NL80211_VENDOR_SUBCMD_RFEATURE_CTRL,
	MTK_NL80211_VENDOR_SUBCMD_WIRELESS_CTRL,
	MTK_NL80211_VENDOR_SUBCMD_SET_DOT11V_WNM,
	MTK_NL80211_VENDOR_SUBCMD_SET_WAPP,
	MTK_NL80211_VENDOR_SUBCMD_SET_CH_MONITOR,
	MTK_NL80211_VENDOR_SUBCMD_BSS_INFO,
	MTK_NL80211_VENDOR_SUBCMD_SET_MAP_R3,
	MTK_NL80211_VENDOR_SUBCMD_SET_HS_ANQP_RSP,
	MTK_NL80211_VENDOR_SUBCMD_SET_HS,
	MTK_NL80211_VENDOR_SUBCMD_SET_OFFCH_SCAN,
	MTK_NL80211_VENDOR_SUBCMD_SET_DFS_INFO,
	MTK_NL80211_VENDOR_SUBCMD_SET_WSC,
	MTK_NL80211_VENDOR_SUBCMD_SET_MBO_MSG,
	MTK_NL80211_VENDOR_SUBCMD_SET_RRM_COMMAND,
	MTK_NL80211_VENDOR_SUBCMD_SET_QOS,
	MTK_NL80211_VENDOR_SUBCMD_GET_CAP,
	MTK_NL80211_VENDOR_SUBCMD_GET_RUNTIME_INFO,
	MTK_NL80211_VENDOR_SUBCMD_GET_STATISTIC,
	MTK_NL80211_VENDOR_SUBCMD_GET_SRG_BITMAP,
	MTK_NL80211_VENDOR_SUBCMD_GET_STATIC_INFO,
	MTK_NL80211_VENDOR_SUBCMD_GET_WNM,
	MTK_NL80211_VENDOR_SUBCMD_GET_HS,
	MTK_NL80211_VENDOR_SUBCMD_GET_WSC,
	MTK_NL80211_VENDOR_SUBCMD_GET_DFS_ZERO_WAIT,
	MTK_NL80211_VENDOR_SUBCMD_GET_FRAME,
	MTK_NL80211_VENDOR_SUBCMD_GET_APCLI_INFO,
	MTK_NL80211_VENDOR_SUBCMD_BBP,
	MTK_NL80211_VENDOR_SUBCMD_MAC,
	MTK_NL80211_VENDOR_SUBCMD_E2P,
	MTK_NL80211_VENDOR_SUBCMD_ATE,
	MTK_NL80211_VENDOR_SUBCMD_STATISTICS,
	MTK_NL80211_VENDOR_SUBCMD_ADD_PMKID_CACHE,
	MTK_NL80211_VENDOR_SUBCMD_RADIUS_DATA,
	MTK_NL80211_VENDOR_SUBCMD_GSITESURVEY,
	MTK_NL80211_VENDOR_SUBCMD_GET_MAC_TABLE,
	MTK_NL80211_VENDOR_SUBCMD_STATIC_WEP_COPY,
	MTK_NL80211_VENDOR_SUBCMD_WSC_PROFILE  ,
	MTK_NL80211_VENDOR_SUBCMD_RF,
	MTK_NL80211_VENDOR_SUBCMD_SET_WSC_PROFILE_U32_ITEM,
	MTK_NL80211_VENDOR_SUBCMD_QUERY_BATABLE,
	MTK_NL80211_VENDOR_SUBCMD_RD,
	MTK_NL80211_VENDOR_SUBCMD_SET_FT_PARAM,
	MTK_NL80211_VENDOR_SUBCMD_SET_WSCOOB,
	MTK_NL80211_VENDOR_SUBCMD_WSC_CALLBACK,
	MTK_NL80211_VENDOR_SUBCMD_RX_STATISTICS,
	MTK_NL80211_VENDOR_SUBCMD_GET_DRIVER_INFO,
	MTK_NL80211_VENDOR_SUBCMD_STA_VLAN,
	MTK_NL80211_VENDOR_SUBCMD_PHY_STATE,
	MTK_NL80211_VENDOR_SUBCMD_VENDOR_SET,
	MTK_NL80211_VENDOR_SUBCMD_VENDOR_SHOW,
	MTK_NL80211_VENDOR_SUBCMD_HS,
	MTK_NL80211_VENDOR_SUBCMD_WNM,
	MTK_NL80211_VENDOR_SUBCMD_MBO_MSG,
	MTK_NL80211_VENDOR_SUBCMD_NEIGHBOR_REPORT,
	MTK_NL80211_VENDOR_SUBCMD_OFFCHANNEL_INFO,
	MTK_NL80211_VENDOR_SUBCMD_OCE_MSG,
	MTK_NL80211_VENDOR_SUBCMD_WAPP_REQ,
	MTK_NL80211_VENDOR_SUBCMD_SET_AP_SECURITY,
	MTK_NL80211_VENDOR_SUBCMD_SET_AP_VOW,
	/* add new commands above here */
	/* used to define NL80211_CMD_MAX below */
	__MTK_NL80211_VENDOR_CMD_AFTER_LAST,
	MTK_NL80211_VENDOR_CMD_MAX = __MTK_NL80211_VENDOR_CMD_AFTER_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_events - MediaTek nl80211 asynchoronous event identifiers
 *
 * @MTK_NL80211_VENDOR_EVENT_UNSPEC: Reserved value 0
 *
 * @MTK_NL80211_VENDOR_EVENT_TEST: Test for nl80211command/event
 */
enum mtk_nl80211_vendor_events {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_EVENT_UNSPEC = 0,
	MTK_NL80211_VENDOR_EVENT_TEST,

	MTK_NL80211_VENDOR_EVENT_RSP_WAPP_EVENT,

	/* add new events above here */
};

/**
 * enum mtk_nl80211_vendor_attr_test - Specifies the values for vendor test
 * command MTK_NL80211_VENDOR_ATTR_TEST
 * @MTK_NL80211_VENDOR_ATTR_TEST：enable nl80211 test
 */
enum mtk_nl80211_vendor_attr_test {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_TEST_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_TEST,

	__MTK_NL80211_VENDOR_ATTR_TEST_LAST,
	MTK_NL80211_VENDOR_ATTR_TEST_MAX =
	__MTK_NL80211_VENDOR_ATTR_TEST_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attr_event_test - Specifies the values for vendor test
 * event MTK_NL80211_VENDOR_ATTR_TEST
 * @MTK_NL80211_VENDOR_ATTR_TEST：receive nl80211 test event
 */
enum mtk_nl80211_vendor_attr_event_test {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_EVENT_TEST_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_EVENT_TEST,

	__MTK_NL80211_VENDOR_ATTR_EVENT_TEST_LAST,
	MTK_NL80211_VENDOR_ATTR_EVENT_TEST_MAX =
	__MTK_NL80211_VENDOR_ATTR_EVENT_TEST_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attrs_wnm - This enum defines
 * attributes required for MTK_NL80211_VENDOR_SUBCMD_SET_DOT11V_WNM.
 * Information in these attributes is used to set/get wnm information
 * to/from driver from/to user application.
 *
 * @MTK_NL80211_VENDOR_ATTR_WNM_CMD:
 * @MTK_NL80211_VENDOR_ATTR_WNM_BTM_REQ: BTM request frame
 * @MTK_NL80211_VENDOR_ATTR_WNM_BTM_RSP:
 */
enum mtk_nl80211_vendor_attrs_dot11v_wnm {
/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_DOT11V_WNM_INVALID = 0,
	MTK_NL80211_VENDOR_ATTR_DOT11V_WNM_CMD,
	MTK_NL80211_VENDOR_ATTR_DOT11V_WNM_BTM_REQ,
	MTK_NL80211_VENDOR_ATTR_DOT11V_WNM_BTM_RSP,
	/* add attributes here, update the policy in nl80211.c */

	__MTK_NL80211_VENDOR_ATTR_DOT11V_WNM_AFTER_LAST,
	MTK_NL80211_VENDOR_DOT11V_WNM_ATTR_MAX = __MTK_NL80211_VENDOR_ATTR_DOT11V_WNM_AFTER_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attrs_vendor_set - This enum defines
 * attributes required for MTK_NL80211_VENDOR_SUBCMD_VENDOR_SET.
 * Information in these attributes is used to set information
 * to driver from user application.
 *
 * @MTK_NL80211_VENDOR_ATTR_VENDOR_SET_CMD_STR: command string
 */
enum mtk_nl80211_vendor_attrs_vendor_set {
/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_VENDOR_SET_INVALID = 0,
	MTK_NL80211_VENDOR_ATTR_VENDOR_SET_CMD_STR,
	/* add attributes here, update the policy in nl80211.c */

	__MTK_NL80211_VENDOR_ATTR_VENDOR_SET_AFTER_LAST,
	MTK_NL80211_VENDOR_ATTR_VENDOR_SET_ATTR_MAX = __MTK_NL80211_VENDOR_ATTR_VENDOR_SET_AFTER_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attrs_vendor_show - This enum defines
 * attributes required for MTK_NL80211_VENDOR_SUBCMD_VENDOR_SHOW.
 * Information in these attributes is used to get information
 * from driver to user application.
 *
 * @MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_CMD_STR: command string
 * @MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_RSP_STR: show rsp string buffer
 */
enum mtk_nl80211_vendor_attrs_vendor_show {
/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_INVALID = 0,
	MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_CMD_STR,
	MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_RSP_STR,
	/* add attributes here, update the policy in nl80211.c */

	__MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_AFTER_LAST,
	MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_ATTR_MAX = __MTK_NL80211_VENDOR_ATTR_VENDOR_SHOW_AFTER_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attrs_statistics - This enum defines
 * attributes required for MTK_NL80211_VENDOR_SUBCMD_STATISTICS.
 * Information in these attributes is used to get wnm information
 * to/from driver from/to user application.
 *
 * @MTK_NL80211_VENDOR_ATTR_STATISTICS_STR: statistic information string
 */
enum mtk_nl80211_vendor_attrs_statistics {
/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_STATISTICS_INVALID = 0,
	MTK_NL80211_VENDOR_ATTR_STATISTICS_STR,
	/* add attributes here, update the policy in nl80211.c */

	__MTK_NL80211_VENDOR_ATTR_STATISTICS_AFTER_LAST,
	MTK_NL80211_VENDOR_ATTR_STATISTICS_ATTR_MAX = __MTK_NL80211_VENDOR_ATTR_STATISTICS_AFTER_LAST - 1
};

/**
 * structure mac_param - This structure defines the payload format of
 * MTK_NL80211_VENDOR_ATTR_MAC_WRITE_PARAM and MTK_NL80211_VENDOR_ATTR_MAC_SHOW_PARAM.
 * Information in this structure is used to get/set mac register information
 * from/to driver.
 *
 * @start: start mac address
 * @end: end mac address
 * @value: value for the mac register
 */
struct GNU_PACKED mac_param {
	unsigned int start;
	unsigned int end;
	unsigned int value;
};

/**
 * enum mtk_nl80211_vendor_attrs_mac - This enum defines
 * attributes required for MTK_NL80211_VENDOR_SUBCMD_MAC.
 * Information in these attributes is used to get/set mac information
 * from/to driver.
 *
 * @MTK_NL80211_VENDOR_ATTR_MAC_WRITE_PARAM: params, refer to struct GNU_PACKED mac_param
 * @MTK_NL80211_VENDOR_ATTR_MAC_SHOW_PARAM: params, refer to struct GNU_PACKED mac_param
 * @MTK_NL80211_VENDOR_ATTR_MAC_RSP_STR: RSP string
 */
enum mtk_nl80211_vendor_attrs_mac {
/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_MAC_INVALID = 0,
	MTK_NL80211_VENDOR_ATTR_MAC_WRITE_PARAM,
	MTK_NL80211_VENDOR_ATTR_MAC_SHOW_PARAM,
	MTK_NL80211_VENDOR_ATTR_MAC_RSP_STR,
	/* add attributes here, update the policy in nl80211.c */

	__MTK_NL80211_VENDOR_ATTR_MAC_AFTER_LAST,
	MTK_NL80211_VENDOR_ATTR_MAC_ATTR_MAX = __MTK_NL80211_VENDOR_ATTR_MAC_AFTER_LAST - 1
};

/**
 * enum mtk_vendor_attr_authmode - This enum defines the value of
 * MTK_NL80211_VENDOR_ATTR_AP_SECURITY_AUTHMODE.
 * Information in these attributes is used set auth mode of a specific bss.
 */
enum mtk_vendor_attr_authmode {
	NL80211_AUTH_OPEN,
	NL80211_AUTH_SHARED,
	NL80211_AUTH_WEPAUTO,
	NL80211_AUTH_WPA,
	NL80211_AUTH_WPAPSK,
	NL80211_AUTH_WPANONE,
	NL80211_AUTH_WPA2,
	NL80211_AUTH_WPA2MIX,
	NL80211_AUTH_WPA2PSK,
	NL80211_AUTH_WPA3,
	NL80211_AUTH_WPA3_192,
	NL80211_AUTH_WPA3PSK,
	NL80211_AUTH_WPA2PSKWPA3PSK,
	NL80211_AUTH_WPA2PSKMIXWPA3PSK,
	NL80211_AUTH_WPA1WPA2,
	NL80211_AUTH_WPAPSKWPA2PSK,
	NL80211_AUTH_WPA_AES_WPA2_TKIPAES,
	NL80211_AUTH_WPA_AES_WPA2_TKIP,
	NL80211_AUTH_WPA_TKIP_WPA2_AES,
	NL80211_AUTH_WPA_TKIP_WPA2_TKIPAES,
	NL80211_AUTH_WPA_TKIPAES_WPA2_AES,
	NL80211_AUTH_WPA_TKIPAES_WPA2_TKIPAES,
	NL80211_AUTH_WPA_TKIPAES_WPA2_TKIP,
	NL80211_AUTH_OWE,
	NL80211_AUTH_FILS_SHA256,
	NL80211_AUTH_FILS_SHA384,
	NL80211_AUTH_WAICERT,
	NL80211_AUTH_WAIPSK,
	NL80211_AUTH_DPP,
	NL80211_AUTH_DPPWPA2PSK,
	NL80211_AUTH_DPPWPA3PSK,
	NL80211_AUTH_DPPWPA3PSKWPA2PSK,
	NL80211_AUTH_WPA2_ENT_OSEN
};

/**
 * enum mtk_vendor_attr_encryptype - This enum defines the value of
 * MTK_NL80211_VENDOR_ATTR_AP_SECURITY_ENCRYPTYPE.
 * Information in these attributes is used set encryption type of a specific bss.
 */
enum mtk_vendor_attr_encryptype {
	NL80211_ENCRYPTYPE_NONE,
	NL80211_ENCRYPTYPE_WEP,
	NL80211_ENCRYPTYPE_TKIP,
	NL80211_ENCRYPTYPE_AES,
	NL80211_ENCRYPTYPE_CCMP128,
	NL80211_ENCRYPTYPE_CCMP256,
	NL80211_ENCRYPTYPE_GCMP128,
	NL80211_ENCRYPTYPE_GCMP256,
	NL80211_ENCRYPTYPE_TKIPAES,
	NL80211_ENCRYPTYPE_TKIPCCMP128,
	NL80211_ENCRYPTYPE_WPA_AES_WPA2_TKIPAES,
	NL80211_ENCRYPTYPE_WPA_AES_WPA2_TKIP,
	NL80211_ENCRYPTYPE_WPA_TKIP_WPA2_AES,
	NL80211_ENCRYPTYPE_WPA_TKIP_WPA2_TKIPAES,
	NL80211_ENCRYPTYPE_WPA_TKIPAES_WPA2_AES,
	NL80211_ENCRYPTYPE_WPA_TKIPAES_WPA2_TKIPAES,
	NL80211_ENCRYPTYPE_WPA_TKIPAES_WPA2_TKIP,
	NL80211_ENCRYPTYPE_SMS4
};

#define MAX_WEP_KEY_LEN 32
/**
 * structure wep_key_param - This structure defines the payload format of
 * MTK_NL80211_VENDOR_ATTR_AP_SECURITY_WEPKEY. Information in this structure
 * is used to set wep key information to a specific bss in wifi driver.
 *
 * @key_idx: key index
 * @key_len: key length
 * @key: key value
 */
struct GNU_PACKED wep_key_param {
	unsigned char key_idx;
	unsigned int key_len;
	unsigned char key[MAX_WEP_KEY_LEN];
};

/**
 * structure vow_group_en_param - This structure defines the payload format of
 * MTK_NL80211_VENDOR_ATTR_AP_VOW_ATC_EN_INFO &
 * MTK_NL80211_VENDOR_ATTR_AP_VOW_BW_CTL_EN_INFO.
 * Information in this structure is used to set vow airtime control enable configuration
 * to a specific group in wifi driver.
 *
 * @group: vow group
 * @en: Enable/Disable
 */
struct GNU_PACKED vow_group_en_param {
	unsigned int group;
	unsigned int en;
};

/**
 * structure vow_group_en_param - This structure defines the payload format of
 * MTK_NL80211_VENDOR_ATTR_AP_VOW_MIN_RATE_INFO &
 * MTK_NL80211_VENDOR_ATTR_AP_VOW_MAX_RATE_INFO.
 * Information in this structure is used to set vow airtime rate configuration
 * to a specific group in wifi driver.
 *
 * @group: vow group
 * @rate: vow rate
 */
struct GNU_PACKED vow_rate_param {
	unsigned int group;
	unsigned int rate;
};

/**
 * structure vow_group_en_param - This structure defines the payload format of
 * MTK_NL80211_VENDOR_ATTR_AP_VOW_MIN_RATIO_INFO &
 * MTK_NL80211_VENDOR_ATTR_AP_VOW_MAX_RATIO_INFO.
 * Information in this structure is used to set vow airtime ratio configuration
 * to a specific group in wifi driver.
 *
 * @group: vow group
 * @ratio: vow ratio
 */
struct GNU_PACKED vow_ratio_param {
	unsigned int group;
	unsigned int ratio;
};

/**
 * enum mtk_nl80211_vendor_attrs_ap_vow - This enum defines
 * attributes required for MTK_NL80211_VENDOR_SUBCMD_SET_AP_VOW.
 * Information in these attributes is used to set vow configuration
 * to driver from user application.
 *
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_ATF_EN_INFO: u8, air time fairness enable attributes
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_ATC_EN_INFO: refer to vow_group_en_param
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_BW_EN_INFO: u8, air time bw enalbe attributes
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_BW_CTL_EN_INFO: refer to vow_group_en_param
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_MIN_RATE_INFO: refer to vow_rate_param
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_MAX_RATE_INFO: refer to vow_rate_param
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_MIN_RATIO_INFO: refer to vow_ratio_param
 * @MTK_NL80211_VENDOR_ATTR_AP_VOW_MAX_RATIO_INFO: refer to vow_ratio_param
 */
enum mtk_nl80211_vendor_attrs_ap_vow{
/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_AP_VOW_INVALID = 0,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_ATF_EN_INFO,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_ATC_EN_INFO,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_BW_EN_INFO,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_BW_CTL_EN_INFO,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_MIN_RATE_INFO,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_MAX_RATE_INFO,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_MIN_RATIO_INFO,
	MTK_NL80211_VENDOR_ATTR_AP_VOW_MAX_RATIO_INFO,
	__MTK_NL80211_VENDOR_ATTR_AP_VOW_AFTER_LAST,
	MTK_NL80211_VENDOR_AP_VOW_ATTR_MAX = __MTK_NL80211_VENDOR_ATTR_AP_VOW_AFTER_LAST - 1
};
/**
 * enum mtk_nl80211_vendor_attrs_ap_security - This enum defines
 * attributes required for MTK_NL80211_VENDOR_SUBCMD_SET_AP_SECURITY.
 * Information in these attributes is used to set security information
 * to driver from user application.
 *
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_AUTHMODE:  u32, auth mode attributes, refer to mtk_vendor_attr_authmode
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_ENCRYPTYPE: u32, encryptype, refer to mtk_vendor_attr_encryptype
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_REKEYINTERVAL: u32, rekey interval in seconds
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_REKEYMETHOD: u8, 0-by time, 1-by pkt count
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_DEFAULTKEYID: u8, default key index
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_WEPKEY: refer to wep_key_param
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PASSPHRASE: string
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PMF: u8 1-support pmf, 0-not support pmf
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PMF_REQUIRE: u8 1-pmf is required, 0-pmf is not required
 * @MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PMF_SHA256: u8 1-pmfsha256 is desired, 0-pmfsha256 is not desired
 */
enum mtk_nl80211_vendor_attrs_ap_security {
/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_INVALID = 0,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_AUTHMODE,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_ENCRYPTYPE,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_REKEYINTERVAL,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_REKEYMETHOD,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_DEFAULTKEYID,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_WEPKEY,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PASSPHRASE,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PMF_CAPABLE,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PMF_REQUIRE,
	MTK_NL80211_VENDOR_ATTR_AP_SECURITY_PMF_SHA256,
	/* add attributes here, update the policy in nl80211.c */

	__MTK_NL80211_VENDOR_ATTR_AP_SECURITY_AFTER_LAST,
	MTK_NL80211_VENDOR_AP_SECURITY_ATTR_MAX = __MTK_NL80211_VENDOR_ATTR_AP_SECURITY_AFTER_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attr_get_static_info - Specifies the vendor attribute values
 * to get static info
 */
enum mtk_nl80211_vendor_attr_get_static_info {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_CHIP_ID,
	MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_DRIVER_VER,
	MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_COEXISTENCE,
	MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_WIFI_VER,
	MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_WAPP_SUPPORT_VER,

	__MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_LAST,
	MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_MAX =
	__MTK_NL80211_VENDOR_ATTR_GET_STATIC_INFO_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attr_get_runtime_info - Specifies the vendor attribute values
 * to get runtime info
 */
enum mtk_nl80211_vendor_attr_get_runtime_info {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_MAX_NUM_OF_STA,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_CHAN_LIST,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_OP_CLASS,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_BSS_INFO,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_NOP_CHANNEL_LIST,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_WMODE,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_WAPP_WSC_PROFILES,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_PMK_BY_PEER_MAC,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_802_11_AUTHENTICATION_MODE,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_802_11_MAC_ADDRESS,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_GET_802_11_CURRENTCHANNEL,

	__MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_LAST,
	MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_MAX =
	__MTK_NL80211_VENDOR_ATTR_GET_RUNTIME_INFO_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attr_get_statistic - Specifies the vendor attribute values
 * to get statistic info
 */
enum mtk_nl80211_vendor_attr_get_statistic {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_GET_STATISTIC_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_GET_802_11_STATISTICS,
	MTK_NL80211_VENDOR_ATTR_GET_TX_PWR,
	MTK_NL80211_VENDOR_ATTR_GET_AP_METRICS,
	MTK_NL80211_VENDOR_ATTR_GET_802_11_PER_BSS_STATISTICS,
	MTK_NL80211_VENDOR_ATTR_GET_CPU_TEMPERATURE,

	_MTK_NL80211_VENDOR_ATTR_GET_STATISTIC_LAST,
	MTK_NL80211_VENDOR_ATTR_GET_STATISTIC_MAX =
	_MTK_NL80211_VENDOR_ATTR_GET_STATISTIC_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attr_wapp_req - Specifies the vendor attribute values
 * to request wifi info
 */
enum mtk_nl80211_vendor_attr_wapp_req {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_WAPP_REQ_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_WAPP_REQ,

	__MTK_NL80211_VENDOR_ATTR_WAPP_REQ_LAST,
	MTK_NL80211_VENDOR_ATTR_WAPP_REQ_MAX =
	__MTK_NL80211_VENDOR_ATTR_WAPP_REQ_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attr_event_rsp_wapp_event - Specifies the vendor attribute values
 * to get wifi info event
 */
enum mtk_nl80211_vendor_attr_event_rsp_wapp_event {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_EVENT_RSP_WAPP_EVENT_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_EVENT_RSP_WAPP_EVENT,

	__MTK_NL80211_VENDOR_ATTR_EVENT_RSP_WAPP_EVENT_LAST,
	MTK_NL80211_VENDOR_ATTR_EVENT_RSP_WAPP_EVENT_MAX =
	__MTK_NL80211_VENDOR_ATTR_EVENT_RSP_WAPP_EVENT_LAST - 1
};

/**
 * enum mtk_nl80211_vendor_attr_get_cap - Specifies the vendor attribute values
 * to get capability info
 */
enum mtk_nl80211_vendor_attr_get_cap {
	/* don't change the order or add anything between, this is ABI! */
	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_INVALID = 0,

	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_CAC_CAP,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_MISC_CAP,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_HT_CAP,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_VHT_CAP,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_WF6_CAPABILTY,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_HE_CAP,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_QUERY_11H_CAPABILITY,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_QUERY_RRM_CAPABILITY,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_QUERY_KVR_CAPABILITY,


	__MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_LAST,
	MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_MAX =
	__MTK_NL80211_VENDOR_ATTR_GET_CAP_INFO_LAST - 1
};

#endif /* __MTK_VENDOR_NL80211_H */
