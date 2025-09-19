SUMMARY = "Device-Tree Blob Overlays"
inherit devicetree
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

do_compile[depends] += "virtual/kernel:do_shared_workdir"
KERNEL_INCLUDE:append = " \
	${STAGING_KERNEL_BUILDDIR}/include \
	${STAGING_KERNEL_BUILDDIR}/include/generated \
"
SRC_URI:append:mt7988a = " \
	file://mt7988a-rfb-emmc.dtso \
	file://mt7988a-rfb-sd.dtso \
	file://mt7988a-rfb-snfi-nand.dtso \
	file://mt7988a-rfb-spim-nand.dtso \
	file://mt7988a-rfb-spim-nand-factory.dtso \
	file://mt7988a-rfb-spim-nand-nmbm.dtso \
	file://mt7988a-rfb-spim-nor.dtso \
	file://mt7988a-rfb-eth0-gsw.dtso \
	file://mt7988a-rfb-eth1-aqr.dtso \
	file://mt7988a-rfb-eth1-an8831x.dtso \
	file://mt7988a-rfb-eth1-cux3410.dtso \
	file://mt7988a-rfb-eth1-i2p5g-phy.dtso \
	file://mt7988a-rfb-eth1-mxl.dtso \
	file://mt7988a-rfb-eth1-sfp.dtso \
	file://mt7988a-rfb-eth2-aqr.dtso \
	file://mt7988a-rfb-eth2-an8831x.dtso \
	file://mt7988a-rfb-eth2-cux3410.dtso \
	file://mt7988a-rfb-eth2-mxl.dtso \
	file://mt7988a-rfb-eth2-mxl86252.dtso \
	file://mt7988a-rfb-eth2-sfp.dtso \
	file://mt7988a-rfb-spidev.dtso \
	file://mt7988a-rfb-4pcie.dtso \
	file://mt7988a-rfb-2pcie.dtso \
	file://mt7988d-rfb-2pcie.dtso \
"
SRC_URI:append:mt7988d = " \
	file://mt7988a-rfb-emmc.dtso \
	file://mt7988a-rfb-sd.dtso \
	file://mt7988a-rfb-snfi-nand.dtso \
	file://mt7988a-rfb-spim-nand.dtso \
	file://mt7988a-rfb-spim-nand-factory.dtso \
	file://mt7988a-rfb-spim-nand-nmbm.dtso \
	file://mt7988a-rfb-spim-nor.dtso \
	file://mt7988a-rfb-eth1-i2p5g-phy.dtso \
	file://mt7988a-rfb-eth2-aqr.dtso \
	file://mt7988a-rfb-eth2-mxl.dtso \
	file://mt7988a-rfb-spidev.dtso \
	file://mt7988a-rfb-eth2-sfp.dtso \
	file://mt7988d-rfb-eth0-gsw.dtso \
"

SRC_URI:append:mt7987 = " \
	file://mt7987-spim-nand-nmbm.dtso \
	file://mt7987-spidev.dtso \
	file://mt7987-spim-nor.dtso \
	file://mt7987-emmc.dtso \
	file://mt7987-sd.dtso \
	file://mt7987-netsys-eth0-an8801sb.dtso \
	file://mt7987-netsys-eth0-an8855.dtso \
	file://mt7987-netsys-eth0-an8855-gsw.dtso \
	file://mt7987-netsys-eth0-e2p5g.dtso \
	file://mt7987-netsys-eth0-mt7531.dtso \
	file://mt7987-netsys-eth0-mt7531-gsw.dtso \
	file://mt7987-netsys-eth1-i2p5g.dtso \
	file://mt7987-netsys-eth2-an8801sb.dtso \
	file://mt7987-netsys-eth2-e2p5g.dtso \
	file://mt7987-netsys-eth2-sfp.dtso \
	file://mt7987-netsys-eth2-usb.dtso \
"

SRC_URI:append:bpi4 = " \
	file://mt7988a-bananapi-bpi-r4-emmc.dtso \
	file://mt7988a-bananapi-bpi-r4-rtc.dtso \
	file://mt7988a-bananapi-bpi-r4-sd.dtso \
	file://mt7988a-bananapi-bpi-r4-wifi-mt7996a.dtso \
"

DT_FILES_PATH = "${WORKDIR}"
COMPATIBLE_MACHINE = "mt*"
