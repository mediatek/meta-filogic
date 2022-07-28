SUMMARY = "Init filogic "
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${WORKDIR}"
inherit systemd

SRC_URI = " \
    file://COPYING \
    file://init-IPv6.sh \
    file://init-IPv6.service \
    "

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = " init-IPv6.service"
FILES_${PN} += "{systemd_unitdir}/system/init-IPv6.service"

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/init-IPv6.sh ${D}${sbindir}
    install -d ${D}${systemd_unitdir}/system/
    install -m 0644 ${S}/init-IPv6.service ${D}${systemd_unitdir}/system
}
