SUMMARY = "User space daemon for extended IEEE 802.11 management"
HOMEPAGE = "http://w1.fi/hostapd/"
SECTION = "kernel/userland"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://hostapd/README;md5=7f182e472078a4029cf7d59eabcc46a3"

DEPENDS = "libnl-tiny openssl ubus ucode udebug"
DEPENDS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', 'telemetry', '', d)}"
LDFLAGS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', ' -ltelemetry_msgsender ', '', d)}"
RDEPENDS:${PN} += "gawk ucode udebug"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
FILESEXTRAPATHS:prepend := "${THISDIR}/files/kernelv6-uc-files:"

SRC_URI = " \
    git://github.com/mediatek/softmac_hostapd.git;protocol=https;branch=wifi8-6.12-alpha;destsuffix=${BPN}-${PV} \
    file://hostapd-full.config \
    file://hostapd-2G-UHR.conf \
    file://hostapd-5G-UHR.conf \
    file://hostapd-6G-UHR.conf \
    file://hostapd-5G-7915.conf \
    file://hostapd-5G-7916.conf \
    file://hostapd.service \
    file://hostapd-init-UHR.sh \
    file://mac80211-EHT.sh \
    file://init-uci-config.service \
    file://kernelv6-uc-files \
    file://wifi-detect.uc \
    file://mac80211.uc \
    file://board.json \
    file://002-rdkb-add-ucode-support.patch;apply=no \
"
SRCREV = "${AUTOREV}"

S = "${UNPACKDIR}/${BPN}-${PV}"

inherit update-rc.d systemd pkgconfig
INITSCRIPT_NAME = "hostapd"

SYSTEMD_AUTO_ENABLE:${PN} = "enable"
SYSTEMD_SERVICE:${PN} = "hostapd.service"
SYSTEMD_SERVICE:${PN} += " init-uci-config.service"
INSANE_SKIP:${PN} += "file-rdeps"
ERROR_QA:remove = "patch-fuzz patch-status"

do_configure:append() {
    install -m 0644 ${UNPACKDIR}/hostapd-full.config ${S}/hostapd/.config
    echo "# Example wpa_supplicant build time configuration" >> ${S}/wpa_supplicant/.config

    echo "CONFIG_MBO=y" >> ${S}/hostapd/.config
    echo "CONFIG_WPS_UPNP=y" >> ${S}/hostapd/.config
    echo "CONFIG_DPP=y" >> ${S}/hostapd/.config
    echo "CONFIG_DPP2=y" >> ${S}/hostapd/.config
    echo "CONFIG_DPP3=y" >> ${S}/hostapd/.config

    echo "CONFIG_ACS=y" >> ${S}/hostapd/.config
    echo "CONFIG_IEEE80211AX=y" >> ${S}/hostapd/.config
    echo "CONFIG_TLS=openssl" >> ${S}/hostapd/.config
    echo "CONFIG_SAE=y" >> ${S}/hostapd/.config
    echo "CONFIG_OWE=y" >> ${S}/hostapd/.config
    echo "CONFIG_SUITEB192=y" >> ${S}/hostapd/.config
    echo "CONFIG_AP=y" >> ${S}/hostapd/.config
    echo "CONFIG_MESH=y" >> ${S}/hostapd/.config
    echo "CONFIG_WEP=y" >> ${S}/hostapd/.config
    echo "CONFIG_FILS=y" >> ${S}/hostapd/.config
    echo "CONFIG_IEEE80211BE=y" >> ${S}/hostapd/.config
    echo "CONFIG_TESTING_OPTIONS=y" >> ${S}/hostapd/.config
    echo "CONFIG_UCODE=y" >> ${S}/hostapd/.config
    echo "CONFIG_LIBNL20=y" >> ${S}/hostapd/.config
    echo "CONFIG_LIBNL_TINY=y" >> ${S}/hostapd/.config
    echo "CONFIG_AFC=y" >> ${S}/hostapd/.config
    echo "CONFIG_SAE_PK=y" >> ${S}/hostapd/.config
    echo "CONFIG_HS20=y" >> ${S}/hostapd/.config
    echo "CONFIG_HE_OVERRIDES=y" >> ${S}/hostapd/.config
    echo "CONFIG_EHT_OVERRIDES=y" >> ${S}/hostapd/.config
    echo "CONFIG_P2P_MANAGER=y" >> ${S}/hostapd/.config
    echo "CONFIG_DEBUG_LINUX_TRACING=y" >> ${S}/hostapd/.config
    echo "CONFIG_CTRL_IFACE_MIB=y" >> ${S}/hostapd/.config
    echo "CONFIG_IEEE80211BN=y" >> ${S}/hostapd/.config
    echo "CONFIG_ENC_ASSOC=y" >> ${S}/hostapd/.config
    echo "CONFIG_PMKSA_PRIVACY=y" >> ${S}/hostapd/.config
}

do_filogic_patches() {
    cd ${S}
        if [ ! -e patch_applied ]; then
            patch -p1 < ${UNPACKDIR}/002-rdkb-add-ucode-support.patch
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
         #install -m 0755 ${B}/hostapd ${D}${sbindir}
         #install -m 0755 ${B}/hostapd_cli ${D}${sbindir}
         install -m 0755 ${S}/hostapd/hostapd ${D}${sbindir}
         install -m 0755 ${S}/hostapd/hostapd_cli ${D}${sbindir}
         install -m 0644 ${UNPACKDIR}/hostapd-2G-UHR.conf ${D}${sysconfdir}/hostapd-2G.conf
         install -m 0644 ${UNPACKDIR}/hostapd-5G-UHR.conf ${D}${sysconfdir}/hostapd-5G.conf
         install -m 0644 ${UNPACKDIR}/hostapd-6G-UHR.conf ${D}${sysconfdir}/hostapd-6G.conf
         install -m 0644 ${UNPACKDIR}/board.json ${D}${sysconfdir}
         install -m 0644 ${UNPACKDIR}/hostapd.service ${D}${systemd_unitdir}/system
         install -m 0755 ${UNPACKDIR}/hostapd-init-UHR.sh ${D}${base_libdir}/rdk/hostapd-init.sh
         install -m 0644 ${UNPACKDIR}/init-uci-config.service ${D}${systemd_unitdir}/system
         install -m 0755 ${UNPACKDIR}/mac80211-EHT.sh ${D}${sbindir}/mac80211.sh
         install -m 0755 ${UNPACKDIR}/kernelv6-uc-files/hostapd.uc ${D}${datadir}/hostap
         install -m 0755 ${UNPACKDIR}/kernelv6-uc-files/wdev.uc ${D}${datadir}/hostap
         install -m 0755 ${UNPACKDIR}/kernelv6-uc-files/common.uc ${D}${datadir}/hostap
         install -m 0755 ${UNPACKDIR}/kernelv6-uc-files/wpa_supplicant.uc ${D}${datadir}/hostap
         install -m 0755 ${UNPACKDIR}/wifi-detect.uc ${D}${datadir}/hostap
         install -m 0755 ${UNPACKDIR}/mac80211.uc ${D}${datadir}/hostap
}

FILES:${PN} += " \
                ${systemd_unitdir}/system/hostapd.service \
                ${sysconfdir}/hostapd-2G-EHT.conf \
                ${sysconfdir}/hostapd-5G-EHT.conf \
                ${sysconfdir}/hostapd-6G-EHT.conf \
                ${sysconfdir}/board.json \
                ${base_libdir}/rdk/hostapd-init.sh \
                ${systemd_unitdir}/system/init-uci-config.service \
                ${datadir}/hostap/hostapd.uc \
                ${datadir}/hostap/wdev.uc \
                ${datadir}/hostap/common.uc \
                ${datadir}/hostap/wpa_supplicant.uc \
                ${datadir}/hostap/wifi-detect.uc \
                ${datadir}/hostap/mac80211.uc \
"
