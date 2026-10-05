DESCRIPTION = "mt76-vendor"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

DEPENDS += "libnl-tiny"

inherit pkgconfig cmake

SRC_URI = " \
    file://COPYING;subdir=${BP}/src \
    file://src;subdir=${BP} \
    "

S = "${UNPACKDIR}/${PN}-${PV}/src"

CFLAGS:append = " -I=${includedir}/libnl-tiny "
EXTRA_OECMAKE += "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
