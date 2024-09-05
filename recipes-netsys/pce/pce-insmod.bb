SUMMARY = "Bring up scripts for Mediatek PCE Driver"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit systemd

SRC_URI = " file://init "

S = "${WORKDIR}/init"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "pce_insmod.service"
FILES_${PN} += "{systemd_unitdir}/system/pce_insmod.service"

SYSTEMD_AUTO_ENABLE_${PN} = "enable"

do_install() {
	install -d ${D}${sbindir}/
	install -m 0755 ${S}/pce_insmod.sh ${D}${sbindir}
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${S}/pce_insmod.service ${D}${systemd_unitdir}/system
}
