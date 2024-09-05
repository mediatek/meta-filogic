SUMMARY = "Mediatek PCE Driver"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit module

DEPENDS += "virtual/kernel"

SRC_URI = "git://gerrit.mediatek.inc/openwrt/feeds/mtk_openwrt_feeds;protocol=https;branch=master;destsuffix=git"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/feed/kernel/pce/src"

PKG_MAKE_FLAGS = " \
	CONFIG_MTK_PCE=m \
	"

EXTRA_CFLAGS += " \
	-Wall -Werror \
	-I${S}/inc \
	"

EXTRA_OEMAKE = " \
	-C ${STAGING_KERNEL_BUILDDIR}/ \
	M=${S} \
	${PKG_MAKE_FLAGS} \
	EXTRA_CFLAGS='${EXTRA_CFLAGS}' \
	"

do_configure[noexec] = "1"
MAKE_TARGETS = "modules"
do_make_scripts[depends] += "virtual/kernel:do_shared_workdir"

do_install_append() {
	# Install header files for depedency modules
	cp -r ${S}/inc/pce ${STAGING_KERNEL_BUILDDIR}/source/include

	# Module
	cat ${D}/usr/include/pce/Module.symvers >> ${TMPDIR}/work-shared/${MACHINE}/kernel-build-artifacts/Module.symvers
}
