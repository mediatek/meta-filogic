SUMMARY = "U-Boot bootloader fw_printenv/setenv utilities"



FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI += " file://fw_env.config \
             file://fw_env_emmc.config \
        "





do_install_append () {
	install -d ${D}${sysconfdir}
	if ${@bb.utils.contains('DISTRO_FEATURES','emmc','true','false',d)}; then
		install -m 0644 ${WORKDIR}/fw_env_emmc.config ${D}${sysconfdir}/fw_env.config
	else
		install -m 0644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/fw_env.config
	fi
}


