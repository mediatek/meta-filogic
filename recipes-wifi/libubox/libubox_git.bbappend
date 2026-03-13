SRC_URI:remove = "file://0001-blobmsg-fix-array-out-of-bounds-GCC-10-warning.patch"

wifi6_ver = "b14c4688612c05c78ce984d7bde633bce8703b1e"
wifi7_ver = "1aa36ee774c8db4d7a396903e0d2e1fb79ee8bf1"

SRCREV = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', '${wifi7_ver}', '${wifi6_ver}', d)}"
