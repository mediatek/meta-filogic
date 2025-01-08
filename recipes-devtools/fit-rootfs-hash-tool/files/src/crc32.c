// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 MediaTek Inc. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 *
 * Simple CRC32 implementation
 */

#include "crc32.h"

#define CRC32_LE_POLY_DEFAULT		0xedb88320
#define CRC32_TABLE_ITEMS		256

static uint32_t crc32_table[CRC32_TABLE_ITEMS];
static int crc32_table_ok;

static uint32_t crc32_le_calc(uint32_t crc, const uint8_t *data, size_t len,
			      const uint32_t *crc_table)
{
	while (len--)
		crc = crc_table[(uint8_t)(crc ^ *data++)] ^ (crc >> 8);

	return crc;
}

static void crc32_le_init(uint32_t *crc_table, uint32_t poly)
{
	uint32_t i, j, v;

	for (i = 0; i < CRC32_TABLE_ITEMS; i++) {
		v = i;

		for (j = 0; j < 8; j++)
			v = (v >> 1) ^ ((v & 1) ? poly : 0);

		crc_table[i] = v;
	}
}

uint32_t crc32(uint32_t crc, const void *data, size_t len)
{
	size_t i;

	if (!crc32_table_ok) {
		crc32_le_init(crc32_table, CRC32_LE_POLY_DEFAULT);
		crc32_table_ok = 1;
	}

	return crc32_le_calc(crc ^ 0xffffffff, data, len, crc32_table) ^ 0xffffffff;
}
