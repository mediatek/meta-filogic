DESCRIPTION = "This package contains a stripped down version of libnl"
HOMEPAGE = "https://git.openwrt.org/?p=project/libnl-tiny.git;a=summary"
LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://nl.c;startline=4;endline=7;md5=f16bd5d25e622bb3001bab76be1f9f91"
SECTION = "libs"

SRC_URI = "git://git.openwrt.org/project/libnl-tiny.git"
SRCREV = "d433990c00e804593f253cc709b8fe901492b530"
PV = "git${SRCPV}"

inherit cmake pkgconfig
S = "${WORKDIR}/git"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"
