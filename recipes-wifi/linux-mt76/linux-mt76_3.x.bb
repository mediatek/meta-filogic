DESCRIPTION = "Mediatek Wireless Drivers"
SECTION = "kernel/modules"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

inherit module

PKG_YEAR="24"
PKG_MONTH="10"

require mt76-3x.inc
SRC_URI = " \
    git://git@github.com/openwrt/mt76.git;protocol=https;branch=master \
    file://COPYING;subdir=git \
    "

DEPENDS += "virtual/kernel"
DEPENDS += "linux-mac80211"

PATCH_SRC = "${@bb.utils.contains('DISTRO_FEATURES', 'kernel6-6', 'kernel6-6-patches', 'patches-${PV}', d)}"
FW_SRC = "${@bb.utils.contains('DISTRO_FEATURES', 'kernel6-6', 'kernel6-6-src', 'src', d)}"

SRC_URI += " \
    file://${FW_SRC} \
    "

FILESEXTRAPATHS:prepend := "${THISDIR}/files/${PATCH_SRC}:"
FILESEXTRAPATHS:prepend := "${THISDIR}/${FW_SRC}:"

require files/${PATCH_SRC}/patches.inc

S = "${WORKDIR}/git"

ADDITIONAL_CFLAGS="-DPKG_YEAR=${PKG_YEAR} -DPKG_MONTH=${PKG_MONTH}"

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
    CONFIG_MT7996E=m \
    "

NOSTDINC_FLAGS += " \
    -DCONFIG_MAC80211_MESH \
    -DCONFIG_NL80211_TESTMODE \
    -DCONFIG_MAC80211_DEBUGFS \
    "

EXTRA_OEMAKE = " \
    -C ${STAGING_KERNEL_BUILDDIR}/ \
    M=${S} \
    ${PKG_MAKE_FLAGS} \
    NOSTDINC_FLAGS="${NOSTDINC_FLAGS}" \
    ADDITIONAL_CFLAGS="${ADDITIONAL_CFLAGS}" \
    "

MAKE_TARGETS = "modules"

do_configure[noexec] = "1"

# make_scripts requires kernel source directory to create
# kernel scripts
do_make_scripts[depends] += "virtual/kernel:do_shared_workdir"

do_install() {
    # Module
    install -d ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt76.ko ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt76-connac-lib.ko ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt7996/mt7996e.ko ${D}/lib/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
}

do_install:append () {
    install -d ${D}/${base_libdir}/firmware/mediatek/mt7996
    IS_MT7996="${@bb.utils.contains('DISTRO_FEATURES','mt7996','true','false',d)}"
    IS_MT7992="${@bb.utils.contains('DISTRO_FEATURES','mt7992','true','false',d)}"
    IS_MT7990="${@bb.utils.contains('DISTRO_FEATURES','mt7990','true','false',d)}"
    if [ $IS_MT7996 = 'true' ]; then
            install -m 644 ${WORKDIR}/${FW_SRC}/firmware/mt7996/mt7996*.* ${D}${base_libdir}/firmware/mediatek/mt7996
    fi

    if [ $IS_MT7992 = 'true' ]; then
            install -m 644 ${WORKDIR}/${FW_SRC}/firmware/mt7996/mt7992*.* ${D}${base_libdir}/firmware/mediatek/mt7996
    fi

    if [ $IS_MT7990 = 'true' ]; then
            install -m 644 ${WORKDIR}/${FW_SRC}/firmware/mt7996/mt7990*.* ${D}${base_libdir}/firmware/mediatek/mt7996
    fi
}

do_install:append_mt7988 () {
    IS_KERNEL_V6="${@bb.utils.contains('DISTRO_FEATURES','kernel6-6','true','false',d)}"
    if [ $IS_KERNEL_V6 = 'false' ]; then
        install -d ${D}/${base_libdir}/firmware/mediatek/

        install -m 644 ${WORKDIR}/src/firmware/mtk_wo_0.bin ${D}${base_libdir}/firmware/mediatek/
        install -m 644 ${WORKDIR}/src/firmware/mtk_wo_1.bin ${D}${base_libdir}/firmware/mediatek/
        install -m 644 ${WORKDIR}/src/firmware/mtk_wo_2.bin ${D}${base_libdir}/firmware/mediatek/
    fi
}

FILES:${PN} += "${base_libdir}/firmware/mediatek/*"

# Make linux-mt76 depend on all of the split-out packages.
python populate_packages:prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS:linux-mt76', ' ' + ' '.join(firmware_pkgs))
}

#RPROVIDES:${PN} += "kernel-module-${PN}-${KERNEL_VERSION}"
#RPROVIDES:${PN} += "kernel-module-${PN}-connac-lib-${KERNEL_VERSION}"

KERNEL_MODULE_AUTOLOAD += "mt7996e"
