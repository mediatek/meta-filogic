#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <uci.h>
#include "wifi-test-tool.h"

static int mac_addr_aton(unsigned char *mac_addr, char *arg)
{
    sscanf(arg, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac_addr[0], &mac_addr[1], &mac_addr[2], &mac_addr[3], &mac_addr[4], &mac_addr[5]);
    return 0;
}

static void mac_addr_ntoa(char *mac_addr, unsigned char *arg)
{
    snprintf(mac_addr, 20, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", arg[0], arg[1],arg[2],arg[3],arg[4],arg[5]);
    return;
}

static int _syscmd(char *cmd, char *retBuf, int retBufSize)
{
    FILE *f;
    char *ptr = retBuf;
    int bufSize=retBufSize, bufbytes=0, readbytes=0, cmd_ret=0;

    
    if((f = popen(cmd, "r")) == NULL) {
        fprintf(stderr,"\npopen %s error\n", cmd);
        return RETURN_ERR;
    }

    while(!feof(f))
    {
        *ptr = 0;
        if(bufSize>=128) {
            bufbytes=128;
        } else {
            bufbytes=bufSize-1;
        }

        fgets(ptr,bufbytes,f);
        readbytes=strlen(ptr);

        if(!readbytes)
            break;

        bufSize-=readbytes;
        ptr += readbytes;
    }
    cmd_ret = pclose(f);
    retBuf[retBufSize-1]=0;

    return cmd_ret >> 8;
}

int phy_index_to_radio(int phyIndex)
{
    char cmd[128] = {0};
    char buf[64] = {0};
    int radioIndex = 0;
    snprintf(cmd, sizeof(cmd), "ls /tmp | grep phy%d | cut -d '-' -f2 | tr -d '\n'", phyIndex);
    _syscmd(cmd, buf, sizeof(buf));

    if (strlen(buf) == 0 || strstr(buf, "wifi") == NULL) {
        fprintf(stderr, "%s: failed to get wifi index\n", __func__);
        return RETURN_ERR;
    }
    sscanf(buf, "wifi%d", &radioIndex);
    return radioIndex;
}

void set_channel(wifi_radio_param *radio_param, char *channel)
{
    if (strcmp(channel, "auto") == 0) {
        radio_param->auto_channel = TRUE;
        radio_param->channel = 0;
    } else {
        radio_param->auto_channel = FALSE;
        radio_param->channel = strtol(channel, NULL, 10);
    }
    return;
}

void set_country(wifi_radio_param *radio_param, char *country)
{
    strcpy(radio_param->country, country);
}

void set_band(wifi_radio_param *radio_param, char *band)
{
    strcpy(radio_param->band, band);
}

void set_noscan(wifi_radio_param *radio_param, char *noscan)
{
    snprintf(radio_param->noscan, 2, "%s", noscan);
    radio_param->noscan[1] = '\0';
}

void set_hwmode(wifi_radio_param *radio_param, char *hwmode)
{
    if (strncmp(hwmode, "11a", 3) == 0)
        strcpy(radio_param->hwmode, "a");
    if (strncmp(hwmode, "11b", 3) == 0)
        strcpy(radio_param->hwmode, "b");
    if (strncmp(hwmode, "11g", 3) == 0)
        strcpy(radio_param->hwmode, "g");
}

void set_htmode(wifi_radio_param *radio_param, char *htmode)
{
    char tmp[16] = {0};
    char *ptr = htmode;
    ULONG bandwidth = 0;
    radio_param->bandwidth = 20;
    while (*ptr) {
        if (isdigit(*ptr)) {
            bandwidth = strtoul(ptr, NULL, 10);
            radio_param->bandwidth = bandwidth;
            break;
        }
        ptr++;
    }

    // HT40     -> 11NGHT40PLUS
    // VHT40+   -> 11ACVHT40PLUS
    // HE80     -> 11AXHE80
    if (strstr(htmode, "+") != NULL) {
        strncpy(tmp, htmode, strlen(htmode) - 1);
        strcat(tmp, "PLUS");
    } else if (strstr(htmode, "-") != NULL) {
        strncpy(tmp, htmode, strlen(htmode) - 1);
        strcat(tmp, "MINUS");
    } else 
        strcpy(tmp, htmode);


    if (strstr(htmode, "VHT") != NULL) {
        snprintf(radio_param->htmode, sizeof(radio_param->htmode), "11AC%s", tmp);
    } else if (strstr(htmode, "HT") != NULL && strstr(htmode, "NO") == NULL) {
        snprintf(radio_param->htmode, sizeof(radio_param->htmode), "11NG%s", tmp);
    } else if (strstr(htmode, "HE") != NULL) {
        snprintf(radio_param->htmode, sizeof(radio_param->htmode), "11AX%s", tmp);
    } else {        // NOHT or NONE should be parsed with the band, so just fill the original string.
        strcpy(radio_param->htmode, tmp);
    }

}

void set_disable(wifi_radio_param *radio_param, char *disable)
{
    if (strcmp(disable, "1") == 0)
        radio_param->disabled = TRUE;
    else
        radio_param->disabled = FALSE;
}

void set_rxant(wifi_radio_param *radio_param, char *mask)
{
    radio_param->rxantenna = strtol(mask, NULL, 16);
}

void set_txant(wifi_radio_param *radio_param, char *mask)
{
    radio_param->txantenna = strtol(mask, NULL, 16);
}

void set_htcoex(wifi_radio_param *radio_param, char *ht_coex)
{
    radio_param->ht_coex = strtol(ht_coex, NULL, 10);
}

void set_rts(wifi_radio_param *radio_param, char *rts)
{
    radio_param->rtsThreshold = strtol(rts, NULL, 10);
}

void set_radionum(wifi_intf_param *intf_param, char *phy_name)
{
    int radio_num = 0;
    char *ptr = phy_name;
    int phyId = 0;

    while (*ptr) {
        if (isdigit(*ptr)) {
            phyId = strtoul(ptr, NULL, 10);
            radio_num = phy_index_to_radio(phyId);
            intf_param->radio_index = radio_num;
            break;
        }
        ptr++;
    }
}

void set_ssid(wifi_intf_param *intf_param, char *ssid)
{
    strncpy(intf_param->ssid, ssid, 32);
}

void set_encryption(wifi_intf_param *intf_param, char *encryption_mode)
{
    if (strcmp(encryption_mode, "none") == 0) {
        intf_param->security.mode = wifi_security_mode_none;
        intf_param->security.encr = wifi_encryption_none;
    }else if(strncmp(encryption_mode, "psk2", 4) == 0){
        intf_param->security.mode = wifi_security_mode_wpa2_personal;
    }else if(strncmp(encryption_mode, "psk-",4) == 0){
       intf_param->security.mode = wifi_security_mode_wpa_wpa2_personal;
    }else if(strncmp(encryption_mode, "psk",3) == 0){
        intf_param->security.mode = wifi_security_mode_wpa_personal;
    }else if(strncmp(encryption_mode, "wpa2",4) == 0){
        intf_param->security.mode = wifi_security_mode_wpa2_enterprise;
    }else if(strncmp(encryption_mode, "wpa-",4) == 0){
        intf_param->security.mode = wifi_security_mode_wpa_wpa2_enterprise;
    }else if(strcmp(encryption_mode, "sae") == 0){
        intf_param->security.mode = wifi_security_mode_wpa3_personal;
    }else if(strcmp(encryption_mode, "wpa3") == 0){
        intf_param->security.mode = wifi_security_mode_wpa3_enterprise;
    }else if(strcmp(encryption_mode, "sae-mixed") == 0){
        intf_param->security.mode = wifi_security_mode_wpa3_transition;
    }else if(strcmp(encryption_mode, "owe") == 0){
        intf_param->security.mode = wifi_security_mode_owe;
    }

    if(strstr(encryption_mode, "tkip") && (strstr(encryption_mode, "ccmp") || strstr(encryption_mode, "aes") )){
        intf_param->security.encr = wifi_encryption_aes_tkip;
    }else if (strstr(encryption_mode, "tkip")){
        intf_param->security.encr = wifi_encryption_tkip;
    }else{
        intf_param->security.encr = wifi_encryption_aes;
    }

    if(!strcmp(encryption_mode, "wpa3") || !strcmp(encryption_mode, "sae") || !strcmp(encryption_mode, "owe")){
        intf_param->security.mfp = wifi_mfp_cfg_required;
    }else if (!strcmp(encryption_mode, "sae-mixed")){
        intf_param->security.mfp = wifi_mfp_cfg_optional;
    }else{
        intf_param->security.mfp = wifi_mfp_cfg_disabled;
    }
}

void set_key(wifi_intf_param *intf_param, char *key)
{
    strncpy(intf_param->security.u.key.key, key, 64);
}

void set_ifname(wifi_intf_param *intf_param, char *ifname)
{
    if (strlen(ifname) > 15)
        return;
    strncpy(intf_param->ifname, ifname, strlen(ifname) + 1);
}

void set_wds(wifi_intf_param *intf_param, char *wds_enable)
{
    intf_param->wds_mode = FALSE;
    if (strncmp(wds_enable, "1", 1) == 0)
        intf_param->wds_mode = TRUE;
}

void set_hidden(wifi_intf_param *intf_param, char *hidden)
{
    intf_param->hidden = strtol(hidden, NULL, 10);
}

int set_interface_bssid(int phy_index, int offset, mac_address_t *bssid)
{
    FILE *f;
    char mac_file[64] = {0};
    char mac_address[20] = {0};

    sprintf(mac_file, "/sys/class/ieee80211/phy%d/macaddress", phy_index);
    f = fopen(mac_file, "r");
    if (f == NULL)
        return -1;
    fgets(mac_address, 20, f);
    fclose(f);

    mac_addr_aton(&(*bssid)[0], mac_address);
    (*bssid)[0] += offset*2;
    return 0;
}

void set_radio_param(wifi_radio_param radio_parameter)
{
    int ret = 0;
    wifi_radio_operationParam_t operationParam = {0};

    if(radio_parameter.radio_index == -1)
        return;

    if (radio_parameter.disabled == TRUE) {
        wifi_setRadioEnable(radio_parameter.radio_index, FALSE);
        return;
    }

    fprintf(stderr, "Start setting radio\n");

    if (radio_parameter.txantenna != 0 && radio_parameter.txantenna == radio_parameter.rxantenna) {
        ret = wifi_setRadioTxChainMask(radio_parameter.radio_index, radio_parameter.txantenna);
        if (ret != RETURN_OK)
            fprintf(stderr, "[Set Tx Chain mask failed!!!]\n");
    }

    // Get current radio setting
    ret = wifi_getRadioOperatingParameters(radio_parameter.radio_index, &operationParam);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Get OperatingParameters failed!!!]\n");
    operationParam.enable = TRUE;

    // Channel
    operationParam.autoChannelEnabled = radio_parameter.auto_channel;
    operationParam.channel = radio_parameter.channel;

    //bandwidth
    if (radio_parameter.bandwidth == 20){
        operationParam.channelWidth = WIFI_CHANNELBANDWIDTH_20MHZ;
    }else if (radio_parameter.bandwidth == 40){
        operationParam.channelWidth = WIFI_CHANNELBANDWIDTH_40MHZ;
    }else if (radio_parameter.bandwidth == 80){
        operationParam.channelWidth = WIFI_CHANNELBANDWIDTH_80MHZ;
    }else if (radio_parameter.bandwidth == 160){
        operationParam.channelWidth = WIFI_CHANNELBANDWIDTH_160MHZ;
    }

    // htmode
    unsigned int mode = 0;      // enum wifi_ieee80211Variant_t
    if (strcmp(radio_parameter.band, "2g") == 0) {
        mode |= WIFI_80211_VARIANT_B | WIFI_80211_VARIANT_G;
        if (strcmp(radio_parameter.htmode, "NOHT") == 0 || strcmp(radio_parameter.htmode, "NONE") == 0)
            strcpy(radio_parameter.htmode, "11G");

        if (strstr(radio_parameter.htmode, "HE") != NULL)
            mode |= WIFI_80211_VARIANT_N | WIFI_80211_VARIANT_AX;

    } else if (strcmp(radio_parameter.band, "5g") == 0) {
        mode |= WIFI_80211_VARIANT_A;
        if (strcmp(radio_parameter.htmode, "NOHT") == 0 || strcmp(radio_parameter.htmode, "NONE") == 0) 
            strcpy(radio_parameter.htmode, "11A");

        if (strstr(radio_parameter.htmode, "HE") != NULL)
            mode |= WIFI_80211_VARIANT_N | WIFI_80211_VARIANT_AC | WIFI_80211_VARIANT_AX;
    }else if (strcmp(radio_parameter.band, "6g") == 0) {
        mode |= WIFI_80211_VARIANT_A | WIFI_80211_VARIANT_N | WIFI_80211_VARIANT_AC | WIFI_80211_VARIANT_AX;;
    }    

    if (strstr(radio_parameter.htmode, "VHT") != NULL)
        mode |= WIFI_80211_VARIANT_N | WIFI_80211_VARIANT_AC;
    else if (strstr(radio_parameter.htmode, "HT") != NULL && strstr(radio_parameter.htmode, "NO") == NULL)
        mode |= WIFI_80211_VARIANT_N;

    operationParam.variant = mode;

    // rtsThreshold, zero means not set
    if ((radio_parameter.rtsThreshold < 65535) && radio_parameter.rtsThreshold)
        operationParam.rtsThreshold = radio_parameter.rtsThreshold;

    //ht_coex
    operationParam.obssCoex = radio_parameter.ht_coex;

    // apply setting
    ret = wifi_setRadioOperatingParameters(radio_parameter.radio_index, &operationParam);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Apply setting failed!!!]\n");

    // Country
    fprintf(stderr, "Set Country: %s\n", radio_parameter.country);
    ret = wifi_setRadioCountryCode(radio_parameter.radio_index, radio_parameter.country);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Set Country failed!!!]\n");
    ret = 0;

    // hwmode
    fprintf(stderr, "Set hwmode: %s\n", radio_parameter.hwmode);
    ret = wifi_setRadioHwMode(radio_parameter.radio_index, radio_parameter.hwmode);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Set hwmode failed!!!]\n");
    ret = 0;

    // noscan
    fprintf(stderr, "Set noscan: %s \n", radio_parameter.noscan);
    if(strlen(radio_parameter.noscan)){
        ret = wifi_setNoscan(radio_parameter.radio_index, radio_parameter.noscan);
        if (ret != RETURN_OK)
            fprintf(stderr, "[Set noscan failed!!!]\n");
    }
    ret = 0;

}

void set_ap_param(wifi_intf_param ap_param , wifi_vap_info_map_t *map)
{
    int ret = 0;
    int vap_index_in_map = 0;
    int phy_index = 0;
    int key_len = 0;
    wifi_vap_info_t vap_info = {0};
    BOOL radio_enable = FALSE;

    if(ap_param.radio_index == -1)
        return;

    wifi_getRadioEnable(ap_param.radio_index, &radio_enable);
    if (radio_enable == FALSE)
        return;


    // get the index of the map
    for (int i = 0; i < map->num_vaps; i++) {
        if (map->vap_array[i].vap_index == ap_param.ap_index) {
            vap_index_in_map = i;
            break;
        }
    }


    fprintf(stderr, "Start setting ap\n");

    vap_info = map->vap_array[vap_index_in_map];
    vap_info.u.bss_info.enabled = TRUE;
    phy_index = radio_index_to_phy(vap_info.radio_index);
    if (set_interface_bssid(phy_index, ap_param.mac_offset, &vap_info.u.bss_info.bssid) == -1) {
        fprintf(stderr, "Get mac address failed.\n");
        return;
    }

    // SSID
    strncpy(vap_info.u.bss_info.ssid, ap_param.ssid, 33);
    vap_info.u.bss_info.ssid[32] = '\0';

    // interface
    if (strlen(ap_param.ifname) != 0) {
        strncpy(vap_info.vap_name, ap_param.ifname, 16);
        vap_info.vap_name[15] = "\0";
    }

    // Security
    if (ap_param.security.mode == wifi_security_mode_wpa3_personal || ap_param.security.mode == wifi_security_mode_wpa3_transition){
        // OpenWrt script only set psk, here we choose to set both psk and sae.
        ap_param.security.u.key.type = wifi_security_key_type_psk_sae;
    } else {
        key_len = strlen(ap_param.security.u.key.key);
        if (key_len == 64)
            ap_param.security.u.key.type = wifi_security_key_type_psk;
        else if (key_len >= 8 && key_len < 64)
            ap_param.security.u.key.type = wifi_security_key_type_pass;
    }

    vap_info.u.bss_info.security.mode = ap_param.security.mode;
    vap_info.u.bss_info.security.encr = ap_param.security.encr;
    vap_info.u.bss_info.security.mfp = ap_param.security.mfp;
    vap_info.u.bss_info.security.u.key.type = ap_param.security.u.key.type;
    strncpy(vap_info.u.bss_info.security.u.key.key, ap_param.security.u.key.key, 64);

    // hidden
    vap_info.u.bss_info.showSsid = (ap_param.hidden ? 0 : 1);

    // Replace the setting with uci config
    map->vap_array[vap_index_in_map] = vap_info;
}

void set_sta_param(wifi_intf_param sta_param)
{
    wifi_sta_network_t *sta = NULL;
    mac_address_t sta_mac = {0};
    char sta_mac_str[20] = {0};
    char key_mgmt[16] = {0};
    char pairwise[16] = {0};
    int phy_index = 0;

    sta = calloc(1, sizeof(wifi_sta_network_t));

    phy_index = radio_index_to_phy(sta_param.radio_index);
    set_interface_bssid(phy_index, sta_param.mac_offset, &sta_mac);
    mac_addr_ntoa(sta_mac_str, sta_mac);
    snprintf(sta->ssid, 31, "%s", sta_param.ssid);
    sta->ssid[31] = '\0';
    snprintf(sta->psk, 64, "%s", sta_param.password);

    if (sta_param.security.mode == wifi_security_mode_none)
        strcpy(key_mgmt, "NONE");
    else if (sta_param.security.mode == wifi_security_mode_wpa3_personal)
        strcpy(key_mgmt, "SAE");
    else if (sta_param.security.mode == wifi_security_mode_owe)
        strcpy(key_mgmt, "OWE");
    else
        strcpy(key_mgmt, "WPA-PSK");
    snprintf(sta->key_mgmt, 64, "%s", key_mgmt);

    if (sta_param.security.encr == wifi_encryption_aes)
        strcpy(pairwise, "CCMP");
    else if (sta_param.security.encr == wifi_encryption_tkip)
        strcpy(pairwise, "TKIP");
    else
        strcpy(pairwise, "CCMP TKIP");
    snprintf(sta->pairwise, 64, "%s", pairwise);

    if (strlen(sta_param.security.u.key.key) > 0)
        strncpy(sta->psk, sta_param.security.u.key.key, 127);
    sta->psk[127] = '\0';
    sta->psk_len = strlen(sta->psk);

    if (sta_param.wds_mode == TRUE)
        sta->flags |= WIFI_STA_NET_F_4ADDR_MULTI_AP;

    wifi_createSTAInterface(sta_param.sta_index, sta_mac_str, sta_param.wds_mode);

    if (wifi_setSTANetworks(sta_param.sta_index, &sta, 1, FALSE) == RETURN_ERR) {
        fprintf(stderr, "Write to sta %d config file failed\n", sta_param.sta_index);
        free(sta);
        return;
    }
    free(sta);

    if (wifi_setSTAEnabled(sta_param.sta_index, TRUE) == RETURN_ERR) {
        fprintf(stderr, "Enable station failed\n");
        return;
    }
}

int apply_uci_config ()
{
    struct uci_context *uci_ctx = uci_alloc_context();
    struct uci_package *uci_pkg = NULL;
    struct uci_element *e;
    // struct uci_section *s;
    const char cfg_name[] = "wireless";
    int max_radio_num = 0;
    BOOL parsing_radio = FALSE;
    int apCount[3] = {0};
    int staCount[3] = {0};
    wifi_vap_info_map_t vap_map[3] = {0};
    int ret = 0;
    int i = 0;

    wifi_getMaxRadioNumber(&max_radio_num);
    fprintf(stderr, "max radio number: %d\n", max_radio_num);
    for (i = 0; i < max_radio_num ;i++ ){
        ret = wifi_getRadioVapInfoMap(i, &vap_map[i]);
        if (ret != RETURN_OK) {     // if failed, we set assume this vap as the first vap.
            fprintf(stderr, "[Get vap map failed!!!]\n");
            vap_map[i].num_vaps = MAX_NUM_VAP_PER_RADIO;
        } 
    }
    if (uci_load(uci_ctx, cfg_name, &uci_pkg) != UCI_OK) {
        uci_free_context(uci_ctx);
        fprintf(stderr, "%s: load uci failed.\n", __func__);
        return RETURN_ERR;
    }

    uci_foreach_element(&uci_pkg->sections, e) {

        struct uci_section *s = uci_to_section(e);
        struct uci_element *option = NULL;
        wifi_radio_param radio_param = {0};
        wifi_intf_param intf_param = {0};
        int phyId = 0;
        radio_param.radio_index = -1;
        intf_param.ap_index = -1;

        if (strcmp(s->type, "wifi-device") == 0) {
            sscanf(s->e.name, "radio%d", &phyId);
            radio_param.radio_index = phy_index_to_radio(phyId);
            parsing_radio = TRUE;
            fprintf(stderr, "\n----- Start parsing radio %d config. -----\n", radio_param.radio_index);
        } else if (strcmp(s->type, "wifi-iface") == 0) {
            parsing_radio = FALSE;
        }

        uci_foreach_element(&s->options, option) {

            struct uci_option *op = uci_to_option(option);
            if (parsing_radio == TRUE) {
                // transform the type from input string and store the value in radio_param.
                if (strcmp(op->e.name, "channel") == 0)
                    set_channel(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "hwmode") == 0)
                    set_hwmode(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "htmode") == 0)
                    set_htmode(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "disabled") == 0)
                    set_disable(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "band") == 0)
                    set_band(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "country") == 0)
                    set_country(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "noscan") == 0)
                    set_noscan(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "rxantenna") == 0)
                    set_rxant(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "txantenna") == 0)
                    set_txant(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "ht_coex") == 0)
                    set_htcoex(&radio_param, op->v.string);
                else if (strcmp(op->e.name, "rts") == 0)
                    set_rts(&radio_param, op->v.string);
                else
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
            } else {        
                // parsing iface
                if (strcmp(op->e.name, "device") == 0){
                    set_radionum(&intf_param, op->v.string);
                }else if (strcmp(op->e.name, "mode") == 0){
                    intf_param.mac_offset = staCount[intf_param.radio_index] + apCount[intf_param.radio_index];
                    if (strncmp(op->v.string, "sta", 3) == 0) {
                        intf_param.sta_mode = TRUE;
                        intf_param.sta_index = intf_param.radio_index + staCount[intf_param.radio_index]*max_radio_num;
                        staCount[intf_param.radio_index] ++ ;
                        fprintf(stderr, "\n----- Start parsing sta %d config. -----\n", intf_param.sta_index);
                    } else if (strncmp(op->v.string, "ap", 2) == 0) {
                        intf_param.sta_mode = FALSE;
                        intf_param.ap_index = intf_param.radio_index + apCount[intf_param.radio_index]*max_radio_num;
                        apCount[intf_param.radio_index] ++ ;
                        fprintf(stderr, "\n----- Start parsing ap %d config. -----\n", intf_param.ap_index);
                    }
                }else if (strcmp(op->e.name, "ssid") == 0){
                    set_ssid(&intf_param, op->v.string);
                }else if (strcmp(op->e.name, "encryption") == 0){
                    set_encryption(&intf_param, op->v.string);
                }else if (strcmp(op->e.name, "key") == 0){
                    set_key(&intf_param, op->v.string);
                }else if (strcmp(op->e.name, "ifname") == 0){
                    set_ifname(&intf_param, op->v.string);
                }else if (strcmp(op->e.name, "wds") == 0){
                    set_wds(&intf_param, op->v.string);
                }else if (strcmp(op->e.name, "hidden") == 0){
                    set_hidden(&intf_param, op->v.string);
                }else{
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
                }    
            }
        }
        if (parsing_radio == TRUE)
            set_radio_param(radio_param);
        else if (intf_param.sta_mode == TRUE)
            set_sta_param(intf_param);
        else
            set_ap_param(intf_param, &vap_map[intf_param.radio_index]);
    }
    fprintf(stderr, "\n----- Start setting Vaps. -----\n");

    for (i = 0; i < max_radio_num ;i++ ){
        ret = wifi_createVAP(i, &vap_map[i]);
        if (ret != RETURN_OK)
            fprintf(stderr, "[Apply vap setting failed!!!]\n");
    }
    
    uci_unload(uci_ctx, uci_pkg);
    uci_free_context(uci_ctx);
    return RETURN_OK;
}

int main(int argc, char **argv)
{
    if (argc != 2 || strcmp(argv[1], "reload") != 0) {
        fprintf(stderr, "Usage: wifi reload.\nThis tool is only for RDKB MSP/SQC test.\n");
        return -1;
    }
    apply_uci_config();
    return 0;
}
