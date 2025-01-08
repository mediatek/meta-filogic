#
# Copyright (C) 2024 MediaTek Inc. All rights reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /COPYING for more information.
#

include libfdt/Makefile.libfdt

OBJS := main.o crc32.o sha1.o $(LIBFDT_SRCS:%.c=libfdt/%.o)
DEPS := $(OBJS:%.o=%.d)

CC ?= gcc
CFLAGS ?= -O2 -ffunction-sections
LDFLAGS ?= -Wl,--gc-sections
OPTFLAGS ?= -ggdb

all: fit-rootfs-hash-tool

fit-rootfs-hash-tool: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(OPTFLAGS) -Ilibfdt -MD -c -o $@ $<

clean:
	rm -f fit-rootfs-hash-tool $(OBJS) $(DEPS)

.PHONY: clean

-include $(DEPS)
