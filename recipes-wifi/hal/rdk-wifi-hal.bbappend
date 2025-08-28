FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " \
  ${@bb.utils.contains('DISTRO_FEATURES','OneWifi','','file://fix_build_fail.patch',d)} \
"


