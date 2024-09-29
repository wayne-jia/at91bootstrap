/*
 * Copyright (C) 2013 Microchip Technology Inc. and its subsidiaries
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef	__AT91_XLCDC_H__
#define	__AT91_XLCDC_H__

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
#define LCDC_ATTRE	0x3C
#define LCDC_ATTRS	0x40

#ifdef CONFIG_SAM9X7
#define LCDC_BASE_BASE	0x60
#endif
#define LCDC_BASEIER	(LCDC_BASE_BASE)
#define LCDC_BASEIDR	(LCDC_BASE_BASE + 0x04)
#define LCDC_BASEIMR	(LCDC_BASE_BASE + 0x08)
#define LCDC_BASEISR	(LCDC_BASE_BASE + 0x0C)
#define LCDC_BASEEN		(LCDC_BASE_BASE + 0x10)
#define LCDC_BASECLA	(LCDC_BASE_BASE + 0x14)
#define LCDC_BASEFBA	(LCDC_BASE_BASE + 0x18)
#define LCDC_BASECFG(i)	(LCDC_BASE_BASE + 0x1C + ((i) * 0x4))

#define LCDC_WPMR	0xE4
#define LCDC_WPSR	0xE8

#ifdef CONFIG_SAM9X7
#define LCDC_OVR1_BASE	0x160
#endif
#define LCDC_OVR1IER	(LCDC_OVR1_BASE)
#define LCDC_OVR1IDR	(LCDC_OVR1_BASE + 0x04)
#define LCDC_OVR1IMR	(LCDC_OVR1_BASE + 0x08)
#define LCDC_OVR1ISR	(LCDC_OVR1_BASE + 0x0C)
#define LCDC_OVR1EN		(LCDC_OVR1_BASE + 0x10)
#define LCDC_OVR1CLA	(LCDC_OVR1_BASE + 0x14)
#define LCDC_OVR1FBA	(LCDC_OVR1_BASE + 0x18)
#define LCDC_OVR1CFG(i)	(LCDC_OVR1_BASE + 0x1C + ((i) * 0x4))

#ifdef CONFIG_SAM9X7
#define LCDC_HEO_BASE	0x360
#endif
#define LCDC_HEOIER		(LCDC_HEO_BASE)
#define LCDC_HEOIDR		(LCDC_HEO_BASE + 0x04)
#define LCDC_HEOIMR		(LCDC_HEO_BASE + 0x08)
#define LCDC_HEOISR		(LCDC_HEO_BASE + 0x0C)
#define LCDC_HEOEN		(LCDC_HEO_BASE + 0x10)
#define LCDC_HEOCLA		(LCDC_HEO_BASE + 0x14)
#define LCDC_HEOYFBA(i)		(LCDC_HEO_BASE + 0x18 + ((i*3) * 0x4))
#define LCDC_HEOCBFBA(i)	(LCDC_HEO_BASE + 0x18 + ((i) * 0x4))
#define LCDC_HEOCRFBA(i)	(LCDC_HEO_BASE + 0x18 + ((i) * 0x4))
#define LCDC_HEOCFG(i)		(LCDC_HEO_BASE + 0x30 + ((i) * 0x4))
#define LCDC_HEOVTAP10P(i)	(LCDC_HEO_BASE + 0xB0 + ((i) * 0x8))
#define LCDC_HEOVTAP32P(i)	(LCDC_HEO_BASE + 0xB4 + ((i) * 0x8))
#define LCDC_HEOHTAP10P(i)	(LCDC_HEO_BASE + 0x130 + ((i) * 0x8))
#define LCDC_HEOHTAP32P(i)	(LCDC_HEO_BASE + 0x134 + ((i) * 0x8))

#define LCDC_BASECLUT(i)	(0x700 + ((i) * 0x4))
#define LCDC_OVR1CLUT(i)	(0xB00 + ((i) * 0x4))
#define LCDC_OVR2CLUT(i)	(0xF00 + ((i) * 0x4))
#define LCDC_HEOCLUT(i)		(0x1300 + ((i) * 0x4))

/*
 * Register Field Definitions
 */
/* LCDC_CFG0 */
#define LCDC_CFG0_CLKPOL	(0x1 << 0)
#define LCDC_CFG0_CLKBYP	(0x1 << 1)
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
#define LCDC_CFG5_MODE_MASK		(0x7 << 8)
#define LCDC_CFG5_MODE(x)		(((x) << 8) & LCDC_CFG5_MODE_MASK)
#define     LCDC_CFG5_MODE_OUTPUT_12BPP	(0x0 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_16BPP	(0x1 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_18BPP	(0x2 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_24BPP	(0x3 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_DPI_16BPPCFG1	(0x0 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_DPI_16BPPCFG2	(0x1 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_DPI_16BPPCFG3	(0x2 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_DPI_18BPPCFG1	(0x3 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_DPI_18BPPCFG2	(0x4 << 8)
#define     LCDC_CFG5_MODE_OUTPUT_DPI_24BPP		(0x5 << 8)
#define LCDC_CFG5_DPI			(0x1 << 11)
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
#define LCDC_EN_SDEN	(0x1 << 5)
#define LCDC_EN_CMEN	(0x1 << 6)

/* LCDC_DIS */
#define LCDC_DIS_CLKDIS		(0x1 << 0)
#define LCDC_DIS_SYNCDIS	(0x1 << 1)
#define LCDC_DIS_DISPDIS	(0x1 << 2)
#define LCDC_DIS_PWMDIS		(0x1 << 3)
#define LCDC_DIS_SDDIS		(0x1 << 5)
#define LCDC_DIS_CMDIS		(0x1 << 6)
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
#define LCDC_SR_SDSTS	(0x1 << 5)
#define LCDC_SR_CMSTS	(0x1 << 6)

/* LCDC_ATTR */
#define LCDC_ATTR_BASE		(0x1 << 0)
#define LCDC_ATTR_OVR1		(0x1 << 1)
#define LCDC_ATTR_OVR2		(0x1 << 2)
#define LCDC_ATTR_HEO		(0x1 << 3)
#define LCDC_ATTR_BASECL	(0x1 << 8)
#define LCDC_ATTR_OVR1CL	(0x1 << 9)
#define LCDC_ATTR_OVR2CL	(0x1 << 10)
#define LCDC_ATTR_HEOCL		(0x1 << 11)
#define LCDC_ATTR_SIP		(0x1 << 31)

/* LCDC_LAYER */
#define LAYER_END	(0x1 << 0)
#define LAYER_ERROR	(0x1 << 1)
#define LAYER_OVF	(0x1 << 2)

#define LAYER_ENABLE	(0x1 << 0)

#define LAYER_CLA_MASK	(0x3fffffff << 2)
#define LAYER_CLA(x)	(((x) << 2) & LAYER_CLA_MASK)

#define LAYER_FBA_MASK	(0xffffffff)
#define LAYER_FBA(x)	((x) & LAYER_FBA_MASK)

#define LAYER_BLEN_MASK		(0x7 << 4)
#define LAYER_BLEN(x)	(((x) << 4) & LAYER_BLEN_MASK)
#define     LAYER_BLEN_INCR1	(0 << 4)
#define     LAYER_BLEN_INCR4	(1 << 4)
#define     LAYER_BLEN_INCR8	(2 << 4)
#define     LAYER_BLEN_INCR16	(3 << 4)
#define     LAYER_BLEN_INCR32	(4 << 4)
#define LAYER_BLENCC_MASK	(0x7 << 8)
#define LAYER_BLENCC(x)	(((x) << 8) & LAYER_BLENCC_MASK)
#define     LAYER_BLENCC_INCR1	(0 << 8)
#define     LAYER_BLENCC_INCR4	(1 << 8)
#define     LAYER_BLENCC_INCR8	(2 << 8)
#define     LAYER_BLENCC_INCR16	(3 << 8)
#define     LAYER_BLENCC_INCR32	(4 << 8)

#define LAYER_CLUTEN	(0x1 << 0)
#define LAYER_GAM		(0x1 << 2)
#define LAYER_RGBMODE_MASK	(0xf << 4)
#define LAYER_RGBMODE(x)	(((x) << 4) & LAYER_RGBMODE_MASK)
#define     LAYER_RGB_444			(0x0 << 4)
#define     LAYER_ARGB_4444			(0x1 << 4)
#define     LAYER_RGBA_4444			(0x2 << 4)
#define     LAYER_RGB_565			(0x3 << 4)
#define     LAYER_ARGB_1555			(0x4 << 4)
#define     LAYER_RGB_666			(0x5 << 4)
#define     LAYER_RGB_666PACKED		(0x6 << 4)
#define     LAYER_ARGB_1666			(0x7 << 4)
#define     LAYER_ARGB_PACKED		(0x8 << 4)
#define     LAYER_RGB_888			(0x9 << 4)
#define     LAYER_RGB_888_PACKED	(0x10 << 4)
#define     LAYER_ARGB_1888			(0x11 << 4)
#define     LAYER_ARGB_8888			(0x12 << 4)
#define     LAYER_RGBA_8888			(0x13 << 4)
#define LAYER_CLUTMODE_MASK	(0x3 << 8)
#define LAYER_CLUTMODE(x)	(((x) << 8) & LAYER_CLUTMODE_MASK)
#define     LAYER_CLUT_1BPP	(0x0 << 8)
#define     LAYER_CLUT_2BPP	(0x1 << 8)
#define     LAYER_CLUT_4BPP	(0x2 << 8)
#define     LAYER_CLUT_8BPP	(0x3 << 8)

#define LAYER_POS(x, y)			(((x) & 0x7ff) | (((y) & 0x7ff) << 16))
#define LAYER_SIZE(x, y)		((((x) - 1) & 0x7ff) | ((((y) - 1) & 0x7ff) << 16))
#define LAYER_STRIDE(x)			((x) & 0xffffffff)
#define LAYER_R_G_B(r, g, b)	((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define LAYER_RGB(x)			((x) & 0xffffff)

#define LAYER_DMA		(0x1 << 0)
#define LAYER_REP		(0x1 << 1)
#define LAYER_DISCEN	(0x1 << 4)
#define LAYER_VIDPRI	(0x1 << 5)

#define LAYER_DISCXPOS_MASK (0x7ff << 0)
#define LAYER_DISCXPOS(x)	(((x) << 0) & LAYER_DISCXPOS_MASK)
#define LAYER_DISCYPOS_MASK (0x7ff << 16)
#define LAYER_DISCYPOS(x)	(((x) << 16) & LAYER_DISCYPOS_MASK)

#define LAYER_DISCXSIZE_MASK (0x7ff << 0)
#define LAYER_DISCXSIZE(x)	(((x) << 0) & LAYER_DISCXSIZE_MASK)
#define LAYER_DISCYSIZE_MASK (0x7ff << 16)
#define LAYER_DISCYSIZE(x)	(((x) << 16) & LAYER_DISCYSIZE_MASK)

#define LAYER_VXSYEN	(0x1 << 0)
#define LAYER_VXSCEN	(0x1 << 1)
#define LAYER_HXSYEN	(0x1 << 4)
#define LAYER_HXSCEN	(0x1 << 5)

#define LAYER_VXSYFACT_MASK	(0xffffff << 0)
#define LAYER_VXSYFACT(x)	(((x) << 0) & LAYER_VXSYFACT_MASK)

#define LAYER_VXSCFACT_MASK	(0xffffff << 0)
#define LAYER_VXSCFACT(x)	(((x) << 0) & LAYER_VXSCFACT_MASK)

#define LAYER_HXSYFACT_MASK	(0xffffff << 0)
#define LAYER_HXSYFACT(x)	(((x) << 0) & LAYER_HXSYFACT_MASK)

#define LAYER_HXSCFACT_MASK	(0xffffff << 0)
#define LAYER_HXSCFACT(x)	(((x) << 0) & LAYER_HXSCFACT_MASK)

#define LAYER_VXSYOFF_MASK	(0xf << 0)
#define LAYER_VXSYOFF(x)	(((x) << 0) & LAYER_VXSYOFF_MASK)
#define LAYER_VXSYOFF1_MASK	(0xf << 8)
#define LAYER_VXSYOFF1(x)	(((x) << 8) & LAYER_VXSYOFF1_MASK)
#define LAYER_VXSCOFF_MASK	(0xf << 16)
#define LAYER_VXSCOFF(x)	(((x) << 16) & LAYER_VXSCOFF_MASK)
#define LAYER_VXSCOFF1_MASK	(0xf << 24)
#define LAYER_VXSCOFF1(x)	(((x) << 24) & LAYER_VXSCOFF1_MASK)

#define LAYER_HXSYOFF_MASK	(0xf << 0)
#define LAYER_HXSYOFF(x)	(((x) << 0) & LAYER_HXSYOFF_MASK)
#define LAYER_HXSCOFF_MASK	(0xf << 16)
#define LAYER_HXSCOFF(x)	(((x) << 16) & LAYER_HXSCOFF_MASK)

#define LAYER_VXSYCFG_MASK	(0x3 << 0)
#define LAYER_VXSYCFG(x)	(((x) << 0) & LAYER_VXSYCFG_MASK)
#define LAYER_VXSYTAP2		(0x1 << 4)
#define LAYER_VXSY1201N		(0x1 << 5)
#define LAYER_VXSYBICU		(0x1 << 6)
#define LAYER_VXSYCFG1_MASK	(0x3 << 8)
#define LAYER_VXSYCFG1(x)	(((x) << 8) & LAYER_VXSYCFG1_MASK)
#define LAYER_VXSCCFG_MASK	(0x3 << 16)
#define LAYER_VXSCCFG(x)	(((x) << 16) & LAYER_VXSCCFG_MASK)
#define LAYER_VXSCTAP2		(0x1 << 20)
#define LAYER_VXSC1201N		(0x1 << 21)
#define LAYER_VXSCBICU		(0x1 << 22)
#define LAYER_VXSCCFG1_MASK	(0x3 << 24)
#define LAYER_VXSCCFG1(x)	(((x) << 24) & LAYER_VXSCCFG1_MASK)

#define LAYER_HXSYCFG_MASK	(0x3 << 0)
#define LAYER_HXSYCFG(x)	(((x) << 0) & LAYER_HXSYCFG_MASK)
#define LAYER_HXSYTAP2		(0x1 << 4)
#define LAYER_HXSYBICU		(0x1 << 5)
#define LAYER_HXSCCFG_MASK	(0x3 << 16)
#define LAYER_HXSCCFG(x)	(((x) << 16) & LAYER_HXSCCFG_MASK)
#define LAYER_HXSCTAP2		(0x1 << 20)
#define LAYER_HXSCBICU		(0x1 << 21)

#endif	/* #ifndef __AT91_XLCDC_H__ */
