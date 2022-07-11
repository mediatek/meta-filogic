SUMMARY = "Command to config switch"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${WORKDIR}"

DEPENDS = "libnl"

SRC_URI = " \
    file://COPYING \
    file://Makefile \
    file://switch_753x.c \
    file://switch_extend.h \
    file://switch_fun.c \
    file://switch_fun.h \
    file://switch_ioctl.c \
    file://switch_ioctl.h \
    file://switch_netlink.c \
    file://switch_netlink.h \
    "

CFLAGS_prepend = " \
    -D_GNU_SOURCE \
    -I${STAGING_INCDIR}/libnl3 \
    -I${S} \
    "

do_compile() {
    oe_runmake -C ${S} "LIBS=-L${STAGING_LIBDIR} -lnl-3 -lnl-genl-3 -lm"
}

do_install() {
    install -d ${D}/usr/sbin
    install -d ${D}/lib/network
    install -m 0755 ${WORKDIR}/switch ${D}/usr/sbin
}
