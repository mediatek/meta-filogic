SUMMARY = "mtk factory read and write"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${UNPACKDIR}"
inherit systemd

SRC_URI = " \
    file://COPYING \
    file://mtk_factory_rw.sh \
    file://init-MacAddr.sh \
    file://init-MacAddr.service \
    file://fix-rdkb-get-board-name-issue.patch \
    "

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = " init-MacAddr.service"
FILES:${PN} += "{systemd_unitdir}/system/init-MacAddr.service"
ERROR_QA:remove = "patch-fuzz patch-status"
do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${UNPACKDIR}/mtk_factory_rw.sh ${D}${sbindir}
    install -m 0755 ${UNPACKDIR}/init-MacAddr.sh ${D}${sbindir}
    install -d ${D}${systemd_unitdir}/system/
    install -m 0644 ${S}/init-MacAddr.service ${D}${systemd_unitdir}/system
}
