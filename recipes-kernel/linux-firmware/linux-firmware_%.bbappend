FILES:${PN}-mt7988 = "${base_libdir}/firmware/mediatek/mt7988/"

FILES:${PN}-mt7988 = " \
	${nonarch_base_libdir}/firmware/mediatek/mt7988/mt7988_wo_0.bin \
	${nonarch_base_libdir}/firmware/mediatek/mt7988/mt7988_wo_1.bin \
"
PACKAGES =+ "${PN}-mt7988"

FILES:${PN}-airoha = "${base_libdir}/firmware/airoha/"

FILES:${PN}-airoha = " \
	${nonarch_base_libdir}/firmware/airoha/EthMD32.dm.bin \
	${nonarch_base_libdir}/firmware/airoha/EthMD32.DSP.bin \
"
PACKAGES =+ "${PN}-airoha"