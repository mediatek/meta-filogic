FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "git://git01.mediatek.com/filogic/rdk-b/rdkb_hal;branch=master;protocol=https;destsuffix=git/source/platform/rdkb_hal"


SRCREV = "${AUTOREV}"

DEPENDS += "utopia-headers"
CFLAGS:append = " \
    -I=${includedir}/utctx \
"

do_configure:prepend(){
    rm ${S}/platform_hal.c
    ln -sf ${S}/rdkb_hal/src/platform/platform_hal.c ${S}/platform_hal.c
}