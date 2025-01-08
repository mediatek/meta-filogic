// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 MediaTek Inc. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 *
 * Tool for adding rootfs hash node to FIT image
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <errno.h>
#include <endian.h>
#include <libfdt.h>
#include "crc32.h"
#include "sha1.h"

#define FIT_FDT_EXTRA_SIZE			0x1000

/* Only supports native byte-order for squashfs */
#define SQUASHFS_MAGIC		0x73717368

struct squashfs_super_block {
	uint32_t s_magic;
	uint32_t padding0[9];
	uint64_t bytes_used;
};

static int read_file(const char *file, void **buffer, uint32_t *filelen, uint32_t extra_len)
{
	size_t rdlen;
	uint8_t *ptr;
	int ret = 0;
	FILE *f;
	long len;

	f = fopen(file, "rb");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s', error %d\n", file, errno);
		return -errno;
	}

	ret = fseek(f, 0, SEEK_END);
	if (ret < 0) {
		ret = ferror(f);
		fprintf(stderr, "fseek() failed, error %d\n", ret);
		goto cleanup;
	}

	len = ftell(f);
	if (len < 0) {
		ret = ferror(f);
		fprintf(stderr, "ftell() failed, error %d\n", ret);
		goto cleanup;
	}

	ret = fseek(f, 0, SEEK_SET);
	if (ret < 0) {
		ret = ferror(f);
		fprintf(stderr, "fseek() failed, error %d\n", ret);
		goto cleanup;
	}

	ptr = malloc(len + extra_len);
	if (!ptr) {
		ret = ferror(f);
		fprintf(stderr, "Failed to allocate memory\n");
		goto cleanup;
	}

	rdlen = fread(ptr, 1, len, f);
	if (rdlen != len) {
		ret = ferror(f);
		fprintf(stderr, "Failed to read file, error %d\n", ret);
		free(ptr);
		*buffer = NULL;
		goto cleanup;
	}

	*buffer = (void *)ptr;

	if (extra_len)
		memset(ptr + len, 0, extra_len);

	if (filelen)
		*filelen = len;

cleanup:
	fclose(f);
	return ret;
}

static int write_file(const char *file, const void *buffer, uint32_t len)
{
	size_t wrlen;
	FILE *f;
	int ret;

	f = fopen(file, "wb");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s', error %d\n", file, errno);
		return -errno;
	}

	wrlen = fwrite(buffer, 1, len, f);
	ret = ferror(f);
	fclose(f);

	if (wrlen != len) {
		fprintf(stderr, "Failed to write file, error %d\n", ret);
		return ret;
	}

	return 0;
}

static uint32_t squashfs_actual_size(const void *data, uint32_t maxlen)
{
	struct squashfs_super_block sb;
	uint64_t size;

	memcpy(&sb, data, sizeof(sb));

	if (le32toh(sb.s_magic) != SQUASHFS_MAGIC) {
		fprintf(stderr, "Invalid rootfs data\n");
		return 0;
	}

	size = le64toh(sb.bytes_used);
	if (size > maxlen) {
		fprintf(stderr, "rootfs is incomplete\n");
		return 0;
	}

	return (uint32_t)size;
}

int main(int argc, char *argv[])
{
	uint32_t i, fit_len, rootfs_len, rootfs_crc;
	uint8_t rootfs_sha1[SHA1_SUM_LEN];
	int ret, nodeoffset, subnodeoffs;
	void *fit, *rootfs;
	const char *str;
	fdt32_t val;

	if (argc < 3) {
		printf("Usage: <fit-image> <rootfs-file>\n");
		return 0;
	}

	ret = read_file(argv[2], &rootfs, &rootfs_len, 0);
	if (ret)
		return 1;

	rootfs_len = squashfs_actual_size(rootfs, rootfs_len);
	if (!rootfs_len)
		return 2;

	rootfs_crc = crc32(0, rootfs, rootfs_len);
	sha1(rootfs, rootfs_len, rootfs_sha1);

	free(rootfs);

	ret = read_file(argv[1], &fit, &fit_len, FIT_FDT_EXTRA_SIZE);
	if (ret)
		return 3;

	/* Extend fdt size */
	ret = fdt_open_into(fit, fit, fdt_totalsize(fit) + FIT_FDT_EXTRA_SIZE);
	if (ret) {
		fprintf(stderr, "Failed to extend fit size\n");
		return 4;
	}

	/* Find and remove rootfs node */
	nodeoffset = fdt_path_offset(fit, "/rootfs");
	if (nodeoffset >= 0)
		fdt_del_node(fit, nodeoffset);

	/* find "/" node. */
	nodeoffset = fdt_path_offset(fit, "/");
	if (nodeoffset < 0) {
		fprintf(stderr, "Root node not found\n");
		return 5;
	}

	/* Add new rootfs node */
	nodeoffset = fdt_add_subnode(fit, nodeoffset, "rootfs");
	if (nodeoffset < 0) {
		fprintf(stderr, "Failed to add `rootfs' node\n");
		return 6;
	}

	/* Add size prop */
	val = cpu_to_fdt32(rootfs_len);
	ret = fdt_setprop(fit, nodeoffset, "size", &val, sizeof(val));
	if (ret < 0) {
		fprintf(stderr, "Failed to set rootfs `size' prop\n");
		return 7;
	}

	/* Add crc32 hash node */
	subnodeoffs = fdt_add_subnode(fit, nodeoffset, "hash-1");
	if (nodeoffset < 0) {
		fprintf(stderr, "Failed to add crc32 `hash-1' node\n");
		return 8;
	}

	/* Add value prop */
	val = cpu_to_fdt32(rootfs_crc);
	ret = fdt_setprop(fit, subnodeoffs, "value", &val, sizeof(val));
	if (ret < 0) {
		fprintf(stderr, "Failed to set crc32 `value' prop\n");
		return 9;
	}

	/* Add algo prop */
	str = "crc32";
	ret = fdt_setprop_string(fit, subnodeoffs, "algo", str);
	if (ret < 0) {
		fprintf(stderr, "Failed to set crc32 `algo' prop\n");
		return 10;
	}

	/* Add sha1 hash node */
	subnodeoffs = fdt_add_subnode(fit, nodeoffset, "hash-2");
	if (nodeoffset < 0) {
		fprintf(stderr, "Failed to add sha1 `hash-2' node\n");
		return 11;
	}

	/* Add value prop */
	ret = fdt_setprop(fit, subnodeoffs, "value", rootfs_sha1, sizeof(rootfs_sha1));
	if (ret < 0) {
		fprintf(stderr, "Failed to set sha1 `value' prop\n");
		return 12;
	}

	/* Add algo prop */
	str = "sha1";
	ret = fdt_setprop_string(fit, subnodeoffs, "algo", str);
	if (ret < 0) {
		fprintf(stderr, "Failed to set sha1 `algo' prop\n");
		return 13;
	}

	/* Shrink FIT image */
	fdt_pack(fit);

	/* Save FIT image */
	ret = write_file(argv[1], fit, fdt_totalsize(fit));
	if (ret)
		return 14;

	printf("Added `rootfs' node:\n");
	printf("    size  = 0x%x\n", rootfs_len);
	printf("    crc32 = 0x%08x\n", rootfs_crc);
	printf("    sha1  = ");

	for (i = 0; i < sizeof(rootfs_sha1); i++) {
		if (i % sizeof(fdt32_t) == 0)
			printf("0x");

		printf("%02x", rootfs_sha1[i]);

		if (i % sizeof(fdt32_t) == 3)
			printf(" ");
	}

	printf("\n");

	return 0;
}
