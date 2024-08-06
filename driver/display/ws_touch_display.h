/*******************************************************************************
  MPLAB Harmony Generated Display Driver Header File

  File Name:
    ws_touch_display.h

  Summary:
    Build-time generated header file that
    adds display and touch driver support for the
    Waveshare 4inch DSI Round LCD Touch Display.
    https://www.waveshare.com/4inch-dsi-lcd-c.htm

    Created with MPLAB Harmony Version 3.0
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END
#ifndef _WS_TOUCH_DISPLAY_H
#define _WS_TOUCH_DISPLAY_H

#include "arch/at91_dsi.h"

/* Buffer Size */
#define DISP_RX_BUF_SIZE    186
#define DISP_TX_BUF_SIZE    4

#ifdef __cplusplus
    extern "C" {
#endif

/*******************************************************************************
  Display Driver State

  Summary:
    Holds application data.
*/
typedef enum
{
    /* Initial state. */
    DISP_STATE_INIT,

    /* Power on display */
    DISP_STATE_POWER_ON,

    /* Set Display Brightness */
    DISP_STATE_SET_BRIGHTNESS,

    DISP_STATE_IDLE
} DISP_STATES;

/*******************************************************************************
  Display Driver Data

  Summary:
    Holds application data.
*/
typedef struct
{
    /* Current state */
    DISP_STATES  state;

    /* I2C driver client address */
    u8 i2cAddress;

    /* I2C Receive Buffer */
    u8 rxBuffer[DISP_RX_BUF_SIZE];

    /* I2C Transmit Buffer */
    u8 txBuffer[DISP_TX_BUF_SIZE];

    /* Update display brightness */
    bool updateBrightness;

    /* General purpose count */
    u32 count;
} DISP_DATA;

/*******************************************************************************
  Function:
    void DISP_WS_Initialize(void)

  Summary:
    Initializes the display.
*/
void DISP_WS_Initialize(void);

/*******************************************************************************
  Function:
    void DISP_WS_Update(void)

  Summary:
    Polls for touch events and processes it.
*/
void DISP_WS_Update(void);

/*******************************************************************************
  Function:
    bool DISP_WS_SetBrightness(u8 brightness)

  Summary:
    Sets the display brightness.

  Parameters:
    brightness - 8-bit value from 0 (off) to 255 (brightest).

  Returns:
    0, on success. 1, on failure.
*/
bool DISP_WS_SetBrightness(u8 brightness);

/*******************************************************************************
  Function:
    void DISP_WS_PowerOn(void)

  Summary:
    Power on the display, init the display sequence
*/
void DISP_WS_PowerOn(void);

/*******************************************************************************
  Function:
    void DISP_WS_Brightness(void)

  Summary:
    Set the backlight to MAX in initialization
*/
void DISP_WS_Brightness(void);

#ifdef __cplusplus
    }
#endif

#endif
