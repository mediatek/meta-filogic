DESCRIPTION = "OpenWrt system message/RPC bus"
HOMEPAGE = "http://git.openwrt.org/?p=project/libubox.git;a=summary"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://ubusd.c;beginline=1;endline=12;md5=1b6a7aecd35bdd25de35da967668485d"
SECTION = "base"
DEPENDS = "json-c libubox"

SRC_URI = "\
          git://git.openwrt.org/project/ubus.git;protocol=https;nobranch=1;destsuffix=${BPN}-git \
          file://0001-lua5.3-support.patch \
"

SRC_URI:append:wrynose = " file://lua_version_rawlen.patch"

wifi6_ver = "4fc532c8a55ba8217ad67d7fd47c5eb9a8aba044"
wifi7_ver = "24864e7840b3a02a9ef76284a373f6b2f00b8a9b"

SRCREV = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', '${wifi7_ver}', '${wifi6_ver}', d)}"

PV = "git${SRCPV}"

ABI_VERSION = "1.0"

S = "${UNPACKDIR}/${BPN}-git"
ERROR_QA:remove = "patch-fuzz patch-status"
inherit cmake pkgconfig

#from class/openwrt-lua
OECMAKE_C_FLAGS += "-DLUA_COMPAT_5_3"
EXTRA_OECMAKE += "-DLUAPATH=${libdir}/lua/5.3"

DEPENDS += "lua"
OECMAKE_C_FLAGS += "-I${STAGING_INCDIR}/lua5.3"
CFLAGS += "-I${STAGING_INCDIR}/lua5.3"

do_configure:prepend () {
    if [ -e "${S}/CMakeLists.txt" ] ; then
        sed -i -e \
        "s:ARCHIVE DESTINATION lib:ARCHIVE DESTINATION \${CMAKE_INSTALL_LIBDIR}:g" \
        -e "s:LIBRARY DESTINATION lib:LIBRARY DESTINATION \${CMAKE_INSTALL_LIBDIR}:g" \
        ${S}/CMakeLists.txt
    fi
}

EXTRA_OECMAKE += "\
                -DCMAKE_INSTALL_LIBDIR:PATH=/lib \
                "
CFLAGS:append = " \
    -Wno-error=implicit-function-declaration \
"

do_install:append () {
    install -dm 0755 ${D}/sbin
    install -m 0755 ${S}/lua/publisher.lua ${D}${bindir}
    install -m 0755 ${S}/lua/subscriber.lua ${D}${bindir}
    install -m 0755 ${S}/lua/test_client.lua ${D}${bindir}
    install -m 0755 ${S}/lua/test.lua ${D}${bindir}
}

PACKAGES =+ "\
            lib${PN}${ABI_VERSION} \
            lib${PN}-lua \
            lib${PN}-examples \
            ${PN}d \
            "
PACKAGES += "lib${PN}"
FILES:lib${PN} += "${libdir}/libubus.so"

FILES_SOLIBSDEV = ""
FILES:${PN}  += "${datadir}/lua/"
FILES:${PN}-dbg  += "${libdir}/lua/.debug"

FILES:lib${PN}${ABI_VERSION} += "${libdir}/libubus.so"
FILES:lib${PN}-lua += "${libdir}/lua/5.3/*"
FILES:lib${PN}-examples += "${bindir}/*.lua"
FILES:${PN}d += "${sbindir} ${base_sbindir}"

INSANE_SKIP:${PN} += "installed-vs-shipped"
