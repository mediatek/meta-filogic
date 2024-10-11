SUMMARY = "Mediatek Ethernet firmware files"
DESCRIPTION = "Firmware for Mediatek 2.5G phy. "

LICENSE = "CLOSED"

SRC_URI_mt7988 += " \
    file://mt7988 \
"
SRC_URI_mt7987 += " \
    file://mt7987 \
"
S = "${WORKDIR}"

inherit allarch

do_install_mt7988() {
    install -d ${D}/${base_libdir}/firmware/mediatek/mt7988/
    install -m 644 ${WORKDIR}/mt7988/* ${D}${base_libdir}/firmware/mediatek/mt7988/
}

do_install_mt7987() {
    install -d ${D}/${base_libdir}/firmware/mediatek/mt7987/
    install -m 644 ${WORKDIR}/mt7987/* ${D}${base_libdir}/firmware/mediatek/mt7987/
}

FILES_${PN} += "${base_libdir}/firmware/mediatek/*"

# Make Mediatek-eth-firmware depend on all of the split-out packages.
python populate_packages_prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS_mediatek-eth-firmware', ' ' + ' '.join(firmware_pkgs))
}
