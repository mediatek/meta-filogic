// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (C) 2021-2022 Mediatek Inc. */
#include "atenl.h"

static struct nla_policy testdata_policy[NUM_MT76_TM_ATTRS] = {
	[MT76_TM_ATTR_STATE] = { .type = NLA_U8 },
	[MT76_TM_ATTR_MTD_PART] = { .type = NLA_STRING },
	[MT76_TM_ATTR_MTD_OFFSET] = { .type = NLA_U32 },
	[MT76_TM_ATTR_BAND_IDX] = { .type = NLA_U8 },
};

int atenl_nl_init(struct atenl *an)
{
	int ret;

	ret = unl_genl_init(&an->unl, "nl80211");
	if (ret < 0)
		atenl_err("Failed to connect to nl80211: %s\n", strerror(-ret));

	return ret;
}

static int atenl_nl_check_flash_cb(struct nl_msg *msg, void *arg)
{
	struct atenl *an = (struct atenl *)arg;
	struct nlattr *tb[NUM_MT76_TM_ATTRS];
	struct nlattr *attr;
	int ret;

	attr = unl_find_attr(&an->unl, msg, NL80211_ATTR_TESTDATA);
	if (!attr)
		return NL_SKIP;

	ret = nla_parse_nested(tb, MT76_TM_ATTR_MAX, attr, testdata_policy);
	if (ret) {
		atenl_err("Failed to parse netlink attributes: %s\n", strerror(-ret));
		return NL_SKIP;
	}

	if (!tb[MT76_TM_ATTR_BAND_IDX]) {
		atenl_err("Failed to get band idx\n");
		return NL_SKIP;
	}

	an->band_idx = nla_get_u32(tb[MT76_TM_ATTR_BAND_IDX]);

	if (!tb[MT76_TM_ATTR_MTD_PART] || !tb[MT76_TM_ATTR_MTD_OFFSET])
		return NL_SKIP;

	an->flash_part = strdup(nla_get_string(tb[MT76_TM_ATTR_MTD_PART]));
	an->flash_offset = nla_get_u32(tb[MT76_TM_ATTR_MTD_OFFSET]);

	return NL_SKIP;
}

int atenl_nl_check_flash(struct atenl *an)
{
	struct nl_msg *msg;
	int ret;

	/* User has a specified flash partition */
	if (an->flash_part)
		return 0;

	msg = unl_genl_msg(&an->unl, NL80211_CMD_TESTMODE, true);
	if (!msg) {
		atenl_err("Failed to allocate netlink message\n");
		return -1;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, get_band_val(an, 0, phy_idx)))
		goto free;

	ret = unl_genl_request(&an->unl, msg, atenl_nl_check_flash_cb, an);
	if (ret) {
		atenl_err("Failed to request netlink message: %s\n", strerror(-ret));
		return -1;
	}

	return 0;

free:
	atenl_err("Failed to build netlink message\n");
	nlmsg_free(msg);

	return -1;
}

int atenl_nl_write_eeprom(struct atenl *an, u32 offset, u8 *val)
{
	struct nl_msg *msg;
	void *ptr, *a;
	int i, ret;

	msg = unl_genl_msg(&an->unl, NL80211_CMD_TESTMODE, false);
	if (!msg) {
		atenl_err("Failed to allocate netlink message\n");
		return -1;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, get_band_val(an, 0, phy_idx)))
		goto free;

	ptr = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!ptr)
		goto free;

	if (nla_put_u8(msg, MT76_TM_ATTR_EEPROM_ACTION,
		       MT76_TM_EEPROM_ACTION_UPDATE_DATA) ||
	    nla_put_u32(msg, MT76_TM_ATTR_EEPROM_OFFSET, offset))
		goto free;

	a = nla_nest_start(msg, MT76_TM_ATTR_EEPROM_VAL);
	if (!a)
		goto free;

	for (i = 0; i < MT76_TM_EEPROM_BLOCK_SIZE; i++) {
		if (nla_put_u8(msg, i, val[i]))
			goto free;
	}

	nla_nest_end(msg, a);

	nla_nest_end(msg, ptr);

	ret = unl_genl_request(&an->unl, msg, NULL, NULL);
	if (ret) {
		atenl_err("Failed to request netlink message: %s\n", strerror(-ret));
		return -1;
	}

	return 0;

free:
	atenl_err("Failed to build netlink message\n");
	nlmsg_free(msg);
	return -1;
}

static int atenl_nl_set_eeprom_action(struct atenl *an, u8 action)
{
	struct nl_msg *msg;
	void *ptr;
	int ret;

	msg = unl_genl_msg(&an->unl, NL80211_CMD_TESTMODE, false);
	if (!msg) {
		atenl_err("Failed to allocate netlink message\n");
		return -1;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, get_band_val(an, 0, phy_idx)))
		goto free;

	ptr = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!ptr)
		goto free;

	if (nla_put_u8(msg, MT76_TM_ATTR_EEPROM_ACTION, action))
		goto free;

	nla_nest_end(msg, ptr);

	ret = unl_genl_request(&an->unl, msg, NULL, NULL);
	if (ret) {
		atenl_err("Failed to request netlink message: %s\n", strerror(-ret));
		return -1;
	}

	return 0;

free:
	atenl_err("Failed to build netlink message\n");
	nlmsg_free(msg);
	return -1;
}

int atenl_nl_write_efuse_all(struct atenl *an)
{
	return atenl_nl_set_eeprom_action(an, MT76_TM_EEPROM_ACTION_WRITE_TO_EFUSE);
}

int atenl_nl_write_ext_eeprom_all(struct atenl *an)
{
	return atenl_nl_set_eeprom_action(an, MT76_TM_EEPROM_ACTION_WRITE_TO_EXT_EEPROM);
}

int atenl_nl_update_buffer_mode(struct atenl *an)
{
	return atenl_nl_set_eeprom_action(an, MT76_TM_EEPROM_ACTION_UPDATE_BUFFER_MODE);
}

static int atenl_nl_get_wiphy_cb(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
	struct atenl *an = (struct atenl *)arg;
	int ret;

	ret = nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);
	if (ret) {
		atenl_err("Failed to parse netlink attributes: %s\n", strerror(-ret));
		return NL_SKIP;
	}

	if (!tb_msg[NL80211_ATTR_WIPHY]) {
		atenl_err("Failed to find wiphy attributes\n");
		return NL_SKIP;
	}

	if (tb_msg[NL80211_ATTR_WIPHY_RADIOS])
		an->is_single_wiphy = true;

	return NL_SKIP;
}

int atenl_nl_get_wiphy(struct atenl *an)
{
	struct nl_msg *msg;
	int ret;

	msg = unl_genl_msg(&an->unl, NL80211_CMD_GET_WIPHY, true);
	if (!msg) {
		atenl_err("Failed to allocate netlink message\n");
		return -1;
	}

	if (nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP))
		goto free;

	ret = unl_genl_request(&an->unl, msg, atenl_nl_get_wiphy_cb, an);
	if (ret) {
		atenl_err("Failed to request netlink message: %s\n", strerror(-ret));
		return -1;
	}

	return 0;

free:
	atenl_err("Failed to build netlink message\n");
	nlmsg_free(msg);
	return -1;
}
