SUMMARY = "Mediatek Ethernet firmware files"
DESCRIPTION = "Firmware for Mediatek 2.5G phy. "

LICENSE = "CLOSED"

SRC_URI = " \
    file://i2p5ge-phy-pmb.bin \
"
S = "${WORKDIR}"

inherit allarch

do_install() {
    install -d ${D}/${base_libdir}/firmware/mediatek/mt7988/
    install -m 644 ${WORKDIR}/i2p5ge-phy-pmb.bin ${D}${base_libdir}/firmware/mediatek/mt7988/
}

FILES_${PN} += "${base_libdir}/firmware/mediatek/mt7988/*"

# Make Mediatek-eth-firmware depend on all of the split-out packages.
python populate_packages_prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS_mediatek-eth-firmware', ' ' + ' '.join(firmware_pkgs))
}
