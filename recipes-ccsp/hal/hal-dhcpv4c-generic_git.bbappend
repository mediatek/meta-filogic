SRC_URI += "git://gerrit.mediatek.inc/gateway/rdk-b/rdkb_hal;protocol=https;destsuffix=git/source/dhcpv4c/rdkb_hal;branch=master"

SRCREV = "${AUTOREV}"

#CFLAGS:append = " -DUDHCPC_SWITCH "

do_configure:prepend(){
    rm ${S}/dhcpv4c_api.c
    ln -sf ${S}/rdkb_hal/src/dhcpv4c/dhcpv4c_api.c ${S}/dhcpv4c_api.c
}

