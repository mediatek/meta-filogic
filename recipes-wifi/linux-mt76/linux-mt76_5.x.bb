DESCRIPTION = "Mediatek Wireless Drivers"
SECTION = "kernel/modules"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

inherit module

PKG_YEAR = "26"
PKG_MONTH = "11"

SRC_URI = " \
    git://github.com/mediatek/softmac_mt76.git;protocol=https;branch=wifi8-6.12-alpha;destsuffix=${BPN}-${PV} \
    "
SRC_URI:append = " \
    file://COPYING;subdir=${BP} \
    "
SRCREV = "${AUTOREV}"
DEPENDS += "virtual/kernel"
DEPENDS += "linux-mac80211"

S = "${UNPACKDIR}/${BPN}-${PV}"
ERROR_QA:remove = "patch-fuzz patch-status"
ADDITIONAL_CFLAGS = "-DPKG_YEAR=${PKG_YEAR} -DPKG_MONTH=${PKG_MONTH}"

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
    CONFIG_MT7999E=m \
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

do_fix_compiler_types() {
    sed -i '/#ifdef CONFIG_CC_HAS_COUNTED_BY/{n;s/.*/#ifndef __counted_by\n&\n#endif/}' "${STAGING_KERNEL_DIR}/include/linux/compiler_types.h"
}
addtask fix_compiler_types after do_patch before do_compile
do_fix_compiler_types[depends] += "virtual/kernel:do_shared_workdir"
# make_scripts requires kernel source directory to create
# kernel scripts
do_make_scripts[depends] += "virtual/kernel:do_shared_workdir"

do_install() {
    # Module
    install -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt76.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt76-connac-lib.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
    install -m 0644 ${B}/mt7999/mt7999e.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/updates/drivers/net/wireless/mediatek/mt76/
}

do_install:append () {
    install -d ${D}/${base_libdir}/firmware/mediatek/mt7999

    install -m 644 ${UNPACKDIR}/${BPN}-${PV}/firmware/mt7999/* ${D}${base_libdir}/firmware/mediatek/mt7999
}


FILES:${PN} += "${base_libdir}/firmware/mediatek/*"

# Make linux-mt76 depend on all of the split-out packages.
python populate_packages:prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS:linux-mt76', ' ' + ' '.join(firmware_pkgs))
}

#RPROVIDES:${PN} += "kernel-module-${PN}-${KERNEL_VERSION}"
#RPROVIDES:${PN} += "kernel-module-${PN}-connac-lib-${KERNEL_VERSION}"

KERNEL_MODULE_AUTOLOAD += "mt7999e"
INSANE_SKIP:${PN} += "installed-vs-shipped"
