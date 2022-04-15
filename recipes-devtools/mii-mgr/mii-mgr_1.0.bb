SUMMARY = "An program to read/write from/to phy from userspace"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

S = "${WORKDIR}"

SRC_URI = " \
    file://COPYING \
    file://mii_mgr.c \
    file://mii_mgr.h \
    "

do_compile() {
    ${CC} ${LDFLAGS} mii_mgr.c -o mii_mgr
}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/mii_mgr ${D}${sbindir}
    install -m 0755 ${WORKDIR}/mii_mgr ${D}${sbindir}/mii_mgr_cl45
}
