DESCRIPTION = "Linux Kernel for Mediatek SoCs"
SECTION = "kernel"
LICENSE = "GPLv2"

COMPATIBLE_MACHINE = "^filogic$"

PE = "1"
PV = "${LINUX_VERSION}+git${SRCPV}"

inherit kernel siteinfo
require recipes-kernel/linux/linux-yocto.inc

LIC_FILES_CHKSUM="file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

KERNEL_MODULE_AUTOLOAD += "${@bb.utils.contains("MACHINE_FEATURES", "pitft28r", "stmpe-ts", "", d)}"

# A LOADADDR is needed when building a uImage format kernel. This value is not
# set by default in rpi-4.8.y and later branches so we need to provide it
# manually. This value unused if KERNEL_IMAGETYPE is not uImage.
# KERNEL_EXTRA_ARGS += "LOADADDR=0x48008000"

# Set a variable in .configure
# $1 - Configure variable to be set
# $2 - value [n/y/value]
