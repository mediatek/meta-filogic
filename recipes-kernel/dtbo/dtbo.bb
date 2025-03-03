SUMMARY = "Device-Tree Blob Overlays"
inherit devicetree
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

do_compile[depends] += "virtual/kernel:do_shared_workdir"
KERNEL_INCLUDE:append = " \
	${STAGING_KERNEL_BUILDDIR}/include \
	${STAGING_KERNEL_BUILDDIR}/include/generated \
"
SRC_URI:append:mt7988 = " \
	file://mt7988a-rfb-emmc.dtso \
	file://mt7988a-rfb-sd.dtso \
	file://mt7988a-rfb-snfi-nand.dtso \
	file://mt7988a-rfb-spim-nand.dtso \
	file://mt7988a-rfb-spim-nand-factory.dtso \
	file://mt7988a-rfb-spim-nand-nmbm.dtso \
	file://mt7988a-rfb-spim-nor.dtso \
	file://mt7988a-rfb-eth0-gsw.dtso \
	file://mt7988a-rfb-eth1-aqr.dtso \
	file://mt7988a-rfb-eth1-cux3410.dtso \
	file://mt7988a-rfb-eth1-i2p5g-phy.dtso \
	file://mt7988a-rfb-eth1-mxl.dtso \
	file://mt7988a-rfb-eth1-sfp.dtso \
	file://mt7988a-rfb-eth2-aqr.dtso \
	file://mt7988a-rfb-eth2-cux3410.dtso \
	file://mt7988a-rfb-eth2-sfp.dtso \
	file://mt7988a-rfb-spidev.dtso \
"

DT_FILES_PATH = "${WORKDIR}"
COMPATIBLE_MACHINE = "mt*"
