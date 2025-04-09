#Basic Configuration
DESCRIPTION = "Mtd tool"
SECTION = "base"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

DEPENDS += "libubox"

inherit autotools systemd pkgconfig

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    "

S = "${WORKDIR}/git/src"

META_LDFLAGS_ADD = "-lubox"

LDFLAGS:remove = "-Wl,--as-needed"

CFLAGS:append = " -Wall -flto"
LDFLAGS:append = " -flto=jobserver ${META_LDFLAGS_ADD}"

do_compile:prepend(){
    cd ${S}
}

do_install() {
    install -d ${D}/usr/bin/
    install -m 0755 ${S}/mtd ${D}/usr/bin/
}

