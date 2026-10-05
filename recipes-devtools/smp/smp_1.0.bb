SUMMARY = "SMP IRQ Affinity tool"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${UNPACKDIR}"
inherit systemd

SRC_URI = " \
    file://COPYING \
    file://smp-mt76.sh \
    file://flowtable.sh \
    file://smp.service \
    file://smp.sh \
    file://smp-dispatch.sh \
    file://001-rdkb-smp-ifname.patch \
    "
ERROR_QA:remove = "patch-fuzz patch-status"
SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = " smp.service"
FILES:${PN} += "{systemd_unitdir}/system/smp.service"

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${S}/smp-mt76.sh ${D}${sbindir}
    install -m 0755 ${S}/flowtable.sh ${D}${sbindir}
    install -m 0755 ${S}/smp.sh ${D}${sbindir}
    install -m 0755 ${S}/smp-dispatch.sh ${D}${sbindir}
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${S}/smp.service ${D}${systemd_unitdir}/system
}
