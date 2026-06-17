# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 MediaTek Inc.
#

require optee-os.inc

DEPENDS += "optee-os-tadevkit optee-apps"

EARLY_TA_PATH = "$(find ${STAGING_DIR_HOST}/usr/lib/early -type f)"
EXTRA_OEMAKE += 'EARLY_TA_PATHS="${EARLY_TA_PATH}"'

EXTRA_OEMAKE += " \
	CFG_BUILD_IN_TREE_TA=n \
	CFG_CORE_DYN_SHM=y \
	CFG_BL32_TZRAM_BASE="0x4fa00000" \
	CFG_BL32_TZRAM_SIZE="0x500000" \
	CFG_BL32_LOAD_OFFSET="0x1000" \
	CFG_DRAM_BASE="0x40000000" \
	CFG_DRAM_SIZE="0x40000000" \
"

do_deploy() {
    install -d ${DEPLOYDIR}
    install -m 644 ${S}/out/arm-plat-mediatek/core/tee.bin ${DEPLOYDIR}/tee.bin
}
