// Copyright (C) 2006 Microchip Technology Inc. and its subsidiaries
//
// SPDX-License-Identifier: MIT

#include "common.h"
#include "hardware.h"
#include "board.h"
#include "string.h"
#include "arch/at91_pio.h"
#include "arch/at91_xlcdc.h"
#include "debug.h"
#include "div.h"
#include "pmc.h"
#include "gpio.h"
#include "timer.h"

#ifdef CONFIG_LVDSC
#include "lvdsc.h"
#endif

#define XLCDC_BMP_ADDR (LOGO_FB_ADDRESS + 16)

#define BMP_LINE_ALIGN 4

struct xlcdc_desc {
	u16 width;     /**< Display image width */
	u16 height;    /**< Display image height */
	u16 framerate; /**< Frame rate in Hz */

	u16 timing_vfp; /**< Vertical front porch in number of lines */
	u16 timing_vbp; /**< Vertical back porch in number of lines */
	u16 timing_vpw; /**< Vertical pulse width in number of lines */
	u16 timing_hfp; /**< Horizontal front porch in LCDDOTCLK cycles */
	u16 timing_hbp; /**< Horizontal back porch in LCDDOTCLK cycles */
	u16 timing_hpw; /**< Horizontal pulse width in LCDDOTCLK cycles */

	u16 ovr_mode;
	u16 ovr_width;
	u16 ovr_height;
	u16 ovr_sc_width;
	u16 ovr_sc_height;
	u16 ovr_xpos;
	u16 ovr_ypos;
	u32 ovr_xstride;
	u32 ovr_pstride;
	u8 *ovr_buf;

	u32 clut_size;

	struct bmp_desc *bmp;
};

typedef enum {
	BI_RGB       = 0x0000,
	BI_RLE8      = 0x0001,
	BI_RLE4      = 0x0002,
	BI_BITFIELDS = 0x0003,
	BI_JPEG      = 0x0004,
	BI_PNG       = 0x0005,
	BI_CMYK      = 0x000B,
	BI_CMYKRLE8  = 0x000C,
	BI_CMYKRLE4  = 0x000D
} Compression;

struct bit_fields {
	u32 r_mask;
	u32 g_mask;
	u32 b_mask;
} __attribute__ ((packed, aligned(1)));

struct bmp_desc {
	u8  bf_type[2];
	u32 bf_size;
	u16 bf_reserved1;
	u16 bf_reserver2;
	u32 bf_offbits;

	u32 bi_size;
	u32 bi_width;
	u32 bi_height;
	u16 bi_planes;
	u16 bi_bitcount;
	u32 bi_compression;
	u32 bi_sizeimage;
	u32 bi_xpelspermeter;
	u32 bi_ypelspermeter;
	u32 bi_clrused;
	u32 bi_clrimportant;
	u32 clut[];
} __attribute__ ((packed, aligned(1)));

static struct xlcdc_desc xlcdc = {
	.width       = BOARD_LCD_WIDTH,
	.height      = BOARD_LCD_HEIGHT,
	.framerate   = BOARD_LCD_FRAMERATE,
	.timing_vfp  = BOARD_LCD_TIMING_VFP,
	.timing_vbp  = BOARD_LCD_TIMING_VBP,
	.timing_vpw  = BOARD_LCD_TIMING_VPW,
	.timing_hfp  = BOARD_LCD_TIMING_HFP,
	.timing_hbp  = BOARD_LCD_TIMING_HBP,
	.timing_hpw  = BOARD_LCD_TIMING_HPW,

	.bmp         = (struct bmp_desc *)(XLCDC_BMP_ADDR),
};

static unsigned char lookup_table[16] = {
	0x0, 0x8, 0x4, 0xc,
	0x2, 0xa, 0x6, 0xe,
	0x1, 0x9, 0x5, 0xd,
	0x3, 0xb, 0x7, 0xf,
};

static inline unsigned char reverse_bit(unsigned char c)
{
	return (lookup_table[c & 0xf] << 4) | lookup_table[c >> 4];
}

static inline unsigned char reverse_4bit(unsigned char c)
{
	return (c >> 4) | (c << 4);
}

static inline unsigned int xlcdc_get_clock(void)
{
#if defined(CONFIG_SAM9X7)
	return pmc_get_generic_clock(CONFIG_SYS_ID_XLCDC);
#else
	#error "XLCDC: Could NOT get source clock!"
#endif
}

static unsigned int xlcdc_readl(unsigned int reg)
{
	return readl(CONFIG_SYS_BASE_XLCDC + reg);
}

static void xlcdc_writel(unsigned int reg, unsigned int value)
{
	writel(value, CONFIG_SYS_BASE_XLCDC + reg);
}

static inline void wait_lcdsr_equal(u32 mask)
{
	while (xlcdc_readl(LCDC_SR) & mask);
}

static inline void wait_lcdsr_nonequal(u32 mask)
{
	while (!(xlcdc_readl(LCDC_SR) & mask));
}

static inline void wait_attrs_equal(u32 mask)
{
	while (xlcdc_readl(LCDC_ATTRS) & mask);
}

static inline void wait_attrs_nonequal(u32 mask)
{
	while (!(xlcdc_readl(LCDC_ATTRS) & mask));
}

static void set_clut(u32 reg_base, const u8 *clut, u16 size)
{
	u32 i;

	for (i = 0; i < size*4; i += 4)
		xlcdc_writel(reg_base + i, clut[i] | clut[i+1]<<8 | clut[i+2]<<16);
}

static u32 get_factor(u32 srcsize, u32 dstsize)
{
	return div((u32)((1 << 20) * srcsize), dstsize);
}

#ifndef CONFIG_LVDSC
static u32 clock_div(void)
{
	u32 clk_div, remainder;

	division(xlcdc_get_clock(), BOARD_LCD_PIXEL_CLOCK, (unsigned int *)&clk_div,
				(unsigned int *)&remainder);
	if (remainder > (BOARD_LCD_PIXEL_CLOCK / 2))
		clk_div++;

	return clk_div;
}
#endif

static void xlcdc_set_backlight(u8 level)
{
#ifndef BOARD_LCD_PIN_BL
	u32 cfg = xlcdc_readl(LCDC_CFG(6)) & ~LCDC_CFG6_PWMCVAL_MASK;
	xlcdc_writel(LCDC_CFG(6), cfg | LCDC_CFG6_PWMCVAL(level));
#else
	pio_set_gpio_output(BOARD_LCD_PIN_BL, level ? 1 : 0);
#endif
}

static void xlcdc_stop(void)
{
#ifndef BOARD_LCD_PIN_BL
	xlcdc_writel(LCDC_DIS, LCDC_DIS_PWMDIS);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_equal(LCDC_SR_PWMSTS);
#endif

#ifdef CONFIG_MIPI_DSI
	xlcdc_writel(LCDC_DIS, LCDC_DIS_CMDIS);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_equal(LCDC_SR_CMSTS);
#endif

	xlcdc_writel(LCDC_DIS, LCDC_DIS_SDDIS);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_nonequal(LCDC_SR_SDSTS);

	xlcdc_writel(LCDC_DIS, LCDC_DIS_DISPDIS);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_equal(LCDC_SR_DISPSTS);

	xlcdc_writel(LCDC_DIS, LCDC_DIS_SYNCDIS);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_equal(LCDC_SR_LCDSTS);

	xlcdc_writel(LCDC_DIS, LCDC_DIS_CLKDIS);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_equal(LCDC_SR_CLKSTS);
}

static void xlcdc_start(void)
{
#ifdef CONFIG_LVDSC
	xlcdc_writel(LCDC_CFG(0), LCDC_CFG0_CLKPWMSEL |
				 LCDC_CFG0_CLKBYP |
				 LCDC_CFG0_CLKPOL);
#else
	xlcdc_writel(LCDC_CFG(0), LCDC_CFG0_CLKDIV(clock_div()) |
				 LCDC_CFG0_CLKPWMSEL |
				 LCDC_CFG0_CLKPOL);
#endif
	wait_lcdsr_equal(LCDC_SR_SIPSTS);

	xlcdc_writel(LCDC_CFG(1), LCDC_CFG1_VSPW(xlcdc.timing_vpw - 1) |
				 LCDC_CFG1_HSPW(xlcdc.timing_hpw - 1));
	wait_lcdsr_equal(LCDC_SR_SIPSTS);

	xlcdc_writel(LCDC_CFG(2), LCDC_CFG2_VBPW(xlcdc.timing_vbp - 1) |
				 LCDC_CFG2_VFPW(xlcdc.timing_vfp - 1));
	wait_lcdsr_equal(LCDC_SR_SIPSTS);

	xlcdc_writel(LCDC_CFG(3), LCDC_CFG3_HBPW(xlcdc.timing_hbp - 1) |
				 LCDC_CFG3_HFPW(xlcdc.timing_hfp - 1));
	wait_lcdsr_equal(LCDC_SR_SIPSTS);

	xlcdc_writel(LCDC_CFG(4), LCDC_CFG4_RPF(xlcdc.height - 1) |
				 LCDC_CFG4_PPL(xlcdc.width - 1));
	wait_lcdsr_equal(LCDC_SR_SIPSTS);

	xlcdc_writel(LCDC_CFG(5), LCDC_CFG5_GUARDTIME(0) |
				 LCDC_CFG5_DPI |
				 LCDC_CFG5_MODE_OUTPUT_DPI_24BPP |
				 LCDC_CFG5_DISPDLY |
				 LCDC_CFG5_VSPDLYS |
				 LCDC_CFG5_VSPOL |
				 LCDC_CFG5_HSPOL);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);

#ifndef BOARD_LCD_PIN_BL
	xlcdc_writel(LCDC_CFG(6), LCDC_CFG6_PWMCVAL(0) |
				 LCDC_CFG6_PWMPOL |
				 LCDC_CFG6_PWMPS(6));
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
#endif

	xlcdc_writel(LCDC_EN, LCDC_EN_CLKEN);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_nonequal(LCDC_SR_CLKSTS);

	xlcdc_writel(LCDC_EN, LCDC_EN_SYNCEN);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_nonequal(LCDC_SR_LCDSTS);

	xlcdc_writel(LCDC_EN, LCDC_EN_DISPEN);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_nonequal(LCDC_SR_DISPSTS);

	xlcdc_writel(LCDC_EN, LCDC_EN_SDEN);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_equal(LCDC_SR_SDSTS);

#ifdef CONFIG_MIPI_DSI
	xlcdc_writel(LCDC_EN, LCDC_EN_CMEN);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_nonequal(LCDC_SR_CMSTS);
#endif

#ifndef BOARD_LCD_PIN_BL
	xlcdc_writel(LCDC_EN, LCDC_EN_PWMEN);
	wait_lcdsr_equal(LCDC_SR_SIPSTS);
	wait_lcdsr_nonequal(LCDC_SR_PWMSTS);
#endif
}

void xlcdc_show_base(void)
{
	xlcdc_writel(LCDC_BASECFG(1), LAYER_RGB_888_PACKED);
	xlcdc_writel(LCDC_BASECFG(3), LAYER_RGB(LOGO_RGBDEF));
	xlcdc_writel(LCDC_BASECFG(4), 0);
	xlcdc_writel(LCDC_BASEEN, LAYER_ENABLE);
	xlcdc_writel(LCDC_ATTRE, LCDC_ATTR_BASE);
	wait_attrs_equal(LCDC_ATTR_SIP);
}

void xlcdc_show_heo(void)
{
	u32 xfactor, yfactor;

	/* Base configuration */
	xlcdc_writel(LCDC_HEOCFG(0), LAYER_BLEN(4) |
								LAYER_BLENCC(1));
	xlcdc_writel(LCDC_HEOCFG(1), xlcdc.ovr_mode);
	xlcdc_writel(LCDC_HEOCFG(2), LAYER_POS(xlcdc.ovr_xpos, xlcdc.ovr_ypos));
	xlcdc_writel(LCDC_HEOCFG(3), LAYER_SIZE(xlcdc.ovr_sc_width, xlcdc.ovr_sc_height));
	xlcdc_writel(LCDC_HEOCFG(4), LAYER_SIZE(xlcdc.ovr_width, xlcdc.ovr_height));
	if (xlcdc.ovr_xstride)
		xlcdc_writel(LCDC_HEOCFG(5), LAYER_STRIDE(xlcdc.ovr_xstride));
	if (xlcdc.ovr_pstride)
		xlcdc_writel(LCDC_HEOCFG(6), LAYER_STRIDE(xlcdc.ovr_pstride));
	xlcdc_writel(LCDC_HEOCFG(9), LAYER_RGB(LOGO_RGBDEF));

	/* Configure CULT if needed */
	if (xlcdc.ovr_mode | LAYER_CLUTEN)
		set_clut(LCDC_HEOCLUT(0), (u8 *)xlcdc.bmp->clut, xlcdc.clut_size);

	/* Configure scaler if needed */
	if (xlcdc.ovr_sc_width >= xlcdc.ovr_width) {
		xfactor = get_factor(xlcdc.ovr_width, xlcdc.ovr_sc_width);
		yfactor = get_factor(xlcdc.ovr_height, xlcdc.ovr_sc_height);

		xlcdc_writel(LCDC_HEOCFG(23),
					LAYER_HXSCEN | LAYER_HXSYEN |
					LAYER_VXSCEN | LAYER_VXSYEN);
		xlcdc_writel(LCDC_HEOCFG(24), yfactor);
		xlcdc_writel(LCDC_HEOCFG(25), yfactor);
		xlcdc_writel(LCDC_HEOCFG(26), xfactor);
		xlcdc_writel(LCDC_HEOCFG(27), xfactor);
		xlcdc_writel(LCDC_HEOCFG(28), 0);
		xlcdc_writel(LCDC_HEOCFG(29), 0);
		xlcdc_writel(LCDC_HEOCFG(30),
					LAYER_VXSCBICU | LAYER_VXSCCFG(1) |
					LAYER_VXSYBICU | LAYER_VXSYCFG(1));
		xlcdc_writel(LCDC_HEOCFG(31),
					LAYER_HXSCBICU | LAYER_HXSCCFG(1) |
					LAYER_HXSYBICU | LAYER_HXSYCFG(1));
	}

	xlcdc_writel(LCDC_HEOYFBA(0), LAYER_FBA((u32)xlcdc.ovr_buf));
	xlcdc_writel(LCDC_HEOCFG(12), LAYER_DMA);

	xlcdc_writel(LCDC_HEOEN, LAYER_ENABLE);
	xlcdc_writel(LCDC_ATTRE, LCDC_ATTR_HEO);
	wait_attrs_equal(LCDC_ATTR_SIP);
}

void lcdc_init(void)
{
	/* Invalidate bmp struct buffer */
	xlcdc.bmp->bf_type[0] = 0;
	xlcdc.bmp->bf_type[1] = 0;

	at91_xlcdc_hw_init();
#ifdef CONFIG_LVDSC
	lvdsc_start();
#endif
	xlcdc_stop();
	xlcdc_start();
	xlcdc_show_base();
}

int lcdc_display(void)
{
	u32 i;
	u32 line_bytes;
	u8 line_padding;
	struct bit_fields *bf = NULL;

	if ((xlcdc.bmp->bf_type[0] != 'B') || (xlcdc.bmp->bf_type[1] != 'M')) {
		dbg_info("XLCDC: bmp file not found\n\r");
		return -1;
	}

	switch (xlcdc.bmp->bi_bitcount) {
	case 32:
		line_bytes = xlcdc.bmp->bi_width * 4;

		if (xlcdc.bmp->bi_compression == BI_RGB) {
			xlcdc.ovr_mode = LAYER_ARGB_8888;
		} else if (xlcdc.bmp->bi_compression == BI_BITFIELDS) {
			bf = (struct bit_fields *)&xlcdc.bmp->clut;

			if ((bf->r_mask == 0xff0000) && (bf->g_mask == 0xff00) && (bf->b_mask == 0xff))
				xlcdc.ovr_mode = LAYER_ARGB_8888;
			else
				goto UNSUPPORTED;
		}
		break;

	case 24:
		line_bytes = xlcdc.bmp->bi_width * 3;
		xlcdc.ovr_mode = LAYER_RGB_888_PACKED;
		break;

	case 16:
		line_bytes = xlcdc.bmp->bi_width * 2;

		if (xlcdc.bmp->bi_compression == BI_RGB) {
			xlcdc.ovr_mode = LAYER_ARGB_1555;
		} else if (xlcdc.bmp->bi_compression == BI_BITFIELDS) {
			bf = (struct bit_fields *)&xlcdc.bmp->clut;

			if ((bf->r_mask == 0x7c00) && (bf->g_mask == 0x03e0) && (bf->b_mask == 0x001f))
				xlcdc.ovr_mode = LAYER_ARGB_1555;
			else if ((bf->r_mask == 0xf800) && (bf->g_mask == 0x07e0) && (bf->b_mask == 0x001f))
				xlcdc.ovr_mode = LAYER_RGB_565;
			else
				goto UNSUPPORTED;
		} else {
			goto UNSUPPORTED;
		}
		break;

	case 8:
		line_bytes = xlcdc.bmp->bi_width * 1;
		xlcdc.ovr_mode  = LAYER_CLUT_8BPP | LAYER_CLUTEN;
		xlcdc.clut_size = 256;
		break;

	case 4:
		line_bytes = xlcdc.bmp->bi_width / 2;
		if (xlcdc.bmp->bi_width & 0x1) {
			line_bytes++;
			dbg_info("XLCDC: ERROR logo width %d is not 2 pixels aligned\n", xlcdc.bmp->bi_width);
		}
		xlcdc.ovr_mode = LAYER_CLUT_4BPP | LAYER_CLUTEN;
		xlcdc.clut_size = 16;
		break;

	case 1:
		line_bytes = xlcdc.bmp->bi_width / 8;
		if (xlcdc.bmp->bi_width & 0x7) {
			line_bytes++;
			dbg_info("XLCDC: ERROR logo width %d is not 8 pixels aligned\n", xlcdc.bmp->bi_width);
		}
		xlcdc.ovr_mode = LAYER_CLUT_1BPP | LAYER_CLUTEN;
		xlcdc.clut_size = 2;
		break;

	default:
UNSUPPORTED:
		dbg_info("XLCDC: unsupported bmp format, bitcount=%d compression=%x\n\r",
					xlcdc.bmp->bi_bitcount, xlcdc.bmp->bi_compression);
		if ((xlcdc.bmp->bi_compression == BI_BITFIELDS) && bf) {
			dbg_info("XLCDC: r_mask=%x, g_mask=%x, b_mask=%x\n\r",
						bf->r_mask, bf->g_mask, bf->b_mask);
		}
		return -1;
	}

	line_padding = line_bytes & (BMP_LINE_ALIGN - 1);
	if (line_padding)
		line_padding = BMP_LINE_ALIGN - line_padding;

	xlcdc.ovr_buf    = (u8 *)(XLCDC_BMP_ADDR + xlcdc.bmp->bf_offbits);
	xlcdc.ovr_width  = xlcdc.bmp->bi_width;

	if ((xlcdc.bmp->bi_bitcount == 1) || (xlcdc.bmp->bi_bitcount == 4)) {
		u32 lines = (int)xlcdc.bmp->bi_height >= 0 ? xlcdc.bmp->bi_height : -xlcdc.bmp->bi_height;

		for (i = 0; i < ((line_bytes + line_padding) * lines); i++) {
				if (xlcdc.bmp->bi_bitcount == 1)
					xlcdc.ovr_buf[i] = reverse_bit(xlcdc.ovr_buf[i]);
				else if (xlcdc.bmp->bi_bitcount == 4)
					xlcdc.ovr_buf[i] = reverse_4bit(xlcdc.ovr_buf[i]);
		}
	}

	if (((int)xlcdc.bmp->bi_height) > 0) {
		xlcdc.ovr_height  = xlcdc.bmp->bi_height;
		xlcdc.ovr_xstride = -(line_bytes * 2 + line_padding);
		xlcdc.ovr_buf     += (line_bytes + line_padding) * (xlcdc.ovr_height - 1);
	} else {
		xlcdc.ovr_height  = -xlcdc.bmp->bi_height;
		xlcdc.ovr_xstride = line_padding;
	}

	xlcdc.ovr_sc_width  = (LOGO_SCALE == 0) ? xlcdc.ovr_width : (xlcdc.ovr_width * (8 + LOGO_SCALE) / 8);
	xlcdc.ovr_sc_height = (LOGO_SCALE == 0) ? xlcdc.ovr_height : (xlcdc.ovr_height * (8 + LOGO_SCALE) / 8);
	xlcdc.ovr_xpos      = (xlcdc.ovr_sc_width >= xlcdc.width) ? 0 : ((xlcdc.width - xlcdc.ovr_sc_width) / 2);
	xlcdc.ovr_ypos      = (xlcdc.ovr_sc_height >= xlcdc.height) ? 0 : ((xlcdc.height - xlcdc.ovr_sc_height) / 2);

	xlcdc_show_heo();
	if (LOGO_BL_DELAY)
		mdelay(LOGO_BL_DELAY);
	xlcdc_set_backlight(LOGO_BL);

	return 0;
}

int bmp_size(void *p)
{
	struct bmp_desc *bmp = p;

	if ((bmp->bf_type[0] == 'B') && (bmp->bf_type[1] == 'M')) {
		return bmp->bf_size;
	}

	return 0;
}
