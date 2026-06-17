# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2019 MediaTek Inc.
#
# Author: Sam Shih <sam.shih@mediatek.com>
#

TARGET := ar-tool

.PHONY: all clean

all: ${TARGET}

%: %.py Makefile
	cp $< $@

clean:
	rm ${TARGET}
