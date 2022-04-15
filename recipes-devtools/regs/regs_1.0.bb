SUMMARY = "An program to read/write from/to a pci device from userspace"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

S = "${WORKDIR}"

SRC_URI = " \
    file://COPYING \
    file://regs.c \
    "

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} regs.c -o regs
}

do_install() {
    install -d ${D}${base_bindir}
    install -m 0755 ${WORKDIR}/regs ${D}${base_bindir}
}
