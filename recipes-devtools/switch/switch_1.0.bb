SUMMARY = "Command to config switch"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"


DEPENDS = "libnl"

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    file://rdkb-change-bridge-name.patch \
    "

S = "${WORKDIR}/git/src"

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
    install -m 0755 ${S}/switch ${D}/usr/sbin
}
