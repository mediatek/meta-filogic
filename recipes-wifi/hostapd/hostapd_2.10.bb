SUMMARY = "User space daemon for extended IEEE 802.11 management"
HOMEPAGE = "http://w1.fi/hostapd/"
SECTION = "kernel/userland"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://hostapd/README;md5=c905478466c90f1cefc0df987c40e172"

DEPENDS = "libnl openssl ubus"
DEPENDS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', 'telemetry', '', d)}"
LDFLAGS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', ' -ltelemetry_msgsender ', '', d)}"
RDEPENDS:${PN} += "gawk"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
FILESEXTRAPATHS:prepend := "${THISDIR}/files/patches:"

SRCREV ?= "b704dc72ef824dfdd96674b90179b274d1d38105"
SRC_URI = " \
    git://w1.fi/hostap.git;protocol=https;branch=main \
    file://hostapd-full.config \
    file://hostapd-2G.conf \
    file://hostapd-5G.conf \
    file://hostapd-6G.conf \
    file://hostapd-5G-7915.conf \
    file://hostapd-5G-7916.conf \
    file://hostapd.service \
    file://hostapd-init.sh \
    file://mac80211.sh \
    file://init-uci-config.service \
    file://src \
    file://001-rdkb-remove-ubus-support.patch;apply=no \
"
require files/patches/patches.inc

B = "${WORKDIR}/git/hostapd"
S = "${WORKDIR}/git"

inherit update-rc.d systemd pkgconfig features_check
INITSCRIPT_NAME = "hostapd"

SYSTEMD_AUTO_ENABLE:${PN} = "enable"
SYSTEMD_SERVICE:${PN} = "hostapd.service"
SYSTEMD_SERVICE:${PN} += " init-uci-config.service"

do_unpack:append() {
    bb.build.exec_func('do_copy_openwrt_src', d)
}

do_copy_openwrt_src() {
    cp -Rfp ${WORKDIR}/src/* ${S}/
}

do_configure:append() {
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
    echo "CONFIG_TESTING_OPTIONS=y" >> ${B}/.config
}

do_filogic_patches() {
    cd ${S}
        if [ ! -e patch_applied ]; then
            patch -p1 < ${WORKDIR}/001-rdkb-remove-ubus-support.patch
            touch patch_applied
        fi
}

addtask filogic_patches after do_patch before do_compile

do_compile() {
    export CFLAGS="-MMD -O2 -Wall -g -I${STAGING_INCDIR}/libnl3"
    export EXTRA_CFLAGS="${CFLAGS}"
    make V=1
}

do_install() {
         install -d ${D}${sbindir} ${D}${sysconfdir} ${D}${systemd_unitdir}/system/ ${D}${base_libdir}/rdk
         install -m 0755 ${B}/hostapd ${D}${sbindir}
         install -m 0755 ${B}/hostapd_cli ${D}${sbindir}
         install -m 0644 ${WORKDIR}/hostapd-2G.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd-5G.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd-6G.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd-5G-7915.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd-5G-7916.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd.service ${D}${systemd_unitdir}/system
         install -m 0755 ${WORKDIR}/hostapd-init.sh ${D}${base_libdir}/rdk
         install -m 0644 ${WORKDIR}/init-uci-config.service ${D}${systemd_unitdir}/system
         install -m 0755 ${WORKDIR}/mac80211.sh ${D}${sbindir}
}

FILES:${PN} += " \
                ${systemd_unitdir}/system/hostapd.service \
                ${sysconfdir}/hostapd-2G.conf \
                ${sysconfdir}/hostapd-5G.conf \
                ${sysconfdir}/hostapd-6G.conf \
                ${sysconfdir}/hostapd-5G-7915.conf \
                ${sysconfdir}/hostapd-5G-7916.conf \
                ${base_libdir}/rdk/hostapd-init.sh \
                ${systemd_unitdir}/system/init-uci-config.service \
"
