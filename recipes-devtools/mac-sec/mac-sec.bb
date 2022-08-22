SUMMARY = "Init filogic "
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${WORKDIR}"
inherit systemd

SRC_URI = " \
    file://COPYING \
    file://MACSec.sh \
    file://MACSec.service \
    "

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_AUTO_ENABLE_${PN} = "disable"
SYSTEMD_SERVICE_${PN} = " MACSec.service"
FILES_${PN} += "{systemd_unitdir}/system/MACSec.service"

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/MACSec.sh ${D}${sbindir}
    install -d ${D}${systemd_unitdir}/system/
    install -m 0644 ${S}/MACSec.service ${D}${systemd_unitdir}/system
}
