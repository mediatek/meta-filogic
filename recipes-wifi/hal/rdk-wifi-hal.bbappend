FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " \
  file://fix_build_fail.patch;apply=no \
"

do_filogic_patches() {
    DISTRO_ONE_WIFI_ENABLED="${@bb.utils.contains('DISTRO_FEATURES','OneWifi','true','false',d)}"
    cd ${S}

    if [ ! -e filogic_patch_applied ] && [ $DISTRO_ONE_WIFI_ENABLED = 'false' ] && ([ "${PROJECT_BRANCH}" != "rdkb-2025q2-kirkstone" ]); then
        patch -p1 < ${WORKDIR}/fix_build_fail.patch
        touch filogic_patch_applied
    fi
}
addtask filogic_patches after do_unpack before do_configure


