SRC_URI:remove = "\
        file://0001-blobmsg-fix-array-out-of-bounds-GCC-10-warning.patch \
        git://git.openwrt.org/project/libubox.git;branch=master \
        "
SRC_URI:append = "git://git@github.com/openwrt/libubox.git;protocol=https;branch=openwrt-25.12"

wifi6_ver = "b14c4688612c05c78ce984d7bde633bce8703b1e"
wifi7_ver = "7dd127841e82eb1cfb61185da37dde7b9bd9ba6d"

SRCREV = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', '${wifi7_ver}', '${wifi6_ver}', d)}"
