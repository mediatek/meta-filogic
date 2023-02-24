FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " file://*.patch;apply=no \
    "

#This is workaround for missing do_patch when RDK uses external sources
do_filogic_patches() {
    cd ${S}
        if [ ! -e patch_applied ]; then
            patch -p1 < ${WORKDIR}/sta-network.patch
            patch -p1 < ${WORKDIR}/add_undefined_dfs_function.patch
            patch -p1 < ${WORKDIR}/0001-Add-owe-mode.patch
            if ${@bb.utils.contains('DISTRO_FEATURES','wifi_eht','true','false',d)}; then
                patch -p1 < ${WORKDIR}/0002-Add-EHT-support.patch
            fi
            touch patch_applied
        fi
}
addtask filogic_patches after do_unpack before do_compile

CFLAGS_append = " -DWIFI_HAL_VERSION_3"
