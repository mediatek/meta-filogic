# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 MediaTek Inc.
#

require optee-os.inc

SUMMARY = "OP-TEE Trusted OS TA devkit"
DESCRIPTION = "OP-TEE TA devkit for build TAs"
HOMEPAGE = "https://www.op-tee.org/"

do_install() {
	install -d ${D}${libdir}
	install -d ${D}${libdir}/early/tadevkit
	install -d ${D}${nonarch_base_libdir}/optee_armtz/

	cp -r --no-preserve=ownership ${B}/out/arm-plat-mediatek/export-ta_arm64 \
		${D}${libdir}

	install ${B}/out/arm-plat-mediatek/ta/trusted_keys/*.stripped.elf \
		${D}${libdir}/early/tadevkit/
	install -m 644 ${B}/out/arm-plat-mediatek/ta/pkcs11/*.ta \
		${D}${nonarch_base_libdir}/optee_armtz
}

PACKAGES =+ "${PN}-earlyta"

FILES:${PN} = "${nonarch_base_libdir}/optee_armtz/*.ta"
FILES:${PN}-staticdev = "${libdir}/*.a"
FILES:${PN}-dev = "${libdir}"
FILES:${PN}-earlyta = "/usr/lib/early/tadevkit"

INSANE_SKIP:${PN}-dev = "staticdev"
ERROR_QA:remove = "already-stripped"
