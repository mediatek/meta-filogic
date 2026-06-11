# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 MediaTek Inc.
#

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " file://tools/0001-image-fit-make-string-of-algo-param-const.patch \
             file://tools/0002-mkimage-allow-to-specify-sign-algo-on-cmdline.patch \
             file://tools/0003-lib-rsa-add-rsa3072-algo.patch \
             file://tools/0004-lib-rsa-set-salt-length-rsa-pss-param.patch \
             file://tools/0980-mkimage-add-offline-sign-support.patch \
           "
