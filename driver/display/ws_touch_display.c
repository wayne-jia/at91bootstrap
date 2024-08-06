/*******************************************************************************
  MPLAB Harmony Generated Display Driver Async Implementation File

  File Name:
    ws_touch_display.c

  Summary:
    Implements display and touch driver support for the
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

#include "ws_touch_display.h"
#include "timer.h"
#include "twi.h"
#include "string.h"
#include "debug.h"

/* Driver Data */
DISP_DATA disp_data;

/* Display Initialization Sequence */
#define DISP_INIT_BYTES 4
static u8 initSeq[DISP_INIT_BYTES][2] = {
    {0xC0, 0x1},
    {0xC2, 0x1},
    {0xAC, 0x1},
    {0xAD, 0x1},
};

/* Display Brightness Sequence */
#define DISP_BRT_BYTES 2
static u8 brightSeq[DISP_BRT_BYTES][2] = {
    {0xAB, 55},
    {0xAA, 0x1},
};

/* Delay Wrappers */
/* Wait and go to next state */
static inline void DISP_DelayNextState(u32 timeMillis, DISP_STATES stateNext)
{
    mdelay(timeMillis);
    disp_data.state = stateNext;
}

/* Transfer Wrappers */
/* Display Register Write */
static int DISP_WriteRegister(u16 reg, u8 val)
{
    disp_data.txBuffer[0] = reg & 0xFF;
    disp_data.txBuffer[1] = val;

    if (twi_write(0, disp_data.i2cAddress, disp_data.txBuffer[0], 1, &disp_data.txBuffer[1], 1)) {
        dbg_info("ERROR: i2c write ws panel\n\r");
        return -1;
    }

    return 0;
}

/* Driver Interface Functions */
bool DISP_WS_SetBrightness(u8 brightness)
{
    if (disp_data.updateBrightness)
    {
        /* Previous Update Pending */
        return 1;
    }
    else
    {
        /* Update Brightness */
        brightSeq[0][1] = 0xFF - brightness;
        disp_data.updateBrightness = true;
        return 0;
    }
}

void DISP_WS_Initialize(void)
{
    memset(&disp_data, 0, sizeof(disp_data));

    disp_data.state = DISP_STATE_INIT;
    disp_data.i2cAddress = 0x45;
}

void DISP_WS_PowerOn(void)
{
    disp_data.state = DISP_STATE_POWER_ON;
    while (disp_data.state == DISP_STATE_POWER_ON)
    {
        DISP_WS_Update();
    }
}

void DISP_WS_Brightness(void)
{
    disp_data.state = DISP_STATE_SET_BRIGHTNESS;
    while (disp_data.state == DISP_STATE_SET_BRIGHTNESS)
    {
        DISP_WS_Update();
    }
}

void DISP_WS_Update(void)
{
    
    switch(disp_data.state)
    {
        case DISP_STATE_INIT:
        {
            break;
        }
        case DISP_STATE_POWER_ON:
        {
            if (disp_data.count < DISP_INIT_BYTES)
            {
                DISP_WriteRegister(initSeq[disp_data.count][0],
                                    initSeq[disp_data.count][1]);
                /* Increment count */
                disp_data.count++;

                /* Delay next transaction */
                DISP_DelayNextState(1, DISP_STATE_POWER_ON);
            }
            else
            {
                /* Reset count */
                disp_data.count = 0;

                /* Set DSI Video Mode */
                DSI_VideoMode();

                /* Delay Next State */
                DISP_DelayNextState(1, DISP_STATE_IDLE);
            }
            break;
        }
        case DISP_STATE_SET_BRIGHTNESS:
        {
            /* Iterate over the sequence */
            if (disp_data.count < DISP_BRT_BYTES)
            {
                DISP_WriteRegister(brightSeq[disp_data.count][0],
                                    brightSeq[disp_data.count][1]);
                /* Increment count */
                disp_data.count++;

                /* Delay next transaction */
                DISP_DelayNextState(1, DISP_STATE_SET_BRIGHTNESS);
            }
            else
            {
                /* Reset count */
                disp_data.count = 0;
                /* Update flag */
                disp_data.updateBrightness = false;
                /* Next State */
                disp_data.state = DISP_STATE_IDLE;
            }
            break;
        }
        case DISP_STATE_IDLE:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}
