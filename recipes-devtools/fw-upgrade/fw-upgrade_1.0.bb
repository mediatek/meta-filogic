SUMMARY = "firmware upgrade tool"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

S = "${UNPACKDIR}"


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
            ${base_libdir}/upgrade/* \
            "

do_install() {
    IS_ITB_IMAGE="${@bb.utils.contains('DISTRO_FEATURES','kernelv6','true','false',d)}"
    install -d ${D}${base_libdir}/upgrade
    install -d ${D}${sbindir}
    if [ $IS_ITB_IMAGE = 'true' ]; then
        install -m 0755 ${UNPACKDIR}/platform_v2.sh ${D}${base_libdir}/upgrade/platform.sh
        install -m 0755 ${UNPACKDIR}/nand_v2.sh ${D}${base_libdir}/upgrade/nand.sh
        install -m 0755 ${UNPACKDIR}/mmc_v2.sh ${D}${base_libdir}/upgrade/mmc.sh
        install -m 0755 ${UNPACKDIR}/do_stage2_v2 ${D}${base_libdir}/upgrade/do_stage2
        install -m 0755 ${UNPACKDIR}/sysupgrade_v2 ${D}${sbindir}/sysupgrade
    else
        install -m 0755 ${UNPACKDIR}/platform.sh ${D}${base_libdir}/upgrade
        install -m 0755 ${UNPACKDIR}/nand.sh ${D}${base_libdir}/upgrade
        install -m 0755 ${UNPACKDIR}/mmc.sh ${D}${base_libdir}/upgrade
        install -m 0755 ${UNPACKDIR}/do_stage2 ${D}${base_libdir}/upgrade
        install -m 0755 ${UNPACKDIR}/sysupgrade ${D}${sbindir}
    fi
}
