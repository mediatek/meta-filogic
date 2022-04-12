DESCRIPTION = "Mediatek Wireless Drivers"
SECTION = "kernel/modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

inherit module

PV = "1.0"

SRCREV ?= "053668acdaf83d0350ce3e21720e20eac9fbbcd7"
SRC_URI = " \
    git://git@github.com/openwrt/mt76.git;protocol=https \
    file://COPYING;subdir=git \
    "

DEPENDS += "virtual/kernel"
DEPENDS += "linux-mac80211"

FILESEXTRAPATHS_prepend := "${THISDIR}/files/patches:"

require files/patches/patches.inc

SRC_URI_append += " \
    file://100-Revert-of-net-pass-the-dst-buffer-to-of_get_mac_addr.patch \
    "
S = "${WORKDIR}/git"

NOSTDINC_FLAGS = " \
    -I${B} \
    -I${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211-backport/uapi \
    -I${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211-backport \
    -I${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211/uapi \
    -I${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211 \
    -include backport/autoconf.h \
    -include backport/backport.h \
    "

PKG_MAKE_FLAGS = " \
    CONFIG_MAC80211_DEBUGFS=y \
    CONFIG_NL80211_TESTMODE=y \
    CONFIG_MT76_CONNAC_LIB=m \
    CONFIG_MT7615_COMMON=m \
    CONFIG_MT7915E=m \
    CONFIG_MT7986_WMAC=y \
    "

NOSTDINC_FLAGS += " \
    -DCONFIG_MAC80211_MESH \
    -DCONFIG_NL80211_TESTMODE \
    -DCONFIG_MAC80211_DEBUGFS \
    -DCONFIG_MT7986_WMAC \
    "

EXTRA_OEMAKE = " \
    -C ${STAGING_KERNEL_BUILDDIR}/ \
    M=${S} \
    ${PKG_MAKE_FLAGS} \
    NOSTDINC_FLAGS="${NOSTDINC_FLAGS}" \
    "

MAKE_TARGETS = "modules"

do_configure[noexec] = "1"

# make_scripts requires kernel source directory to create
# kernel scripts
do_make_scripts[depends] += "virtual/kernel:do_shared_workdir"

do_install() {
    # Module
    install -d ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -d ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/mt7615/
    install -d ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/mt7915/
    install -m 0644 ${B}/mt76.ko ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt76-connac-lib.ko ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt7615/mt7615-common.ko ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/mt7615/
    install -m 0644 ${B}/mt7915/mt7915e.ko ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/mt7915/
}

KERNEL_MODULE_AUTOLOAD += "mt7915e"
