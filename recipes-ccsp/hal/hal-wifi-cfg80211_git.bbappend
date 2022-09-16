
BRANCH_MTK_IMPL= "master"

SRC_URI = "${CMF_GITHUB_ROOT}/hal-wifi-cfg80211;protocol=${CMF_GIT_PROTOCOL};branch=${BRANCH_MTK_IMPL};name=wifihal"
SRCREV_wifihal_remove = "${AUTOREV}"
SRCREV_wifihal = "16a86d50a6485b74f646355925ae64957bcca4ad"
FILESEXTRAPATHS_remove := "${THISDIR}/files:"
FILESEXTRAPATHS_prepend := "${THISDIR}/hal-wifi-patches:"

SRC_URI += "file://*.patch;apply=no"

do_mtk_patches() {
	cd ${S}/../../
	if [ ! -e mtk_wifi_patch_applied ]; then
		for i in ${WORKDIR}/*.patch; do patch -p1 < $i; done
	fi
	touch mtk_wifi_patch_applied
}
addtask mtk_patches after do_unpack before do_compile

CFLAGS_append = " -DWIFI_HAL_VERSION_3"
