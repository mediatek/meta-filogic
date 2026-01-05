DESCRIPTION = "This package contains a stripped down version of libnl"
HOMEPAGE = "https://git.openwrt.org/?p=project/libnl-tiny.git;a=summary"
LICENSE = "LGPL-2.1-only"
LIC_FILES_CHKSUM = "file://nl.c;startline=4;endline=7;md5=f16bd5d25e622bb3001bab76be1f9f91"
SECTION = "libs"

SRC_URI = "git://git.openwrt.org/project/libnl-tiny.git;branch=master"
SRCREV = "965c4bf49658342ced0bd6e7cb069571b4a1ddff"
SRCREV_kernelv6 = "c0df580adbd4d555ecc1962dbe88e91d75b67a4e"

PV = "git${SRCPV}"

inherit cmake pkgconfig
S = "${WORKDIR}/git"

FILES_SOLIBSDEV = ""
FILES:${PN} += "${libdir}/*.so"
INSANE_SKIP:${PN} += "dev-so"