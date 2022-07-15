FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "git://gerrit.mediatek.inc/gateway/rdk-b/rdkb_hal;protocol=https;destsuffix=git/source/platform/rdkb_hal"


SRCREV = "${AUTOREV}"

DEPENDS += "utopia-headers"
CFLAGS_append = " \
    -I=${includedir}/utctx \
"

do_configure_prepend(){
    rm ${S}/platform_hal.c
    ln -sf ${S}/rdkb_hal/src/platform/platform_hal.c ${S}/platform_hal.c
}