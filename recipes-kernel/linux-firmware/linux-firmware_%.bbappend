FILES:${PN}-mt7988 = "${base_libdir}/firmware/mediatek/mt7988/"

FILES:${PN}-mt7988 = " \
	${nonarch_base_libdir}/firmware/mediatek/mt7988/mt7988_wo_0.bin \
	${nonarch_base_libdir}/firmware/mediatek/mt7988/mt7988_wo_1.bin \
	${nonarch_base_libdir}/firmware/mediatek/mt7988/i2p5ge-phy-pmb.bin\
"
PACKAGES =+ "${PN}-mt7988"
