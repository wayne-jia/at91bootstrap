// Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries
//
// SPDX-License-Identifier: MIT

#ifdef CONFIG_XDMAC

#include "common.h"
#include "hardware.h"
#include "board.h"
#include "timer.h"
#include "xdmac.h"

#define MAX_CHANNEL CONFIG_SYS_XDMAC_MAX_CHANNEL
#define MAX_DWIDTH  CONFIG_SYS_XDMAC_MAX_DWIDTH

#define XDMA_TIMEOUT 1000000

static unsigned int channels = 0;

static unsigned int xdmac_readl(unsigned int reg)
{
	return readl(CONFIG_SYS_BASE_XDMAC + reg);
}

static void xdmac_writel(unsigned int reg, unsigned int value)
{
	writel(value, CONFIG_SYS_BASE_XDMAC + reg);
}

int xdmac_chan_init(int chan, unsigned int config)
{
	unsigned int chan_mask = 1 << chan;

	/* Check channel number and channel status */
	if((chan >= MAX_CHANNEL) || (channels & chan_mask))
		return -1;

	channels |= chan_mask;

	/* Disable channel and channel interrupt */
	xdmac_writel(XDMAC_GD, chan_mask);
	xdmac_writel(XDMAC_GID, chan_mask);

	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CC, config);

	return 0;
}

int xdmac_chan_config(int chan, unsigned int config)
{
	unsigned int chan_mask = 1 << chan;

	if (!(channels & chan_mask))
		return -1;

	/* Disable channel and channel interrupt */
	xdmac_writel(XDMAC_GD, chan_mask);
	xdmac_writel(XDMAC_GID, chan_mask);

	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CC, config);

	return 0;
}

int xdmac_chan_transfer(int chan, const void *src, const void *dest, int size)
{
	unsigned int chan_mask = 1 << chan;

	if (!(channels & chan_mask))
		return -1;

	/* Clear channel interrupt status */
	xdmac_readl(XDMAC_CHAN(chan) + XDMAC_CIS);

	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CSA, (unsigned int)src);
	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CDA, (unsigned int)dest);
	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CUBC, XDMAC_CUBC_UBLEN(size));

	/* Enable channel */
	xdmac_writel(XDMAC_GE, chan_mask);

	return 0;
}

int xdmac_chan_status(int chan)
{
	unsigned int status = xdmac_readl(XDMAC_CHAN(chan) + XDMAC_CIS);

	if (status & (XDMAC_CI_ROE | XDMAC_CI_WBE | XDMAC_CI_RBE))
		return XDMAC_TRANSFER_ERROR;
	else if (status & XDMAC_CI_BI)
		return XDMAC_TRANSFER_COMPLETE;

	return 0;
}

int xdmac_chan_status_wait(int chan, unsigned int us)
{
	int ret;

	ret = xdmac_chan_status(chan);
	while ((us--) && (!ret)) {
		udelay(1);
		ret = xdmac_chan_status(chan);
	}

	return ret;
}

void xdmac_chan_free(int chan)
{
	unsigned int chan_mask = 1 << chan;

	xdmac_writel(XDMAC_GD, chan_mask);
	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CC, 0);

	channels &= ~chan_mask;
}

int xdmac_memcpy(const void *src, const void *dest, int size)
{
	/* Use the last channel for memory copy */
	unsigned int chan = MAX_CHANNEL - 1;
	unsigned int config, count, rest;
	int ret, i;

	/* Find the best transmission width */
	for (i = MAX_DWIDTH; i >= 0; i--)
		if (!((unsigned int)src & ((1 << i) - 1)) && !((unsigned int)dest & ((1 << i) - 1)))
			break;

	config = XDMAC_TRANSFER_MEM(i);
	count  = size >> i;
	rest   = size - (count << i);

	if (count) {
		if (xdmac_chan_init(chan, config) ||
			xdmac_chan_transfer(chan, src, dest, count))
			return -1;

		ret = xdmac_chan_status_wait(chan, XDMA_TIMEOUT);
		xdmac_chan_free(chan);

		if (ret != XDMAC_TRANSFER_COMPLETE)
			return -1;
	}

	/* Copy the rest of the data */
	while (rest) {
		*(char *)(dest + size - rest) = *(char *)(src + size - rest);
		rest--;
	}

	return 0;
}
#endif
