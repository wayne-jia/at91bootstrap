// Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries
//
// SPDX-License-Identifier: MIT

#ifdef CONFIG_XDMAC

#include "common.h"
#include "hardware.h"
#include "board.h"
#include "xdmac.h"

#ifdef CONFIG_SAM9X60
#define MAX_CHANNEL	16
#endif

static unsigned int channels = 0;

static unsigned int xdmac_readl(unsigned int reg)
{
	return readl(CONFIG_SYS_BASE_XDMAC + reg);
}

static void xdmac_writel(unsigned int reg, unsigned int value)
{
	writel(value, CONFIG_SYS_BASE_XDMAC + reg);
}

int xdmac_chan_init(int chan, unsigned int setting)
{
	unsigned int chan_mask = 1 << chan;

	/* Check channel number and channel status */
	if((chan >= MAX_CHANNEL) || (channels & chan_mask))
		return -1;

	channels |= chan_mask;

	/* Disable channel and channel interrupt */
	xdmac_writel(XDMAC_GD, chan_mask);
	xdmac_writel(XDMAC_GID, chan_mask);

	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CC, setting);

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

void xdmac_chan_free(int chan)
{
	unsigned int chan_mask = 1 << chan;

	xdmac_writel(XDMAC_GD, chan_mask);
	xdmac_writel(XDMAC_CHAN(chan) + XDMAC_CC, 0);

	channels &= ~chan_mask;
}
#endif
