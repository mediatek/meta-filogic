SRC_URI:remove = "file://0001-blobmsg-fix-array-out-of-bounds-GCC-10-warning.patch"

wifi6_ver = "b14c4688612c05c78ce984d7bde633bce8703b1e"
wifi7_ver = "7928f1711703767b0ac5ce916c7231d0e1af1ca2"

SRCREV = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', '${wifi7_ver}', '${wifi6_ver}', d)}"
