SUMMARY = "User space daemon for extended IEEE 802.11 management"
HOMEPAGE = "http://w1.fi/hostapd/"
SECTION = "kernel/userland"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://hostapd/README;md5=c905478466c90f1cefc0df987c40e172"

DEPENDS = "libnl openssl"
DEPENDS_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', 'telemetry', '', d)}"
LDFLAGS_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'telemetry2_0', ' -ltelemetry_msgsender ', '', d)}"
RDEPENDS_${PN} += "gawk"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
FILESEXTRAPATHS_prepend := "${THISDIR}/files/patches:"

SRCREV ?= "b859b9bceadccd882252ff0aa2fdba0d3b91764e"
SRC_URI = " \
    git://w1.fi/hostap.git;protocol=https;branch=main \
    file://hostapd-full.config \
    file://hostapd-2G.conf \
    file://hostapd-5G.conf \
    file://hostapd-bhaul2G.conf \
    file://hostapd-bhaul5G.conf \
    file://hostapd.service \
    file://hostapd-init.sh \
    file://src \
    file://001-rdkb-remove-ubus-support.patch;apply=no \
"
require files/patches/patches.inc

B = "${WORKDIR}/git/hostapd"
S = "${WORKDIR}/git"

inherit update-rc.d systemd pkgconfig features_check
INITSCRIPT_NAME = "hostapd"

SYSTEMD_AUTO_ENABLE_${PN} = "enable"
SYSTEMD_SERVICE_${PN} = "hostapd.service"

do_unpack_append() {
    bb.build.exec_func('do_copy_openwrt_src', d)
}

do_copy_openwrt_src() {
    cp -Rfp ${WORKDIR}/src/* ${S}/
}

do_configure_append() {
    install -m 0644 ${WORKDIR}/hostapd-full.config ${B}/.config

    echo "CONFIG_MBO=y" >> ${B}/.config
    echo "CONFIG_WPS_UPNP=y" >> ${B}/.config

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
}

SRC_URI_append_mt7915 += " \
    file://hostapd-5G-7915.conf \
"
do_configure_append_mt7915() {
    ln -sf ${WORKDIR}/hostapd-5G-7915.conf ${WORKDIR}/hostapd-5G.conf  
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
         install -m 0644 ${WORKDIR}/hostapd-bhaul2G.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd-bhaul5G.conf ${D}${sysconfdir}
         install -m 0644 ${WORKDIR}/hostapd.service ${D}${systemd_unitdir}/system
         install -m 0755 ${WORKDIR}/hostapd-init.sh ${D}${base_libdir}/rdk
}

FILES_${PN} += " \
                ${systemd_unitdir}/system/hostapd.service \
                ${sysconfdir}/hostapd-2G.conf \
                ${sysconfdir}/hostapd-5G.conf \
                ${sysconfdir}/hostapd-bhaul2G.conf \
                ${sysconfdir}/hostapd-bhaul5G.conf \
                ${base_libdir}/rdk/hostapd-init.sh \
"
