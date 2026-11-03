/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (C) 2021-2022 Mediatek Inc. */
#ifndef __ATENL_DEBUG_H
#define __ATENL_DEBUG_H

#include <stdbool.h>

extern bool atenl_debug;

#define atenl_info(fmt, ...)	((void)fprintf(stdout, fmt, ##__VA_ARGS__))
#define atenl_err(fmt, ...)	((void)fprintf(stderr, "%s: " fmt, __func__, ##__VA_ARGS__))
#define atenl_dbg(fmt, ...)							\
	do {									\
		if (atenl_debug)						\
			atenl_info("%s: " fmt, __func__, ##__VA_ARGS__);	\
	} while (0)

#endif
