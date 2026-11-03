// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (C) 2021-2022 Mediatek Inc. */

#include "atenl.h"

static int
atenl_reg_set_offset(struct atenl *an, u32 offset)
{
	char dir[64], buf[16] = {};
	int fd, ret;

	/* write offset into regidx */
	ret = snprintf(dir, sizeof(dir),
		       "/sys/kernel/debug/ieee80211/phy%d/mt76/regidx",
		       an->main_phy_idx);
	if (snprintf_error(sizeof(dir), ret))
		return ret;

	fd = open(dir, O_WRONLY);
	if (fd < 0)
		return fd;

	ret = snprintf(buf, sizeof(buf), "0x%x", offset);
	if (snprintf_error(sizeof(buf), ret))
		goto out;

	if (lseek(fd, 0, SEEK_SET) < 0)
		goto out;

	if (write(fd, buf, ret) < 0)
		goto out;

out:
	close(fd);
	return ret;
}

static int
atenl_reg_file_open(struct atenl *an, bool rf)
{
	char dir[64];
	int fd, ret;

	/* read value from regval or rf_regval */
	ret = snprintf(dir, sizeof(dir),
		       "/sys/kernel/debug/ieee80211/phy%d/mt76/%s",
		       an->main_phy_idx, rf ? "rf_regval" : "regval");
	if (snprintf_error(sizeof(dir), ret))
		return ret;

	fd = open(dir, O_RDONLY);

	return fd;
}

static int
atenl_reg_file_read(struct atenl *an, int fd, u32 *res)
{
	char buf[16] = {};
	unsigned long val;
	int ret;

	ret = read(fd, buf, sizeof(buf) - 1);
	if (ret < 0)
		goto out;
	buf[ret] = 0;

	val = strtoul(buf, NULL, 16);
	if (val > (u32) -1)
		return -EINVAL;

	*res = val;
	ret = 0;
out:
	close(fd);

	return ret;
}

static int
atenl_reg_file_write(struct atenl *an, int fd, u32 val)
{
	char buf[16] = {};
	int ret;

	ret = snprintf(buf, sizeof(buf), "0x%x", val);
	if (snprintf_error(sizeof(buf), ret))
		goto out;
	buf[ret] = 0;

	if (lseek(fd, 0, SEEK_SET) < 0)
		goto out;

	if (write(fd, buf, ret) < 0)
		goto out;

	ret = 0;
out:
	close(fd);

	return ret;
}

int atenl_reg_read(struct atenl *an, u32 offset, u32 *res)
{
	int fd, ret;

	ret = atenl_reg_set_offset(an, offset);
	if (ret)
		return ret;

	fd = atenl_reg_file_open(an, false);
	if (fd < 0)
		return fd;

	return atenl_reg_file_read(an, fd, res);
}

int atenl_reg_write(struct atenl *an, u32 offset, u32 val)
{
	int fd, ret;

	ret = atenl_reg_set_offset(an, offset);
	if (ret)
		return ret;

	fd = atenl_reg_file_open(an, false);
	if (fd < 0)
		return fd;

	return atenl_reg_file_write(an, fd, val);
}

int atenl_rf_read(struct atenl *an, u32 wf_sel, u32 offset, u32 *res)
{
	int fd, ret;
	u32 regidx;

	/* merge wf_sel and offset into regidx */
	regidx = FIELD_PREP(GENMASK(31, 28), wf_sel) |
		 FIELD_PREP(GENMASK(27, 0), offset);
	ret = atenl_reg_set_offset(an, regidx);
	if (ret)
		return ret;

	fd = atenl_reg_file_open(an, true);
	if (fd < 0)
		return fd;

	return atenl_reg_file_read(an, fd, res);
}

int atenl_rf_write(struct atenl *an, u32 wf_sel, u32 offset, u32 val)
{
	int fd, ret;
	u32 regidx;

	/* merge wf_sel and offset into regidx */
	regidx = FIELD_PREP(GENMASK(31, 28), wf_sel) |
		 FIELD_PREP(GENMASK(27, 0), offset);
	ret = atenl_reg_set_offset(an, regidx);
	if (ret)
		return ret;

	fd = atenl_reg_file_open(an, true);
	if (fd < 0)
		return fd;

	return atenl_reg_file_write(an, fd, val);
}
