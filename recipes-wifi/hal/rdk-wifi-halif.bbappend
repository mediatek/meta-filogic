FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI:append = " \
        file://sta-network.patch;apply=no \
        file://0002-Add-EHT-support.patch;apply=no \
    "

#This is workaround for missing do_patch when RDK uses external sources
do_filogic_patches() {
    cd ${S}
        if [ ! -e patch_applied ]; then
            patch -p1 < ${WORKDIR}/sta-network.patch
            patch -p1 < ${WORKDIR}/0002-Add-EHT-support.patch
            touch patch_applied
        fi
}
addtask filogic_patches after do_unpack before do_compile

CFLAGS:append = " -DWIFI_HAL_VERSION_3"
