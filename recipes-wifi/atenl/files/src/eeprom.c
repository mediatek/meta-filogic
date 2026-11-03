// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 MediaTek Inc.
 */

#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <blkid/blkid.h>

#include "atenl.h"

char *eeprom_file;

static char*
atenl_eeprom_mode_str(enum atenl_eep_mode mode)
{
	switch (mode) {
	case EEP_EFUSE_MODE:
		return "efuse";
	case EEP_FLASH_MODE:
		return "flash";
	case EEP_EXT_MODE:
		return "external eeprom";
	case EEP_BIN_MODE:
		return "binfile";
	case EEP_DEFAULT_BIN_MODE:
	default:
		return "default bin";
	}
}

static void
atenl_eeprom_set_current_mode(struct atenl *an)
{
	/* TODO: add efuse/external eeprom mode */
	if (an->flash_part)
		an->eeprom_mode = EEP_FLASH_MODE;
	else if (!(~an->flash_offset))
		an->eeprom_mode = EEP_BIN_MODE;
	else
		an->eeprom_mode = EEP_DEFAULT_BIN_MODE;
}

static int
atenl_eeprom_init_chip_id(struct atenl *an, u8 *buf)
{
	an->chip_id = *(u16 *)buf;

	switch (an->chip_id) {
	case MT7915_DEVICE_ID:
		an->adie_id = 0x7975;
		break;
	case MT7916_EEPROM_CHIP_ID:
	case MT7916_DEVICE_ID:
	case MT7981_DEVICE_ID:
		an->adie_id = 0x7976;
		break;
	case MT7986_DEVICE_ID: {
		bool is_7975 = false;
		u32 val;

		atenl_reg_read(an, 0x18050000, &val);

		switch (val & 0xf) {
		case MT7975_ONE_ADIE_SINGLE_BAND:
		case MT7975_DUAL_ADIE_DBDC:
			is_7975 = true;
			break;
		default:
			break;
		}

		an->adie_id = is_7975 ? 0x7975 : 0x7976;
		break;
	}
	case MT7996_DEVICE_ID:
	case MT7992_DEVICE_ID:
	case MT7990_DEVICE_ID:
	case MT7999_DEVICE_ID:
		/* TODO: parse info if required */
		break;
	default:
		atenl_err("Unknown chip id %x\n", an->chip_id);
		return -1;
	}

	return 0;
}

static void
atenl_eeprom_init_max_size(struct atenl *an)
{
	switch (an->chip_id) {
	case MT7915_DEVICE_ID:
		an->eeprom_size = 3584;
		an->eeprom_prek_offs = 0x62;
		break;
	case MT7916_EEPROM_CHIP_ID:
	case MT7916_DEVICE_ID:
	case MT7981_DEVICE_ID:
	case MT7986_DEVICE_ID:
		an->eeprom_size = 4096;
		an->eeprom_prek_offs = 0x19a;
		break;
	case MT7996_DEVICE_ID:
	case MT7992_DEVICE_ID:
	case MT7990_DEVICE_ID:
		an->eeprom_size = 7680;
		an->eeprom_prek_offs = 0x1a5;
		break;
	case MT7999_DEVICE_ID:
		an->eeprom_size = 25600;
		an->eeprom_prek_offs = 0x140b;
		break;
	default:
		break;
	}

	if (!is_connac5(an))
		an->cal_data_offs = an->eeprom_size + PRE_CAL_INFO;
	else
		an->cal_data_offs = an->eeprom_size;
}

static int
atenl_create_file(struct atenl *an)
{
	char fname[64], buf[1024];
	int fd_ori, fd;
	ssize_t len;

	atenl_dbg("init eeprom with %s mode\n",
		  atenl_eeprom_mode_str(an->eeprom_mode));

	snprintf(fname, sizeof(fname),
		 "/sys/kernel/debug/ieee80211/phy%d/mt76/eeprom",
		 an->main_phy_idx);

	fd_ori = open(fname, O_RDONLY);
	if (fd_ori < 0)
		return -1;

	fd = open(eeprom_file, O_RDWR | O_CREAT | O_EXCL, 0644);
	if (fd < 0)
		goto out;

	while ((len = read(fd_ori, buf, sizeof(buf))) > 0) {
		ssize_t w, written = 0;

		while (written < len) {
			w = write(fd, buf + written, len - written);
			if (w <= 0) {
				if (w < 0 && errno == EINTR)
					continue;
				atenl_err("write error: %s\n", strerror(errno));
				goto err;
			}
			written += w;
		}
	}

	if (len < 0) {
		atenl_err("Failed to read from %s\n", fname);
		goto err;
	}

	if (!lseek(fd, 0, SEEK_SET))
		goto out;

	atenl_err("Failed to seek to the start of the file\n");
err:
	unlink(eeprom_file);
	close(fd);
	fd = -1;
out:
	close(fd_ori);
	return fd;
}

static off_t
atenl_eeprom_file_exists(char *file)
{
	struct stat st;
	int ret;

	ret = stat(file, &st);

	return ret < 0 ? 0 : st.st_size;
}

static int
atenl_eeprom_init_file(struct atenl *an)
{
	ssize_t len;
	u8 buf[2];
	int fd;

	if (!atenl_eeprom_file_exists(eeprom_file))
		fd = atenl_create_file(an);
	else
		fd = open(eeprom_file, O_RDWR);

	if (fd < 0)
		return fd;

	len = read(fd, buf, sizeof(buf));
	if (len != sizeof(buf))
		goto close;

	if (lseek(fd, 0, SEEK_SET))
		goto close;

	/* make sure the chip id is correct before further processing */
	if (atenl_eeprom_init_chip_id(an, buf))
		goto close;

	atenl_eeprom_init_max_size(an);

	return fd;

close:
	close(fd);
	return -1;
}

static int
atenl_eeprom_sync_precal(struct atenl *an)
{
	int cal_fd = -1, ret = 0;
	ssize_t len, total = 0;
	char precal_file[64];
	off_t size;

	snprintf(precal_file, sizeof(precal_file),
		 "/sys/kernel/debug/ieee80211/phy%d/mt76/precal",
		 an->main_phy_idx);

	size = atenl_eeprom_file_exists(precal_file);
	if (!size)
		return -1;

	cal_fd = open(precal_file, O_RDONLY);
	if (cal_fd < 0) {
		atenl_err("Failed to open precal file %s\n", precal_file);
		return -1;
	}

	an->cal = calloc(1, size);
	if (!an->cal) {
		atenl_err("Failed to allocate precal data\n");
		ret = -1;
		goto out;
	}

	if (an->clear_cal) {
		an->cal_size = size;
		goto out;
	}

	while ((len = read(cal_fd, an->cal + total, size - total)) > 0) {
		total += len;
		if (total == size)
			break;
	}

	if (len < 0) {
		atenl_err("Failed to read precal data\n");
		ret = -1;
		goto out;
	}

	an->cal_size = size;

out:
	close(cal_fd);

	return ret;
}

static void
atenl_eeprom_init_band_cap(struct atenl *an)
{
#define CONNAC3_BAND_SEL(index)		MT_EE_WIFI_CONNAC3_CONF_BAND##index##_SEL
	u8 *eeprom = an->eeprom_data;

	if (is_mt7915(an)) {
		u8 val = eeprom[MT_EE_WIFI_CONF];
		u8 band_sel = FIELD_GET(MT_EE_WIFI_CONF0_BAND_SEL, val);
		struct atenl_band *anb = &an->anb[0];

		/* MT7915A */
		if (band_sel == MT_EE_BAND_SEL_DEFAULT) {
			anb->valid = true;
			anb->cap = BAND_TYPE_2G_5G;
			return;
		}

		/* MT7915D */
		if (band_sel == MT_EE_BAND_SEL_2GHZ) {
			anb->valid = true;
			anb->cap = BAND_TYPE_2G;
		}

		val = eeprom[MT_EE_WIFI_CONF + 1];
		band_sel = FIELD_GET(MT_EE_WIFI_CONF0_BAND_SEL, val);
		anb++;

		if (band_sel == MT_EE_BAND_SEL_5GHZ) {
			anb->valid = true;
			anb->cap = BAND_TYPE_5G;
		}
	} else if (is_mt7916(an) || is_mt7981(an) || is_mt7986(an)) {
		struct atenl_band *anb;
		u8 val, band_sel;
		int i;

		for (i = 0; i < 2; i++) {
			val = eeprom[MT_EE_WIFI_CONF + i];
			band_sel = FIELD_GET(MT_EE_WIFI_CONF0_BAND_SEL, val);
			anb = &an->anb[i];

			anb->valid = true;
			switch (band_sel) {
			case MT_EE_BAND_SEL_2G:
				anb->cap = BAND_TYPE_2G;
				break;
			case MT_EE_BAND_SEL_5G:
				anb->cap = BAND_TYPE_5G;
				break;
			case MT_EE_BAND_SEL_6G:
				anb->cap = BAND_TYPE_6G;
				break;
			case MT_EE_BAND_SEL_5G_6G:
				anb->cap = BAND_TYPE_5G_6G;
				break;
			default:
				break;
			}
		}
	} else if (is_connac3(an)) {
		struct atenl_band *anb;
		u8 val, band_sel;
		u8 band_sel_mask[MAX_BAND_NUM] = {CONNAC3_BAND_SEL(0), CONNAC3_BAND_SEL(1),
						  CONNAC3_BAND_SEL(2)};
		u16 band_sel_offs[MAX_BAND_NUM] = {MT_EE_WIFI_CONF, MT_EE_WIFI_CONF,
						   MT_EE_WIFI_CONF + 1};
		int i;

		for (i = 0; i < MAX_BAND_NUM; i++) {
			val = eeprom[band_sel_offs[i]];
			band_sel = FIELD_GET(band_sel_mask[i], val);
			anb = &an->anb[i];

			anb->valid = true;
			switch (band_sel) {
			case MT_EE_CONNAC3_BAND_SEL_2GHZ:
				anb->cap = BAND_TYPE_2G;
				break;
			case MT_EE_CONNAC3_BAND_SEL_5GHZ_LOW:
			case MT_EE_CONNAC3_BAND_SEL_5GHZ_HIGH:
			case MT_EE_CONNAC3_BAND_SEL_5GHZ:
				anb->cap = BAND_TYPE_5G;
				break;
			case MT_EE_CONNAC3_BAND_SEL_6GHZ_LOW:
			case MT_EE_CONNAC3_BAND_SEL_6GHZ_HIGH:
			case MT_EE_CONNAC3_BAND_SEL_6GHZ:
				anb->cap = BAND_TYPE_6G;
				break;
			default:
				anb->valid = false;
				break;
			}
		}
	} else if (is_connac5(an)) {
		struct atenl_band *anb;
		u8 val, band_sel;
		int i;

		for (i = 0; i < MAX_BAND_NUM; i++) {
			val = eeprom[MT_EE_WIFI_CONNAC5_CONF(i)];
			band_sel = FIELD_GET(MT_EE_WIFI_CONNAC5_CONF_BAND_SEL, val);
			anb = &an->anb[i];

			anb->valid = true;
			switch (band_sel) {
			case MT_EE_CONNAC3_BAND_SEL_2GHZ:
				anb->cap = BAND_TYPE_2G;
				break;
			case MT_EE_CONNAC3_BAND_SEL_5GHZ_LOW:
			case MT_EE_CONNAC3_BAND_SEL_5GHZ_HIGH:
			case MT_EE_CONNAC3_BAND_SEL_5GHZ:
				anb->cap = BAND_TYPE_5G;
				break;
			case MT_EE_CONNAC3_BAND_SEL_6GHZ_LOW:
			case MT_EE_CONNAC3_BAND_SEL_6GHZ_HIGH:
			case MT_EE_CONNAC3_BAND_SEL_6GHZ:
				anb->cap = BAND_TYPE_6G;
				break;
			default:
				anb->valid = false;
				break;
			}
		}
	}
}

static void
atenl_eeprom_init_antenna_cap(struct atenl *an)
{
	switch (an->chip_id) {
	case MT7915_DEVICE_ID:
		if (an->anb[0].cap == BAND_TYPE_2G_5G) {
			an->anb[0].chainmask = FIELD_GET(GENMASK(2, 0),
							 an->eeprom_data[MT_EE_WIFI_CONF]);
			an->anb[0].rx_chainmask = FIELD_GET(GENMASK(5, 3),
							    an->eeprom_data[MT_EE_WIFI_CONF]);
		} else {
			an->anb[0].chainmask = FIELD_GET(GENMASK(1, 0),
							 an->eeprom_data[MT_EE_WIFI_CONF + 3]);
			an->anb[1].chainmask = FIELD_GET(GENMASK(5, 4),
							 an->eeprom_data[MT_EE_WIFI_CONF + 3]);

			an->anb[0].rx_chainmask = FIELD_GET(GENMASK(3, 2),
							    an->eeprom_data[MT_EE_WIFI_CONF + 3]);
			an->anb[1].rx_chainmask = FIELD_GET(GENMASK(7, 6),
							    an->eeprom_data[MT_EE_WIFI_CONF + 3]);
		}
		break;
	case MT7916_EEPROM_CHIP_ID:
	case MT7916_DEVICE_ID:
	case MT7981_DEVICE_ID:
	case MT7986_DEVICE_ID:
		an->anb[0].chainmask = FIELD_GET(GENMASK(2, 0),
						 an->eeprom_data[MT_EE_WIFI_CONF]);
		an->anb[1].chainmask = FIELD_GET(GENMASK(2, 0),
						 an->eeprom_data[MT_EE_WIFI_CONF + 1]);

		an->anb[0].rx_chainmask = FIELD_GET(GENMASK(5, 3),
						    an->eeprom_data[MT_EE_WIFI_CONF]);
		an->anb[1].rx_chainmask = FIELD_GET(GENMASK(5, 3),
						    an->eeprom_data[MT_EE_WIFI_CONF + 1]);
		break;
	case MT7996_DEVICE_ID:
	case MT7992_DEVICE_ID:
	case MT7990_DEVICE_ID:
		an->anb[0].chainmask = FIELD_GET(GENMASK(5, 3),
						 an->eeprom_data[MT_EE_WIFI_CONF + 1]);
		an->anb[1].chainmask = FIELD_GET(GENMASK(2, 0),
						 an->eeprom_data[MT_EE_WIFI_CONF + 2]);
		an->anb[2].chainmask = FIELD_GET(GENMASK(5, 3),
						 an->eeprom_data[MT_EE_WIFI_CONF + 2]);

		an->anb[0].rx_chainmask = FIELD_GET(GENMASK(2, 0),
						    an->eeprom_data[MT_EE_WIFI_CONF + 3]);
		an->anb[1].rx_chainmask = FIELD_GET(GENMASK(5, 3),
						    an->eeprom_data[MT_EE_WIFI_CONF + 3]);
		an->anb[2].rx_chainmask = FIELD_GET(GENMASK(2, 0),
						    an->eeprom_data[MT_EE_WIFI_CONF + 4]);
		break;
	case MT7999_DEVICE_ID: {
		int i, conf_offset;

		for (i = 0; i < MAX_BAND_NUM; i++) {
			conf_offset = MT_EE_WIFI_CONNAC5_CONF(i);
			an->anb[i].chainmask = FIELD_GET(MT_EE_WIFI_CONNAC5_CONF_TX_PATH,
							 an->eeprom_data[conf_offset + 1]);
			an->anb[i].rx_chainmask = FIELD_GET(MT_EE_WIFI_CONNAC5_CONF_RX_PATH,
							    an->eeprom_data[conf_offset + 1]);
		}

		break;
	}
	default:
		break;
	}
}

int atenl_eeprom_init(struct atenl *an, u8 phy_idx)
{
	int eeprom_fd;
	char buf[30];
	void *p;

	set_band_val(an, 0, phy_idx, phy_idx);
	atenl_nl_check_flash(an);
	atenl_eeprom_set_current_mode(an);

	/* Get the first main phy index for this chip.
	 * For single wiphy, phy_idx (i.e. wiphy idx) would be 0 and an->band_idx will also be 0.
	 */
	an->main_phy_idx = phy_idx - an->band_idx;
	snprintf(buf, sizeof(buf), "/tmp/atenl-eeprom-phy%u", an->main_phy_idx);
	eeprom_file = strdup(buf);
	if (!eeprom_file) {
		atenl_err("Failed to allocate memory for eeprom filename\n");
		return -1;
	}

	eeprom_fd = atenl_eeprom_init_file(an);
	if (eeprom_fd < 0) {
		atenl_err("Failed to open eeprom file %s\n", eeprom_file);
		return -1;
	}

	p = mmap(NULL, an->eeprom_size, PROT_READ | PROT_WRITE,
		 MAP_SHARED, eeprom_fd, 0);
	if (p == MAP_FAILED) {
		atenl_err("Failed to mmap file: %s\n", strerror(errno));
		close(eeprom_fd);
		return -1;
	}

	an->eeprom_data = p;
	an->eeprom_fd = eeprom_fd;

	atenl_eeprom_init_band_cap(an);
	atenl_eeprom_init_antenna_cap(an);

	if (get_band_val(an, 1, valid))
		set_band_val(an, 1, phy_idx, phy_idx + 1);

	if (get_band_val(an, 2, valid))
		set_band_val(an, 2, phy_idx, phy_idx + 2);

	return 0;
}

void atenl_eeprom_close(struct atenl *an)
{
	if (!an->eeprom_fd && !an->eeprom_data)
		return;

	msync(an->eeprom_data, an->eeprom_size, MS_SYNC);
	munmap(an->eeprom_data, an->eeprom_size);
	close(an->eeprom_fd);

	free(eeprom_file);
}

static int
atenl_partition_search(char *file, const char *name, char *data, int data_size)
{
	char *target = NULL;
	char dev[128] = {};
	FILE *f;

	f = fopen(file, "r");
	if (!f) {
		atenl_dbg("Failed to open file %s\n", file);
		return 0;
	}

	while (fgets(dev, sizeof(dev), f)) {
		if (!strcasestr(dev, name))
			continue;

		target = strtok(dev, ":");
		if (target)
			break;
	}

	fclose(f);

	if (!target) {
		atenl_dbg("Failed to find target partition %s in %s\n",
			  name, file);
		return 0;
	}

	if (data) {
		strncpy(data, target, data_size - 1);
		data[data_size - 1] = '\0';
	}

	return 1;
}

static int
atenl_ubi_open(struct atenl *an, int flags)
{
	char *end, part[16] = {}, buf[128] = {};
	int i, fd, part_num, vol_id = -1;

	if (!atenl_partition_search("/proc/mtd", "ubi", part, sizeof(part)))
		return -1;

	part_num = (int)strtoul(part + 3, &end, 10);
	if (strncmp(part, "mtd", 3) != 0 || end == (part + 3)) {
		atenl_err("Invalid part %s\n", part);
		return -1;
	}

	snprintf(buf, sizeof(buf), "/sys/class/ubi/ubi%d", part_num);
	if (access(buf, F_OK)) {
		atenl_err("Failed to access %s: %s\n", buf, strerror(errno));
		return -1;
	}

	/* search for factory volume */
	for (i = 0; i < 128; i++) {
		snprintf(buf, sizeof(buf),
			"/sys/class/ubi/ubi%d_%d/name", part_num, i);

		if (atenl_partition_search(buf, an->flash_part, NULL, 0)) {
			vol_id = i;
			break;
		}
	}

	if (vol_id < 0) {
		atenl_err("Cannot find volume containing %s\n", an->flash_part);
		return -1;
	}

	snprintf(buf, sizeof(buf), "/dev/ubi%d_%d", part_num, vol_id);
	fd = open(buf, flags);

	return fd;
}

static int
atenl_mtd_open(struct atenl *an, int flags)
{
	char *end, part[16] = {}, buf[128] = {};
	int fd, part_num;

	if (!atenl_partition_search("/proc/mtd", an->flash_part, part, sizeof(part)))
		return -1;

	part_num = (int)strtoul(part + 3, &end, 10);
	if (strncmp(part, "mtd", 3) != 0 || end == (part + 3)) {
		atenl_err("Invalid part %s\n", part);
		return -1;
	}

	/* mtdblockX emulates an mtd device as a block device.
	 * Use mtdblockX instead of mtdX to avoid padding & buffer handling.
	 */
	snprintf(buf, sizeof(buf), "/dev/mtdblock%d", part_num);
	fd = open(buf, flags);

	return fd;
}

static int
atenl_mmc_open(struct atenl *an, int flags)
{
	const char *mmc_dev = "/dev/mmcblk0";
	int nparts, part_num;
	blkid_partlist plist;
	blkid_probe probe;
	int i, fd = -1;
	char buf[16];

	probe = blkid_new_probe_from_filename(mmc_dev);
	if (!probe)
		return -1;

	plist = blkid_probe_get_partitions(probe);
	if (!plist)
		goto out;

	nparts = blkid_partlist_numof_partitions(plist);
	if (!nparts)
		goto out;

	for (i = 0; i < nparts; i++) {
		blkid_partition part;
		const char *name;

		part = blkid_partlist_get_partition(plist, i);
		if (!part)
			continue;

		name = blkid_partition_get_name(part);
		if (!name)
			continue;

		if (strncasecmp(name, an->flash_part, strlen(an->flash_part)))
			continue;

		part_num = blkid_partition_get_partno(part);
		snprintf(buf, sizeof(buf), "%sp%d", mmc_dev, part_num);

		fd = open(buf, flags);
		if (fd >= 0)
			break;
	}

out:
	blkid_free_probe(probe);
	return fd;
}

static int
atenl_ubi_write_handler(int fd, off_t vol_size, u32 offs,
			u32 size, u8 **buf, u32 *rem)
{
#define UBI_VOL_IOC_MAGIC	'O'
#define UBI_IOCVOLUP		_IOW(UBI_VOL_IOC_MAGIC, 0, int64_t)
	int64_t update_size;
	u8 *tmp = NULL;
	int ret;

	/* update buffer and size for full volume write */
	if (size < vol_size) {
		tmp = malloc(vol_size);
		if (!tmp)
			return -1;

		ret = lseek(fd, 0, SEEK_SET);
		if (ret < 0)
			goto fail;

		ret = read(fd, tmp, vol_size);
		if (ret != (int)vol_size)
			goto fail;

		memcpy(tmp + offs, *buf, size);
		*buf = tmp;
		*rem = vol_size;
	}

	update_size = (int64_t)*rem;
	if (ioctl(fd, UBI_IOCVOLUP, &update_size) < 0) {
		ret = -1;
		goto fail;
	}

	return 0;

fail:
	free(tmp);
	return ret;
}

static int
atenl_flash_write(struct atenl *an, int fd, u8 *data, u32 offs, u32 size,
		  enum atenl_flash_type type)
{
#define UBI_VOL_IOC_MAGIC	'O'
#define UBI_IOCVOLUP		_IOW(UBI_VOL_IOC_MAGIC, 0, int64_t)
	static const char * const flash_type[] = {
		[FLASH_TYPE_MTD] = "MTD",
		[FLASH_TYPE_EMMC] = "eMMC",
		[FLASH_TYPE_UBI] = "UBI",
	};
	u32 i = 0, rem = size;
	int ret = -EINVAL;
	off_t flash_size;
	u8 *buf = data;

	flash_size = lseek(fd, 0, SEEK_END);
	if (flash_size < 0 || (off_t)size + (off_t)offs > flash_size)
		goto fail;

	/* ubi requires handling for partial write */
	if (type == FLASH_TYPE_UBI)
		ret = atenl_ubi_write_handler(fd, flash_size, offs, size,
					      &buf, &rem);
	else
		ret = lseek(fd, offs, SEEK_SET);
	if (ret < 0)
		goto fail;

	while (rem > 0) {
		ret = write(fd, buf + i, rem);
		if (ret < 0) {
			/* write interrupted */
			if (errno == EINTR)
				continue;
			break;
		}

		rem -= ret;
		i += ret;
	}

	if (buf != data)
		free(buf);

	if (ret < 0)
		goto fail;

	atenl_info("write to %s partition %s offset 0x%x size 0x%x\n",
		   flash_type[type], an->flash_part, offs, size);
	return 0;

fail:
	atenl_err("Failed to write %s: write size %d (flash size %ld) ret = %d\n",
		  flash_type[type], size, (long)flash_size, ret);
	return ret;
}

static int
atenl_flash_write_eeprom(struct atenl *an, int fd, enum atenl_flash_type type)
{
	int ret;

	/* write eeprom data */
	ret = atenl_flash_write(an, fd, an->eeprom_data, an->flash_offset,
				an->eeprom_size, type);
	if (ret)
		return ret;

	if (atenl_eeprom_sync_precal(an) < 0)
		return 0;

	/* write or clear precal data */
	return atenl_flash_write(an, fd, an->cal,
				 an->flash_offset + an->cal_data_offs,
				 an->cal_size, type);
}

static int
atenl_eeprom_write_flash(struct atenl *an)
{
	int fd, ret = -1;

	/* flash_offset = -1 for binfile mode */
	if (an->flash_part == NULL || !(~an->flash_offset)) {
		atenl_err("Flash partition or offset is not specified\n");
		return 0;
	}

	fd = atenl_ubi_open(an, O_RDWR | O_SYNC);
	if (fd >= 0) {
		ret = atenl_flash_write_eeprom(an, fd, FLASH_TYPE_UBI);
		goto out;
	}

	fd = atenl_mtd_open(an, O_RDWR | O_SYNC);
	if (fd >= 0) {
		ret = atenl_flash_write_eeprom(an, fd, FLASH_TYPE_MTD);
		goto out;
	}

	fd = atenl_mmc_open(an, O_RDWR | O_SYNC);
	if (fd >= 0) {
		ret = atenl_flash_write_eeprom(an, fd, FLASH_TYPE_EMMC);
		goto out;
	}

	atenl_err("Failed to open %s\n", an->flash_part);

out:
	close(fd);
	return ret;
}

/* Directly read values from driver's eeprom.
 * It's usally used to get calibrated data from driver.
 */
int atenl_eeprom_read_from_driver(struct atenl *an, u32 offset, int len)
{
	char fname[64], buf[1024];
	u8 *eeprom_data;
	int fd, ret;
	ssize_t rd;

	if (offset + len > an->eeprom_size) {
		atenl_err("Invalid offset %d or length %d\n", offset, len);
		return -1;
	}

	eeprom_data = an->eeprom_data + offset;
	snprintf(fname, sizeof(fname),
		 "/sys/kernel/debug/ieee80211/phy%d/mt76/eeprom",
		 an->main_phy_idx);
	fd = open(fname, O_RDONLY);
	if (fd < 0) {
		atenl_err("Failed to open file %s\n", fname);
		return -1;
	}

	ret = lseek(fd, offset, SEEK_SET);
	if (ret < 0) {
		atenl_err("Failed to seek offset %d\n", offset);
		goto out;
	}

	while ((rd = read(fd, buf, sizeof(buf))) > 0 && len) {
		if (len < rd) {
			memcpy(eeprom_data, buf, len);
			break;
		}

		memcpy(eeprom_data, buf, rd);
		eeprom_data += rd;
		len -= rd;
	}

	ret = 0;
out:
	close(fd);
	return ret;
}

/* Update all eeprom values to driver before writing efuse or ext eeprom */
static int
atenl_eeprom_sync_to_driver(struct atenl *an)
{
	int i;

	for (i = 0; i < an->eeprom_size; i += MT76_TM_EEPROM_BLOCK_SIZE)
		if (atenl_nl_write_eeprom(an, i, &an->eeprom_data[i]))
			return -1;

	return 0;
}

static void
atenl_eeprom_reset_precal_bit(struct atenl *an)
{
	u32 offs = an->eeprom_prek_offs;
	int i;

	switch (an->chip_id) {
	case MT7999_DEVICE_ID:
		for (i = 0; i < MAX_BAND_NUM; i++)
			an->eeprom_data[offs + 0x400 * i] = 0;
		break;
	default:
		an->eeprom_data[offs] = 0;
		break;
	}
}

static int
atenl_eeprom_handle_sync(struct atenl *an, const char *args)
{
	return atenl_eeprom_write_flash(an);
}

static int
atenl_eeprom_handle_flash_clear(struct atenl *an, const char *args)
{
	enum atenl_flash_type type = FLASH_TYPE_EMMC;
	off_t flash_size;
	int fd, ret = -1;
	u8 *buf = NULL;
	u32 size;

	/* flash_offset = -1 for binfile mode */
	if (an->flash_part == NULL || !(~an->flash_offset)) {
		atenl_err("Flash partition or offset is not specified\n");
		return -1;
	}

	fd = atenl_ubi_open(an, O_RDWR | O_SYNC);
	if (fd >= 0) {
		type = FLASH_TYPE_UBI;
		goto clear;
	}

	fd = atenl_mtd_open(an, O_RDWR | O_SYNC);
	if (fd >= 0) {
		type = FLASH_TYPE_MTD;
		goto clear;
	}

	fd = atenl_mmc_open(an, O_RDWR | O_SYNC);
	if (fd < 0) {
		atenl_err("Failed to open %s\n", an->flash_part);
		goto out;
	}

clear:
	atenl_eeprom_sync_precal(an);
	size = an->eeprom_size + an->cal_size;
	flash_size = lseek(fd, 0, SEEK_END);
	if (flash_size < 0) {
		atenl_err("Failed to seek to the end of the file\n");
		goto out;
	}

	if ((off_t)size > flash_size)
		size = flash_size;

	buf = (u8 *)calloc(size, sizeof(char));
	if (!buf) {
		atenl_err("Failed to allocate memory\n");
		goto out;
	}

	if (atenl_flash_write(an, fd, buf, an->flash_offset, size, type)) {
		atenl_err("Failed to clear flash\n");
		goto out;
	}

	atenl_info("clear flash size 0x%x\n", size);
	ret = 0;

out:
	if (fd >= 0)
		close(fd);
	free(buf);
	return ret;
}

static int
atenl_eeprom_handle_precal_clear(struct atenl *an, const char *args)
{
	an->clear_cal = true;
	atenl_eeprom_reset_precal_bit(an);

	return atenl_eeprom_write_flash(an);
}

static int
atenl_eeprom_handle_cap(struct atenl *an, const char *args)
{
	static const char * const cap_to_radio[] = {
		[BAND_TYPE_UNUSE] = "invalid",
		[BAND_TYPE_2G] = "2g",
		[BAND_TYPE_5G] = "5g",
		[BAND_TYPE_2G_5G] = "2/5g",
		[BAND_TYPE_6G] = "6g",
		[BAND_TYPE_2G_6G] = "2/6g",
		[BAND_TYPE_5G_6G] = "5/6g",
		[BAND_TYPE_2G_5G_6G] = "2/5/6g",
	};
	int i;

	for (i = 0; i < MAX_BAND_NUM; i++) {
		struct atenl_band *anb;

		if (!get_band_val(an, i, valid))
			continue;

		anb = &an->anb[i];
		atenl_info("band %d: %s radio, tx antenna %x, rx antenna %x\n",
			   i, cap_to_radio[anb->cap], anb->chainmask, anb->rx_chainmask);
	}

	return 0;
}

static int
atenl_eeprom_handle_reset(struct atenl *an, const char *args)
{
	return unlink(eeprom_file);
}

static int
atenl_eeprom_handle_file(struct atenl *an, const char *args)
{
	atenl_info("%s\n", eeprom_file);
	atenl_info("%s mode\n", atenl_eeprom_mode_str(an->eeprom_mode));

	return 0;
}

static int
atenl_eeprom_handle_set(struct atenl *an, const char *args)
{
	char *s, *token, *end;
	u32 offset, val = 0;
	int count;

	if (!args)
		return -1;

	offset = (u32)strtoul(args, &end, 16);
	if (end == args || *end != '=' || offset >= an->eeprom_size)
		return -1;

	s = end + 1;
	token = strtok(s, ",");
	for (count = 0; token; offset++, count++) {
		if (offset >= an->eeprom_size) {
			offset--;
			count--;
			break;
		}

		val = (u32)strtoul(token, NULL, 16);
		if (offset != 0 && offset != 1)
			an->eeprom_data[offset] = val;
		token = strtok(NULL, ",");
		if (!token)
			break;
	}
	if (count)
		atenl_info("set offsets from 0x%x to 0x%x\n",
				offset - count, offset);
	else
		atenl_info("set offset 0x%x to 0x%x\n", offset, val);

	return 0;
}

static int
atenl_eeprom_handle_read(struct atenl *an, const char *args)
{
	u32 offset;
	char *end;

	if (!args)
		return -1;

	offset = (u32)strtoul(args, &end, 16);
	if (end == args || offset >= an->eeprom_size)
		return -1;

	atenl_info("val = 0x%x (%u)\n",
		   an->eeprom_data[offset], an->eeprom_data[offset]);
	return 0;
}

static int
atenl_eeprom_handle_update_buffermode(struct atenl *an, const char *args)
{
	int ret;

	ret = atenl_eeprom_sync_to_driver(an);
	if (ret)
		return ret;

	return atenl_nl_update_buffer_mode(an);
}

static int
atenl_eeprom_handle_write_efuse(struct atenl *an, const char *args)
{
	int ret;

	ret = atenl_eeprom_sync_to_driver(an);
	if (ret)
		return ret;

	return atenl_nl_write_efuse_all(an);
}

static int
atenl_eeprom_handle_write_ext(struct atenl *an, const char *args)
{
	int ret;

	ret = atenl_eeprom_sync_to_driver(an);
	if (ret)
		return ret;

	return atenl_nl_write_ext_eeprom_all(an);
}

static int
atenl_eeprom_handle_ibf_sync(struct atenl *an, const char *args)
{
	u16 offset, len = 40 * 9;

	if (an->adie_id == 0x7975)
		offset = 0x651;
	else
		offset = 0x60a;

	if (is_connac3(an)) {
		offset = 0xc00;
		/* Group 0: 29, Group 1 ~ 12: 34 for ibf 2.0 */
		if (!is_mt7996(an))
			len = 29 + 34 * 12;
		else
			len = 46 * 9;
	}

	return atenl_eeprom_read_from_driver(an, offset, len);
}

static int
atenl_eeprom_handle_rx_gain_sync(struct atenl *an, const char *args)
{
	int band, ret;

	if (is_connac2(an))
		return 0;

	if (is_connac3(an)) {
		ret = atenl_eeprom_read_from_driver(an, MT_EE_DO_RX_GAIN_CAL, 1);
		if (ret)
			return ret;

		return atenl_eeprom_read_from_driver(an, MT_EE_RX_GAIN_CAL,
						     MT_EE_CAL_RX_GAIN_SIZE);
	}

	for (band = 0; band < MAX_BAND_NUM; band++) {
		ret = atenl_eeprom_read_from_driver(an, MT_EE_CONNAC5_DO_RX_GAIN_CAL(band), 1);
		if (ret)
			return ret;
	}

	return atenl_eeprom_read_from_driver(an, MT_EE_CONNAC5_RX_GAIN_CAL,
					     MT_EE_CONNAC5_CAL_RX_GAIN_SIZE);
}

static const struct atenl_eeprom_cmd cmd_table[] = {
	{ "sync eeprom all",		atenl_eeprom_handle_sync },
	{ "clear eeprom all",		atenl_eeprom_handle_flash_clear },
	{ "clear precal",		atenl_eeprom_handle_precal_clear },
	{ "get cap",			atenl_eeprom_handle_cap },
	{ "eeprom reset",		atenl_eeprom_handle_reset },
	{ "eeprom file",		atenl_eeprom_handle_file },
	{ "eeprom set",			atenl_eeprom_handle_set },
	{ "eeprom read",		atenl_eeprom_handle_read },
	{ "eeprom update buffermode",	atenl_eeprom_handle_update_buffermode },
	{ "eeprom write flash",		atenl_eeprom_handle_sync },
	{ "eeprom write to efuse",	atenl_eeprom_handle_write_efuse },
	{ "eeprom write to ext",	atenl_eeprom_handle_write_ext },
	{ "eeprom ibf sync",		atenl_eeprom_handle_ibf_sync },
	{ "eeprom rx gain sync",	atenl_eeprom_handle_rx_gain_sync },
};

int atenl_eeprom_cmd_handler(struct atenl *an, u8 phy_idx, char *cmd)
{
	int i;

	if (atenl_eeprom_init(an, phy_idx))
		return -1;

	for (i = 0; i < ARRAY_SIZE(cmd_table); i++) {
		size_t len = strlen(cmd_table[i].name);

		if (!strncmp(cmd, cmd_table[i].name, len)) {
			const char *args;

			if (cmd[len] != '\0' && cmd[len] != ' ')
				goto out;

			args = cmd[len] == ' ' ? cmd + len + 1 : NULL;
			return cmd_table[i].handler(an, args);
		}
	}

out:
	atenl_err("Unknown command: %s!\n", cmd);
	return -1;
}
