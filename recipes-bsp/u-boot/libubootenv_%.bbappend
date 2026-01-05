SUMMARY = "U-Boot bootloader fw_printenv/setenv utilities"



FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += " file://fw_env.config \
             file://fw_env_emmc.config \
	     file://fw_env_v2.config \
        "





do_install:append () {
	install -d ${D}${sysconfdir}
	if ${@bb.utils.contains('DISTRO_FEATURES','emmc','true','false',d)}; then
		install -m 0644 ${WORKDIR}/fw_env_emmc.config ${D}${sysconfdir}/fw_env.config
	else
		if ${@bb.utils.contains('DISTRO_FEATURES','kernelv6','true','false',d)}; then
			install -m 0644 ${WORKDIR}/fw_env_v2.config ${D}${sysconfdir}/fw_env.config
		else
			install -m 0644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/fw_env.config		
		fi
	fi
}


