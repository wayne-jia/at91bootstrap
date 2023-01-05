/*
 * Copyright (C) 2013 Microchip Technology Inc. and its subsidiaries
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __XDMAC_H__
#define __XDMAC_H__

#include "arch/at91_xdmac.h"

#define XDMAC_CHANNEL(i)	(i)

#define XDMAC_TRANSFER_COMPLETE	1
#define XDMAC_TRANSFER_ERROR	2

#define XDMAC_TRANSFER_NAND_8BIT	XDMAC_CC_TYPE_MEM_TRAN | \
					XDMAC_CC_DAM_INCREMENTED_AM | \
					XDMAC_CC_SAM_FIXED_AM | \
					XDMAC_CC_SIF(0)| \
					XDMAC_CC_DIF(1)| \
					XDMAC_CC_DWIDTH_BYTE | \
					XDMAC_CC_MBSIZE_SIXTEEN

#define XDMAC_TRANSFER_MEM(width)	XDMAC_CC_TYPE_MEM_TRAN | \
					XDMAC_CC_DAM_INCREMENTED_AM | \
					XDMAC_CC_SAM_INCREMENTED_AM | \
					XDMAC_CC_SIF(0)| \
					XDMAC_CC_DIF(1)| \
					XDMAC_CC_DWIDTH(width) | \
					XDMAC_CC_MBSIZE_SIXTEEN

int xdmac_chan_init(int chan, unsigned int config);
int xdmac_chan_config(int chan, unsigned int config);
int xdmac_chan_transfer(int chan, const void *src, const void *dest, int size);
int xdmac_chan_status(int chan);
int xdmac_chan_status_wait(int chan, unsigned int us);
void xdmac_chan_free(int chan);
int xdmac_memcpy(const void *src, const void *dest, int size);

#endif	/* #ifndef __XDMAC_H__ */
