FILESEXTRAPATHS_append := "${THISDIR}/files:"

CFLAGS_append += " -DWIFI_HAL_VERSION_3 -DCONFIG_DFS -D_WIFI_AX_SUPPORT_ -D_PLATFORM_TURRIS_ "

SRC_URI_append += " \
    file://Fix-dmcli-can-not-set-password.patch;apply=no \
"

# we need to patch to code for ccsp-wifi-agent
do_filogic_patches() {
    cd ${S}
    if [ ! -e filogic_patch_applied ]; then
        patch  -p1 < ${WORKDIR}/Fix-dmcli-can-not-set-password.patch ${S}/source/TR-181/ml/cosa_wifi_dml.c
        touch filogic_patch_applied
    fi
}
addtask filogic_patches after do_unpack before do_configure



