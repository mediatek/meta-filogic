DESCRIPTION = "OpenWrt system message/RPC bus"
HOMEPAGE = "http://git.openwrt.org/?p=project/libubox.git;a=summary"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://ubusd.c;beginline=1;endline=12;md5=1b6a7aecd35bdd25de35da967668485d"
SECTION = "base"
DEPENDS = "json-c libubox"

SRC_URI = "git://git.openwrt.org/project/ubus.git"

SRCREV = "2bebf93cd3343fe49f22a05ef935e460d2d44f67"
PV = "1.0.0+git${SRCPV}"

S = "${WORKDIR}/git"

inherit cmake pkgconfig

EXTRA_OECMAKE = "-DBUILD_EXAMPLES=OFF -DBUILD_LUA=OFF -DBUILD_STATIC=ON"

do_install_append () {
    install -dm 0755 ${D}/sbin
    ln -s /usr/sbin/ubusd ${D}/sbin/ubusd
}

TOOLCHAIN = "gcc"
