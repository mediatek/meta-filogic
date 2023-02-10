FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " file://init_readonlyfs.sh \
                   file://init_readonlyfs-emmc.sh \
                 "
FILES_${PN} += " /overlay \
               /rom \"

do_install_append() {
       if ${@bb.utils.contains('DISTRO_FEATURES','kernel_in_ubi','true','false',d)}; then
       install -d ${D}/overlay
       install -d ${D}/rom
       if ${@bb.utils.contains('DISTRO_FEATURES','emmc','true','false',d)}; then
              install -m 0755 ${WORKDIR}/init_readonlyfs-emmc.sh ${D}${rootlibexecdir}/init_readonlyfs.sh
       else
              install -m 0755 ${WORKDIR}/init_readonlyfs.sh ${D}${rootlibexecdir}/init_readonlyfs.sh
       fi
       [ -e ${D}/sbin/init ] && rm -rf ${D}/sbin/init
       ln -s ${rootlibexecdir}/init_readonlyfs.sh ${D}/sbin/init
       fi
}

FILES_${PN} += " ${rootlibexecdir}/init_readonlyfs.sh "
