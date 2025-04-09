DESCRIPTION = "wifi-test-tool"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=fcf339a1c4fb17c1e878859b11a2cdba"

DEPENDS += "libnl-tiny hal-wifi uci"

FILESEXTRAPATHS:prepend := "${THISDIR}/files/wmm_script:"
inherit autotools coverity

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    file://wmm_script;subdir=git \
    "


S = "${WORKDIR}/git/src"

CFLAGS:append = " -DWIFI_HAL_VERSION_3 -DMTK_UCI_SUPPORT -luci "
CFLAGS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', '-DSINGLE_WIPHY_SUPPORT', '', d)}"
CFLAGS:append = " -I=${includedir}/ccsp "
do_install:append() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/build/wifi_test_tool ${D}${sbindir}/wifi
    install -m 0755 ${WORKDIR}/git/wmm_script/wmm-*.sh ${D}${sbindir}
}
