# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 MediaTek Inc.
#

SUMMARY = "OP-TEE Trusted OS"
DESCRIPTION = "Open Portable Trusted Execution Environment - Trusted side of the TEE"
HOMEPAGE = "https://www.op-tee.org/"

LICENSE = "BSD-2-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=c1f21c4f72f372ef38a5a4aee55ec173"

inherit deploy python3native

DEPENDS = "python3-pyelftools-native python3-cryptography-native dtc-native"

export OPENSSL_MODULES = "${STAGING_LIBDIR_NATIVE}/ossl-modules"

FILESEXTRAPATHS:prepend := "${THISDIR}/files/optee-os:"

SRC_URI = "git://github.com/OP-TEE/optee_os.git;protocol=https;branch=master \
           file://patches/001-specify-python3-for-gen-tee-bin.patch"

SRCREV = "c2b0684fcd89929976a8726e6e3af922b48dd2c7"

S = "${WORKDIR}/git"

COMPATIBLE_MACHINE = "(mt7988|mt7987|mt7986)"

OPTEEMACHINE ?= "${@ \
	'mt7988' if 'filogic880' in d.getVar('MACHINE') else \
	'mt7987' if 'filogic850' in d.getVar('MACHINE') else \
	'mt7986' if 'filogic830' in d.getVar('MACHINE') else \
    'mt7988' \
}"

EXTRA_OEMAKE += " \
    PLATFORM="mediatek" \
    PLATFORM_FLAVOR=${OPTEEMACHINE} \
    CFG_ARM64_core=y \
    CROSS_COMPILE=${HOST_PREFIX} \
    CROSS_COMPILE64=${HOST_PREFIX} \
    CROSS_COMPILE_core=${HOST_PREFIX} \
    CROSS_COMPILE_ta_arm64=${HOST_PREFIX} \
    CROSS_COMPILE_ta_arm32=${HOST_PREFIX} \
    CFG_USER_TA_TARGETS=ta_arm64 \
    CFG_TEE_CORE_LOG_LEVEL=2 \
    CFG_TEE_TA_LOG_LEVEL=2 \
    DEBUG=0 \
"

LDFLAGS = ""
CFLAGS += "--sysroot=${STAGING_DIR_HOST}"

include optee-os-extra.inc
