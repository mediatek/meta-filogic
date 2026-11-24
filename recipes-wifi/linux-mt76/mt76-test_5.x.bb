DESCRIPTION = "mt76-test"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://../COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

DEPENDS += "libnl-tiny linux-mac80211"

inherit pkgconfig cmake

SRC_URI = " \
    git://github.com/mediatek/softmac_mt76.git;protocol=https;branch=wifi8-6.12-alpha;destsuffix=${BPN}-${PV} \
    "
SRC_URI:append = " \
    file://COPYING;subdir=${BP} \
    "
SRCREV = "${AUTOREV}"
DEPENDS += "virtual/kernel"
DEPENDS += "linux-mac80211"
DEPENDS += "linux-mt76"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"


CFLAGS:append = " -I=${includedir}/libnl-tiny -I${STAGING_KERNEL_BUILDDIR}/usr/include/mac80211/uapi "

S = "${UNPACKDIR}/${BPN}-${PV}/tools"

addtask mtk_patches after do_patch before do_configure

INSANE_SKIP:mt76-test-dbg += "buildpaths"



