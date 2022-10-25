DESCRIPTION = "Linux backports"
HOMEPAGE = "https://backports.wiki.kernel.org"
SECTION = "kernel/modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit module

PV = "5.15.74"

SRC_URI = " \
    https://cdn.kernel.org/pub/linux/kernel/projects/backports/stable/v${PV}/backports-${PV}-1.tar.xz \
    file://config \
    file://0001-rdkb-fix_build_issue-mac80211-without_depmod.patch;apply=no \
    "
SRC_URI[sha256sum] = "98098d0cab24cc76a04db738dc746a0c8d38d180398805481224f141cca06423"

DEPENDS += "virtual/kernel"
DEPENDS += "bison-native coreutils-native flex-native"

PROVIDES += "kernel-module-compat"
PROVIDES += "kernel-module-cfg80211"
PROVIDES += "kernel-module-mac80211"

FILESEXTRAPATHS_prepend := "${THISDIR}/files/patches/build:"
FILESEXTRAPATHS_prepend := "${THISDIR}/files/patches/subsys:"

require files/patches/build/build.inc
require files/patches/subsys/subsys.inc

S = "${WORKDIR}/backports-${PV}-1"

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

do_install_prepend () {
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

do_install_append () {
    cat ${D}/usr/include/linux-mac80211/Module.symvers >> ${TMPDIR}/work-shared/${MACHINE}/kernel-build-artifacts/Module.symvers
}
