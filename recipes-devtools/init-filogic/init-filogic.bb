SUMMARY = "Init filogic "
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${WORKDIR}"
inherit systemd

SRC_URI = " \
    file://init-service/ \
    file://COPYING \
    "

RDEPENDS_${PN} += "bash"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = " init-IPv6.service"
SYSTEMD_SERVICE_${PN} += " init-Lanbridge.service"
SYSTEMD_SERVICE_${PN} += " init-PPPQ.service"
SYSTEMD_SERVICE_${PN} += " usb-mount@.service"
FILES_${PN} += "{systemd_unitdir}/system/init-IPv6.service"
FILES_${PN} += "{systemd_unitdir}/system/init-Lanbridge.service"
FILES_${PN} += "{systemd_unitdir}/system/init-PPPQ.service"
FILES_${PN} += "{systemd_unitdir}/system/usb-mount@.service"

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${S}/init-service/init-IPv6.sh ${D}${sbindir}
    install -m 0755 ${S}/init-service/init-bridge.sh ${D}${sbindir}
    install -m 0755 ${S}/init-service/factorydefault ${D}${sbindir}
    install -m 0755 ${S}/init-service/lan_config.sh ${D}${sbindir}
    install -m 0755 ${S}/init-service/l2tp_config.sh ${D}${sbindir}
    install -m 0755 ${S}/init-service/pppoe_config.sh ${D}${sbindir}
    install -m 0755 ${S}/init-service/pptp_config.sh ${D}${sbindir}
    install -m 0755 ${S}/init-service/staticIP_config.sh ${D}${sbindir}
    install -m 0755 ${S}/init-service/init-PPPQ.sh ${D}${sbindir}
    install -d ${D}${systemd_unitdir}/system/
    install -m 0644 ${S}/init-service/init-IPv6.service ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/init-service/init-Lanbridge.service ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/init-service/init-PPPQ.service ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/init-service/usb-mount@.service ${D}${systemd_unitdir}/system
    install -d ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${S}/init-service/99-local-usb-mount.rules ${D}${sysconfdir}/udev/rules.d
    install -d ${D}${bindir}
    install -m 0755 ${S}/init-service/usb-mount.sh ${D}${bindir}
}
