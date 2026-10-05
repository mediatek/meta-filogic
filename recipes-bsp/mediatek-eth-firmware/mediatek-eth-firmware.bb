SUMMARY = "Mediatek Ethernet firmware files"
DESCRIPTION = "Firmware for Mediatek 2.5G phy. "

LICENSE = "CLOSED"

SRC_URI_mt7988 += " \
    file://mt7988 \
"
SRC_URI_mt7987 += " \
    file://mt7987 \
"
S = "${UNPACKDIR}"

inherit allarch

do_install_mt7988() {
    install -d ${D}/${base_libdir}/firmware/mediatek/mt7988/
    install -m 644 ${UNPACKDIR}/mt7988/* ${D}${base_libdir}/firmware/mediatek/mt7988/
}

do_install_mt7987() {
    install -d ${D}/${base_libdir}/firmware/mediatek/mt7987/
    install -m 644 ${UNPACKDIR}/mt7987/* ${D}${base_libdir}/firmware/mediatek/mt7987/
}

SRC_URI:append:mt7988 = "${@' file://mt7988' if 'wrynose' in (d.getVar('OVERRIDES') or '').split(':') else ''}"
SRC_URI:append:mt7987 = "${@' file://mt7987' if 'wrynose' in (d.getVar('OVERRIDES') or '').split(':') else ''}"

do_install:append() {
    for soc in mt7988 mt7987; do
        if [ -d ${UNPACKDIR}/$soc ]; then
            install -d ${D}${base_libdir}/firmware/mediatek/$soc/
            install -m 644 ${UNPACKDIR}/$soc/* ${D}${base_libdir}/firmware/mediatek/$soc/
        fi
    done
}

FILES:${PN} += "${base_libdir}/firmware/mediatek/*"

# Make Mediatek-eth-firmware depend on all of the split-out packages.
python populate_packages:prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS:mediatek-eth-firmware', ' ' + ' '.join(firmware_pkgs))
}
