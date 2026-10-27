DESCRIPTION = "mt76-test"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://../COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

DEPENDS += "libnl-tiny linux-mac80211"

inherit pkgconfig cmake

PV = "1.0"

require mt76.inc
require mt76-3x.inc

SRC_URI = " \
    git://git@github.com/openwrt/mt76.git;protocol=https;branch=master;destsuffix=${BPN}-${PV} \
    file://5000-mt76-add-internal-wed_tiny-header-file.patch;apply=no\
    "
SRC_URI:remove:kernelv6 = "git://git@github.com/openwrt/mt76.git;protocol=https;branch=master;destsuffix=${BPN}-${PV}"
SRC_URI:append:kernelv6 = " \
    git://github.com/mediatek/softmac_mt76.git;protocol=https;branch=mp4.3-6.12;destsuffix=${BPN}-${PV} \
    "
SRC_URI:append = " \
    file://COPYING;subdir=${BP} \
    "
DEPENDS += "virtual/kernel"
DEPENDS += "linux-mac80211"
DEPENDS += "linux-mt76"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"


CFLAGS:append = " -I=${includedir}/libnl-tiny -I${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211/uapi "

S = "${UNPACKDIR}/${BPN}-${PV}/tools"
PATCH_SRC = "${@bb.utils.contains('DISTRO_FEATURES','wifi_eht','${UNPACKDIR}/patches-3.x','${UNPACKDIR}/patches',d)}"

SRC_URI += "file://${@bb.utils.contains('DISTRO_FEATURES','wifi_eht','patches-3.x/','patches/',d)};apply=no"

do_mtk_patches() {
	cd ${S}/../
    if ${@bb.utils.contains('DISTRO_FEATURES','kernelv6','true','false',d)}; then
        touch mtk_wifi_patch_applied
    fi
	if [ ! -e mtk_wifi_patch_applied ]; then
        for i in ${PATCH_SRC}/*.patch; do patch -p1 < $i; done
	fi
	touch mtk_wifi_patch_applied
}
addtask mtk_patches after do_patch before do_configure

INSANE_SKIP:mt76-test-dbg += "buildpaths"



