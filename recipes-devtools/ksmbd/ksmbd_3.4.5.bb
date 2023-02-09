SUMMARY = "a linux kernel server which implements SMB3 protocol in kernel space for sharing files over network"
SECTION = "Drivers"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

inherit module

DEPENDS = "virtual/kernel"

SRC_URI = " \
    git://git@github.com/cifsd-team/ksmbd.git;protocol=https;tag=${PV} \
    file://COPYING;subdir=git \
    "

S = "${WORKDIR}/git"

PKG_MAKE_FLAGS = " \
    CONFIG_SMB_INSECURE_SERVER=y \
    "

EXTRA_OEMAKE = " \
    -C ${STAGING_KERNEL_BUILDDIR}/ \
    M=${S} \
    EXTRA_CFLAGS="-DCONFIG_SMB_INSECURE_SERVER=1" \
    ${PKG_MAKE_FLAGS} \
    CONFIG_SMB_SERVER=m \
    "

MAKE_TARGETS = "modules"
#do_make_scripts[depends] += "virtual/kernel:do_shared_workdir"
do_install() {
    install -d ${D}/lib/modules/${KERNEL_VERSION}/
    install -m 0644 ${B}/ksmbd.ko ${D}/lib/modules/${KERNEL_VERSION}/
}

KERNEL_MODULE_AUTOLOAD += "ksmbd"