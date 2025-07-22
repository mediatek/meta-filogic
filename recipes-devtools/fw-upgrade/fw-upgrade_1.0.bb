SUMMARY = "firmware upgrade tool"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${WORKDIR}"


SRC_URI = " \
    file://COPYING \
    file://platform.sh \
    file://nand.sh \
    file://mmc.sh \
    file://sysupgrade \
    file://do_stage2 \
    file://platform_v2.sh \
    file://nand_v2.sh \
    file://mmc_v2.sh \
    file://sysupgrade_v2 \
    file://do_stage2_v2 \
    "



FILES:${PN} += " \
            /lib/upgrade \
            "

do_install() {
    IS_ITB_IMAGE="${@bb.utils.contains('DISTRO_FEATURES','kernel6-6','true','false',d)}"
    install -d ${D}/lib/upgrade
    install -d ${D}${sbindir}
    if [ $IS_ITB_IMAGE = 'true' ]; then
        install -m 0755 ${WORKDIR}/platform_v2.sh ${D}/lib/upgrade/platform.sh
        install -m 0755 ${WORKDIR}/nand_v2.sh ${D}/lib/upgrade/nand.sh
        install -m 0755 ${WORKDIR}/mmc_v2.sh ${D}/lib/upgrade/mmc.sh
        install -m 0755 ${WORKDIR}/do_stage2_v2 ${D}/lib/upgrade/do_stage2
        install -m 0755 ${WORKDIR}/sysupgrade_v2 ${D}${sbindir}/sysupgrade
    else
        install -m 0755 ${WORKDIR}/platform.sh ${D}/lib/upgrade
        install -m 0755 ${WORKDIR}/nand.sh ${D}/lib/upgrade
        install -m 0755 ${WORKDIR}/mmc.sh ${D}/lib/upgrade
        install -m 0755 ${WORKDIR}/do_stage2 ${D}/lib/upgrade
        install -m 0755 ${WORKDIR}/sysupgrade ${D}${sbindir}
    fi
}
