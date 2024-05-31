SUMMARY = "Marvell Ethernet firmware files"
DESCRIPTION = "Firmware for Marvell AQR113C. "

LICENSE = "CLOSED"

SRC_URI = " \
    file://Rhe-05.06-Candidate7-AQR_Mediatek_23B_StartOff_ID45623_VER36657.cld \
    file://Rhe-05.06-Candidate9-AQR_Mediatek_23B_P5_ID45824_LCLVER1.cld\
    file://AQR-G4_v5.7.0-AQR_EVB_Generic_X3410_StdCfg_MDISwap_USX_ID46316_VER2148.cld \
"

S = "${WORKDIR}"

inherit allarch

do_install() {
    install -d ${D}/${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/Rhe-05.06-Candidate9-AQR_Mediatek_23B_P5_ID45824_LCLVER1.cld ${D}${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/Rhe-05.06-Candidate7-AQR_Mediatek_23B_StartOff_ID45623_VER36657.cld ${D}${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/AQR-G4_v5.7.0-AQR_EVB_Generic_X3410_StdCfg_MDISwap_USX_ID46316_VER2148.cld  ${D}${base_libdir}/firmware/
}

FILES_${PN} += "${base_libdir}/firmware/"

# Make marvell-eth-firmware depend on all of the split-out packages.
python populate_packages_prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS_marvell-eth-firmware', ' ' + ' '.join(firmware_pkgs))
}
