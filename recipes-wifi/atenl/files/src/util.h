/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (C) 2021-2022 Mediatek Inc. */
#ifndef __ATENL_UTIL_H
#define __ATENL_UTIL_H

#include <linux/const.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64, ktime_t;

#ifndef __WORDSIZE
#define __WORDSIZE (__SIZEOF_LONG__ * 8)
#endif

#ifndef BITS_PER_LONG
#define BITS_PER_LONG __WORDSIZE
#endif

#define UL(x)		(_UL(x))

#define GENMASK_INPUT_CHECK(h, l) 0
#define __GENMASK(h, l) \
	(((~UL(0)) - (UL(1) << (l)) + 1) & \
	 (~UL(0) >> (BITS_PER_LONG - 1 - (h))))
#define GENMASK(h, l) \
	(GENMASK_INPUT_CHECK(h, l) + __GENMASK(h, l))

#define __bf_shf(x) (__builtin_ffsll(x) - 1)
#define FIELD_GET(_mask, _reg)						\
	({								\
		(typeof(_mask))(((_reg) & (_mask)) >> __bf_shf(_mask));	\
	})
#define FIELD_PREP(_mask, _val)						\
	({								\
		((typeof(_mask))(_val) << __bf_shf(_mask)) & (_mask);	\
	})

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

static inline int snprintf_error(size_t size, int res)
{
	return res < 0 || (unsigned int) res >= size;
}

#endif
