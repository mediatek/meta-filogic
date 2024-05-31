DESCRIPTION = "Tiny scripting and templating language"
SECTION = "lang"
LICENSE_NAME = "ISC"
LICENSE = "${LICENSE_NAME}"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b42eb47dc3802282b0d1be1bc8f5336c"

SRC_URI = " \
    git://git@github.com/jow-/ucode.git;protocol=https \
    file://0001-change-cmakelist.patch \
"

SRCREV = "0d823e702bfe5f2bb5be694030a98afedf34aa6b"
PV = "git${SRCPV}"

S = "${WORKDIR}/git"


DEPENDS += "libnl-tiny libubox ubus uci"

INSANE_SKIP_${PN} += "file-rdeps dev-deps dev-so"
FILES_SOLIBSDEV = ""

FILES_${PN}-dev = "${includedir}/*"
FILES_${PN} = "${bindir}/* ${libdir}/libucode.so* ${libdir}/ucode/*"

inherit cmake pkgconfig python3native  


EXTRA_OECMAKE = "\
	-DFS_SUPPORT=ON \
	-DMATH_SUPPORT=ON \
	-DNL80211_SUPPORT=ON \
	-DRESOLV_SUPPORT=ON \
	-DRTNL_SUPPORT=ON \
	-DSTRUCT_SUPPORT=ON \
	-DUBUS_SUPPORT=ON \
	-DUCI_SUPPORT=ON \
	-DULOOP_SUPPORT=ON \
	-DSOCKET_SUPPORT=OFF \
	"

