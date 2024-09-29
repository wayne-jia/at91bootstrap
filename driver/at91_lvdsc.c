// Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries
//
// SPDX-License-Identifier: MIT

#include "common.h"
#include "hardware.h"
#include "board.h"
#include "string.h"
#include "arch/at91_pmc/pmc.h"
#include "arch/at91_lvdsc.h"
#include "debug.h"
#include "div.h"
#include "pmc.h"

#define COREPLLCK_MIN	600000000
#define COREPLLCK_MAX	1200000000
#define LVDSPLLCK_MIN	175000000
#define LVDSPLLCK_MAX	550000000
#define LVDSPLL_COUNT	0x3f
#define PIXCK_DIV		7

#define FRACR_ONE		(1 << 22)
#define FRACR_STEP		128

#define LVDSPLL_CLOCK (BOARD_LCD_PIXEL_CLOCK * PIXCK_DIV)
#if (LVDSPLL_CLOCK < LVDSPLLCK_MIN) || (LVDSPLL_CLOCK > LVDSPLLCK_MAX)
	#error "LVDSC: LVDS clock out of range!"
#endif

static unsigned int lvdsc_readl(unsigned int reg)
{
	return readl(CONFIG_SYS_BASE_LVDSC + reg);
}

static void lvdsc_writel(unsigned int reg, unsigned int value)
{
	writel(value, CONFIG_SYS_BASE_LVDSC + reg);
}

#if !defined(BOARD_LVDS_PLL_MUL) || \
	!defined(BOARD_LVDS_PLL_DIV) || \
	!defined(BOARD_LVDS_PLL_FRACR)
static void pll_config(u32 *pll_mul, u32 *pll_div, u32 *pll_fracr)
{
	u32 core_clk, lvds_clk, real_clk;
	u32 match = 0xffffffff;
	u32 tmp;
	int i, j;

	for (i = 0; ;i++) {
		core_clk = (i + 1) * BOARD_MAINOSC;

		if ((core_clk >= COREPLLCK_MIN) && (core_clk <= COREPLLCK_MAX)) {
			for (j = 0; ;j++) {
				lvds_clk = div(core_clk, (j + 1));

				if ((lvds_clk >= LVDSPLLCK_MIN) && (lvds_clk <= LVDSPLLCK_MAX)) {
					if (lvds_clk > LVDSPLL_CLOCK) {
						tmp = lvds_clk - LVDSPLL_CLOCK;
					} else if (lvds_clk < LVDSPLL_CLOCK) {
						tmp = LVDSPLL_CLOCK - lvds_clk;
					} else {
						*pll_mul = i;
						*pll_div = j;
						break;
					}

					if (tmp < match) {
						match = tmp;
						*pll_mul = i;
						*pll_div = j;
					}
				} else if (lvds_clk < LVDSPLLCK_MIN) {
					break;
				}
			}
		} else if (core_clk > COREPLLCK_MAX) {
			break;
		}
	}

	core_clk = LVDSPLL_CLOCK * (*pll_div + 1);
	real_clk = BOARD_MAINOSC * (*pll_mul + 1);
	if (core_clk != real_clk) {
		if (core_clk < real_clk) {
			tmp = real_clk - core_clk;
			tmp = BOARD_MAINOSC - tmp;
			(*pll_mul)--;
		} else {
			tmp = core_clk - real_clk;
		}

		tmp = div(FRACR_STEP * tmp, BOARD_MAINOSC);
		*pll_fracr = div(FRACR_ONE * tmp, FRACR_STEP);
	}
}
#endif

void lvdsc_start(void)
{
	struct pmc_pll_cfg pll_cfg;
	u32 p_mul = 0, p_div = 0, fracr = 0;
	u32 reg;

#if !defined(BOARD_LVDS_PLL_MUL) || \
	!defined(BOARD_LVDS_PLL_DIV) || \
	!defined(BOARD_LVDS_PLL_FRACR)
	pll_config(&p_mul, &p_div, &fracr);
	dbg_info("#define BOARD_LVDS_PLL_MUL %d\n\r",p_mul);
	dbg_info("#define BOARD_LVDS_PLL_DIV %d\n\r",p_div);
	dbg_info("#define BOARD_LVDS_PLL_FRACR %d\n\r",fracr);
#else
	p_mul = BOARD_LVDS_PLL_MUL;
	p_div = BOARD_LVDS_PLL_DIV;
	fracr = BOARD_LVDS_PLL_FRACR;
#endif
	
	/* Configure & Enable LVDSPLL */
	pll_cfg.mul   = p_mul;
	pll_cfg.div   = p_div;
	pll_cfg.count = LVDSPLL_COUNT;
	pll_cfg.fracr = fracr;
	pll_cfg.acr  = AT91C_PLL_ACR_DEFAULT_LVDS;
	pmc_sam9x60_cfg_pll(PLL_ID_LVDS, &pll_cfg);

	lvdsc_writel(LVDSC_CR, 0);
	while (lvdsc_readl(LVDSC_SR) & LVDSC_SR_CS);

	reg = 0;
#ifdef BOARD_LVDS_JEIDA
	reg |= LVDSC_CFGR_MAPPING;
#endif
#ifdef BOARD_LVDS_BALANCED
	ret |= LVDSC_CFGR_DC_BAL;
#endif
#ifdef BOARD_LVDS_DEN_LOW
	ret |= LVDSC_CFGR_LCDC_DEN_POL;
#endif
#ifdef BOARD_LVDS_18BITS
	ret |= LVDSC_CFGR_LCDC_PIXSIZE;
#endif
	lvdsc_writel(LVDSC_CFGR, reg);
	lvdsc_writel(LVDSC_UCBR, 0);
	lvdsc_writel(LVDSC_ACR, LVDSC_ACR_DCBIAS(9));
	lvdsc_writel(LVDSC_CR, LVDSC_CR_SER_EN);
}
