// Copyright (C) 2006 Microchip Technology Inc. and its subsidiaries
//
// SPDX-License-Identifier: MIT

#include "common.h"
#include "hardware.h"
#include "board.h"
#include "string.h"
#include "arch/at91_lcdc.h"
#include "debug.h"
#include "div.h"
#include "pmc.h"
#include "timer.h"
#include "spi.h"

#define LCDC_DMA_ADDR LOGO_FB_ADDRESS
#define LCDC_BMP_ADDR (LOGO_FB_ADDRESS + sizeof(struct dma_desc))

#define LCDC_XPHIDEF  4
#define LCDC_YPHIDEF  4

#define BMP_LINE_ALIGN 4

struct dma_desc {
	u32 addr;
	u32 ctrl;
	u32 next;
	u32 reserved;
};

struct lcdc_desc {
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
	struct dma_desc *ovr_dma;
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

static struct lcdc_desc lcdc = {
	.width       = BOARD_LCD_WIDTH,
	.height      = BOARD_LCD_HEIGHT,
	.framerate   = BOARD_LCD_FRAMERATE,
	.timing_vfp  = BOARD_LCD_TIMING_VFP,
	.timing_vbp  = BOARD_LCD_TIMING_VBP,
	.timing_vpw  = BOARD_LCD_TIMING_VPW,
	.timing_hfp  = BOARD_LCD_TIMING_HFP,
	.timing_hbp  = BOARD_LCD_TIMING_HBP,
	.timing_hpw  = BOARD_LCD_TIMING_HPW,

	.ovr_dma     = (struct dma_desc *)LCDC_DMA_ADDR,
	.bmp         = (struct bmp_desc *)(LCDC_BMP_ADDR),
};

static u32 heo_upscaling_xcoef[] = {
	0xf74949f7,
	0x00000000,
	0xf55f33fb,
	0x000000fe,
	0xf5701efe,
	0x000000ff,
	0xf87c0dff,
	0x00000000,
	0x00800000,
	0x00000000,
	0x0d7cf800,
	0x000000ff,
	0x1e70f5ff,
	0x000000fe,
	0x335ff5fe,
	0x000000fb,
};

static u32 heo_upscaling_ycoef[] = {
	0x00004040,
	0x00075920,
	0x00056f0c,
	0x00027b03,
	0x00008000,
	0x00037b02,
	0x000c6f05,
	0x00205907,
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

static inline unsigned int lcdc_get_base(void)
{
	return CONFIG_SYS_BASE_LCDC;
}

static inline unsigned int lcdc_get_clock(void)
{
#ifdef CONFIG_SAMA5D2
	return pmc_mck_get_rate(CONFIG_SYS_ID_LCDC);
#elif CONFIG_SAM9X60
	return pmc_get_generic_clock(CONFIG_SYS_ID_LCDC);
#endif
}

static unsigned int lcdc_readl(unsigned int reg)
{
	return readl(lcdc_get_base() + reg);
}

static void lcdc_writel(unsigned int reg, unsigned int value)
{
	writel(value, lcdc_get_base() + reg);
}

static void wait_for_clock_domain_sync(void)
{
	while (lcdc_readl(LCDC_SR) & LCDC_SR_SIPSTS);
}

static void set_phicoeff(u32 reg_base, const u32 *coeff_tab, u16 size)
{
	u32 i;

	for (i = 0; i < size; i++)
		lcdc_writel(reg_base + (i * 4), coeff_tab[i]);
}

static void set_clut(u32 reg_base, const u8 *clut, u16 size)
{
	u32 i;

	for (i = 0; i < size*4; i += 4)
		lcdc_writel(reg_base + i, clut[i] | clut[i+1]<<8 | clut[i+2]<<16);
}

static u32 get_factor(u32 srcsize, u32 dstsize, u32 phidef)
{
	u32 factor, max_memsize;

	factor = div((256 * ((8 * (srcsize - 1)) - phidef)), (dstsize - 1));
	max_memsize = ((factor * (dstsize - 1)) + (256 * phidef)) / 2048;

	if (max_memsize > srcsize - 1)
		factor--;

	return factor;
}

static void lcdc_set_backlight(u8 level)
{
	u32 cfg = lcdc_readl(LCDC_CFG(6)) & ~LCDC_CFG6_PWMCVAL_MASK;
	lcdc_writel(LCDC_CFG(6), cfg | LCDC_CFG6_PWMCVAL(level));
}

static void lcdc_off(void)
{
	lcdc_writel(LCDC_DIS, LCDC_DIS_PWMDIS);
	while (lcdc_readl(LCDC_SR) & LCDC_SR_PWMSTS);

	lcdc_writel(LCDC_DIS, LCDC_DIS_DISPDIS);
	while (lcdc_readl(LCDC_SR) & LCDC_SR_DISPSTS);

	lcdc_writel(LCDC_DIS, LCDC_DIS_SYNCDIS);
	while (lcdc_readl(LCDC_SR) & LCDC_SR_LCDSTS);

	lcdc_writel(LCDC_DIS, LCDC_DIS_CLKDIS);
	while (lcdc_readl(LCDC_SR) & LCDC_SR_CLKSTS);
}

static void lcdc_on(void)
{
	u32 pixel_clock = lcdc.framerate;
	u32 clk_div, remainder;

	pixel_clock *= lcdc.timing_hpw + lcdc.timing_hbp +
		lcdc.width + lcdc.timing_hfp;
	pixel_clock *= lcdc.timing_vpw + lcdc.timing_vbp +
		lcdc.height + lcdc.timing_vfp;

	division(lcdc_get_clock(), pixel_clock, (unsigned int *)&clk_div, (unsigned int *)&remainder);
	if (remainder > (pixel_clock / 2))
		clk_div++;

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_CFG(0), LCDC_CFG0_CLKDIV(clk_div - 2) |
				 LCDC_CFG0_CGDISPP |
				 LCDC_CFG0_CGDISHEO |
				 LCDC_CFG0_CGDISBASE |
				 LCDC_CFG0_CLKPWMSEL);

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_CFG(1), LCDC_CFG1_VSPW(lcdc.timing_vpw - 1) |
				 LCDC_CFG1_HSPW(lcdc.timing_hpw - 1));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_CFG(2), LCDC_CFG2_VBPW(lcdc.timing_vbp) |
				 LCDC_CFG2_VFPW(lcdc.timing_vfp - 1));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_CFG(3), LCDC_CFG3_HBPW(lcdc.timing_hbp - 1) |
				 LCDC_CFG3_HFPW(lcdc.timing_hfp - 1));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_CFG(4), LCDC_CFG4_RPF(lcdc.height - 1) |
				 LCDC_CFG4_PPL(lcdc.width - 1));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_CFG(5), LCDC_CFG5_GUARDTIME(0) |
				 LCDC_CFG5_MODE_OUTPUT_16BPP |
				 LCDC_CFG5_DISPDLY |
				 LCDC_CFG5_VSPDLYS |
				 LCDC_CFG5_VSPOL |
				 LCDC_CFG5_HSPOL);

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_CFG(6), LCDC_CFG6_PWMCVAL(0) |
				 //LCDC_CFG6_PWMPOL |
				 LCDC_CFG6_PWMPS(6));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_EN, LCDC_EN_CLKEN);
	while (!(lcdc_readl(LCDC_SR) & LCDC_SR_CLKSTS));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_EN, LCDC_EN_SYNCEN);
	while (!(lcdc_readl(LCDC_SR) & LCDC_SR_LCDSTS));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_EN, LCDC_EN_DISPEN);
	while (!(lcdc_readl(LCDC_SR) & LCDC_SR_DISPSTS));

	wait_for_clock_domain_sync();
	lcdc_writel(LCDC_EN, LCDC_EN_PWMEN);
}

void lcdc_show_base(void)
{
	lcdc_writel(LCDC_BASECFG(1), LAYER_RGB_565);
	lcdc_writel(LCDC_BASECFG(3), LAYER_RGB(LOGO_RGBDEF));
	lcdc_writel(LCDC_BASECHER, LAYER_UPDATE | LAYER_CH);
}

void lcdc_show_heo(void)
{
	/* Base configuration */
	lcdc_writel(LCDC_HEOCFG(0), LAYER_ROT);
	lcdc_writel(LCDC_HEOCFG(1), lcdc.ovr_mode);
	lcdc_writel(LCDC_HEOCFG(2), LAYER_POS(lcdc.ovr_xpos, lcdc.ovr_ypos));
	lcdc_writel(LCDC_HEOCFG(3), LAYER_SIZE(lcdc.ovr_sc_width, lcdc.ovr_sc_height));
	lcdc_writel(LCDC_HEOCFG(4), LAYER_SIZE(lcdc.ovr_width, lcdc.ovr_height));
	if (lcdc.ovr_xstride)
		lcdc_writel(LCDC_HEOCFG(5), LAYER_STRIDE(lcdc.ovr_xstride));
	if (lcdc.ovr_pstride)
		lcdc_writel(LCDC_HEOCFG(6), LAYER_STRIDE(lcdc.ovr_pstride));
	lcdc_writel(LCDC_HEOCFG(9), LAYER_RGB(LOGO_RGBDEF));

	/* Configure CULT if needed */
	if (lcdc.ovr_mode | LAYER_CLUT)
		set_clut(LCDC_HEOCLUT(0), (u8 *)lcdc.bmp->clut, lcdc.clut_size);

	/* Configure scaler if needed */
	if (lcdc.ovr_width != lcdc.ovr_sc_width) {
		set_phicoeff(LCDC_HEOCFG(17), heo_upscaling_xcoef, sizeof(heo_upscaling_xcoef) / sizeof(u32));
		set_phicoeff(LCDC_HEOCFG(33), heo_upscaling_ycoef, sizeof(heo_upscaling_ycoef) / sizeof(u32));
		lcdc_writel(LCDC_HEOCFG(13), LAYER_SCAL |
			(get_factor(lcdc.ovr_height, lcdc.ovr_sc_height, LCDC_YPHIDEF) << 16) |
			(get_factor(lcdc.ovr_width, lcdc.ovr_sc_width, LCDC_XPHIDEF) << 0));
	}

	lcdc_writel(LCDC_HEOADDR, (u32)lcdc.ovr_buf);
	lcdc_writel(LCDC_HEOCTRL, LAYER_DFETCH);
	lcdc_writel(LCDC_HEONEXT, (u32)lcdc.ovr_dma);
	lcdc_writel(LCDC_HEOCFG(12), LAYER_GA(0xff) | LAYER_DMA | LAYER_OVR | LAYER_GAEN);

	lcdc_writel(LCDC_HEOCHER, LAYER_UPDATE | LAYER_CH);
}

static void write_command(unsigned char data)
{
	unsigned char i;
	lcd_pio_CS(0);
	lcd_pio_SCL(0);
	lcd_pio_SDA(0);
	lcd_pio_SCL(1);
	
	for (i=0; i<8; i++) {
		lcd_pio_SCL(0);
		if (data & 0x80)
			lcd_pio_SDA(1);
		else
			lcd_pio_SDA(0);

		lcd_pio_SCL(1);
		data = data << 1;
	}
	lcd_pio_CS(1); //CS 1
}

static void write_data(unsigned char data)
{
	unsigned char i;
	lcd_pio_CS(0);
	lcd_pio_SCL(0);
	lcd_pio_SDA(1);
	lcd_pio_SCL(1);
	
	for (i=0; i<8; i++) {
		lcd_pio_SCL(0);
		if (data & 0x80)
			lcd_pio_SDA(1);
		else
			lcd_pio_SDA(0);

		lcd_pio_SCL(1);
		data = data << 1;
	}
	lcd_pio_CS(1); //CS 1
}

#if 1  //??
void BOARD_InitLCD_SPI(void)
{
	at91_spi0_hw_init();

	lcd_pio_RES(1);
	mdelay(5);
	lcd_pio_RES(0);
	mdelay(20);
	lcd_pio_RES(1);
	mdelay(5);

	write_command(0xFE);
	write_command(0xEF);

	write_command(0xEB);
	write_data(0x14);
	write_command(0x84);
	write_data(0x65);
	write_command(0x85);
	write_data(0xFF);
	write_command(0x86);
	write_data(0xFF);
	write_command(0x87);
	write_data(0xFF);
	write_command(0x88);
	write_data(0x0A);
	write_command(0x89);
	write_data(0x21);
	write_command(0x8A);
	write_data(0x40);
	write_command(0x8B);
	write_data(0x80);
	write_command(0x8C);
	write_data(0x01);
	write_command(0x8D);
	write_data(0x01);
	write_command(0x8E);
	write_data(0xFF);
	write_command(0x8F);
	write_data(0xFF);

	write_command(0xB6);
	write_data(0x00);
	write_data(0x00);

	write_command(0x36);
	write_data(0x48);


	write_command(0x3a);//´«?¸??¡?
	write_data(0x55);///RGB MODE SELECTED

	write_command(0xf6);//½???ýT?
	write_data(0xc6);//RGBmode-16/18bit

	write_command(0xb0);//RGB?ºN?
	write_data(0x42);////40:DE MODE   60:SYNC MODE

	write_command(0xb5);//??T?
	write_data(0x08);//vfp[7:0]      host  8
	write_data(0x09);//vbp[6:0]      host  4
	write_data(0x14);//hbp[4:0]      host 20


	write_command(0x90);
	write_data(0x08);
	write_data(0x08);
	write_data(0x08);
	write_data(0x08);

	write_command(0xBD);
	write_data(0x06);
	///////////add///////////////////////
	write_command(0xA6);
	write_data(0x74);

	write_command(0xBF);
	write_data(0x1C);

	write_command(0xA7);
	write_data(0x45);

	write_command(0xA9);
	write_data(0xBB);

	write_command(0xB8);
	write_data(0x63);
	/////////////////////////////////
	write_command(0xBC);
	write_data(0x00);

	write_command(0xFF);
	write_data(0x60);
	write_data(0x01);
	write_data(0x04);

	write_command(0xC3);
	write_data(0x21);
	write_command(0xC4);
	write_data(0x21);

	write_command(0xC9);
	write_data(0x25);

	write_command(0xBE);
	write_data(0x11);

	write_command(0xE1);
	write_data(0x10);
	write_data(0x0E);

	write_command(0xDF);
	write_data(0x21);
	write_data(0x0c);
	write_data(0x02);

	write_command(0xF0);
	write_data(0x45);
	write_data(0x09);
	write_data(0x08);
	write_data(0x08);
	write_data(0x26);
	write_data(0x2A);

	write_command(0xF1);
	write_data(0x43);
	write_data(0x70);
	write_data(0x72);
	write_data(0x36);
	write_data(0x37);
	write_data(0x6F);

	write_command(0xF2);
	write_data(0x45);
	write_data(0x09);
	write_data(0x08);
	write_data(0x08);
	write_data(0x26);
	write_data(0x2A);

	write_command(0xF3);
	write_data(0x43);
	write_data(0x70);
	write_data(0x72);
	write_data(0x36);
	write_data(0x37);
	write_data(0x6F);

	write_command(0xED);
	write_data(0x1B);
	write_data(0x0B);

	write_command(0xAE);
	write_data(0x77);

	write_command(0xCD);
	write_data(0x63);


	write_command(0x70);
	write_data(0x07);
	write_data(0x07);
	write_data(0x04);
	write_data(0x0E);
	write_data(0x0F);
	write_data(0x09);
	write_data(0x07);
	write_data(0x08);
	write_data(0x03);

	write_command(0xE8);
	write_data(0x24);
	/////////////////////////////////////////////
	write_command(0x60);
	write_data(0x38); //STV1
	write_data(0x0B);
	write_data(0x6D);
	write_data(0x6D);

	write_data(0x39);//STV2
	write_data(0xF0);
	write_data(0x6D);
	write_data(0x6D);


	write_command(0x61);
	write_data(0x38);//STV3
	write_data(0xF4);
	write_data(0x6D);
	write_data(0x6D);

	write_data(0x38);//STV4
	write_data(0xF7);
	write_data(0x6D);
	write_data(0x6D);
	/////////////////////////////////////
	write_command(0x62);
	write_data(0x38);
	write_data(0x0D);
	write_data(0x71);
	write_data(0xED);
	write_data(0x70);
	write_data(0x70);
	write_data(0x38);
	write_data(0x0F);
	write_data(0x71);
	write_data(0xEF);
	write_data(0x70);
	write_data(0x70);

	write_command(0x63);
	write_data(0x38);
	write_data(0x11);
	write_data(0x71);
	write_data(0xF1);
	write_data(0x70);
	write_data(0x70);
	write_data(0x38);
	write_data(0x13);
	write_data(0x71);
	write_data(0xF3);
	write_data(0x70);
	write_data(0x70);
	///////////////////////////////////////////////////////
	write_command(0x64);
	write_data(0x28);
	write_data(0x29);
	write_data(0xF1);
	write_data(0x01);
	write_data(0xF1);
	write_data(0x00);
	write_data(0x07);

	//??
	write_command(0x66);
	write_data(0x3C);
	write_data(0x00);
	write_data(0xCD);
	write_data(0x67);
	write_data(0x45);
	write_data(0x45);
	write_data(0x10);
	write_data(0x00);
	write_data(0x00);
	write_data(0x00);

	write_command(0x67);
	write_data(0x00);
	write_data(0x3C);
	write_data(0x00);
	write_data(0x00);
	write_data(0x00);
	write_data(0x01);
	write_data(0x54);
	write_data(0x10);
	write_data(0x32);
	write_data(0x98);



	write_command(0x74);
	write_data(0x10);
	write_data(0x85);
	write_data(0x80);
	write_data(0x00);
	write_data(0x00);
	write_data(0x4E);
	write_data(0x00);

	write_command(0x98);
	write_data(0x3e);
	write_data(0x07);



	write_command(0x35);
	write_data(0x00);
	write_command(0x21);
	mdelay(5);
	//--------end gamma setting--------------//

	write_command(0x11);
	mdelay(5);
	write_command(0x29);
	//mdelay(120);
	write_command(0x2C);
	//mdelay(120);

}

#else  //??
void BOARD_InitLCD_SPI(void)
{
	at91_spi0_hw_init();

	lcd_pio_RES(1);
	//dbg_info("read1 RES:%d should be 1\n\r", read_gpio_RES());
	mdelay(5);
	lcd_pio_RES(0);
	//dbg_info("read2 RES:%d should be 0\n\r", read_gpio_RES());
	mdelay(20);
	lcd_pio_RES(1);
	//dbg_info("read3 RES:%d should be 1\n\r", read_gpio_RES());
	mdelay(5);

	write_command(0xFE);
	write_command(0xEF);

	write_command(0xEB);
	write_data(0x14);
	write_command(0x84);
	write_data(0x65);
	write_command(0x85);
	write_data(0xF1);
	write_command(0x86);
	write_data(0x98);
	write_command(0x87);
	write_data(0x28);
	write_command(0x88);
	write_data(0x0A);
	write_command(0x89);
	write_data(0x21);
	write_command(0x8A);
	write_data(0x40);
	write_command(0x8B);
	write_data(0x80);
	write_command(0x8C);
	write_data(0x01);
	write_command(0x8D);
	write_data(0x03);//0x00
	write_command(0x8E);
	write_data(0xDF);
	write_command(0x8F);
	write_data(0x52);

	write_command(0xB6);//??/????
	write_data(0x00);
	write_data(0x40);//GS 

	write_command(0x36);//??XY???,?????
	write_data(0x08);

	write_command(0xf6);//??????
	write_data(0xc6);//RGBmode-16/18bit

	write_command(0xb0);//RGB????
	write_data(0x40);////40:DE MODE   60:SYNC MODE

	write_command(0x3A); //16/18bit??
	write_data(0x55);

	write_command(0xb5);
	write_data(0x08);
	write_data(0x09);
	write_data(0x14);

	write_command(0x90);
	write_data(0x08);
	write_data(0x08);
	write_data(0x08);
	write_data(0x08);

	write_command(0xBD);
	write_data(0x06);

	//write_command(0xA6);
	//write_data(0x74);

	//write_command(0xBF);
	//write_data(0x1C);

	//write_command(0xA7);
	//write_data(0x45);

	write_command(0xA9);
	write_data(0xCC);

	//write_command(0xB8);
	//write_data(0x63);


	write_command(0xBC);
	write_data(0x00);

	write_command(0xFF);
	write_data(0x60);
	write_data(0x01);
	write_data(0x04);

	write_command(0xC3);///1a 1b		
	write_data(0x21);//0x17

	write_command(0xC4);///2a 2b		
	write_data(0x21);//0x17/

	write_command(0xC9);
	write_data(0x25);///vrg1a  2a 0x25

	write_command(0xBE);
	write_data(0x11);

	write_command(0xE1);
	write_data(0x10);
	write_data(0x0E);

	write_command(0xDF);
	write_data(0x21);
	write_data(0x0c);
	write_data(0x02);

	write_command(0xF0);
	write_data(0x45);
	write_data(0x09);
	write_data(0x08);
	write_data(0x08);
	write_data(0x26);
	write_data(0x2A);

	write_command(0xF1);
	write_data(0x43);
	write_data(0x70);
	write_data(0x72);
	write_data(0x36);
	write_data(0x37);
	write_data(0x6F);

	write_command(0xF2);
	write_data(0x45);
	write_data(0x09);
	write_data(0x08);
	write_data(0x08);
	write_data(0x26);
	write_data(0x2A);

	write_command(0xF3);
	write_data(0x43);
	write_data(0x70);
	write_data(0x72);
	write_data(0x36);
	write_data(0x37);
	write_data(0x6F);

	write_command(0xED);
	write_data(0x1B);
	write_data(0x0B);

	//write_command(0xAC);
	//write_data(0x47);
	write_command(0xAE);
	write_data(0x77);
	//write_command(0xCB);
	//write_data(0x02);
	write_command(0xCD);
	write_data(0x63);

	write_command(0x70);
	write_data(0x07);
	write_data(0x07);
	write_data(0x04);
	write_data(0x0E);
	write_data(0x0F);
	write_data(0x09);
	write_data(0x07);
	write_data(0x08);
	write_data(0x03);

	write_command(0xE8);
	write_data(0x24);

	write_command(0x60);
	write_data(0x38);
	write_data(0x0B);
	write_data(0x6D);
	write_data(0x6D);

	write_data(0x39);
	write_data(0xF0);
	write_data(0x6D);
	write_data(0x6D);


	write_command(0x61);
	write_data(0x38);
	write_data(0xF4);
	write_data(0x6D);
	write_data(0x6D);

	write_data(0x38);
	write_data(0xF7);
	write_data(0x6D);
	write_data(0x6D);
	/////////////////////////////////////
	write_command(0x62);
	write_data(0x38);
	write_data(0x0D);
	write_data(0x71);
	write_data(0xED);
	write_data(0x70);
	write_data(0x70);
	write_data(0x38);
	write_data(0x0F);
	write_data(0x71);
	write_data(0xEF);
	write_data(0x70);
	write_data(0x70);

	write_command(0x63);
	write_data(0x38);
	write_data(0x11);
	write_data(0x71);
	write_data(0xF1);
	write_data(0x70);
	write_data(0x70);
	write_data(0x38);
	write_data(0x13);
	write_data(0x71);
	write_data(0xF3);
	write_data(0x70);
	write_data(0x70);
	///////////////////////////////////////////////////////
	write_command(0x64);
	write_data(0x28);
	write_data(0x29);
	write_data(0xF1);
	write_data(0x01);
	write_data(0xF1);
	write_data(0x00);
	write_data(0x07);


	//·´?
	write_command(0x66);
	write_data(0x3C);
	write_data(0x00);
	write_data(0x98);
	write_data(0x10);
	write_data(0x32);
	write_data(0x45);
	write_data(0x01);
	write_data(0x00);
	write_data(0x00);
	write_data(0x00);

	write_command(0x67);
	write_data(0x00);
	write_data(0x3C);
	write_data(0x00);
	write_data(0x00);
	write_data(0x00);
	write_data(0x10);
	write_data(0x54);
	write_data(0x67);
	write_data(0x45);
	write_data(0xcd);
	////////////////////////////////////////////////

	write_command(0x74);
	write_data(0x10);
	write_data(0x85);
	write_data(0x80);
	write_data(0x00);
	write_data(0x00);
	write_data(0x4E);
	write_data(0x00);

	write_command(0x98);
	write_data(0x3e);
	write_data(0x07);
	write_command(0x99);
	write_data(0x3e);
	write_data(0x07);

	write_command(0x35);//?????
	write_command(0x21);//????
	mdelay(5);

	write_command(0x11);//??
	mdelay(5);
	write_command(0x29);
	write_command(0x2C);
}
#endif

void lcdc_init(void)
{
	/* Invalidate bmp struct buffer */
	lcdc.bmp->bf_type[0] = 0;
	lcdc.bmp->bf_type[1] = 0;

	at91_lcdc_hw_init();
	lcdc_off();
	lcdc_on();
	lcdc_set_backlight(0);
	BOARD_InitLCD_SPI();
	lcdc_show_base();
}

int lcdc_display(void)
{
	u32 i;
	u32 line_bytes;
	u8 line_padding;
	struct bit_fields *bf = NULL;

	if ((lcdc.bmp->bf_type[0] != 'B') || (lcdc.bmp->bf_type[1] != 'M')) {
		dbg_printf("LCDC: bmp file not found\n\r");
		return -1;
	}

	switch (lcdc.bmp->bi_bitcount) {
	case 32:
		line_bytes = lcdc.bmp->bi_width * 4;

		if (lcdc.bmp->bi_compression == BI_RGB) {
			lcdc.ovr_mode = LAYER_ARGB_8888;
		} else if (lcdc.bmp->bi_compression == BI_BITFIELDS) {
			bf = (struct bit_fields *)&lcdc.bmp->clut;

			if ((bf->r_mask == 0xff0000) && (bf->g_mask == 0xff00) && (bf->b_mask == 0xff))
				lcdc.ovr_mode = LAYER_ARGB_8888;
			else
				goto UNSUPPORTED;
		}
		break;

	case 24:
		line_bytes = lcdc.bmp->bi_width * 3;
		lcdc.ovr_mode = LAYER_RGB_888_PACKED;
		break;

	case 16:
		line_bytes = lcdc.bmp->bi_width * 2;

		if (lcdc.bmp->bi_compression == BI_RGB)
			lcdc.ovr_mode = LAYER_TRGB_1555;
		else if (lcdc.bmp->bi_compression == BI_BITFIELDS) {
			bf = (struct bit_fields *)&lcdc.bmp->clut;

			if ((bf->r_mask == 0x7c00) && (bf->g_mask == 0x03e0) && (bf->b_mask == 0x001f))
				lcdc.ovr_mode = LAYER_TRGB_1555;
			else if ((bf->r_mask == 0xf800) && (bf->g_mask == 0x07e0) && (bf->b_mask == 0x001f))
				lcdc.ovr_mode = LAYER_RGB_565;
			else
				goto UNSUPPORTED;
		} else {
			goto UNSUPPORTED;
		}
		break;

	case 8:
		line_bytes = lcdc.bmp->bi_width * 1;
		lcdc.ovr_mode  = LAYER_CLUT_8BPP | LAYER_CLUT;
		lcdc.clut_size = 256;
		break;

	case 4:
		line_bytes = lcdc.bmp->bi_width / 2;
		if (lcdc.bmp->bi_width & 0x1) {
			line_bytes++;
			dbg_printf("LCDC: ERROR logo width %d is not 2 pixels aligned\n", lcdc.bmp->bi_width);
		}
		lcdc.ovr_mode = LAYER_CLUT_4BPP | LAYER_CLUT;
		lcdc.clut_size = 16;
		break;

	case 1:
		line_bytes = lcdc.bmp->bi_width / 8;
		if (lcdc.bmp->bi_width & 0x7) {
			line_bytes++;
			dbg_printf("LCDC: ERROR logo width %d is not 8 pixels aligned\n", lcdc.bmp->bi_width);
		}
		lcdc.ovr_mode = LAYER_CLUT_1BPP | LAYER_CLUT;
		lcdc.clut_size = 2;
		break;

	default:
UNSUPPORTED:
		dbg_printf("LCDC: unsupported bmp format, bitcount=%d compression=%x\n\r", lcdc.bmp->bi_bitcount, lcdc.bmp->bi_compression);
		if ((lcdc.bmp->bi_compression == BI_BITFIELDS) && bf)
			dbg_printf("LCDC: r_mask=%x, g_mask=%x, b_mask=%x\n\r", bf->r_mask, bf->g_mask, bf->b_mask);
		return -1;
	}

	line_padding = line_bytes & (BMP_LINE_ALIGN - 1);
	if (line_padding)
		line_padding = BMP_LINE_ALIGN - line_padding;

	lcdc.ovr_buf    = (u8 *)(LCDC_BMP_ADDR + lcdc.bmp->bf_offbits);
	lcdc.ovr_width  = lcdc.bmp->bi_width;

	if ((lcdc.bmp->bi_bitcount == 1) || (lcdc.bmp->bi_bitcount == 4)) {
		u32 lines = (int)lcdc.bmp->bi_height >= 0 ? lcdc.bmp->bi_height : -lcdc.bmp->bi_height;

		for (i = 0; i < ((line_bytes + line_padding) * lines); i++) {
				if (lcdc.bmp->bi_bitcount == 1)
					lcdc.ovr_buf[i] = reverse_bit(lcdc.ovr_buf[i]);
				else if (lcdc.bmp->bi_bitcount == 4)
					lcdc.ovr_buf[i] = reverse_4bit(lcdc.ovr_buf[i]);
		}
	}

	if (((int)lcdc.bmp->bi_height) > 0) {
		lcdc.ovr_height  = lcdc.bmp->bi_height;
		lcdc.ovr_xstride = -(line_bytes * 2 + line_padding);
		lcdc.ovr_buf     += (line_bytes + line_padding) * (lcdc.ovr_height - 1);
	} else {
		lcdc.ovr_height  = -lcdc.bmp->bi_height;
		lcdc.ovr_xstride = line_padding;
	}

	lcdc.ovr_sc_width  = (LOGO_SCALE == 0) ? lcdc.ovr_width : (lcdc.ovr_width * (8 + LOGO_SCALE) / 8);
	lcdc.ovr_sc_height = (LOGO_SCALE == 0) ? lcdc.ovr_height : (lcdc.ovr_height * (8 + LOGO_SCALE) / 8);
	lcdc.ovr_xpos      = (lcdc.ovr_sc_width >= lcdc.width) ? 0 : ((lcdc.width - lcdc.ovr_sc_width) / 2);
	lcdc.ovr_ypos      = (lcdc.ovr_sc_height >= lcdc.height) ? 0 : ((lcdc.height - lcdc.ovr_sc_height) / 2);

	lcdc.ovr_dma->addr = (u32)lcdc.ovr_buf;
	lcdc.ovr_dma->ctrl = LAYER_DFETCH;
	lcdc.ovr_dma->next = (u32)lcdc.ovr_dma;
	lcdc.ovr_dma->reserved = 0;

	lcdc_show_heo();
	if (LOGO_BL_DELAY)
		mdelay(LOGO_BL_DELAY);
	lcdc_set_backlight(LOGO_BL);

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
