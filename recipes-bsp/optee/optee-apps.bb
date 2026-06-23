# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 MediaTek Inc.
#

SUMMARY = "MTK Proprietary TAs"
DESCRIPTION = "MTK Proprietary TAs"

LICENSE = "BSD-2-Clause"

inherit python3native

require optee-ta-sign.inc

DEPENDS = "optee-client optee-os-tadevkit python3-cryptography-native openssl"

S = "${WORKDIR}/git"

TA_DEV_KIT_DIR = "${STAGING_DIR_HOST}${libdir}/export-ta_arm64"

LDFLAGS = ""
CFLAGS += "--sysroot=${STAGING_DIR_HOST}"
export OPENSSL_MODULES = "${STAGING_LIBDIR_NATIVE}/ossl-modules"

EXTRA_OEMAKE += " \
		TA_DEV_KIT_DIR=${TA_DEV_KIT_DIR} \
		OPTEE_CLIENT_EXPORT=${STAGING_DIR_HOST} \
		CROSS_COMPILE=${HOST_PREFIX} \
		O=${S}/out \
"
EXTRA_OEMAKE += " \
		CFG_MTK_RNG=y \
		CFG_MTK_SECURE_STORAGE=y \
		${@bb.utils.contains('DISTRO_FEATURES', 'firmware_encryption', 'CFG_FW_ENC_EARLY_TA=y', '', d)} \
		CFG_OVERLAYFS_ENCRYPTION_TA=y \
"

do_install() {
	install -d ${D}${nonarch_base_libdir}/optee_armtz/
	install -d ${D}${bindir}
	install -d ${D}${includedir}
	install -d ${D}${libdir}/early/apps

	if [ -d ${S}/out ]; then
		oe_runmake install DESTDIR=${S}/install_dir
	fi

	install ${S}/install_dir/bin/* ${D}${bindir} || true
	install -m 644 ${S}/install_dir/lib/optee_armtz/* \
		${D}${nonarch_base_libdir}/optee_armtz/ || true
	install ${S}/install_dir/ta/early/*.stripped.elf \
		${D}/usr/lib/early/apps || true
	install -m 644 ${S}/install_dir/include/* \
		${D}${includedir} || true
}

PACKAGES =+ "${PN}-earlyta"

FILES:${PN} = " \
	${bindir} \
	${nonarch_base_libdir}/optee_armtz/ \
"
FILES:${PN}-dev = "${includedir}"
FILES:${PN}-earlyta = "/usr/lib/early/apps"

ERROR_QA:remove = "already-stripped"

RDEPENDS:${PN} = "optee-client"

include optee-apps-extra.inc
