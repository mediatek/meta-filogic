FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:remove = "${CMF_GITHUB_ROOT}/hardware-abstraction-layer;protocol=https;${BRANCH_hardware_abstraction_layer};name=platformhal"
SRC_URI = "${CMF_GITHUB_ROOT}/hardware-abstraction-layer;protocol=https;${BRANCH_hardware_abstraction_layer};destsuffix=${BP};name=platformhal"
SRC_URI += "git://github.com/mediatek/rdkb_hal;branch=main;protocol=https;destsuffix=${BP}/source/platform/rdkb_hal"

SRCREV = "${AUTOREV}"
S = "${UNPACKDIR}/${BP}/source/platform"
DEPENDS += "utopia-headers"
CFLAGS:append = " \
    -I=${includedir}/utctx \
"

do_configure:prepend(){
    rm ${S}/platform_hal.c
    ln -sf ${S}/rdkb_hal/src/platform/platform_hal.c ${S}/platform_hal.c
}
