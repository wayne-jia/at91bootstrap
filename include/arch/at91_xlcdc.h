/*******************************************************************************
  XLCDC Peripheral Library

  Company:
    Microchip Technology Inc.

  File Name:
    plib_xlcdc.h

  Summary:
    Contains XLCDC peripheral library function declarations.

  Description:
    This library implements a SUBSET of the register configurations
    for the XLCDC peripheral. 

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

#ifndef _AT91_XLCDC_H
#define _AT91_XLCDC_H


#include "types.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
    extern "C" {
#endif
//DOM-IGNORE-END

/*******************************************************************************
  XLCDC Layer Order

  Summary:
    Enumerates the layers in order.

  Description:
    The order depends on the HEO video priority selection.

  Remarks:
    None.
*/
typedef enum
{
    XLCDC_LAYER_BASE,
    XLCDC_LAYER_OVR1,
    XLCDC_LAYER_HEO,
    XLCDC_LAYER_OVR2,
} XLCDC_LAYER;

/*******************************************************************************
  XLCDC CLUT Color Modes

  Summary:
    Enumerates the supported color modes.

  Description:
    This enumeration lists the available CLUT color modes on XLCDC Layers.

  Remarks:
    Applies to all layers
*/
typedef enum XLCDC_CLUT_COLOR_MODE
{
    XLCDC_CLUT_COLOR_MODE_1BPP,
    XLCDC_CLUT_COLOR_MODE_2BPP,
    XLCDC_CLUT_COLOR_MODE_3BPP,
    XLCDC_CLUT_COLOR_MODE_4BPP,
} XLCDC_CLUT_COLOR_MODE;

/*******************************************************************************
  XLCDC RGB Color Modes

  Summary:
    Enumerates the supported color modes.

  Description:
    This enumeration lists the available RGB color modes on XLCDC Layers.

  Remarks:
    Applies to all layers
*/
typedef enum XLCDC_RGB_COLOR_MODE
{
    XLCDC_RGB_COLOR_MODE_CLUT = -1,
    XLCDC_RGB_COLOR_MODE_RGB_444,
    XLCDC_RGB_COLOR_MODE_ARGB_4444,
    XLCDC_RGB_COLOR_MODE_RGBA_4444,
    XLCDC_RGB_COLOR_MODE_RGB_565,
    XLCDC_RGB_COLOR_MODE_ARGB_1555,
    XLCDC_RGB_COLOR_MODE_RGB_666,
    XLCDC_RGB_COLOR_MODE_RGB_666_PACKED,
    XLCDC_RGB_COLOR_MODE_ARGB_1666,
    XLCDC_RGB_COLOR_MODE_ARGB_1666_PACKED,
    XLCDC_RGB_COLOR_MODE_RGB_888,
    XLCDC_RGB_COLOR_MODE_RGB_888_PACKED,
    XLCDC_RGB_COLOR_MODE_ARGB_1888,
    XLCDC_RGB_COLOR_MODE_ARGB_8888,
    XLCDC_RGB_COLOR_MODE_RGBA_8888,
} XLCDC_RGB_COLOR_MODE;

/*******************************************************************************
  XLCDC YCBCR Color Modes

  Summary:
    Enumerates the supported color modes.

  Description:
    This enumeration lists the available YCbCr modes on the XLCDC HEO Layer.

  Remarks:
    Applies to all layers
*/
typedef enum XLCDC_YCBCR_COLOR_MODE
{
    XLCDC_YCBCR_COLOR_MODE_AYCBCR_444,
    XLCDC_YCBCR_COLOR_MODE_YCBCR_422_0, //Cr(n)Y(n+1)Cb(n)Y(n) 4:2:2
    XLCDC_YCBCR_COLOR_MODE_YCBCR_422_1, //Y(n+1)Cr(n)Y(n)Cb(n) 4:2:2
    XLCDC_YCBCR_COLOR_MODE_YCBCR_422_2, //Cb(n)Y(n+1)Cr(n)Y(n) 4:2:2
    XLCDC_YCBCR_COLOR_MODE_YCBCR_422_3, //Y(n+1)Cb(n)Y(n)Cr(n) 4:2:2
    XLCDC_YCBCR_COLOR_MODE_YCBCR_422_SEMIPLANAR,
    XLCDC_YCBCR_COLOR_MODE_YCBCR_422_PLANAR,
    XLCDC_YCBCR_COLOR_MODE_YCBCR_420_SEMIPLANAR,
    XLCDC_YCBCR_COLOR_MODE_YCBCR_420_PLANAR,
} XLCDC_YCBCR_COLOR_MODE;


///============DRV_XLCDC DEFINE
typedef enum gfxDriverIOCTLRequest
{
    GFX_IOCTL_RESET, // resets the device, arg = NULL
    GFX_IOCTL_GET_COLOR_MODE, // returns the supported color mode for the driver, arg = gfxIOCTLArg_Value
    GFX_IOCTL_GET_BUFFER_COUNT, // returns the driver buffer count, arg = gfxIOCTLArg_Value
    GFX_IOCTL_GET_DISPLAY_SIZE, // returns the driver buffer count, arg = gfxIOCTLArg_DisplaySize
    GFX_IOCTL_GET_STATUS, // returns the driver status. 0 = idle/ready, arg = gfxIOCTLArg_Value
    GFX_IOCTL_ENABLE_GPU, // tells the driver to utilize a GPU if possible, arg = gfxIOCTLArg_Value
    
    GFX_IOCTL_LAYER_SWAP, // indicates that the driver should swap the current layer, arg = NULL
    GFX_IOCTL_FRAME_START, // indicates that the driver should begin a new frame, arg = gfxIOCTLArg_Value
    GFX_IOCTL_FRAME_END, // indicates that the driver should end the current frame, arg = NULL
    GFX_IOCTL_GET_VSYNC_COUNT, // gets the current driver vsync count, arg = gfxIOCTLArg_Value
    GFX_IOCTL_GET_FRAMEBUFFER, // gets a pointer to the internal driver frame buffer, arg = gfxIOCTLArg_Value
    GFX_IOCTL_SET_PALETTE, // sets the current driver palette, arg = gfxIOCTLArg_Palette
    GFX_IOCTL_SET_VBLANK_SYNC, // sets the driver to sync update w/ vblank or not, arg = gfxIOCTLArg_Value	
    
    GFX_IOCTL_GET_LAYER_COUNT, // gets the driver layer count, arg = gfxIOCTLArg_Value
    GFX_IOCTL_GET_ACTIVE_LAYER, // gets the active driver layer, arg = gfxIOCTLArg_Value
    GFX_IOCTL_SET_ACTIVE_LAYER, // sets the active driver layer, arg = gfxIOCTLArg_Value
	
    GFX_IOCTL_SET_BRIGHTNESS, // sets the screen backlight brightness, arg = gfxIOCTLArg_Value

    GFX_IOCTL_SET_LAYER_LOCK, // locks a layer, arg = gfxIOCTLArg_LayerValue
    GFX_IOCTL_GET_LAYER_ENABLED, // get layer enabled, arg = gfxIOCTLArg_LayerValue
    GFX_IOCTL_SET_LAYER_ENABLED, // set layer enabled, arg = gfxIOCTLArg_LayerValue
    GFX_IOCTL_SET_LAYER_UNLOCK, // unlocks a layer, arg = gfxIOCTLArg_LayerValue
    GFX_IOCTL_GET_LAYER_RECT, // get layer rect, arg = gfxIOCTLArg_LayerRect
    GFX_IOCTL_SET_LAYER_POSITION, // set layer position, arg = gfxIOCTLArg_LayerPosition
    GFX_IOCTL_SET_LAYER_SIZE, // sets a layer size, arg = gfxIOCTLArg_LayerSize
    GFX_IOCTL_SET_LAYER_WINDOW_SIZE, // set layer clipped window size, arg = gfxIOCTLArg_LayerWindowSize
    GFX_IOCTL_SET_LAYER_ALPHA, // set layer alpha value, arg = gfxIOCTLArg_LayerValue
    GFX_IOCTL_SET_LAYER_BASE_ADDRESS, // set layer base address, arg = gfxIOCTLArg_LayerValue
    GFX_IOCTL_SET_LAYER_COLOR_MODE, // set layer color mode, arg = gfxIOCTLArg_LayerValue

    GFX_IOCTL_SET_BLIT_CALLBACK, // set a callback for scratch buffer blit completion, arg = gfxIOCTLArg_Value
    GFX_IOCTL_SET_IRQ_CALLBACK, // set a callback for the driver IRQ, arg = gfxIOCTLArg_LayerValue

} gfxDriverIOCTLRequest;

typedef struct
{
    u32 id;
} gfxIOCTLArg_LayerArg;

typedef struct
{
    gfxIOCTLArg_LayerArg base;
    
    union
    {
        u32 v_uint;
        s32 v_int;
        void* v_pointer;
        bool v_bool;
        XLCDC_RGB_COLOR_MODE v_colormode;
    } value;
} gfxIOCTLArg_LayerValue;

typedef enum gfxBitsPerPixel
{
    GFX_BPP1,
    GFX_BPP4,
    GFX_BPP8,
    GFX_BPP16,
    GFX_BPP24,
    GFX_BPP32
} gfxBitsPerPixel;

typedef struct gfxColorModeInfo
{
    u32 size;
    u32 bpp;
    gfxBitsPerPixel bppOrdinal;

    struct
    {
        u32 red;
        u32 green;
        u32 blue;
        u32 alpha;
    } mask;
    
    struct
    {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    } shift;
    
} gfxColorModeInfo;

/* Driver Settings */
#define XLCDC_HOR_RES           720
#define XLCDC_VER_RES           720

typedef void* gfxBuffer;

typedef struct gfxSize
{
    s32 width;
    s32 height;
} gfxSize;

typedef struct gfxPixelBuffer
{
    XLCDC_RGB_COLOR_MODE mode;
    
    gfxSize size;
    u32 pixel_count;
    
    u32 buffer_length;
    gfxBuffer pixels;
} gfxPixelBuffer;

#define FB_BPP_TYPE             u32
#define FB_PTR_TYPE             FB_BPP_TYPE *
#define FB_TYPE_SZ              sizeof(FB_BPP_TYPE)

typedef struct
{
    bool enabled;
    int pixelformat;
    u32 resx;
    u32 resy;
    u32 startx;
    u32 starty;
    u32 sizex;
    u32 sizey;
    u32 xstride;
    u32 pstride;
    u32 alpha;
    FB_PTR_TYPE baseaddr;
    gfxPixelBuffer pixelBuffer;
} LAYER_ATTRIBUTES;

/*******************************************************************************
  Function:
    void XLCDC_EnableClocks(void)

   Summary:
    Enables XLCDC clocks.

   Description:
    Enables and configures Peripheral, Pixel and PWM clocks.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_EnableClocks(void);

/*******************************************************************************
  Function:
    void XLCDC_SetupTimingEngine(void)

   Summary:
    Configures the XLCDC Timing Engine.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_SetupTimingEngine(void);

/*******************************************************************************
  Function:
    void XLCDC_SetupBaseLayer(void)

   Summary:
    Enables the XLCDC Base Layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_SetupBaseLayer(void);

/*******************************************************************************
  Function:
    void XLCDC_SetupOVR1Layer(void)

   Summary:
    Enables the XLCDC OVR1 Layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_SetupOVR1Layer(void);

/*******************************************************************************
  Function:
    void XLCDC_SetupOVR2Layer(void)

   Summary:
    Enables the XLCDC OVR2 Layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_SetupOVR2Layer(void);

/*******************************************************************************
  Function:
    void XLCDC_SetupHEOLayer(void)

   Summary:
    Enables the XLCDC HEO Layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_SetupHEOLayer(void);

/*******************************************************************************
  Function:
    void XLCDC_EnableBacklight(void)

   Summary:
    Enables the XLCDC Backlight.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_EnableBacklight(void);

/*******************************************************************************
  Function:
    void XLCDC_DisableBacklight(void)

   Summary:
    Disables the XLCDC Backlight.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_DisableBacklight(void);

/*******************************************************************************
  Function:
    void XLCDC_SetBacklightBrightness(u8 brightness)

   Summary:
    Sets the XLCDC Backlight Brightness.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    brightness - Brightness in the range of 0 to 255.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_SetBacklightBrightness(u8 brightness);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerEnable(XLCDC_LAYER layer, bool enable, bool update)

   Summary:
    Enable an XLCDC layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    enable - If True, enables layer. If False, disables layer.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    None.
*/
bool XLCDC_SetLayerEnable(XLCDC_LAYER layer, bool enable, bool update);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerOpts(XLCDC_LAYER layer, u8 alpha, bool enable_dma, bool update)

   Summary:
    Sets the alpha value and enables/disables the DMA of a selected layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    alpha - Alpha in the range of 0 to 255.
    enable_dma - If True, enables DMA.
                 If False, disables DMA.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    Base layer does not support alpha, alpha only affects the layer window.
*/
bool XLCDC_SetLayerOpts(XLCDC_LAYER layer, u8 alpha, bool enable_dma, bool update);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerRGBColorMode(XLCDC_LAYER layer, XLCDC_RGB_COLOR_MODE mode, bool update)

   Summary:
    Sets the RGB color mode of the selected layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    mode - Any XLCDC_RGB_COLOR_MODE type.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    Does not effect YUV or CLUT modes.
*/
bool XLCDC_SetLayerRGBColorMode(XLCDC_LAYER layer, XLCDC_RGB_COLOR_MODE mode, bool update);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerWindowXYPos(XLCDC_LAYER layer, u32 xpos, u32 ypos, bool update)

   Summary:
    Sets the Window X, Y Position of the selected layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    xpos - Integer specifying Window X Position.
    ypos - Integer specifying Window Y Position.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    None.
*/
bool XLCDC_SetLayerWindowXYPos(XLCDC_LAYER layer, u32 xpos, u32 ypos, bool update);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerWindowXYSize(XLCDC_LAYER layer, u32 xsize, u32 ysize, bool update)

   Summary:
    Sets the Window X, Y Size of the selected layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    xsize - Integer specifying Window X Size.
    ysize - Integer specifying Window Y Size.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    None.
*/
bool XLCDC_SetLayerWindowXYSize(XLCDC_LAYER layer, u32 xsize, u32 ysize, bool update);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerXStride(XLCDC_LAYER layer, u32 xstride, bool update)

   Summary:
    Sets the row stride of the selected layer.

   Description:
    Specifies row striding offset in bytes.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    xstride - Integer specifying row stride in bytes.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    None.
*/
bool XLCDC_SetLayerXStride(XLCDC_LAYER layer, u32 xstride, bool update);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerPStride(XLCDC_LAYER layer, u32 pstride, bool update)

   Summary:
    Sets the pixel stride of the selected layer.

   Description:
    Specifies pixel striding offset in bytes(between consecutive pixels).

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    pstride - Integer specifying pixel stride in bytes.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    None.
*/
bool XLCDC_SetLayerPStride(XLCDC_LAYER layer, u32 pstride, bool update);

/*******************************************************************************
  Function:
    bool XLCDC_SetLayerAddress(XLCDC_LAYER layer, u32 address, bool update)

   Summary:
    Sets the frame buffer address of the selected layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    address - Integer specifying the frame buffer address in memory.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    Ensure address is 32-byte aligned.
*/
bool XLCDC_SetLayerAddress(XLCDC_LAYER layer, u32 address, bool update);

/*******************************************************************************
  Function:
   bool XLCDC_UpdateLayerAttributes(XLCDC_LAYER layer, bool update);

   Summary:
    Updates the attributes of the selected layer.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    layer - One the four XLCDC layers of type XLCDC_LAYER.
    update - If True, updates attributes immediately.
             If False, update manually using XLCDC_UpdateLayerAttributes(layer).

   Returns:
    0, on success. 1, on failure.

   Remarks:
    None.
*/
bool XLCDC_UpdateLayerAttributes(XLCDC_LAYER layer);

/*******************************************************************************
  Function:
    void XLCDC_MIPIColorModeSignalEnable(bool enable)

   Summary:
    Enable or Disable the MIPI DSI Host's Color Mode On signal.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    enable - If True, enables Color Mode On signal. 
             If False, disables Color Mode On signal.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_MIPIColorModeSignalEnable(bool enable);

/*******************************************************************************
  Function:
    void XLCDC_Stop(void)

   Summary:
    Stops the XLCDC peripheral.

   Description:
    Disables the XLCDC digital blocks.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_Stop(void);

/*******************************************************************************
  Function:
    void XLCDC_Start(void)

   Summary:
    Starts the XLCDC peripheral.

   Description:
    Enables the XLCDC digital blocks.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_Start(void);

/*******************************************************************************
  Function:
    void XLCDC_Initialize(void)

   Summary:
    Initializes the XLCDC peripheral.

   Description:
    None.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    None.

   Remarks:
    None.
*/
void XLCDC_Initialize(void);



void xlcdc_init(void);
void xlcdc_display(void);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
    }
#endif
// DOM-IGNORE-END

#endif
