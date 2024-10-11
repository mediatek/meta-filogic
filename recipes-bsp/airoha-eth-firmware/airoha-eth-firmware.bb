SUMMARY = "Airoha Ethernet firmware files"
DESCRIPTION = "Firmware for Airoha 2.5G phy. "

LICENSE = "CLOSED"

SRC_URI = " \
    file://EthMD32.dm.bin \
    file://EthMD32.DSP.bin \
"
S = "${WORKDIR}"

inherit allarch

do_install() {
    install -d ${D}/${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/EthMD32.dm.bin ${D}${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/EthMD32.DSP.bin ${D}${base_libdir}/firmware/
}

FILES_${PN} += "${base_libdir}/firmware/*"

# Make Mediatek-eth-firmware depend on all of the split-out packages.
python populate_packages_prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS_mediatek-eth-firmware', ' ' + ' '.join(firmware_pkgs))
}
