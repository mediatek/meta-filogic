SUMMARY = "netfilter_flowtable tools"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"


DEPENDS = "libnfnetlink libmnl"

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    "

S = "${WORKDIR}/git/src"

CFLAGS_prepend = " \
    -fPIC -D_GNU_SOURCE \
    -I${S} \
    "

do_compile() {
    oe_runmake -C ${S} "LIBS=-L${STAGING_LIBDIR} -lnfnetlink  -lm"
}

do_install() {
    install -d ${D}/usr/bin
    install -m 0755 ${S}/ftnl ${D}/usr/bin
}
