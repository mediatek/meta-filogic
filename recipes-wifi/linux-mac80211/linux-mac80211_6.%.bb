DESCRIPTION = "Linux backports"
HOMEPAGE = "https://backports.wiki.kernel.org"
SECTION = "kernel/modules"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit module

PV = "6.12.6"
SHASUM-kernelv5 = "28ec39425a1b3270e1422d92a8131a6a3d8919cc13e8ee250c315e55d922ba68"

require version.inc
SRC_URI[sha256sum] = "${@bb.utils.contains('DISTRO_FEATURES', 'kernelv6', '${SHASUM-kernelv6}', '${SHASUM-kernelv5}', d)}"
SRC_URI = " \
    http://mirror2.openwrt.org/sources/backports-${PV}.tar.xz \
    file://config \
    file://0001-rdkb-fix_build_issue-mac80211-without_depmod.patch;apply=no \
    "


DEPENDS += "virtual/kernel"
DEPENDS += "bison-native coreutils-native flex-native"
PATCH_SRC = "${@bb.utils.contains('DISTRO_FEATURES', 'kernelv6', 'kernelv6-patches', 'patches-6.x', d)}"

FILESEXTRAPATHS:prepend := "${THISDIR}/files/${PATCH_SRC}/subsys:"

require files/${PATCH_SRC}/subsys/subsys.inc

S = "${WORKDIR}/backports-${PV}"

do_filogic_patches() {
    cd ${S}
    if [ ! -e patch_applied ]; then
        patch -p1 < ${WORKDIR}/0001-rdkb-fix_build_issue-mac80211-without_depmod.patch
        touch patch_applied
    fi
}
addtask filogic_patches after do_unpack before do_compile

EXTRA_OEMAKE = " \
    KLIB_BUILD=${STAGING_KERNEL_BUILDDIR} \
    KLIB=${D} \
    "

do_configure () {
    make CFLAGS="" CPPFLAGS="" CXXFLAGS="" LDFLAGS="" CC="${BUILD_CC}" \
         LD="${BUILD_LD}" AR="${BUILD_AR}" LEX="flex" -C ${S}/kconf O=${S}/kconf conf

    cp ${WORKDIR}/config ${S}/.config
    oe_runmake allnoconfig
}

do_install:prepend () {
    mkdir -p \
	${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211 \
	${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211-backport \
	${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211/ath \
	${STAGING_KERNEL_BUILDDIR}/usr/include/net/mac80211
    cp -Rf ${S}/net/mac80211/*.h ${S}/include/* ${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211/
    cp -Rf ${S}/backport-include/* ${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211-backport/
    cp -Rf ${S}/net/mac80211/rate.h ${STAGING_KERNEL_BUILDDIR}/usr/include/net/mac80211/
    cp -Rf ${S}/drivers/net/wireless/ath/*.h ${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211/ath/
    rm -f ${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211-backport/linux/module.h
}

do_install:append() {
    # Module
    cat ${D}/usr/include/linux-mac80211/Module.symvers >> ${TMPDIR}/work-shared/${MACHINE}/kernel-build-artifacts/Module.symvers
}

PROVIDES += "kernel-module-compat"
PROVIDES += "kernel-module-cfg80211"
PROVIDES += "kernel-module-mac80211"
