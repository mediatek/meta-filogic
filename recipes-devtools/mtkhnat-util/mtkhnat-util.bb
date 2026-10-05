DESCRIPTION = "Hardware-QoS-tool"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

DEPENDS += "libnl-tiny uci"

inherit autotools coverity

SRC_URI = " \
    file://COPYING;subdir=${BP}/src \
    file://src;subdir=${BP} \
    "

S = "${UNPACKDIR}/${PN}-${PV}/src"

CFLAGS:append = " -luci "
CFLAGS:append:wrynose = " -Wno-incompatible-pointer-types "

do_install:append() {
    install -d ${D}${sysconfdir}
    install -d ${D}${sysconfdir}/config
    install -m 0755 ${UNPACKDIR}/${PN}-${PV}/src/mtkhnat.config ${D}${sysconfdir}/config/mtkhnat
}
