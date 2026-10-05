SRC_URI:remove = "${CMF_GITHUB_ROOT}/hardware-abstraction-layer;protocol=https;${BRANCH_hardware_abstraction_layer};name=dhcpv4hal"
SRC_URI = "${CMF_GITHUB_ROOT}/hardware-abstraction-layer;protocol=https;${BRANCH_hardware_abstraction_layer};destsuffix=${BP};name=dhcpv4hal"
SRC_URI += "git://github.com/mediatek/rdkb_hal;protocol=https;destsuffix=${BP}/source/dhcpv4c/rdkb_hal;branch=main"

SRCREV = "${AUTOREV}"
S = "${UNPACKDIR}/${BP}/source/dhcpv4c"
#CFLAGS:append = " -DUDHCPC_SWITCH "

do_configure:prepend(){
    rm ${S}/dhcpv4c_api.c
    ln -sf ${S}/rdkb_hal/src/dhcpv4c/dhcpv4c_api.c ${S}/dhcpv4c_api.c
}

