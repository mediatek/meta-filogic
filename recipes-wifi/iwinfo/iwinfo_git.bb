# Copyright (C) 2015 Khem Raj <raj.khem@gmail.com>
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "Library for accessing wireless device drivers"
HOMEPAGE = "http://git.openwrt.org/?p=project/iwinfo.git;a=summary"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"
SECTION = "base"
DEPENDS += "uci lua ubus libnl-tiny"

SRCREV = "215820132b943b700d56441ecbd5a4efa09edc7c"

SRC_URI = "git://git.openwrt.org/project/iwinfo.git;branch=master \
           file://0002-fix-order-of-linker-cmdline-to-help-linking.patch \
           file://0003-Replace-typedef-loaL_reg-with-luaL_Reg.patch \
           "

S = "${WORKDIR}/git"

CFLAGS += "-fPIC -D_GNU_SOURCE"
CFLAGS += " -I=${includedir}/libnl-tiny "
# iwinfo breaks with parallel make
PARALLEL_MAKE = ""

EXTRA_OEMAKE = "\
    'BACKENDS=nl80211' \
    'SOVERSION=openwrt' \
"

do_install() {
	install -D -m 0755 ${B}/libiwinfo.so ${D}${libdir}/libiwinfo.so
    install -D -m 0755 ${B}/iwinfo.so ${D}${libdir}/lua/iwinfo.so
    install -D -m 0755 ${B}/iwinfo ${D}${bindir}/iwinfo
	install -D -m 0644 ${S}/include/iwinfo.h ${D}${includedir}/iwinfo.h
	install -D -m 0644 ${S}/include/iwinfo/utils.h ${D}${includedir}/iwinfo/utils.h
}

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/libiwinfo.so ${libdir}/lua/iwinfo.so"
