SUMMARY = "Client for Wi-Fi Protected Access (WPA)"
DESCRIPTION = "wpa_supplicant is a WPA Supplicant for Linux, BSD, Mac OS X, and Windows with support for WPA and WPA2 (IEEE 802.11i / RSN). Supplicant is the IEEE 802.1X/WPA component that is used in the client stations. It implements key negotiation with a WPA Authenticator and it controls the roaming and IEEE 802.11 authentication/association of the wlan driver."
HOMEPAGE = "http://w1.fi/wpa_supplicant/"
BUGTRACKER = "http://w1.fi/security/"
SECTION = "network"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://hostapd/README;md5=c905478466c90f1cefc0df987c40e172"

DEPENDS = "dbus libnl-tiny ubus ucode udebug"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
FILESEXTRAPATHS_prepend := "${THISDIR}/files/patches-${PV}:"

SRCREV ?= "e5ccbfc69ecf297590341ae8b461edba9d8e964c"
SRC_URI = "git://w1.fi/hostap.git;protocol=https;branch=main \
           file://wpa-supplicant.sh \
           file://wpa_supplicant.conf \
           file://wpa_supplicant.conf-sane \
           file://99_wpa_supplicant \
           file://wpa_supplicant-full.config \
           file://src-${PV} \
           file://002-rdkb-add-ucode-support.patch;apply=no \
           "
require files/patches-${PV}/patches.inc

S = "${WORKDIR}/git"

inherit pkgconfig systemd

PACKAGECONFIG ?= "openssl"
PACKAGECONFIG[openssl] = ",,openssl"

CVE_PRODUCT = "wpa_supplicant"

EXTRA_OEMAKE = "'LIBDIR=${libdir}' 'INCDIR=${includedir}' 'BINDIR=${sbindir}'"

do_unpack_append() {
    bb.build.exec_func('do_copy_openwrt_src', d)
}

do_copy_openwrt_src() {
    cp -Rfp ${WORKDIR}/src-${PV}/* ${S}/
}

do_filogic_patches() {
    cd ${S}
        if [ ! -e patch_applied ]; then
            patch -p1 < ${WORKDIR}/002-rdkb-add-ucode-support.patch
            touch patch_applied
        fi
}

addtask filogic_patches after do_patch before do_compile

do_configure () {
	${MAKE} -C wpa_supplicant clean

	# For rebuild
	rm -f wpa_supplicant/*.d wpa_supplicant/dbus/*.d
}

do_configure_append () {
	# from Openwrt defconfig
	install -m 0644 ${WORKDIR}/wpa_supplicant-full.config wpa_supplicant/.config

	# RDKB
	echo "CONFIG_BUILD_WPA_CLIENT_SO=y" >> wpa_supplicant/.config
	echo "CFLAGS +=-I${STAGING_INCDIR}/libnl-tiny -D_GNU_SOURCE" >> wpa_supplicant/.config
	echo "DRV_CFLAGS +=-I${STAGING_INCDIR}/libnl-tiny" >> wpa_supplicant/.config
	# mtk add
	echo "CONFIG_MBO=y" >> wpa_supplicant/.config
	echo "CONFIG_WPS_UPNP=y" >> wpa_supplicant/.config
	echo "CONFIG_DPP=y" >> wpa_supplicant/.config
	echo "CONFIG_DPP2=y" >> wpa_supplicant/.config
	echo "CONFIG_DPP3=y" >> wpa_supplicant/.config

	# OpenWRT hostapd Makefile add
	echo "CONFIG_ACS=y" >> wpa_supplicant/.config
	echo "CONFIG_IEEE80211AX=y" >> wpa_supplicant/.config
	echo "CONFIG_TLS=openssl" >> wpa_supplicant/.config
	echo "CONFIG_SAE=y" >> wpa_supplicant/.config
	echo "CONFIG_OWE=y" >> wpa_supplicant/.config
	echo "CONFIG_SUITEB192=y" >> wpa_supplicant/.config
	echo "CONFIG_WEP=y" >> wpa_supplicant/.config
	echo "CONFIG_AP=y" >> wpa_supplicant/.config
	echo "CONFIG_MESH=y" >> wpa_supplicant/.config
	echo "CONFIG_IEEE80211BE=y" >> wpa_supplicant/.config
	echo "CONFIG_IEEE80211AC=y" >> wpa_supplicant/.config
	echo "CONFIG_HE_OVERRIDES=y" >> wpa_supplicant/.config
	echo "CONFIG_EHT_OVERRIDES=y" >> wpa_supplicant/.config
	echo "CONFIG_UCODE=y" >> wpa_supplicant/.config
	echo "CONFIG_LIBNL20=y" >> wpa_supplicant/.config
	echo "CONFIG_LIBNL_TINY=y" >> wpa_supplicant/.config
}

do_compile () {
	oe_runmake -C wpa_supplicant
	oe_runmake -C wpa_supplicant libwpa_client.a
}

do_install () {
	oe_runmake -C wpa_supplicant DESTDIR="${D}" install

	install -d ${D}${docdir}/wpa_supplicant
	install -m 644 wpa_supplicant/README ${WORKDIR}/wpa_supplicant.conf ${D}${docdir}/wpa_supplicant

	install -d ${D}${sysconfdir}
	install -m 600 ${WORKDIR}/wpa_supplicant.conf-sane ${D}${sysconfdir}/wpa_supplicant.conf

	install -d ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}${sysconfdir}/network/if-post-down.d/
	install -d ${D}${sysconfdir}/network/if-down.d/
	install -m 755 ${WORKDIR}/wpa-supplicant.sh ${D}${sysconfdir}/network/if-pre-up.d/wpa-supplicant
	ln -sf ../if-pre-up.d/wpa-supplicant ${D}${sysconfdir}/network/if-post-down.d/wpa-supplicant

	install -d ${D}/${sysconfdir}/dbus-1/system.d
	install -m 644 ${S}/wpa_supplicant/dbus/dbus-wpa_supplicant.conf ${D}/${sysconfdir}/dbus-1/system.d
	install -d ${D}/${datadir}/dbus-1/system-services
	install -m 644 ${S}/wpa_supplicant/dbus/*.service ${D}/${datadir}/dbus-1/system-services

	if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
		install -d ${D}/${systemd_system_unitdir}
		install -m 644 ${S}/wpa_supplicant/systemd/*.service ${D}/${systemd_system_unitdir}
	fi

	install -d ${D}/etc/default/volatiles
	install -m 0644 ${WORKDIR}/99_wpa_supplicant ${D}/etc/default/volatiles

	install -d ${D}${includedir}
	install -m 0644 ${S}/src/common/wpa_ctrl.h ${D}${includedir}

	install -d ${D}${libdir}
	install -m 0644 ${S}/wpa_supplicant/libwpa_client.so ${D}${libdir}
}

pkg_postinst:${PN} () {
	# If we're offline, we don't need to do this.
	if [ "x$D" = "x" ]; then
		killall -q -HUP dbus-daemon || true
	fi
}

PACKAGE_BEFORE_PN += "${PN}-passphrase ${PN}-cli"
PACKAGES =+ "${PN}-lib"
PACKAGES += "${PN}-plugins"
ALLOW_EMPTY:${PN}-plugins = "1"

PACKAGES_DYNAMIC += "^${PN}-plugin-.*$"
NOAUTOPACKAGEDEBUG = "1"

FILES:${PN}-passphrase = "${sbindir}/wpa_passphrase"
FILES:${PN}-cli = "${sbindir}/wpa_cli"
FILES:${PN} += "${datadir}/dbus-1/system-services/* ${systemd_system_unitdir}/* ${datadir}/hostap/*"
FILES:${PN}-dbg += "${sbindir}/.debug ${libdir}/.debug"

CONFFILES:${PN} += "${sysconfdir}/wpa_supplicant.conf"

RRECOMMENDS:${PN} = "${PN}-passphrase ${PN}-cli ${PN}-plugins"

SYSTEMD_SERVICE:${PN} = "wpa_supplicant.service"
SYSTEMD_AUTO_ENABLE = "disable"

# move from cmf
FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/libwpa_client.so"

