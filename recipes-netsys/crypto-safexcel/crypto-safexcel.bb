SUMMARY = "Firmwares for EIP-197 Upstream Lookaside Driver"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
	file://src/firmware/ipue.bin \
	file://src/firmware/ifpp.bin \
	"

S = "${WORKDIR}"

inherit allarch

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	# Module
	install -d ${D}/${base_libdir}/firmware/inside-secure/eip197_minifw
	install -m 0755 ${THISDIR}/files/src/firmware/ipue.bin ${D}/${base_libdir}/firmware/inside-secure/eip197_minifw/
	install -m 0755 ${THISDIR}/files/src/firmware/ifpp.bin ${D}/${base_libdir}/firmware/inside-secure/eip197_minifw/
}

FILES_${PN} += "${base_libdir}/firmware/inside-secure/eip197_minifw/*"
