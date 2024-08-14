/*
 * Copyright (C) 2013 Microchip Technology Inc. and its subsidiaries
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef	__AT91_LCDC_H__
#define	__AT91_LCDC_H__

/*
 * Register Definitions
 */
#define LCDC_CFG(i)	((i) * 0x4)
#define LCDC_EN		0x20
#define LCDC_DIS	0x24
#define LCDC_SR		0x28
#define LCDC_IER	0x2c
#define LCDC_IDR	0x30
#define LCDC_IMR	0x34
#define LCDC_ISR	0x38
#define LCDC_ATTR	0x3C
#define LCDC_QOSCFG	0x40
#define LCDC_QOS1M	0x44
#define LCDC_QOS2M	0x48
#define LCDC_QOS3M	0x4C
#define LCDC_QOSMIN	0x50

#if defined(CONFIG_SAMA5D2)
#define LCDC_BASE_BASE	0x40
#elif defined(CONFIG_SAM9X60)
#define LCDC_BASE_BASE	0x60
#endif
#define LCDC_BASECHER	(LCDC_BASE_BASE)
#define LCDC_BASECHDR	(LCDC_BASE_BASE + 0x04)
#define LCDC_BASECHSR	(LCDC_BASE_BASE + 0x08)
#define LCDC_BASEIER	(LCDC_BASE_BASE + 0x0C)
#define LCDC_BASEIDR	(LCDC_BASE_BASE + 0x10)
#define LCDC_BASEIMR	(LCDC_BASE_BASE + 0x14)
#define LCDC_BASEISR	(LCDC_BASE_BASE + 0x18)
#define LCDC_BASEHEAD	(LCDC_BASE_BASE + 0x1C)
#define LCDC_BASEADDR	(LCDC_BASE_BASE + 0x20)
#define LCDC_BASECTRL	(LCDC_BASE_BASE + 0x24)
#define LCDC_BASENEXT	(LCDC_BASE_BASE + 0x28)
#define LCDC_BASECFG(i)	(LCDC_BASE_BASE + 0x2C + ((i) * 0x4))

#if defined(CONFIG_SAMA5D2)
#define LCDC_OVR1_BASE	0x140
#elif defined(CONFIG_SAM9X60)
#define LCDC_OVR1_BASE	0x160
#endif
#define LCDC_OVR1CHER	(LCDC_OVR1_BASE)
#define LCDC_OVR1CHDR	(LCDC_OVR1_BASE + 0x04)
#define LCDC_OVR1CHSR	(LCDC_OVR1_BASE + 0x08)
#define LCDC_OVR1IER	(LCDC_OVR1_BASE + 0x0C)
#define LCDC_OVR1IDR	(LCDC_OVR1_BASE + 0x10)
#define LCDC_OVR1IMR	(LCDC_OVR1_BASE + 0x14)
#define LCDC_OVR1ISR	(LCDC_OVR1_BASE + 0x18)
#define LCDC_OVR1HEAD	(LCDC_OVR1_BASE + 0x1C)
#define LCDC_OVR1ADDR	(LCDC_OVR1_BASE + 0x20)
#define LCDC_OVR1CTRL	(LCDC_OVR1_BASE + 0x24)
#define LCDC_OVR1NEXT	(LCDC_OVR1_BASE + 0x28)
#define LCDC_OVR1CFG(i)	(LCDC_OVR1_BASE + 0x2C + ((i) * 0x4))

#if defined(CONFIG_SAMA5D2)
#define LCDC_HEO_BASE	0x340
#elif defined(CONFIG_SAM9X60)
#define LCDC_HEO_BASE	0x360
#endif
#define LCDC_HEOCHER	(LCDC_HEO_BASE)
#define LCDC_HEOCHDR	(LCDC_HEO_BASE + 0x04)
#define LCDC_HEOCHSR	(LCDC_HEO_BASE + 0x08)
#define LCDC_HEOIER	(LCDC_HEO_BASE + 0x0C)
#define LCDC_HEOIDR	(LCDC_HEO_BASE + 0x10)
#define LCDC_HEOIMR	(LCDC_HEO_BASE + 0x14)
#define LCDC_HEOISR	(LCDC_HEO_BASE + 0x18)
#define LCDC_HEOHEAD	(LCDC_HEO_BASE + 0x1C)
#define LCDC_HEOADDR	(LCDC_HEO_BASE + 0x20)
#define LCDC_HEOCTRL	(LCDC_HEO_BASE + 0x24)
#define LCDC_HEONEXT	(LCDC_HEO_BASE + 0x28)
#define LCDC_HEOUHEAD	(LCDC_HEO_BASE + 0x2C)
#define LCDC_HEOUADDR	(LCDC_HEO_BASE + 0x30)
#define LCDC_HEOUCTRL	(LCDC_HEO_BASE + 0x34)
#define LCDC_HEOUNEXT	(LCDC_HEO_BASE + 0x38)
#define LCDC_HEOVHEAD	(LCDC_HEO_BASE + 0x3C)
#define LCDC_HEOVADDR	(LCDC_HEO_BASE + 0x40)
#define LCDC_HEOVCTRL	(LCDC_HEO_BASE + 0x44)
#define LCDC_HEOVNEXT	(LCDC_HEO_BASE + 0x48)
#define LCDC_HEOCFG(i)	(LCDC_HEO_BASE + 0x4C + ((i) * 0x4))
#define LCDC_HEOCLUT(i)	(0x1200 + ((i) * 0x4))

/*
 * Register Field Definitions
 */
/* LCDC_CFG0 */
#define LCDC_CFG0_CLKPOL	(0x1 << 0)
#define LCDC_CFG0_CLKSEL	(0x1 << 2)
#define LCDC_CFG0_CLKPWMSEL	(0x1 << 3)
#define LCDC_CFG0_CGDISBASE	(0x1 << 8)
#define LCDC_CFG0_CGDISOVR1	(0x1 << 9)
#define LCDC_CFG0_CGDISOVR2	(0x1 << 10)
#define LCDC_CFG0_CGDISHEO	(0x1 << 11)
#define LCDC_CFG0_CGDISPP	(0x1 << 13)
#define LCDC_CFG0_CLKDIV_MASK	(0xff << 16)
#define LCDC_CFG0_CLKDIV(x)	(((x) << 16) & LCDC_CFG0_CLKDIV_MASK)

/* LCDC_CFG1 */
#define LCDC_CFG1_HSPW_MASK	(0x3ff << 0)
#define LCDC_CFG1_HSPW(x)	(((x) << 0) & LCDC_CFG1_HSPW_MASK)
#define LCDC_CFG1_VSPW_MASK	(0x3ff << 16)
#define LCDC_CFG1_VSPW(x)	(((x) << 16) & LCDC_CFG1_VSPW_MASK)

/* LCDC_CFG2 */
#define LCDC_CFG2_VFPW_MASK	(0x3ff << 0)
#define LCDC_CFG2_VFPW(x)	(((x) << 0) & LCDC_CFG2_VFPW_MASK)
#define LCDC_CFG2_VBPW_MASK	(0x3ff << 16)
#define LCDC_CFG2_VBPW(x)	(((x) << 16) & LCDC_CFG2_VBPW_MASK)

/* LCDC_CFG3 */
#define LCDC_CFG3_HFPW_MASK	(0x3ff << 0)
#define LCDC_CFG3_HFPW(x)	(((x) << 0) & LCDC_CFG3_HFPW_MASK)
#define LCDC_CFG3_HBPW_MASK	(0x3ff << 16)
#define LCDC_CFG3_HBPW(x)	(((x) << 16) & LCDC_CFG3_HBPW_MASK)

/* LCDC_CFG4 */
#define LCDC_CFG4_PPL_MASK	(0x7ff << 0)
#define LCDC_CFG4_PPL(x)	(((x) << 0) & LCDC_CFG4_PPL_MASK)
#define LCDC_CFG4_RPF_MASK	(0x7ff << 16)
#define LCDC_CFG4_RPF(x)	(((x) << 16) & LCDC_CFG4_RPF_MASK)

/* LCDC_CFG5 */
#define LCDC_CFG5_HSPOL			(0x1 << 0)
#define LCDC_CFG5_VSPOL			(0x1 << 1)
#define LCDC_CFG5_VSPDLYS		(0x1 << 2)
#define LCDC_CFG5_VSPDLYE		(0x1 << 3)
#define LCDC_CFG5_DISPPOL		(0x1 << 4)
#define LCDC_CFG5_DITHER		(0x1 << 6)
#define LCDC_CFG5_DISPDLY		(0x1 << 7)
#define LCDC_CFG5_MODE_MASK		(0x3 << 8)
#define LCDC_CFG5_MODE(x)		(((x) << 8) & LCDC_CFG5_MODE_MASK)
#define     LCDC_CFG5_MODE_OUTPUT_12BPP	(0x0 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_16BPP	(0x1 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_18BPP	(0x2 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_24BPP	(0x3 << 8)
#define LCDC_CFG5_VSPSU			(0x1 << 12)
#define LCDC_CFG5_VSPHO			(0x1 << 13)
#define LCDC_CFG5_GUARDTIME_MASK	(0xff << 16)
#define LCDC_CFG5_GUARDTIME(x)		(((x) << 16) & LCDC_CFG5_GUARDTIME_MASK)

/* LCDC_CFG6 */
#define LCDC_CFG6_PWMPS_MASK	(0x7 << 0)
#define LCDC_CFG6_PWMPS(x)	(((x) << 0) & LCDC_CFG6_PWMPS_MASK)
#define LCDC_CFG6_PWMPOL	(0x1 << 4)
#define LCDC_CFG6_PWMCVAL_MASK	(0xff << 8)
#define LCDC_CFG6_PWMCVAL(x)	(((x) << 8) & LCDC_CFG6_PWMCVAL_MASK)

/* LCDC_CFG7 */
#define LCDC_CFG7_ROW_MASK	(0x7ff << 0)
#define LCDC_CFG7_ROW(x)	(((x) << 0) & LCDC_CFG7_ROW_MASK)

/* LCDC_EN */
#define LCDC_EN_CLKEN	(0x1 << 0)
#define LCDC_EN_SYNCEN	(0x1 << 1)
#define LCDC_EN_DISPEN	(0x1 << 2)
#define LCDC_EN_PWMEN	(0x1 << 3)

/* LCDC_DIS */
#define LCDC_DIS_CLKDIS		(0x1 << 0)
#define LCDC_DIS_SYNCDIS	(0x1 << 1)
#define LCDC_DIS_DISPDIS	(0x1 << 2)
#define LCDC_DIS_PWMDIS		(0x1 << 3)
#define LCDC_DIS_CLKRST		(0x1 << 8)
#define LCDC_DIS_SYNCRST	(0x1 << 9)
#define LCDC_DIS_DISPRST	(0x1 << 10)
#define LCDC_DIS_PWMRST		(0x1 << 11)

/* LCDC_SR */
#define LCDC_SR_CLKSTS	(0x1 << 0)
#define LCDC_SR_LCDSTS	(0x1 << 1)
#define LCDC_SR_DISPSTS	(0x1 << 2)
#define LCDC_SR_PWMSTS	(0x1 << 3)
#define LCDC_SR_SIPSTS	(0x1 << 4)

/* LCDC_LAYER */
#define LAYER_CH	(0x1 << 0)
#define LAYER_UPDATE	(0x1 << 1)
#define LAYER_A2Q	(0x1 << 2)
#define LAYER_RST	(0x1 << 8)

#define LAYER_DFETCH	(0x1 << 0)
#define LAYER_LFETCH	(0x1 << 1)
#define LAYER_IRQ_DMA	(0x1 << 2)
#define LAYER_IRQ_DSCR	(0x1 << 3)
#define LAYER_IRQ_ADD	(0x1 << 4)
#define LAYER_IRQ_DONE	(0x1 << 5)
#define LAYER_IRQ_OVR	(0x1 << 6)

#define LAYER_HEAD(x)	(((x) << 2) & 0xfffffffc)
#define LAYER_ADDR(x)	(((x) << 0) & 0xffffffff)
#define LAYER_NEXT(x)	(((x) << 0) & 0xffffffff)

#define LAYER_BLEN_MASK		(0x3 << 4)
#define LAYER_BLEN_SINGLE	(0 << 4)
#define LAYER_BLEN_INCR4	(1 << 4)
#define LAYER_BLEN_INCR8	(2 << 4)
#define LAYER_BLEN_INCR16	(3 << 4)
#define LAYER_DLBO		(0x1 << 8)
#define LAYER_ROT		(0x1 << 12)
#define LAYER_LOCK		(0x1 << 13)

#define LAYER_CLUT		(0x1 << 0)
#define LAYER_RGB_MASK		(0xf << 4)
#define LAYER_RGB_444		(0 << 4)
#define LAYER_ARGB_4444		(1 << 4)
#define LAYER_RGBA_4444		(2 << 4)
#define LAYER_RGB_565		(3 << 4)
#define LAYER_TRGB_1555		(4 << 4)
#define LAYER_18BPP_RGB_666	(5 << 4)
#define LAYER_RGB_666PACKED	(6 << 4)
#define LAYER_TRGB_1666		(7 << 4)
#define LAYER_TRGB_PACKED	(8 << 4)
#define LAYER_RGB_888		(9 << 4)
#define LAYER_RGB_888_PACKED	(10 << 4)
#define LAYER_TRGB_1888		(11 << 4)
#define LAYER_ARGB_8888		(12 << 4)
#define LAYER_RGBA_8888		(13 << 4)
#define LAYER_CLUT_MASK		(0x3 << 8)
#define LAYER_CLUT_1BPP		(0 << 8)
#define LAYER_CLUT_2BPP		(1 << 8)
#define LAYER_CLUT_4BPP		(2 << 8)
#define LAYER_CLUT_8BPP		(3 << 8)

#define LAYER_POS(x, y)		(((x) & 0x7ff) | (((y) & 0x7ff) << 16))
#define LAYER_SIZE(x, y)	((((x) - 1) & 0x7ff) | ((((y) - 1) & 0x7ff) << 16))
#define LAYER_STRIDE(x)		((x) & 0xffffffff)
#define LAYER_R_G_B(r, g, b)	((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define LAYER_RGB(x)		((x) & 0xffffff)

#define LAYER_CRKEY	(0x1 << 0)
#define LAYER_INV	(0x1 << 1)
#define LAYER_ITER2BL	(0x1 << 2)
#define LAYER_ITER	(0x1 << 3)
#define LAYER_REVALPHA	(0x1 << 4)
#define LAYER_GAEN	(0x1 << 5)
#define LAYER_LAEN	(0x1 << 6)
#define LAYER_OVR	(0x1 << 7)
#define LAYER_DMA	(0x1 << 8)
#define LAYER_REP	(0x1 << 9)
#define LAYER_DSTKEY	(0x1 << 10)
#define LAYER_DISC	(0x1 << 11)
#define LAYER_GA(x)	(((x) & 0xff) << 16)

#define LAYER_SCAL		(0x1 << 31)
#define LAYER_FACTOR(x, y)	((x & 0x3fff) | ((y & 0x3fff) << 16))

#endif	/* #ifndef __AT91_LCDC_H__ */
