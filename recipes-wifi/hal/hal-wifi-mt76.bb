SUMMARY = "WIFI HAL for RDK CCSP components"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://../../LICENSE;md5=175792518e4ac015ab6696d16c4f607e"

PROVIDES = "hal-wifi"
RPROVIDES:${PN} = "hal-wifi"
SRCREV = "${AUTOREV}"

inherit autotools coverity

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "git://gerrit.mediatek.inc/gateway/rdk-b/rdkb_hal;protocol=https;branch=master;destsuffix=git/ \
        file://LICENSE;subdir=git \
        "
SRCREV_wifihal = "${AUTOREV}"
SRCREV_FORMAT = "wifihal"

PV = "${RDK_RELEASE}+git${SRCPV}"
S = "${WORKDIR}/git/src/wifi/"

DEPENDS += "rdk-wifi-halif libnl libev hostapd wpa-supplicant"
CFLAGS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'extender', '-D_TURRIS_EXTENDER_', '', d)}"
CFLAGS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', '-DSINGLE_WIPHY_SUPPORT', '', d)}"
CFLAGS:append = " -I=${includedir}/ccsp -I=${includedir}/libnl3"
CFLAGS:append = " -DWIFI_HAL_VERSION_3 -DDYNAMIC_IF_NAME"
LDFLAGS:append = " -lnl-nf-3 -lnl-route-3 -lnl-3 -lnl-xfrm-3 -lnl-genl-3 -lev -lwpa_client -lpthread"

RDEPENDS:${PN}_dunfell += " wpa-supplicant"
RDEPENDS:${PN} += "wpa-supplicant"
