DESCRIPTION = "This package contains a stripped down version of libnl"
HOMEPAGE = "https://git.openwrt.org/?p=project/libnl-tiny.git;a=summary"
LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://nl.c;startline=4;endline=7;md5=f16bd5d25e622bb3001bab76be1f9f91"
SECTION = "libs"

SRC_URI = "git://git.openwrt.org/project/libnl-tiny.git"
SRCREV = "db3b2cdbca5277723326a2720024a59fb821ae36"
PV = "git${SRCPV}"

inherit cmake pkgconfig
S = "${WORKDIR}/git"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"
