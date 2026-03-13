SUMMARY = "Third Party PHY firmware files"
DESCRIPTION = "Firmware for Third Party  PHY. "

LICENSE = "CLOSED"

SRC_URI = " \
    file://Rhe-05.06-Candidate9-AQR_Mediatek_23B_P5_ID45824_LCLVER1.cld\
    file://AQR-G4_v5.7.0-AQR_EVB_Generic_X3410_StdCfg_MDISwap_USX_ID46316_VER2148.cld \
    file://an8811hb/ \
    file://as21x1x_fw.bin \
"

S = "${WORKDIR}"

inherit allarch

do_install() {
    install -d ${D}/${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/Rhe-05.06-Candidate9-AQR_Mediatek_23B_P5_ID45824_LCLVER1.cld ${D}${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/AQR-G4_v5.7.0-AQR_EVB_Generic_X3410_StdCfg_MDISwap_USX_ID46316_VER2148.cld  ${D}${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/as21x1x_fw.bin  ${D}${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/an8811hb/EthMD32_CRC.DM.bin  ${D}${base_libdir}/firmware/
    install -m 644 ${WORKDIR}/an8811hb/EthMD32_CRC.DSP.bin  ${D}${base_libdir}/firmware/
    mkdir -p ${TOPDIR}/firmware/airoha/an8811hb
    cp -rf ${WORKDIR}/Rhe-05.06-Candidate9-AQR_Mediatek_23B_P5_ID45824_LCLVER1.cld ${TOPDIR}/firmware
    cp -rf ${WORKDIR}/AQR-G4_v5.7.0-AQR_EVB_Generic_X3410_StdCfg_MDISwap_USX_ID46316_VER2148.cld ${TOPDIR}/firmware
    cp -rf ${WORKDIR}/as21x1x_fw.bin ${TOPDIR}/firmware
    cp -rf ${WORKDIR}/an8811hb/EthMD32_CRC.DM.bin ${TOPDIR}/firmware/airoha/an8811hb/
    cp -rf ${WORKDIR}/an8811hb/EthMD32_CRC.DSP.bin ${TOPDIR}/firmware/airoha/an8811hb/
}

FILES:${PN} += "${base_libdir}/firmware/"

# Make eth-firmware depend on all of the split-out packages.
python populate_packages:prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS:eth-firmware', ' ' + ' '.join(firmware_pkgs))
}
