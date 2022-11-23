SRC_URI += "git://gerrit.mediatek.inc/gateway/rdk-b/rdkb_hal;protocol=https;destsuffix=git/source/ethsw/rdkb_hal"

SRCREV = "${AUTOREV}"

CFLAGS_append = "${@bb.utils.contains('DISTRO_FEATURES', 'rdkb_wan_manager', ' -DFEATURE_RDKB_WAN_MANAGER ', '', d)}"

CFLAGS_append_mt7988 = " -DTHREE_GMACS_SUPPORT" 

do_configure_prepend(){
   if ${@bb.utils.contains('DISTRO_FEATURES','switch_gsw_mode','true','false',d)}; then
   ln -sf ${S}/rdkb_hal/src/ethsw/ccsp_hal_ethsw_gsw.c ${S}/ccsp_hal_ethsw.c
   else
   ln -sf ${S}/rdkb_hal/src/ethsw/ccsp_hal_ethsw.c ${S}/ccsp_hal_ethsw.c
   fi
}
