DESCRIPTION = "Unified Configuration Interface (UCI)"
SECTION = "libs"
LICENSE_NAME = "${@bb.utils.contains('DISTRO_CODENAME', 'dunfell', 'LGPL-2.1', 'LGPL-2.1-only', d)}"
LICENSE = "${LICENSE_NAME}"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE_NAME};md5=1a6d268fd218675ffea8be556788b780"

SRC_URI = " \
    git://git.openwrt.org/project/uci.git \
"

SRCREV = "f84f49f00fb70364f58b4cce72f1796a7190d370"
PV = "1.0.0+git${SRCPV}"

S = "${WORKDIR}/git"


DEPENDS += "libubox"

INSANE_SKIP_${PN} += "dev-deps"
FILES_SOLIBSDEV = ""

FILES_${PN}-dev = "${includedir}/*"
FILES_${PN} = "${bindir}/uci ${libdir}/libuci.so"

inherit cmake

EXTRA_OECMAKE = " -DBUILD_LUA=OFF -DCMAKE_SKIP_RPATH=TRUE"

do_install() {
    install -d ${D}${libdir}
    install -d ${D}${bindir}
    install -d ${D}${includedir}
    install -m 0755 ${B}/uci ${D}${bindir}
    install -m 0755 ${B}/libuci.so ${D}${libdir}/
    install -m 0644 ${S}/uci*.h ${D}${includedir}
}
