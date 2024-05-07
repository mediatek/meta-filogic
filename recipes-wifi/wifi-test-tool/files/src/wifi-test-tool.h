#include "uci_config.h"
#include "wifi_hal.h"

typedef enum {
    WIFI_MODE_A = 0x01,
    WIFI_MODE_B = 0x02,
    WIFI_MODE_G = 0x04,
    WIFI_MODE_N = 0x08,
    WIFI_MODE_AC = 0x10,
    WIFI_MODE_AX = 0x20,
} wifi_ieee80211_Mode;

typedef struct {
    int radio_index;
    char type[16];
    char phy[16];
    char macaddr[18];
    BOOL disabled;
    int channel;
    BOOL auto_channel;
    char channels[64];      // list type
    char hwmode[2];
    char band[8];
    char htmode[16];
    int bandwidth;
    int eht_320_conf;
    int pure_mode;
    int chanbw;
    char ht_capab[8];
    int txpower;
    BOOL diversity;
    int rxantenna;
    int txantenna;
    char country[3];
    BOOL country_ie;
    int distance;
    int beacon_int;
    BOOL legacy_rates;
    char require_mode[2];
    int cell_density;
    char basic_rate[64];
    char supported_rates[64];
    int log_level;
    char hostapd_options[64];
    char noscan[2];
    unsigned int rtsThreshold;
    BOOL ht_coex;
    BOOL background_radar;
    UCHAR he_bss_color;
    ULONG transmit_power;
} wifi_radio_param;

typedef struct {
    wifi_radio_param *radio_info;   // for multiple vap
    bool sta_mode;
    bool wds_mode;
    int radio_index;
    int ap_index;
    int sta_index;
    char ssid[33];
    int wpa;
    char password[64];
    wifi_vap_security_t security;
    int mac_offset;
    char ifname[16];
    bool hidden;
    bool igmpsn_enable;
    int wps_state;
    bool wps_pushbutton;
    char macfilter[10];
    char maclist[512];
    char mac_address[20];
} wifi_intf_param;

struct params
{
    char * name;
    char * value;
};

INT wifi_getMaxRadioNumber(INT *max_radio_num);
