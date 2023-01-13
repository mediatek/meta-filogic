DESCRIPTION = "testmode daemon for nl80211"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

DEPENDS += "libnl-tiny"
RDEPENDS_${PN} += "busybox"
inherit pkgconfig cmake

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    file://ated.sh;subdir=git \
    file://iwpriv.sh;subdir=git \
    "

S = "${WORKDIR}/git/src"

CFLAGS_append = " -I=${includedir}/libnl-tiny "


do_install_append() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/git/ated.sh ${D}${sbindir}/ated
    install -m 0755 ${WORKDIR}/git/iwpriv.sh ${D}${sbindir}/iwpriv
    install -m 0755 ${WORKDIR}/git/iwpriv.sh ${D}${sbindir}/mwctl
}

