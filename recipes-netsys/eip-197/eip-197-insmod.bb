SUMMARY = "Bring up scripts for Mediatek EIP-197 Driver"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

FILESEXTRAPATHS:prepend := "${THISDIR}/init:"

S = "${WORKDIR}/init"

inherit systemd

SRC_URI = "file://init"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = " eip-197_insmod.service"
FILES:${PN} += "{systemd_unitdir}/system/eip-197_insmod.service"

SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${S}/eip-197_insmod.sh ${D}${sbindir}/eip-197_insmod.sh
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${S}/eip-197_insmod.service ${D}${systemd_unitdir}/system
}
