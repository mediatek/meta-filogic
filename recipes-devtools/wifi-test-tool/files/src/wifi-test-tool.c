#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <uci.h>
#include "wifi-test-tool.h"

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

    while (*ptr) {
        if (isdigit(*ptr)) {
            radio_num = strtoul(ptr, NULL, 10);
            ap_param->radio_index = radio_num;
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
    if (strstr(encryption_mode, "3") != NULL) {
        strcpy(ap_param->enctyption_mode, "WPA3-");
    } else if (strstr(encryption_mode, "2") != NULL) {
        strcpy(ap_param->enctyption_mode, "WPA2-");
    } else if (strstr(encryption_mode, "1") != NULL) {
        strcpy(ap_param->enctyption_mode, "WPA-");
    } else if (strstr(encryption_mode, "mix") != NULL) {
        strcpy(ap_param->enctyption_mode, "WPA-WPA2-");
    }

    if (strstr(encryption_mode, "psk") != NULL) {
        strcat(ap_param->enctyption_mode, "Personal");
    } else if (strstr(encryption_mode, "wpa") != NULL) {
        strcat(ap_param->enctyption_mode, "Enterprise");
    }
    
    if (strcmp(encryption_mode, "none") == 0) {
        strcpy(ap_param->enctyption_mode, "None");
    }
}

void set_key(wifi_ap_param *ap_param, char *key)
{
    strncpy(ap_param->password, key, 64);
}

void set_radio_param(wifi_radio_param radio_parameter)
{
    BOOL enable;
    BOOL current;
    int ret = 0;
    struct params param;
    wifi_radio_operationParam_t operationParam = {0};

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
    }

    if (strstr(radio_parameter.htmode, "VHT") != NULL)
        mode |= WIFI_80211_VARIANT_N | WIFI_80211_VARIANT_AC;
    else if (strstr(radio_parameter.htmode, "HT") != NULL && strstr(radio_parameter.htmode, "NO") == NULL)
        mode |= WIFI_80211_VARIANT_N;

    operationParam.variant = mode;

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

    // get current setting
    vap_info = vap_map.vap_array[vap_index_in_map];

    fprintf(stderr, "Start setting ap\n");
    // SSID
    strncpy(vap_info.u.bss_info.ssid, ap_param.ssid, 33);
    vap_info.u.bss_info.ssid[32] = '\0';

    // wpa and security mode
    fprintf(stderr, "Set encryption mode: %s\n", ap_param.enctyption_mode);
    ret = wifi_setApSecurityModeEnabled(ap_param.ap_index, ap_param.enctyption_mode);
    if (ret != RETURN_OK)
        fprintf(stderr, "[Set encryption mode failed!!!]\n");

    // key
    if (strlen(ap_param.password) > 0) {
        fprintf(stderr, "Set password: %s\n", ap_param.password);
        ret = wifi_setApSecurityKeyPassphrase(ap_param.ap_index, ap_param.password);
        if (ret != RETURN_OK)
            fprintf(stderr, "[Set password failed!!!]\n");
    }


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
        radio_param.radio_index = -1;
        ap_param.ap_index = -1;

        if (strcmp(s->type, "wifi-device") == 0) {
            sscanf(s->e.name, "radio%d", &radio_param.radio_index);
            parsing_radio = TRUE;
            fprintf(stderr, "\n----- Start parsing radio %d config. -----\n", radio_param.radio_index);
        } else if (strcmp(s->type, "wifi-iface") == 0) {
            sscanf(s->e.name, "default_radio%d", &ap_param.ap_index);
            parsing_radio = FALSE;
            fprintf(stderr, "\n----- Start parsing ap %d config. -----\n", ap_param.ap_index);
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
                    set_band(&radio_param, op->v.string);
                else
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
            } else {        
                // parsing iface
                if (strcmp(op->e.name, "device") == 0)
                    set_radionum(&ap_param, op->v.string);
                else if (strcmp(op->e.name, "ssid") == 0)
                    set_ssid(&ap_param, op->v.string);
                else if (strcmp(op->e.name, "encryption") == 0)
                    set_encryption(&ap_param, op->v.string);
                else if (strcmp(op->e.name, "key") == 0)
                    set_key(&ap_param, op->v.string);
                else
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
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
