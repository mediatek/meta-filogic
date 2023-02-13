SUMMARY = "firmware upgrade tool"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${WORKDIR}"


SRC_URI = " \
    file://COPYING \
    file://platform.sh \
    file://nand.sh \
    file://mmc.sh \
    file://sysupgrade \
    file://do_stage2 \
    "



FILES_${PN} += " \
            /lib/upgrade \
            "

do_install() {
    install -d ${D}/lib/upgrade
    install -m 0755 ${WORKDIR}/platform.sh ${D}/lib/upgrade
    install -m 0755 ${WORKDIR}/nand.sh ${D}/lib/upgrade
    install -m 0755 ${WORKDIR}/mmc.sh ${D}/lib/upgrade
    install -m 0755 ${WORKDIR}/do_stage2 ${D}/lib/upgrade
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/sysupgrade ${D}${sbindir}
}
