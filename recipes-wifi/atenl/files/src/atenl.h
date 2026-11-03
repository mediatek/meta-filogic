/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (C) 2021-2022 Mediatek Inc. */
#ifndef __ATENL_H
#define __ATENL_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/nl80211.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unl.h>

#include "nl.h"
#include "util.h"
#include "debug.h"

/* workaround for legacy codebase */
#undef NL80211_ATTR_WIPHY_RADIOS
#undef NL80211_ATTR_MAX
#define NL80211_ATTR_WIPHY_RADIOS	331
#define NL80211_ATTR_MAX		(__NL80211_ATTR_AFTER_LAST > NL80211_ATTR_WIPHY_RADIOS ? \
					 __NL80211_ATTR_AFTER_LAST - 1 : \
					 NL80211_ATTR_WIPHY_RADIOS + 1)

#define PRE_CAL_INFO		16
#define DPD_INFO_CH_SHIFT	30
#define DPD_INFO_2G_SHIFT	20
#define DPD_INFO_5G_SHIFT	10
#define DPD_INFO_6G_SHIFT	0
#define DPD_INFO_MASK		GENMASK(9, 0)
#define MT_EE_CAL_UNIT		1024

#define set_band_val(_an, _band, _field, _val)	\
	_an->anb[_band]._field = (_val)
#define get_band_val(_an, _band, _field)	\
	(_an->anb[_band]._field)

enum atenl_eep_mode {
	EEP_EFUSE_MODE = 1,
	EEP_FLASH_MODE,
	EEP_EXT_MODE,
	EEP_BIN_MODE,
	EEP_DEFAULT_BIN_MODE,
};

struct atenl_band {
	bool valid;
	u8 phy_idx;
	u8 cap;
	u8 chainmask;
	u8 rx_chainmask;
};

#define MAX_BAND_NUM	3

struct atenl {
	struct atenl_band anb[MAX_BAND_NUM];
	u16 chip_id;
	u16 adie_id;
	u8 main_phy_idx;

	const char *flash_part;
	u32 flash_offset;
	u8 band_idx;
	u8 *eeprom_data;
	int eeprom_fd;
	u16 eeprom_size;
	u32 eeprom_prek_offs;
	enum atenl_eep_mode eeprom_mode;

	u8 *cal;
	u32 cal_size;
	u32 cal_data_offs;
	bool clear_cal;

	bool is_single_wiphy;

	struct unl unl;
};

struct atenl_eeprom_cmd {
	const char *name;
	int (*handler)(struct atenl *an, const char *args);
};

enum atenl_flash_type {
	FLASH_TYPE_MTD,
	FLASH_TYPE_EMMC,
	FLASH_TYPE_UBI,
};

enum atenl_band_type {
	BAND_TYPE_UNUSE,
	BAND_TYPE_2G,
	BAND_TYPE_5G,
	BAND_TYPE_2G_5G,
	BAND_TYPE_6G,
	BAND_TYPE_2G_6G,
	BAND_TYPE_5G_6G,
	BAND_TYPE_2G_5G_6G,
};

/* for mt7915 */
enum {
	MT_EE_BAND_SEL_DEFAULT,
	MT_EE_BAND_SEL_5GHZ,
	MT_EE_BAND_SEL_2GHZ,
	MT_EE_BAND_SEL_DUAL,
};

/* for mt7916/mt7981/mt7986 */
enum {
	MT_EE_BAND_SEL_2G,
	MT_EE_BAND_SEL_5G,
	MT_EE_BAND_SEL_6G,
	MT_EE_BAND_SEL_5G_6G,
};

/* for connac 3 & 5 */
enum {
	MT_EE_CONNAC3_BAND_SEL_DEFAULT,
	MT_EE_CONNAC3_BAND_SEL_2GHZ,
	MT_EE_CONNAC3_BAND_SEL_5GHZ,
	MT_EE_CONNAC3_BAND_SEL_6GHZ,
	MT_EE_CONNAC3_BAND_SEL_5GHZ_LOW,
	MT_EE_CONNAC3_BAND_SEL_5GHZ_HIGH,
	MT_EE_CONNAC3_BAND_SEL_6GHZ_LOW,
	MT_EE_CONNAC3_BAND_SEL_6GHZ_HIGH,
};

#define MT_EE_WIFI_CONF				0x190
#define MT_EE_WIFI_CONF0_BAND_SEL		GENMASK(7, 6)
#define MT_EE_WIFI_CONNAC3_CONF_BAND0_SEL	GENMASK(2, 0)
#define MT_EE_WIFI_CONNAC3_CONF_BAND1_SEL	GENMASK(5, 3)
#define MT_EE_WIFI_CONNAC3_CONF_BAND2_SEL	GENMASK(2, 0)

#define MT_EE_WIFI_CONNAC5_CONF(_band)		(0x170 + (_band) * 0x10)
#define MT_EE_WIFI_CONNAC5_CONF_BAND_SEL	GENMASK(2, 0)
#define MT_EE_WIFI_CONNAC5_CONF_TX_PATH		GENMASK(2, 0)
#define MT_EE_WIFI_CONNAC5_CONF_RX_PATH		GENMASK(5, 3)

#define MT_EE_DO_RX_GAIN_CAL			0x1a1
#define MT_EE_RX_GAIN_CAL			0x1830

#define MT_EE_CAL_RX_GAIN_SIZE			748

#define MT_EE_CONNAC5_DO_RX_GAIN_CAL(_band)	(0x1411 + (_band) * 0x400)
#define MT_EE_CONNAC5_RX_GAIN_CAL		0x5b00
#define MT_EE_CONNAC5_CAL_RX_GAIN_SIZE		0x440

enum {
	MT7976_ONE_ADIE_DBDC		= 0x7,
	MT7975_ONE_ADIE_SINGLE_BAND	= 0x8, /* AX7800 */
	MT7976_ONE_ADIE_SINGLE_BAND	= 0xa, /* AX7800 */
	MT7975_DUAL_ADIE_DBDC		= 0xd, /* AX6000 */
	MT7976_DUAL_ADIE_DBDC		= 0xf, /* AX6000 */
};

#define MT7916_EEPROM_CHIP_ID		0x7916

/* Wi-Fi6 device id */
#define MT7915_DEVICE_ID		0x7915
#define MT7915_DEVICE_ID_2		0x7916
#define MT7916_DEVICE_ID		0x7906
#define MT7916_DEVICE_ID_2		0x790a
#define MT7981_DEVICE_ID		0x7981
#define MT7986_DEVICE_ID		0x7986

/* Wi-Fi7 device id */
#define MT7996_DEVICE_ID		0x7990
#define MT7996_DEVICE_ID_2		0x7991
#define MT7992_DEVICE_ID		0x7992
#define MT7992_DEVICE_ID_2		0x799a
#define MT7990_DEVICE_ID		0x7993
#define MT7990_DEVICE_ID_2		0x799b

/* Wi-Fi8 device id */
#define MT7999_DEVICE_ID		0x80F2
#define MT7999_DEVICE_ID_2		0x80F3

static inline bool is_mt7915(struct atenl *an)
{
	return an->chip_id == MT7915_DEVICE_ID;
}

static inline bool is_mt7916(struct atenl *an)
{
	/* Merlin is special case:
	 * pcie id is 0x7906/0x790a but eeprom chip id use 0x7916,
	 * since 0x7916 is already used by the second pcie of Harrier.
	 */
	return (an->chip_id == MT7916_EEPROM_CHIP_ID) ||
	       (an->chip_id == MT7916_DEVICE_ID);
}

static inline bool is_mt7981(struct atenl *an)
{
	return an->chip_id == MT7981_DEVICE_ID;
}

static inline bool is_mt7986(struct atenl *an)
{
	return an->chip_id == MT7986_DEVICE_ID;
}

static inline bool is_connac2(struct atenl *an)
{
	return is_mt7915(an) || is_mt7916(an) || is_mt7981(an) || is_mt7986(an);
}

static inline bool is_mt7996(struct atenl *an)
{
	return an->chip_id == MT7996_DEVICE_ID;
}

static inline bool is_mt7992(struct atenl *an)
{
	return an->chip_id == MT7992_DEVICE_ID;
}

static inline bool is_mt7990(struct atenl *an)
{
	return an->chip_id == MT7990_DEVICE_ID;
}

static inline bool is_connac3(struct atenl *an)
{
	return is_mt7996(an) || is_mt7992(an) || is_mt7990(an);
}

static inline bool is_mt7999(struct atenl *an)
{
	return an->chip_id == MT7999_DEVICE_ID;
}

static inline bool is_connac5(struct atenl *an)
{
	return is_mt7999(an);
}

int atenl_nl_init(struct atenl *an);
int atenl_nl_check_flash(struct atenl *an);
int atenl_nl_write_eeprom(struct atenl *an, u32 offset, u8 *val);
int atenl_nl_write_efuse_all(struct atenl *an);
int atenl_nl_write_ext_eeprom_all(struct atenl *an);
int atenl_nl_update_buffer_mode(struct atenl *an);
int atenl_nl_get_wiphy(struct atenl *an);
int atenl_eeprom_init(struct atenl *an, u8 phy_idx);
void atenl_eeprom_close(struct atenl *an);
int atenl_eeprom_read_from_driver(struct atenl *an, u32 offset, int len);
int atenl_eeprom_cmd_handler(struct atenl *an, u8 phy_idx, char *cmd);
int atenl_reg_read(struct atenl *an, u32 offset, u32 *res);
int atenl_reg_write(struct atenl *an, u32 offset, u32 val);
int atenl_rf_read(struct atenl *an, u32 wf_sel, u32 offset, u32 *res);
int atenl_rf_write(struct atenl *an, u32 wf_sel, u32 offset, u32 val);

#endif
