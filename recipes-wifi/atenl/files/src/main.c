// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (C) 2021-2022 Mediatek Inc. */

#include "atenl.h"

bool atenl_debug;

static int phy_lookup_idx(struct atenl *an, const char *phyname)
{
	char buf[128];
	FILE *f;
	size_t len;
	int ret;

	atenl_nl_get_wiphy(an);
	/* TODO: Handle single wiphy model */
	if (an->is_single_wiphy)
		return 0;

	ret = snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", phyname);
	if (snprintf_error(sizeof(buf), ret))
		return -1;

	f = fopen(buf, "r");
	if (!f)
		return -1;

	len = fread(buf, 1, sizeof(buf) - 1, f);
	fclose(f);

	if (!len)
		return -1;

	buf[len] = 0;
	return atoi(buf);
}

static void usage(char *progname)
{
	printf("Usage:\n");
	printf("  %s -i phyX -c <command>\n", progname);
	printf("options:\n"
	       "  -h = show help text\n"
	       "  -i = phy name of driver interface, please use first phy for dbdc\n"
	       "  -c = eeprom-related command\n"
	       "  -p = specify the flash partition name and offset (<name>:<offs>)\n"
	       "  -d = show debug log\n");
	printf("examples:\n"
	       "  %s -i phy0 -c \"eeprom read 0\"\n", progname);
}

int main(int argc, char **argv)
{
	char *progname, *phy = "phy0", *cmd = NULL;
	int opt, phy_idx, ret = -1;
	struct atenl *an;
	char *token;

	progname = argv[0];

	an = calloc(1, sizeof(struct atenl));
	if (!an) {
		atenl_err("Failed to allocate memory for atenl\n");
		return -1;
	}

	while(1) {
		opt = getopt(argc, argv, "hdi:c:p:");
		if (opt == -1)
			break;

		switch (opt) {
		case 'h':
			usage(progname);
			ret = 0;
			goto out;
		case 'd':
			atenl_debug = true;
			break;
		case 'i':
			phy = optarg;
			break;
		case 'c':
			cmd = optarg;
			break;
		case 'p':
			token = strtok(optarg, ":");
			if (!token)
				goto out;
			an->flash_part = token;

			token = strtok(NULL, ":");
			if (!token)
				goto out;
			an->flash_offset = strtol(token, NULL, 0);
			break;
		default:
			atenl_err("Not supported option: %c\n", opt);
			goto out;
		}
	}

	ret = atenl_nl_init(an);
	if (ret)
		goto out;

	phy_idx = phy_lookup_idx(an, phy);
	if (phy_idx < 0 || phy_idx > UCHAR_MAX) {
		atenl_err("Could not find phy '%s'\n", phy);
		goto out;
	}

	if (!cmd) {
		atenl_err("No command specified\n");
		usage(progname);
		goto out;
	}

	ret = atenl_eeprom_cmd_handler(an, phy_idx, cmd);
out:
	atenl_eeprom_close(an);
	unl_free(&an->unl);
	free(an->cal);
	free(an);

	return ret;
}
