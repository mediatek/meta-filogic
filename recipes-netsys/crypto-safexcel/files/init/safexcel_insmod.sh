#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 MediaTek Inc.
#

# insmod eip-197 driver
echo insmod EIP-197 lookaside driver

#modprobe crypto-eip-inline
modprobe crypto_safexcel
