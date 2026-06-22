# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 MediaTek Inc.
#

OPTEE_KEYDIR ?= "${SECURE_BOOT_KEYDIR_secureboot}"
TA_SIGN_KEYNAME ?= "default.pem"

TA_SIGN_KEY = "${OPTEE_KEYDIR}/${TA_SIGN_KEYNAME}"
TA_PUBLIC_KEY = "${TA_SIGN_KEY}"

EXTRA_OEMAKE += " \
	TA_SIGN_KEY=${TA_SIGN_KEY} \
	TA_PUBLIC_KEY=${TA_PUBLIC_KEY} \
"
