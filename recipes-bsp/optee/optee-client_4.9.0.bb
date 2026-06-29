# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 MediaTek Inc.
#

SUMMARY = "OP-TEE Client API"
DESCRIPTION = "Open Portable Trusted Execution Environment - Normal World Client side of the TEE"
HOMEPAGE = "https://www.op-tee.org/"

LICENSE = "BSD-2-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=69663ab153298557a59c67a60a743e5b"

inherit systemd cmake pkgconfig

DEPENDS += "util-linux"

SRC_URI = "git://github.com/OP-TEE/optee_client.git;protocol=https;branch=master"

SRCREV = "9f5e90918093c1d1cd264d8149081b64ab7ba672"

S = "${WORKDIR}/git"

EXTRA_OECMAKE = " \
	-DCFG_TEE_CLIENT_LOG_LEVEL=1 \
	-DCFG_TEE_SUPP_LOG_LEVEL=1 \
	-DBUILD_SHARED_LIBS=ON \
	-DRPMB_EMU=OFF \
"

EXTRA_OECMAKE += " -DCFG_ENABLE_SYSTEMD=ON -DSYSTEMD_UNIT_DIR=${systemd_system_unitdir}/"
EXTRA_OECMAKE += " -DCFG_TEE_SUPPL_USER=root -DCFG_TEE_SUPPL_GROUP=root"
EXTRA_OECMAKE += " -DCFG_ENABLE_UDEV=ON -DUDEV_UDEV_DIR=${nonarch_base_libdir}/udev/rules.d/"

SYSTEMD_SERVICE:${PN} = "tee-supplicant@teepriv0.service"
SYSTEMD_AUTO_ENABLE = "enable"

PACKAGES =+ "${PN}-teeacl ${PN}-seteec ${PN}-ckteec"

FILES:${PN} = " \
	${sbindir}/tee-supplicant \
	${systemd_system_unitdir}/tee-supplicant@.service \
	${libdir}/libteec.so.* \
	${nonarch_base_libdir}/udev/rules.d/ \
"

FILES:${PN}-teeacl = "${libdir}/libteeacl.so.*"
FILES:${PN}-seteec = "${libdir}/libseteec.so.*"
FILES:${PN}-ckteec = "${libdir}/libckteec.so.*"
