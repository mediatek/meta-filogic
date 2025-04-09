SUMMARY = "An program to read/write from/to a pci device from userspace"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    "

S = "${WORKDIR}/git/src"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} regs.c -o regs
}

do_install() {
    install -d ${D}${base_bindir}
    install -m 0755 ${S}/regs ${D}${base_bindir}
}
