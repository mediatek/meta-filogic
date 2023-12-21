DESCRIPTION = "OpenWrt debug service"
SECTION = "utils"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

SRC_URI = " \
    git://git.openwrt.org/project/udebug.git;protocol=https \
    file://udebug.config \
    file://udebug.init \
    file://COPYING;subdir=git \
"

SRCREV = "6d3f51f9fda706f0cf4732c762e4dbe8c21e12cf"
PV = "git${SRCPV}"

S = "${WORKDIR}/git"


DEPENDS += "libnl-tiny libubox ubus ucode"

INSANE_SKIP_${PN} += "file-rdeps dev-deps dev-so"
FILES_SOLIBSDEV = ""

FILES_${PN}-dev = "${includedir}/*"
FILES_${PN} = "${sbindir}/* ${libdir}/*.so* ${libdir}/ucode/*"

inherit cmake pkgconfig python3native  


