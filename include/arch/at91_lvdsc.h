/*
 * Copyright (C) 2013 Microchip Technology Inc. and its subsidiaries
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef	__AT91_LVDSC_H__
#define	__AT91_LVDSC_H__

/*
 * Register Definitions
 */
#define LVDSC_CR	0x00
#define LVDSC_CFGR	0x04
#define LVDSC_UCBR	0x08
#define LVDSC_SR	0x0C
#define LVDSC_ACR	0x14
#define LVDSC_WPMR	0xE4
#define LVDSC_WPSR	0xE8

/*
 * Register Field Definitions
 */
/* LVDSC_CR */
#define LVDSC_CR_SER_EN	(0x1 << 0)

/* LVDSC_CFGR */
#define LVDSC_CFGR_LCDC_PIXSIZE	(0x1 << 0)
#define LVDSC_CFGR_LCDC_DEN_POL	(0x1 << 1)
#define LVDSC_CFGR_DC_BAL		(0x1 << 5)
#define LVDSC_CFGR_MAPPING		(0x1 << 6)

/* LVDSC_UCBR */
#define LVDSC_UCBR_RESA3	(0x1 << 2)

/* LVDSC_SR */
#define LVDSC_SR_CS	(0x1 << 0)

/* LVDSC_ACR */
#define LVDSC_ACR_DCBIAS_MASK		(0x1f << 0)
#define LVDSC_ACR_DCBIAS(x)			(((x) << 0) & LVDSC_ACR_DCBIAS_MASK)
#define LVDSC_ACR_PREEMP_A0_MASK	(0x7 << 8)
#define LVDSC_ACR_PREEMP_A0(x)		(((x) << 8) & LVDSC_ACR_PREEMP_A0_MASK)
#define LVDSC_ACR_PREEMP_A1_MASK	(0x7 << 12)
#define LVDSC_ACR_PREEMP_A1(x)		(((x) << 12) & LVDSC_ACR_PREEMP_A1_MASK)
#define LVDSC_ACR_PREEMP_A2_MASK	(0x7 << 16)
#define LVDSC_ACR_PREEMP_A2(x)		(((x) << 16) & LVDSC_ACR_PREEMP_A2_MASK)
#define LVDSC_ACR_PREEMP_A3_MASK	(0x7 << 20)
#define LVDSC_ACR_PREEMP_A3(x)		(((x) << 20) & LVDSC_ACR_PREEMP_A3_MASK)
#define LVDSC_ACR_PREEMP_CLK1_MASK	(0x7 << 24)
#define LVDSC_ACR_PREEMP_CLK1(x)	(((x) << 24) & LVDSC_ACR_PREEMP_CLK1_MASK)

/* LVDSC_WPMR */
#define LVDSC_WPMR_WPEN		(0x1 << 0)
#define LVDSC_WPMR_LCKWPEN	(0x1 << 4)
#define LVDSC_WPMR_WPKEY	(0x4C5644 << 8)

/* LVDSC_WPSR */
#define LVDSC_WPSR_WPVS					(0x1 << 0)
#define LVDSC_WPSR_SWE					(0x1 << 3)
#define LVDSC_WPSR_WPVSRC_MASK			(0xffff << 8)
#define LVDSC_WPSR_SWETYP_WRITE_RO		(0x1 << 24)
#define LVDSC_WPSR_SWETYP_UNDEF_RW		(0x2 << 24)
#define LVDSC_WPSR_SWETYP_WEIRD_ACTION	(0x3 << 24)

#endif	/* #ifndef __AT91_LVDSC_H__ */
