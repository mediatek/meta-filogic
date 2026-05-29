#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 MediaTek Inc.
#

module=hnat
if [ -d /sys/kernel/debug/${module} ] 
then
echo 0 > /proc/sys/net/bridge/bridge-nf-call-iptables
echo 0 > /proc/sys/net/bridge/bridge-nf-call-ip6tables
else
module=mtk_ppe
fi

echo 2 > /sys/kernel/debug/${module}/qos_toggle