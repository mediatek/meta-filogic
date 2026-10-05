FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI:append = " file://init_readonlyfs.sh \
                   file://init_readonlyfs-emmc.sh \
                   file://kernelv6-init_readonlyfs.sh \
                   file://kernelv6-init_readonlyfs-emmc.sh \
                 "
FILES:${PN} += " /overlay \
               /rom \"

ALTERNATIVE:${PN}:append = "${@bb.utils.contains('DISTRO_FEATURES','kernel_in_ubi',' init','',d)}"
ALTERNATIVE_TARGET[init] = "${nonarch_libdir}/systemd/init_readonlyfs.sh"
ALTERNATIVE_LINK_NAME[init] = "${base_sbindir}/init"
ALTERNATIVE_PRIORITY[init] = "400"

do_install:append() {
       if ${@bb.utils.contains('DISTRO_FEATURES','kernel_in_ubi','true','false',d)}; then
              install -d ${D}/overlay
              install -d ${D}/rom
              if ${@bb.utils.contains('DISTRO_FEATURES','kernelv6','true','false',d)}; then
                     if ${@bb.utils.contains('DISTRO_FEATURES','emmc','true','false',d)}; then
                            install -m 0755 ${UNPACKDIR}/kernelv6-init_readonlyfs-emmc.sh ${D}${nonarch_libdir}/systemd/init_readonlyfs.sh
                     else
                            install -m 0755 ${UNPACKDIR}/kernelv6-init_readonlyfs.sh ${D}${nonarch_libdir}/systemd/init_readonlyfs.sh
                     fi
              else
                     if ${@bb.utils.contains('DISTRO_FEATURES','emmc','true','false',d)}; then
                            install -m 0755 ${UNPACKDIR}/init_readonlyfs-emmc.sh ${D}${nonarch_libdir}/systemd/init_readonlyfs.sh
                     else
                            install -m 0755 ${UNPACKDIR}/init_readonlyfs.sh ${D}${nonarch_libdir}/systemd/init_readonlyfs.sh
                     fi
              fi
       fi
}

FILES:${PN} += " ${nonarch_libdir}/systemd/init_readonlyfs.sh "
FILES:${PN} += "${sbindir}"
PACKAGECONFIG:remove = "vconsole"
