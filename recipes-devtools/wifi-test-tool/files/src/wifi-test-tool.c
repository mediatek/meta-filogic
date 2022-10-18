#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <uci.h>
#include "wifi-test-tool.h"


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
    fprintf(stderr, "%s:  radio index = %d \n", __func__, radioIndex);
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

void set_radionum(wifi_ap_param *ap_param, char *radio_name)
{
    int radio_num;
    char *ptr = radio_name;
    int phyId = 0;

    while (*ptr) {
        if (isdigit(*ptr)) {
            radio_num = strtoul(ptr, NULL, 10);
            phyId = phy_index_to_radio(radio_num);
            ap_param->radio_index = phyId;
            break;
        }
        ptr++;
    }
}

void set_ssid(wifi_ap_param *ap_param, char *ssid)
{
    strncpy(ap_param->ssid, ssid, 32);
}

void set_encryption(wifi_ap_param *ap_param, char *encryption_mode)
{
    if (strcmp(encryption_mode, "none") == 0) {
        ap_param->security.mode = wifi_security_mode_none;
        ap_param->security.encr = wifi_encryption_none;
    }else if(strncmp(encryption_mode, "psk2", 4) == 0){
        ap_param->security.mode = wifi_security_mode_wpa2_personal;
    }else if(strncmp(encryption_mode, "psk-",4) == 0){
       ap_param->security.mode = wifi_security_mode_wpa_wpa2_personal;
    }else if(strncmp(encryption_mode, "psk",3) == 0){
        ap_param->security.mode = wifi_security_mode_wpa_personal;
    }else if(strncmp(encryption_mode, "wpa2",4) == 0){
        ap_param->security.mode = wifi_security_mode_wpa2_enterprise;
    }else if(strncmp(encryption_mode, "wpa-",4) == 0){
        ap_param->security.mode = wifi_security_mode_wpa_wpa2_enterprise;
    }else if(strcmp(encryption_mode, "sae") == 0){
        ap_param->security.mode = wifi_security_mode_wpa3_personal;
    }else if(strcmp(encryption_mode, "wpa3") == 0){
        ap_param->security.mode = wifi_security_mode_wpa3_enterprise;
    }else if(strcmp(encryption_mode, "sae-mixed") == 0){
        ap_param->security.mode = wifi_security_mode_wpa3_transition;
    }

    if(strstr(encryption_mode, "tkip") && (strstr(encryption_mode, "ccmp") || strstr(encryption_mode, "aes") )){
        ap_param->security.encr = wifi_encryption_aes_tkip;
    }else if (strstr(encryption_mode, "tkip")){
        ap_param->security.encr = wifi_encryption_tkip;
    }else{
        ap_param->security.encr = wifi_encryption_aes;
    }

    if(!strcmp(encryption_mode, "wpa3") || !strcmp(encryption_mode, "sae")){
        ap_param->security.mfp = wifi_mfp_cfg_required;
    }else if (!strcmp(encryption_mode, "sae-mixed")){
        ap_param->security.mfp = wifi_mfp_cfg_optional;
    }else{
        ap_param->security.mfp = wifi_mfp_cfg_disabled;
    }

    if (!strcmp(encryption_mode, "sae")){
        ap_param->security.u.key.type = wifi_security_key_type_sae;
    }else if (!strcmp(encryption_mode, "sae-mixed")){
        ap_param->security.u.key.type = wifi_security_key_type_psk_sae;
    }else{
        ap_param->security.u.key.type = wifi_security_key_type_psk;
    }

}

void set_key(wifi_ap_param *ap_param, char *key)
{
    strncpy(ap_param->security.u.key.key, key, 64);
}

int set_ap_bssid(int radio_index, int offset, mac_address_t *bssid)
{
    FILE *f;
    char mac_file[64] = {0};
    char mac_address[20] = {0};
    char *tmp = NULL;

    sprintf(mac_file, "/sys/class/net/wlan%d/address", radio_index);
    f = fopen(mac_file, "r");
    if (f == NULL)
        return -1;
    fgets(mac_address, 20, f);
    fclose(f);

    sscanf(mac_address, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &(*bssid)[0], &(*bssid)[1], &(*bssid)[2], &(*bssid)[3], &(*bssid)[4], &(*bssid)[5]);
    (*bssid)[0] += (offset + 1)*2;
    return 0;
}

void set_radio_param(wifi_radio_param radio_parameter)
{
    BOOL enable;
    BOOL current;
    int ret = 0;
    struct params param;
    wifi_radio_operationParam_t operationParam = {0};

    if(radio_parameter.radio_index == -1)
        return;

    if (radio_parameter.disabled == TRUE) {
        wifi_setRadioEnable(radio_parameter.radio_index, FALSE);
        return;
    }
    operationParam.enable = TRUE;

    fprintf(stderr, "Start setting radio\n");
    ret = wifi_getRadioOperatingParameters(radio_parameter.radio_index, &operationParam);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Get OperatingParameters failed!!!]\n");

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

    // noscan
    fprintf(stderr, "Set noscan: %s\n", radio_parameter.noscan);
    ret = wifi_setNoscan(radio_parameter.radio_index, radio_parameter.noscan);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Set noscan failed!!!]\n");
    ret = 0;

    // apply setting
    ret = wifi_setRadioOperatingParameters(radio_parameter.radio_index, &operationParam);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Apply setting failed!!!]\n");

}

void set_ap_param(wifi_ap_param ap_param)
{
    int ret = 0;
    int vap_index_in_map = 0;
    wifi_vap_info_t vap_info = {0};
    wifi_vap_info_map_t vap_map = {0};

    if(ap_param.radio_index == -1)
        return;
    ret = wifi_getRadioVapInfoMap(ap_param.radio_index, &vap_map);
    if (ret != RETURN_OK) {     // if failed, we set assume this vap as the first vap.
        fprintf(stderr, "[Get vap map failed!!!]\n");
        vap_map.num_vaps = MAX_NUM_VAP_PER_RADIO;
    } else {                    // get the index of the map
        for (int i = 0; i < vap_map.num_vaps; i++) {
            if (vap_map.vap_array[i].vap_index == ap_param.ap_index) {
                vap_index_in_map = i;
                break;
            }
        }
    }

    fprintf(stderr, "Start setting ap\n");

    vap_info = vap_map.vap_array[vap_index_in_map];
    vap_info.u.bss_info.enabled = TRUE;
    if (set_ap_bssid(vap_info.radio_index, vap_index_in_map, &vap_info.u.bss_info.bssid) == -1) {
        fprintf(stderr, "Get mac address failed.\n");
        return -1;
    }

    // SSID
    strncpy(vap_info.u.bss_info.ssid, ap_param.ssid, 33);
    vap_info.u.bss_info.ssid[32] = '\0';

    vap_info.u.bss_info.security.mode = ap_param.security.mode;
    vap_info.u.bss_info.security.encr = ap_param.security.encr;
    vap_info.u.bss_info.security.mfp = ap_param.security.mfp;
    vap_info.u.bss_info.security.u.key.type = ap_param.security.u.key.type;
    strncpy(vap_info.u.bss_info.security.u.key.key, ap_param.security.u.key.key, 64);
    

    // Replace the setting with uci config
    vap_map.vap_array[vap_index_in_map] = vap_info;
    ret = wifi_createVAP(ap_param.radio_index, &vap_map);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Apply vap setting failed!!!]\n");

    // restart ap
    wifi_setApEnable(ap_param.ap_index, FALSE);
    wifi_setApEnable(ap_param.ap_index, TRUE);
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

    wifi_getMaxRadioNumber(&max_radio_num);
    fprintf(stderr, "max radio number: %d\n", max_radio_num);
    if (uci_load(uci_ctx, cfg_name, &uci_pkg) != UCI_OK) {
        uci_free_context(uci_ctx);
        fprintf(stderr, "%s: load uci failed.\n", __func__);
        return RETURN_ERR;
    }

    uci_foreach_element(&uci_pkg->sections, e) {

        struct uci_section *s = uci_to_section(e);
        struct uci_element *option = NULL;
        wifi_radio_param radio_param = {0};
        wifi_ap_param ap_param = {0};
        int phyId = 0;
        radio_param.radio_index = -1;
        ap_param.ap_index = -1;

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
                else
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
            } else {        
                // parsing iface
                if (strcmp(op->e.name, "device") == 0){
                    set_radionum(&ap_param, op->v.string);
                    if (ap_param.radio_index != -1){
                        ap_param.ap_index = ap_param.radio_index + apCount[ap_param.radio_index]*max_radio_num;
                        fprintf(stderr, "\n----- Start parsing ap %d config. -----\n", ap_param.ap_index);
                        apCount[ap_param.radio_index] ++ ;
                    }   
                }else if (strcmp(op->e.name, "ssid") == 0){
                    set_ssid(&ap_param, op->v.string);
                }else if (strcmp(op->e.name, "encryption") == 0){
                    set_encryption(&ap_param, op->v.string);
                }else if (strcmp(op->e.name, "key") == 0){
                    set_key(&ap_param, op->v.string);
                }else{
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
                }    
            }
        }
        if (parsing_radio == TRUE)
            set_radio_param(radio_param);
        else
            set_ap_param(ap_param);
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
