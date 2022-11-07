SRC_URI += "git://gerrit.mediatek.inc/gateway/rdk-b/rdkb_hal;protocol=https;destsuffix=git/source/wifi/rdkb_hal"

SRCREV = "${AUTOREV}"

CFLAGS_append = " -DWIFI_HAL_VERSION_3"

do_configure_prepend(){
    rm ${S}/wifi_hal.c
    ln -sf ${S}/rdkb_hal/src/wifi/wifi_hal.c ${S}/wifi_hal.c
}
