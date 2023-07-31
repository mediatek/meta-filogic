DESCRIPTION = "This package contains a stripped down version of libnl"
HOMEPAGE = "https://git.openwrt.org/?p=project/libnl-tiny.git;a=summary"
LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://nl.c;startline=4;endline=7;md5=f16bd5d25e622bb3001bab76be1f9f91"
SECTION = "libs"

SRC_URI = "git://git.openwrt.org/project/libnl-tiny.git"
SRCREV = "bc92a280186f9becc53c0f17e4e43cfbdeec7e7b"
PV = "git${SRCPV}"

inherit cmake pkgconfig
S = "${WORKDIR}/git"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"
INSANE_SKIP_${PN} += "dev-so"