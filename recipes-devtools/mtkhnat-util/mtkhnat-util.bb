DESCRIPTION = "Hardware-QoS-tool"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

DEPENDS += "libnl-tiny uci"

inherit autotools coverity

SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    "

S = "${WORKDIR}/git/src"

CFLAGS_append = " -luci "

do_install_append() {
    install -d ${D}${sysconfdir}
    install -d ${D}${sysconfdir}/config
    install -m 0755 ${WORKDIR}/git/src/mtkhnat.config ${D}${sysconfdir}/config/mtkhnat
}
