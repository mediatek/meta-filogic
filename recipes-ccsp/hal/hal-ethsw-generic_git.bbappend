SRC_URI += "git://gerrit.mediatek.inc/gateway/rdk-b/rdkb_hal;protocol=https;branch=master;destsuffix=git/source/ethsw/rdkb_hal"

SRCREV = "${AUTOREV}"

CFLAGS:append = "${@bb.utils.contains('DISTRO_FEATURES', 'rdkb_wan_manager', ' -DFEATURE_RDKB_WAN_MANAGER ', '', d)}"

CFLAGS:append = "${@bb.utils.contains('DISTRO_FEATURES', 'three_gmac', ' -DTHREE_GMACS_SUPPORT ', '', d)}"

do_configure:prepend(){
   if ${@bb.utils.contains('DISTRO_FEATURES','switch_gsw_mode','true','false',d)}; then
   ln -sf ${S}/rdkb_hal/src/ethsw/ccsp_hal_ethsw_gsw.c ${S}/ccsp_hal_ethsw.c
   else
   ln -sf ${S}/rdkb_hal/src/ethsw/ccsp_hal_ethsw.c ${S}/ccsp_hal_ethsw.c
   fi
}
