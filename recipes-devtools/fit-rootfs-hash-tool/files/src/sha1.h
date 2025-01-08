/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 MediaTek Inc. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 *
 * SHA1 implementation
 */

#ifndef _SHA1_H_
#define _SHA1_H_

#include <stdint.h>

#define SHA1_SUM_LEN		20

/**
 * \brief	   SHA-1 context structure
 */
typedef struct
{
    unsigned long total[2];	/*!< number of bytes processed	*/
    uint32_t state[5];		/*!< intermediate digest state	*/
    unsigned char buffer[64];	/*!< data block being processed */
} sha1_context;

/**
 * \brief	   SHA-1 context setup
 *
 * \param ctx	   SHA-1 context to be initialized
 */
void sha1_starts(sha1_context *ctx);

/**
 * \brief	   SHA-1 process buffer
 *
 * \param ctx	   SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen	   length of the input data
 */
void sha1_update(sha1_context *ctx, const unsigned char *input,
		 unsigned int ilen);

/**
 * \brief	   SHA-1 final digest
 *
 * \param ctx	   SHA-1 context
 * \param output   SHA-1 checksum result
 */
void sha1_finish( sha1_context *ctx, unsigned char output[20] );

/**
 * \brief	   Calculate SHA-1 once
 *
 * \param data	   data buffer
 * \param len	   length of the input data
 * \param output   SHA-1 checksum result
 */
void sha1(const void *data, size_t len, void *output);

#endif /* _SHA1_H_ */
