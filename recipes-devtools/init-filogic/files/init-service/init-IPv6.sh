#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 MediaTek Inc.
#

sysevent set multinet-up 1
/lib/rdk/dibbler-init.sh
sleep 2