SUMMARY = "User space daemon for extended IEEE 802.11 management"
HOMEPAGE = "http://w1.fi/hostapd/"
SECTION = "kernel/userland"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://hostapd/README;md5=0e430ef1be3d6eebf257cf493fc7661d"

DEPENDS = "libnl-tiny openssl ubus ucode udebug"
DEPENDS_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', 'telemetry', '', d)}"
LDFLAGS_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', ' -ltelemetry_msgsender ', '', d)}"
RDEPENDS_${PN} += "gawk ucode udebug"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
FILESEXTRAPATHS_prepend := "${THISDIR}/files/patches-${PV}:"

SRCREV ?= "7e0e69cfeac300414ef0492bc76a2aa164443249"
SRC_URI = " \
    git://w1.fi/hostap.git;protocol=https;branch=main \
    file://hostapd-full.config \
    file://hostapd-2G-EHT.conf \
    file://hostapd-5G-EHT.conf \
    file://hostapd-6G-EHT.conf \
    file://hostapd-5G-7915.conf \
    file://hostapd-5G-7916.conf \
    file://hostapd.service \
    file://hostapd-init-EHT.sh \
    file://mac80211-EHT.sh \
    file://init-uci-config.service \
    file://hostapd.uc \
    file://common.uc \
    file://wdev.uc \
    file://wpa_supplicant.uc \
    file://src-${PV} \
    file://002-rdkb-add-ucode-support.patch;apply=no \
"
require files/patches-${PV}/patches.inc

B = "${WORKDIR}/git/hostapd"
S = "${WORKDIR}/git"

inherit update-rc.d systemd pkgconfig features_check
INITSCRIPT_NAME = "hostapd"

SYSTEMD_AUTO_ENABLE_${PN} = "enable"
SYSTEMD_SERVICE_${PN} = "hostapd.service"
SYSTEMD_SERVICE_${PN} += " init-uci-config.service"
INSANE_SKIP_${PN} += "file-rdeps"


do_configure_append() {
    install -m 0644 ${WORKDIR}/hostapd-full.config ${B}/.config

    echo "CONFIG_MBO=y" >> ${B}/.config
    echo "CONFIG_WPS_UPNP=y" >> ${B}/.config
    echo "CONFIG_DPP=y" >> ${B}/.config
    echo "CONFIG_DPP2=y" >> ${B}/.config
    echo "CONFIG_DPP3=y" >> ${B}/.config

    echo "CONFIG_ACS=y" >> ${B}/.config
    echo "CONFIG_IEEE80211AX=y" >> ${B}/.config
    echo "CONFIG_TLS=openssl" >> ${B}/.config
    echo "CONFIG_SAE=y" >> ${B}/.config
    echo "CONFIG_OWE=y" >> ${B}/.config
    echo "CONFIG_SUITEB192=y" >> ${B}/.config
    echo "CONFIG_AP=y" >> ${B}/.config
    echo "CONFIG_MESH=y" >> ${B}/.config
    echo "CONFIG_WEP=y" >> ${B}/.config
    echo "CONFIG_FILS=y" >> ${B}/.config
    echo "CONFIG_IEEE80211BE=y" >> ${B}/.config
    echo "CONFIG_TESTING_OPTIONS=y" >> ${B}/.config
    echo "CONFIG_UCODE=y" >> ${B}/.config
    echo "CONFIG_LIBNL20=y" >> ${B}/.config
    echo "CONFIG_LIBNL_TINY=y" >> ${B}/.config
    echo "CONFIG_AFC=y" >> ${B}/.config
    echo "CONFIG_SAE_PK=y" >> ${B}/.config
    echo "CONFIG_HS20=y" >> ${B}/.config
    echo "CONFIG_HE_OVERRIDES=y" >> ${B}/.config
    echo "CONFIG_EHT_OVERRIDES=y" >> ${B}/.config
}

do_filogic_patches() {
    cd ${S}
        if [ ! -e patch_applied ]; then
            patch -p1 < ${WORKDIR}/002-rdkb-add-ucode-support.patch
            touch patch_applied
        fi
}

addtask filogic_patches after do_patch before do_compile

do_compile() {
    export CFLAGS="-MMD -O2 -Wall -g -I${STAGING_INCDIR}/libnl-tiny -D_GNU_SOURCE"
    export EXTRA_CFLAGS="${CFLAGS}"
    make V=1
}

do_install() {
         install -d ${D}${sbindir} ${D}${sysconfdir} ${D}${systemd_unitdir}/system/ ${D}${base_libdir}/rdk ${D}${datadir}/hostap
         install -m 0755 ${B}/hostapd ${D}${sbindir}
         install -m 0755 ${B}/hostapd_cli ${D}${sbindir}
         install -m 0644 ${WORKDIR}/hostapd-2G-EHT.conf ${D}${sysconfdir}/hostapd-2G.conf
         install -m 0644 ${WORKDIR}/hostapd-5G-EHT.conf ${D}${sysconfdir}/hostapd-5G.conf
         install -m 0644 ${WORKDIR}/hostapd-6G-EHT.conf ${D}${sysconfdir}/hostapd-6G.conf
         install -m 0644 ${WORKDIR}/hostapd-5G-7915.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd-5G-7916.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd.service ${D}${systemd_unitdir}/system
         install -m 0755 ${WORKDIR}/hostapd-init-EHT.sh ${D}${base_libdir}/rdk/hostapd-init.sh
         install -m 0644 ${WORKDIR}/init-uci-config.service ${D}${systemd_unitdir}/system
         install -m 0755 ${WORKDIR}/mac80211-EHT.sh ${D}${sbindir}/mac80211.sh
         install -m 0755 ${WORKDIR}/hostapd.uc ${D}${datadir}/hostap
         install -m 0755 ${WORKDIR}/wdev.uc ${D}${datadir}/hostap
         install -m 0755 ${WORKDIR}/common.uc ${D}${datadir}/hostap
         install -m 0755 ${WORKDIR}/wpa_supplicant.uc ${D}${datadir}/hostap
}

FILES_${PN} += " \
                ${systemd_unitdir}/system/hostapd.service \
                ${sysconfdir}/hostapd-2G-EHT.conf \
                ${sysconfdir}/hostapd-5G-EHT.conf \
                ${sysconfdir}/hostapd-6G-EHT.conf \
                ${sysconfdir}/hostapd-5G-7915.conf \
                ${sysconfdir}/hostapd-5G-7916.conf \
                ${base_libdir}/rdk/hostapd-init.sh \
                ${systemd_unitdir}/system/init-uci-config.service \
                ${datadir}/hostap/hostapd.uc \
                ${datadir}/hostap/wdev.uc \
                ${datadir}/hostap/common.uc \
                ${datadir}/hostap/wpa_supplicant.uc \
"
