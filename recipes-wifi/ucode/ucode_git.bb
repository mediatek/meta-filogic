DESCRIPTION = "Tiny scripting and templating language"
SECTION = "lang"
LICENSE_NAME = "ISC"
LICENSE = "${LICENSE_NAME}"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b42eb47dc3802282b0d1be1bc8f5336c"

SRC_URI = " \
    git://git@github.com/jow-/ucode.git;protocol=https \
    file://0001-change-cmakelist.patch \
"

SRCREV = "c7d84aae09691a99ae3db427c0b2463732ef84f4"
PV = "git${SRCPV}"

S = "${WORKDIR}/git"


DEPENDS += "libnl-tiny libubox ubus uci"

INSANE_SKIP_${PN} += "file-rdeps dev-deps dev-so"
FILES_SOLIBSDEV = ""

FILES_${PN}-dev = "${includedir}/*"
FILES_${PN} = "${bindir}/ucode ${libdir}/libucode.so* ${libdir}/ucode/*"

inherit cmake pkgconfig python3native  


EXTRA_OECMAKE = "\
	-DFS_SUPPORT=ON \
	-DMATH_SUPPORT=ON \
	-DNL80211_SUPPORT=ON \
	-DRESOLV_SUPPORT=ON \
	-DRTNL_SUPPORT=ON \
	-DSTRUCT_SUPPORT=ON \
	-DUBUS_SUPPORT=OFF \
	-DUCI_SUPPORT=ON \
	-DULOOP_SUPPORT=ON \
	"

