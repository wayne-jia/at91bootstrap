/*
 * Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __DEV_SAM9X7_H__
#define __DEV_SAM9X7_H__

/*
 * PMC Settings
 *
 * The main oscillator is enabled as soon as possible in the lowlevel_clock_init
 * and MCK is switched on the main oscillator.
 * PLL initialization is done later in the hw_init() function
 */
#define MASTER_CLOCK	266666666U

#define BAUD_RATE	115200
#define BOARD_MAINOSC	24000000

#define BOARD_PRESCALER_MAIN_CLOCK	(AT91C_PMC_PRES_CLK | \
					AT91C_PMC_MDIV_1 | \
					AT91C_PMC_CSS_MAIN_CLK)

#define BOARD_PRESCALER_PLLA	(AT91C_PMC_PRES_CLK | \
				AT91C_PMC_MDIV_3 | \
				AT91C_PMC_CSS_PLLA_CLK)

/*
 * DataFlash Settings
 */
#define CONFIG_SYS_SPI_CLOCK    AT91C_SPI_CLK
#define CONFIG_SYS_SPI_MODE     SPI_MODE0

#if defined(CONFIG_SPI_BUS0)
#define CONFIG_SYS_BASE_SPI     AT91C_BASE_SPI0
#elif defined(CONFIG_SPI_BUS1)
#define CONFIG_SYS_BASE_SPI     AT91C_BASE_SPI1
#endif

#if defined(CONFIG_AT91_QSPI_OCTAL)
#define CONFIG_SYS_BASE_QSPI		AT91C_BASE_QSPI0
#define CONFIG_SYS_BASE_QSPI_MEM	AT91C_BASE_QSPI0_MEM
#define CONFIG_SYS_QSPI_MEM_SIZE	AT91C_QSPI0_MEM_SIZE
#define CONFIG_SYS_ID_QSPI		AT91C_ID_QSPI0
#endif

/*
 * NandFlash Settings
 */
#define CONFIG_SYS_NAND_BASE            AT91C_BASE_CS2
#define CONFIG_SYS_NAND_MASK_ALE        (1 << 21)
#define CONFIG_SYS_NAND_MASK_CLE        (1 << 22)

#define CONFIG_SYS_NAND_OE_PIN		AT91C_PIN_PD(0)
#define CONFIG_SYS_NAND_WE_PIN		AT91C_PIN_PD(1)
#define CONFIG_SYS_NAND_ALE_PIN		AT91C_PIN_PD(2)
#define CONFIG_SYS_NAND_CLE_PIN		AT91C_PIN_PD(3)
#define CONFIG_SYS_NAND_ENABLE_PIN      AT91C_PIN_PD(4)

#define CONFIG_LOOKUP_TABLE_ALPHA_OFFSET	0xC000
#define CONFIG_LOOKUP_TABLE_INDEX_OFFSET	0x8000

#define CONFIG_LOOKUP_TABLE_ALPHA_OFFSET_1024	0x18000
#define CONFIG_LOOKUP_TABLE_INDEX_OFFSET_1024	0x10000

/*
 * SDHC Settings
 */
#ifdef CONFIG_SDHC0
#define CONFIG_SYS_BASE_SDHC	AT91C_BASE_SDHC0
#define CONFIG_SYS_ID_SDHC	AT91C_ID_SDMMC0
#endif

#ifdef CONFIG_SDHC1
#define CONFIG_SYS_BASE_SDHC	AT91C_BASE_SDHC1
#define CONFIG_SYS_ID_SDHC	AT91C_ID_SDMMC1
#endif

/*
 * XDMAC Settings
 */
#ifdef CONFIG_XDMAC
#define CONFIG_SYS_BASE_XDMAC	AT91C_BASE_XDMAC
#define CONFIG_SYS_ID_XDMAC	AT91C_ID_XDMAC
#endif

/*
 * XLCDC Settings
 */
#ifdef CONFIG_XLCDC
#define CONFIG_SYS_BASE_XLCDC	AT91C_BASE_XLCD
#define CONFIG_SYS_ID_XLCDC	AT91C_ID_XLCDC

#define BOARD_LCD_WIDTH		800
#define BOARD_LCD_HEIGHT	480
#define BOARD_LCD_FRAMERATE	60

#define BOARD_LCD_TIMING_HFP	88
#define BOARD_LCD_TIMING_HPW	5
#define BOARD_LCD_TIMING_HBP	40
#define BOARD_LCD_TIMING_VFP	23
#define BOARD_LCD_TIMING_VPW	5
#define BOARD_LCD_TIMING_VBP	1

#define BOARD_LCD_PIXEL_CLOCK \
((BOARD_LCD_WIDTH + BOARD_LCD_TIMING_HFP + BOARD_LCD_TIMING_HPW + BOARD_LCD_TIMING_HBP) * \
(BOARD_LCD_HEIGHT + BOARD_LCD_TIMING_VFP + BOARD_LCD_TIMING_VPW + BOARD_LCD_TIMING_VBP) * \
BOARD_LCD_FRAMERATE)

/* LCD module control pins */
#define BOARD_LCD_PIN_RST	AT91C_PIN_PC(25)
#define BOARD_LCD_PIN_EN	AT91C_PIN_PA(14)
#define BOARD_LCD_PIN_DISP	AT91C_PIN_PA(30)
#define BOARD_LCD_PIN_BL	AT91C_PIN_PC(21)

#ifdef CONFIG_LVDSC
#define CONFIG_SYS_BASE_LVDSC	AT91C_BASE_LVDSC
#define CONFIG_SYS_ID_LVDSC	AT91C_ID_LVDSC

#define BOARD_LVDS_PLL_MUL 23
#define BOARD_LVDS_PLL_DIV 2
#define BOARD_LVDS_PLL_FRACR 3899392

#define BOARD_LVDS_JEIDA
#endif

#endif

#endif /* __DEV_SAM9X7_H__ */
