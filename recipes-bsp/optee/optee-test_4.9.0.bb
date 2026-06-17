# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 MediaTek Inc.
#

SUMMARY = "OP-TEE Sanity Test Suite"
DESCRIPTION = "Open Portable Trusted Execution Environment - Test suite"
HOMEPAGE = "https://www.op-tee.org/"

LICENSE = "BSD-2-Clause & GPL-2.0-only"

inherit python3native

DEPENDS = "optee-client optee-os-tadevkit optee-apps python3-cryptography-native openssl"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=a8fa504109e4cd7ea575bc49ea4be560 \
			file://LICENSE-BSD;md5=dca16d6efa93b55d0fd662ae5cd6feeb \
			file://LICENSE-GPL;md5=10e86b5d2a6cb0e2b9dcfdd26a9ac58d \
"

FILESEXTRAPATHS:prepend := "${THISDIR}/files/optee-test:"

SRC_URI = "git://github.com/OP-TEE/optee_test.git;protocol=https;branch=master"

SRCREV = "b27648ea8472cceceb8dda368a965c709066f7aa"

S = "${WORKDIR}/git"

TA_DEV_KIT_DIR = "${STAGING_DIR_HOST}${libdir}/export-ta_arm64"

LDFLAGS = ""
CFLAGS += "--sysroot=${STAGING_DIR_HOST}"
export OPENSSL_MODULES = "${STAGING_LIBDIR_NATIVE}/ossl-modules"

EXTRA_OEMAKE += " \
		TA_DEV_KIT_DIR=${TA_DEV_KIT_DIR} \
		OPTEE_CLIENT_EXPORT=${STAGING_DIR_HOST}/usr \
		CROSS_COMPILE=${HOST_PREFIX} \
"

do_install() {
	install -d ${D}${nonarch_base_libdir}/optee_armtz/
	install -d ${D}${bindir}
	install -d ${D}${libdir}/tee-supplicant/plugins/

	oe_runmake install DESTDIR=${S}/install_dir

	install -m 644 ${S}/install_dir/usr/lib/optee_armtz/* \
		${D}${nonarch_base_libdir}/optee_armtz
	install ${S}/install_dir/usr/bin/xtest ${D}${bindir}
	install ${S}/install_dir/*.plugin ${D}${libdir}/tee-supplicant/plugins/
}

FILES:${PN} = " \
	${nonarch_base_libdir}/optee_armtz/*.ta \
	${bindir}/xtest \
	${libdir}/tee-supplicant/plugins/ \
"

RDEPENDS:${PN} = "optee-client"

include optee-test-extra.inc
