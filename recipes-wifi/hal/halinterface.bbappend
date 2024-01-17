FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " \
        file://sta-network.patch;apply=no \
        file://add_undefined_dfs_function.patch;apply=no \
        file://0001-Add-owe-mode.patch;apply=no \
        file://0002-Add-EHT-support.patch;apply=no \
        file://0003-Add-EHT-support.patch;apply=no \
        file://2023q4dunfell-Add-EHT-support.patch;apply=no \
    "

#This is workaround for missing do_patch when RDK uses external sources
do_filogic_patches() {
    cd ${S}
        if [ ! -e patch_applied ]; then
            patch -p1 < ${WORKDIR}/sta-network.patch
            if ${@bb.utils.contains( 'DISTRO_FEATURES', 'filogic', 'false', 'true', d)}; then
                patch -p1 < ${WORKDIR}/0001-Add-owe-mode.patch
            fi
            if ${@bb.utils.contains( 'DISTRO_FEATURES', '2022q3_support', 'true', 'false', d)}; then
                patch -p1 < ${WORKDIR}/add_undefined_dfs_function.patch
                patch -p1 < ${WORKDIR}/0003-Add-EHT-support.patch
            else
                if [ ${RDK_RELEASE} == "rdkb-2023q4-dunfell" ]; then
                    patch -p1 < ${WORKDIR}/2023q4dunfell-Add-EHT-support.patch
                else
                    patch -p1 < ${WORKDIR}/0002-Add-EHT-support.patch
                fi
            fi
            touch patch_applied
        fi
}
addtask filogic_patches after do_unpack before do_compile

CFLAGS_append = " -DWIFI_HAL_VERSION_3"
