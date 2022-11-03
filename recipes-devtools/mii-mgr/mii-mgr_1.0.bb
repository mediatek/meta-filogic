SUMMARY = "An program to read/write from/to phy from userspace"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    file://fix-rdkb-wan-get-status-fail.patch \
    "

S = "${WORKDIR}/git/src"

do_compile() {
    ${CC} ${LDFLAGS} mii_mgr.c -o mii_mgr
}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${S}/mii_mgr ${D}${sbindir}
    install -m 0755 ${S}/mii_mgr ${D}${sbindir}/mii_mgr_cl45
}
