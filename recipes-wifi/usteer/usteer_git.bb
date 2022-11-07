DESCRIPTION = "OpenWrt AP roaming assist daemon"
HOMEPAGE = "https://git.openwrt.org/project/usteer.git"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://README.md;md5=26019f6caa040c198cccce12a534366f"
SECTION = "kernel/userland"
DEPENDS = "json-c libubox ubus libnl-tiny libpcap"

SRC_URI = "git://git.openwrt.org/project/usteer.git;protocol=https"

SRCREV = "7d2b17c91baf67419c0ce63dc6c65a7659ab6a5c"
PV = "git${SRCPV}"

S = "${WORKDIR}/git"

inherit cmake pkgconfig

do_install_append () {
    install -dm 0755 ${D}/sbin
    ln -s /usr/sbin/usteerd ${D}/sbin/usteerd
}

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit ${@d.getVar('DISTRO', True) == 'rdk' and 'systemd' or 'base'}

SRC_URI_append_rdk += "\
    file://usteer.service \
"
SYSTEMD_AUTO_ENABLE_${PN} = "disable"
SYSTEMD_SERVICE_${PN}_rdk = "usteer.service"

do_install_append_rdk() {
	# Install systemd unit files
	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/usteer.service ${D}${systemd_unitdir}/system
	sed -i -e 's,@SBINDIR@,${sbindir},g' \
            ${D}${systemd_unitdir}/system/usteer.service
}

