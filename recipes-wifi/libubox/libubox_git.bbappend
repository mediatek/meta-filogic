SRC_URI_remove = "file://0001-blobmsg-fix-array-out-of-bounds-GCC-10-warning.patch"

wifi6_ver = "b14c4688612c05c78ce984d7bde633bce8703b1e"
wifi7_ver = "ca3f6d0cdb1e588283c42d039779ceab303ceef2"

SRCREV = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', '${wifi7_ver}', '${wifi6_ver}', d)}"
