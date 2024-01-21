/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2023  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.36 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              STMicroelectronics International NV, 39, Chemin du Champ-des Filles, 1228 Plan Les Ouates, Geneva, SWITZERLAND
Licensed SEGGER software: emWin
License number:           GUI-00429
License model:            Buyout SRC [Buyout Source Code License, signed November 29th 2012]
Licensed product:         -
Licensed platform:        STMs ARM Cortex-M based 32 BIT CPUs
Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2012-12-07 - 2023-12-31
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include "RTOS.h"
#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "PIDConf.h"
#include "RZ_A1H_TFT_Demonstrator.h"

#include "r_typedefs.h"
#include "r_vdc5.h"
#include "r_vdc5_user.h"
#include "iodefine.h"
#include "devdrv_intc.h"
#include "dev_drv.h"
#include "graphics.h"
#include "graphics_drv_wrapper.h"

#define RGB565   0
#define RGB888   1
#define ARGB1555 2
#define ARGB4444 3
#define ARGB8888 4
#define CLUT8    5

#define GESTURE_DEMO 0
#define WEATHER_DEMO 1
#define TEST         2


#if   (LCD_VDC5_CH0_PANEL == LCD_TM035KVHG01)
  #ifdef TEST_CONF
    #define DEMO TEST          // Configure for test application
  #else
    #define DEMO GESTURE_DEMO  // Configure for demo application
  #endif
#elif (LCD_VDC5_CH0_PANEL == LCD_TM043NVHG01)
  #ifdef TEST_CONF
    #define DEMO TEST          // Configure for test application
  #else
    #define DEMO GESTURE_DEMO  // Configure for demo application
  #endif
#elif (LCD_VDC5_CH0_PANEL == LCD_TM050RVHG02)
  #ifdef TEST_CONF
    #define DEMO TEST          // Configure for test application
  #else
    #define DEMO WEATHER_DEMO  // Configure for demo application
  #endif
#endif

/*********************************************************************
*
*       emWin layer configuration
*
**********************************************************************
*/
#if (DEMO == WEATHER_DEMO)

/*********************************************************************
*
*       Number of available layers
*/
#define NUM_LAYERS 3

/*********************************************************************
*
*       Layer 0
*/
//
// Layer size
//
#if (LCD_VDC5_CH0_PANEL == LCD_TM035KVHG01)
  #define XSIZE_PHYS_0 320
  #define YSIZE_PHYS_0 240
  #define LINE_INDEX_INTERRUPT 240
#elif (LCD_VDC5_CH0_PANEL == LCD_TM043NVHG01)
  #define XSIZE_PHYS_0 480
  #define YSIZE_PHYS_0 272
  #define LINE_INDEX_INTERRUPT 272
#elif (LCD_VDC5_CH0_PANEL == LCD_TM050RVHG02)
  #define XSIZE_PHYS_0 800
  #define YSIZE_PHYS_0 480
  #define LINE_INDEX_INTERRUPT 480
#endif
//
// Layer position
//
#define XPOS_0 0
#define YPOS_0 0
//
// Color conversion
//
#define GRAPHICS_FORMAT_0 RGB565
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_0 3
//
// Frame buffer start address
//
#define VRAM_ADDR_0 (void *)((U32)_apFrameBuffer[0]) // Cached area _C_B

/*********************************************************************
*
*       Layer 1
*/
//
// Layer size
//
#define XSIZE_PHYS_1 XSIZE_PHYS_0
#define YSIZE_PHYS_1 120
//
// Layer position
//
#define XPOS_1 (0)
#define YPOS_1 (360)
//
// Color conversion
//
#define GRAPHICS_FORMAT_1 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_1 1
//
// Frame buffer start address
//
#define VRAM_ADDR_1 (void *)((U32)_apFrameBuffer[1]) // Cached area _C_B

/*********************************************************************
*
*       Layer 2
*/
//
// Layer size
//
#define XSIZE_PHYS_2 400
#define YSIZE_PHYS_2 160
//
// Layer position
//
#define XPOS_2 (0)
#define YPOS_2 (0)
//
// Color conversion
//
#define GRAPHICS_FORMAT_2 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_2 1
//
// Frame buffer start address
//
#define VRAM_ADDR_2 (void *)((U32)_apFrameBuffer[2]) // Cached area _C_B

#elif (DEMO == GESTURE_DEMO)

/*********************************************************************
*
*       layer configuration for Gesture demo
*
**********************************************************************
*/

/*********************************************************************
*
*       Number of available layers
*/
#define NUM_LAYERS 3

/*********************************************************************
*
*       Layer 0
*/
//
// Layer size
//
#if (LCD_VDC5_CH0_PANEL == LCD_TM035KVHG01)
  #define XSIZE_PHYS_0 320
  #define YSIZE_PHYS_0 240
  #define LINE_INDEX_INTERRUPT 240
#elif (LCD_VDC5_CH0_PANEL == LCD_TM043NVHG01)
  #define XSIZE_PHYS_0 480
  #define YSIZE_PHYS_0 272
  #define LINE_INDEX_INTERRUPT 272
#elif (LCD_VDC5_CH0_PANEL == LCD_TM050RVHG02)
  #define XSIZE_PHYS_0 800
  #define YSIZE_PHYS_0 480
  #define LINE_INDEX_INTERRUPT 480
#endif
//
// Layer position
//
#define XPOS_0 0
#define YPOS_0 0
//
// Color conversion
//
#define GRAPHICS_FORMAT_0 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_0 3
//
// Frame buffer start address
//
#define VRAM_ADDR_0 (void *)((U32)_apFrameBuffer[0]) // Cached area _C_B

/*********************************************************************
*
*       Layer 1
*/
//
// Layer size
//
#define XSIZE_PHYS_1 XSIZE_PHYS_0
#define YSIZE_PHYS_1 YSIZE_PHYS_0
//
// Layer position
//
#define XPOS_1 (0)
#define YPOS_1 (0)
//
// Color conversion
//
#define GRAPHICS_FORMAT_1 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_1 3
//
// Frame buffer start address
//
#define VRAM_ADDR_1 (void *)((U32)_apFrameBuffer[1]) // Cached area _C_B

/*********************************************************************
*
*       Layer 2
*/
//
// Layer size
//
#define XSIZE_PHYS_2 XSIZE_PHYS_0
#define YSIZE_PHYS_2 YSIZE_PHYS_0
//
// Layer position
//
#define XPOS_2 (0)
#define YPOS_2 (0)
//
// Color conversion
//
#define GRAPHICS_FORMAT_2 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_2 3
//
// Frame buffer start address
//
#define VRAM_ADDR_2 (void *)((U32)_apFrameBuffer[2]) // Cached area _C_B

#elif (DEMO == TEST)

/*********************************************************************
*
*       layer configuration for Gesture demo
*
**********************************************************************
*/

/*********************************************************************
*
*       Number of available layers
*/
#define NUM_LAYERS 1

/*********************************************************************
*
*       Layer 0
*/
//
// Layer size
//
#if (LCD_VDC5_CH0_PANEL == LCD_TM035KVHG01)
  #define XSIZE_PHYS_0 320
  #define YSIZE_PHYS_0 240
  #define LINE_INDEX_INTERRUPT 240
#elif (LCD_VDC5_CH0_PANEL == LCD_TM043NVHG01)
  #define XSIZE_PHYS_0 480
  #define YSIZE_PHYS_0 272
  #define LINE_INDEX_INTERRUPT 272
#elif (LCD_VDC5_CH0_PANEL == LCD_TM050RVHG02)
  #define XSIZE_PHYS_0 800
  #define YSIZE_PHYS_0 480
  #define LINE_INDEX_INTERRUPT 480
#endif
//
// Layer position
//
#define XPOS_0 0
#define YPOS_0 0
//
// Color conversion
//
#define GRAPHICS_FORMAT_0 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_0 3
//
// Frame buffer start address
//
#define VRAM_ADDR_0 (void *)((U32)_apFrameBuffer[0]) // Cached area _C_B

/*********************************************************************
*
*       Layer 1
*/
//
// Layer size
//
#define XSIZE_PHYS_1 XSIZE_PHYS_0
#define YSIZE_PHYS_1 YSIZE_PHYS_0
//
// Layer position
//
#define XPOS_1 (0)
#define YPOS_1 (0)
//
// Color conversion
//
#define GRAPHICS_FORMAT_1 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_1 3
//
// Frame buffer start address
//
#define VRAM_ADDR_1 (void *)((U32)_apFrameBuffer[1]) // Cached area _C_B

/*********************************************************************
*
*       Layer 2
*/
//
// Layer size
//
#define XSIZE_PHYS_2 XSIZE_PHYS_0
#define YSIZE_PHYS_2 YSIZE_PHYS_0
//
// Layer position
//
#define XPOS_2 (0)
#define YPOS_2 (0)
//
// Color conversion
//
#define GRAPHICS_FORMAT_2 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_2 3
//
// Frame buffer start address
//
#define VRAM_ADDR_2 (void *)((U32)_apFrameBuffer[2]) // Cached area _C_B

#else  // DEMO == TEST

/*********************************************************************
*
*       Number of available layers
*/
#define NUM_LAYERS 1

/*********************************************************************
*
*       Layer 0
*/
//
// Layer size
//
#if (LCD_VDC5_CH0_PANEL == LCD_TM035KVHG01)
  #define XSIZE_PHYS_0 320
  #define YSIZE_PHYS_0 240
  #define LINE_INDEX_INTERRUPT 240
#elif (LCD_VDC5_CH0_PANEL == LCD_TM043NVHG01)
  #define XSIZE_PHYS_0 480
  #define YSIZE_PHYS_0 272
  #define LINE_INDEX_INTERRUPT 272
#elif (LCD_VDC5_CH0_PANEL == LCD_TM050RVHG02)
  #define XSIZE_PHYS_0 800
  #define YSIZE_PHYS_0 480
  #define LINE_INDEX_INTERRUPT 480
#endif
//
// Layer position
//
#define XPOS_0 0
#define YPOS_0 0
//
// Color conversion
//
#define GRAPHICS_FORMAT_0 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_0 3
//
// Frame buffer start address
//
#define VRAM_ADDR_0 (void *)((U32)_apFrameBuffer[0]) // Cached area _C_B

/*********************************************************************
*
*       Layer 1
*/
//
// Layer size
//
#define XSIZE_PHYS_1 XSIZE_PHYS_0
#define YSIZE_PHYS_1 YSIZE_PHYS_0
// Layer position
//
#define XPOS_1 (0)
#define YPOS_1 (0)
//
// Color conversion
//
#define GRAPHICS_FORMAT_1 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_1 1
//
// Frame buffer start address
//
#define VRAM_ADDR_1 (void *)((U32)_apFrameBuffer[1]) // Cached area _C_B

/*********************************************************************
*
*       Layer 2
*/
//
// Layer size
//
#define XSIZE_PHYS_2 XSIZE_PHYS_0
#define YSIZE_PHYS_2 YSIZE_PHYS_0
//
// Layer position
//
#define XPOS_2 (0)
#define YPOS_2 (0)
//
// Color conversion
//
#define GRAPHICS_FORMAT_2 ARGB8888
//
// Number of available buffers used for MultiBuffering
//
#define NUM_BUFFERS_2 1
//
// Frame buffer start address
//
#define VRAM_ADDR_2 (void *)((U32)_apFrameBuffer[2]) // Cached area _C_B

#endif

/*********************************************************************
*
*       Internal configuration dependent macros
*
**********************************************************************
*/
//
// Graphics format and display driver dependent on emWin color format and orientation
//
#if   (GRAPHICS_FORMAT_0 == RGB565)
  #define LCD_BITSPERPIXEL_0 16
  #define COLOR_CONVERSION_0 GUICC_M565
#elif (GRAPHICS_FORMAT_0 == ARGB1555)
  #define LCD_BITSPERPIXEL_0 16
  #define COLOR_CONVERSION_0 GUICC_M1555I
#elif (GRAPHICS_FORMAT_0 == ARGB4444)
  #define LCD_BITSPERPIXEL_0 16
  #define COLOR_CONVERSION_0 GUICC_M4444I
#elif (GRAPHICS_FORMAT_0 == RGB888)
  #define LCD_BITSPERPIXEL_0 32
  #define COLOR_CONVERSION_0 GUICC_M888
#elif (GRAPHICS_FORMAT_0 == ARGB8888)
  #define LCD_BITSPERPIXEL_0 32
  #define COLOR_CONVERSION_0 GUICC_M8888I
#endif
#if   (LCD_BITSPERPIXEL_0 == 16)
  #define DISPLAY_DRIVER_0 GUIDRV_LIN_16
#elif (LCD_BITSPERPIXEL_0 == 32)
  #define DISPLAY_DRIVER_0 GUIDRV_LIN_32
#endif

#if NUM_LAYERS > 1

#if   (GRAPHICS_FORMAT_1 == RGB565)
  #define LCD_BITSPERPIXEL_1 16
  #define COLOR_CONVERSION_1 GUICC_M565
#elif (GRAPHICS_FORMAT_1 == ARGB1555)
  #define LCD_BITSPERPIXEL_1 16
  #define COLOR_CONVERSION_1 GUICC_M1555I
#elif (GRAPHICS_FORMAT_1 == ARGB4444)
  #define LCD_BITSPERPIXEL_1 16
  #define COLOR_CONVERSION_1 GUICC_M4444I
#elif (GRAPHICS_FORMAT_1 == RGB888)
  #define LCD_BITSPERPIXEL_1 32
  #define COLOR_CONVERSION_1 GUICC_M888
#elif (GRAPHICS_FORMAT_1 == ARGB8888)
  #define LCD_BITSPERPIXEL_1 32
  #define COLOR_CONVERSION_1 GUICC_M8888I
#endif
#if   (LCD_BITSPERPIXEL_1 == 16)
  #define DISPLAY_DRIVER_1 GUIDRV_LIN_16
#elif (LCD_BITSPERPIXEL_1 == 32)
  #define DISPLAY_DRIVER_1 GUIDRV_LIN_32
#endif

#if NUM_LAYERS > 2

#if   (GRAPHICS_FORMAT_2 == RGB565)
  #define LCD_BITSPERPIXEL_2 16
  #define COLOR_CONVERSION_2 GUICC_M565
#elif (GRAPHICS_FORMAT_2 == ARGB1555)
  #define LCD_BITSPERPIXEL_2 16
  #define COLOR_CONVERSION_2 GUICC_M1555I
#elif (GRAPHICS_FORMAT_2 == ARGB4444)
  #define LCD_BITSPERPIXEL_2 16
  #define COLOR_CONVERSION_2 GUICC_M4444I
#elif (GRAPHICS_FORMAT_2 == RGB888)
  #define LCD_BITSPERPIXEL_2 32
  #define COLOR_CONVERSION_2 GUICC_M888
#elif (GRAPHICS_FORMAT_2 == ARGB8888)
  #define LCD_BITSPERPIXEL_2 32
  #define COLOR_CONVERSION_2 GUICC_M8888I
#endif
#if   (LCD_BITSPERPIXEL_2 == 16)
  #define DISPLAY_DRIVER_2 GUIDRV_LIN_16
#elif (LCD_BITSPERPIXEL_2 == 32)
  #define DISPLAY_DRIVER_2 GUIDRV_LIN_32
#endif

#endif

#endif

//
// Frame buffer stride: Frame buffer stride should be set to a multiple of 32 or 128
// in accordance with the frame buffer burst transfer mode.
//
#define FRAME_BUFFER_STRIDE_0 (((XSIZE_PHYS_0 * (LCD_BITSPERPIXEL_0 / 8)) + 31u) & ~31u)
#define FRAME_BUFFER_HEIGHT_0 (YSIZE_PHYS_0)
#if NUM_LAYERS > 1
  #define FRAME_BUFFER_STRIDE_1 (((XSIZE_PHYS_1 * (LCD_BITSPERPIXEL_1 / 8)) + 31u) & ~31u)
  #define FRAME_BUFFER_HEIGHT_1 (YSIZE_PHYS_1)
  #if NUM_LAYERS > 2
    #define FRAME_BUFFER_STRIDE_2 (((XSIZE_PHYS_2 * (LCD_BITSPERPIXEL_2 / 8)) + 31u) & ~31u)
    #define FRAME_BUFFER_HEIGHT_2 (YSIZE_PHYS_2)
  #endif
#endif

//
// Video buffer address offset into uncached area
//
#define VRAM_UNCACHED_ADDR_OFFS (0x40000000L)
//
// Relocation offset
//
#define RAM_RELOCATION_OFFS (0x00000000L)
//
// Channel selection: VDC5_CHANNEL_0 or VDC5_CHANNEL_1
//
#define GRAPHICS_CHANNEL (VDC5_CHANNEL_0)
//
// Video buffer address offset into uncached area
//
#define VRAM_UNCACHED_ADDR_OFFS (0x40000000L)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32   _aFrameBuffer0[((NUM_BUFFERS_0 * XSIZE_PHYS_0 * YSIZE_PHYS_0 * (LCD_BITSPERPIXEL_0 / 8)) / 4) + 8]; // With additional bytes to be able to align 32 byte wise
#if NUM_LAYERS > 1
  static U32   _aFrameBuffer1[((NUM_BUFFERS_1 * XSIZE_PHYS_1 * YSIZE_PHYS_1 * (LCD_BITSPERPIXEL_1 / 8)) / 4) + 8]; // With additional bytes to be able to align 32 byte wise
  #if NUM_LAYERS > 2
    static U32   _aFrameBuffer2[((NUM_BUFFERS_2 * XSIZE_PHYS_2 * YSIZE_PHYS_2 * (LCD_BITSPERPIXEL_2 / 8)) / 4) + 8]; // With additional bytes to be able to align 32 byte wise
  #endif
#endif

static U32 * _apFrameBuffer[NUM_LAYERS];       // 32 byte aligned buffer address (initialized in LCD_X_Config())
static int   _aPendingBuffer[NUM_LAYERS] = {
  -1,
#if NUM_LAYERS > 1
  -1,
#if NUM_LAYERS > 2
  -1
#endif
#endif
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _SetBaseAddress
*/
static void _SetBaseAddress(int LayerID, U32 Addr) {
  vdc5_read_chg_t read_chg = {0};
  vdc5_error_t    error;

  read_chg.gr_base = (void *)Addr;
  error = R_VDC5_ChangeReadProcess(GRAPHICS_CHANNEL, LayerID, &read_chg);
  if (error != VDC5_OK) {
    while (1); // Never reach...
  }
}

/*********************************************************************
*
*       _ISR_VSyncCallback
*/
static void _ISR_VSyncCallback(void) {
  U32 Addr;

  if (_aPendingBuffer[0] >= 0) {
    Addr = ((U32)(VRAM_ADDR_0) + XSIZE_PHYS_0 * YSIZE_PHYS_0 * _aPendingBuffer[0] * (LCD_BITSPERPIXEL_0 >> 3));
    _SetBaseAddress(VDC5_LAYER_ID_0_RD, Addr);
    GUI_MULTIBUF_ConfirmEx(0, _aPendingBuffer[0]);
    _aPendingBuffer[0] = -1;
  }
#if NUM_LAYERS > 1
  if (_aPendingBuffer[1] >= 0) {
    Addr = ((U32)(VRAM_ADDR_1) + XSIZE_PHYS_1 * YSIZE_PHYS_1 * _aPendingBuffer[1] * (LCD_BITSPERPIXEL_1 >> 3));
    _SetBaseAddress(VDC5_LAYER_ID_2_RD, Addr);
    GUI_MULTIBUF_ConfirmEx(1, _aPendingBuffer[1]);
    _aPendingBuffer[1] = -1;
  }
#if NUM_LAYERS > 2
  if (_aPendingBuffer[2] >= 0) {
    Addr = ((U32)(VRAM_ADDR_2) + XSIZE_PHYS_2 * YSIZE_PHYS_2 * _aPendingBuffer[2] * (LCD_BITSPERPIXEL_2 >> 3));
    _SetBaseAddress(VDC5_LAYER_ID_3_RD, Addr);
    GUI_MULTIBUF_ConfirmEx(2, _aPendingBuffer[2]);
    _aPendingBuffer[2] = -1;
  }
#endif
#endif
}

/*********************************************************************
*
*       _ISR_VSync
*/
static void _ISR_VSync(void) {
  U32 IntState;

  IntState = VDC50.SYSCNT_INT4;              // Get the interrupt state
  if (((IntState & 0x1000) != 0u) && ((VDC50.SYSCNT_INT1 & 0x1000) != 0u)) {
    VDC50.SYSCNT_INT1 = IntState & ~0x1000;  // Disable interrupt generation
    _ISR_VSyncCallback();                    // Call function for handling multi buffering
    VDC50.SYSCNT_INT1 = IntState;            // Enable interrupt generation
  }
}

/*********************************************************************
*
*       _InitController
*/
static int _InitController(void) {
  vdc5_error_t         error = {0};
  vdc5_channel_t       ch = {0};
  vdc5_layer_id_t      layer_id = {0};
  vdc5_graphics_type_t graphics_type = {0};
  vdc5_gr_format_t     gr_format = {0};
  vdc5_period_rect_t   period_rect = {0}; /*! Display area */
  vdc5_gr_disp_sel_t   gr_disp_sel[VDC5_GR_TYPE_NUM] = {0};
  unsigned int         index;

  //
  // Channel selection
  //
  ch = GRAPHICS_CHANNEL;
  //
  // Initialize parameters
  //
  for (index = 0; index < (uint32_t)VDC5_GR_TYPE_NUM; index++){
    gr_disp_sel[index] = VDC5_DISPSEL_IGNORED;
  }
  //
  // Initialization of driver
  //
  error = GrpDrv_Init(ch, VDC5_OFF, VDC5_OFF);
  if (error != VDC5_OK) {
    return 1;
  }
  //
  // Create and start graphics surface 0
  //
  layer_id        = VDC5_LAYER_ID_0_RD;
  graphics_type   = VDC5_GR_TYPE_GR0;
  gr_format       = GRAPHICS_FORMAT_0;
  switch (ch) {
  case VDC5_CHANNEL_0:
    period_rect.vs  = (uint16_t)YPOS_0 + LCD_CH0_DISP_VS;
    period_rect.vw  = (uint16_t)YSIZE_PHYS_0;
    period_rect.hs  = (uint16_t)XPOS_0 + LCD_CH0_DISP_HS;
    period_rect.hw  = (uint16_t)XSIZE_PHYS_0;
    break;
  case VDC5_CHANNEL_1:
    period_rect.vs  = (uint16_t)YPOS_0 + LCD_CH1_DISP_VS;
    period_rect.vw  = (uint16_t)YSIZE_PHYS_0;
    period_rect.hs  = (uint16_t)XPOS_0 + LCD_CH1_DISP_HS;
    period_rect.hw  = (uint16_t)XSIZE_PHYS_0;
    break;
  default:
    return 1;
  }
  error = GrpDrv_GraphicsCreateSurface(ch, layer_id, VRAM_ADDR_0, FRAME_BUFFER_STRIDE_0, gr_format, &period_rect);
  if (error != VDC5_OK) {
    return 1;
  }
  gr_disp_sel[graphics_type] = VDC5_DISPSEL_CURRENT;
#if NUM_LAYERS > 1
  //
  // Create and start graphics surface 0
  //
  layer_id        = VDC5_LAYER_ID_2_RD;
  graphics_type   = VDC5_GR_TYPE_GR2;
  gr_format       = GRAPHICS_FORMAT_1;
  switch (ch) {
  case VDC5_CHANNEL_0:
    period_rect.vs  = (uint16_t)50;  // Write something, these values are getting obsolete later
    period_rect.vw  = (uint16_t)YSIZE_PHYS_1;
    period_rect.hs  = (uint16_t)50; // Write something, these values are getting obsolete later
    period_rect.hw  = (uint16_t)XSIZE_PHYS_1;
    break;
  case VDC5_CHANNEL_1:
    period_rect.vs  = (uint16_t)YPOS_1 + LCD_CH1_DISP_VS;
    period_rect.vw  = (uint16_t)YSIZE_PHYS_1;
    period_rect.hs  = (uint16_t)XPOS_1 + LCD_CH1_DISP_HS;
    period_rect.hw  = (uint16_t)XSIZE_PHYS_1;
    break;
  default:
    return 1;
  }
  error = GrpDrv_GraphicsCreateSurface(ch, layer_id, VRAM_ADDR_1, FRAME_BUFFER_STRIDE_1, gr_format, &period_rect);
  if (error != VDC5_OK) {
    return 1;
  }
  gr_disp_sel[graphics_type] = VDC5_DISPSEL_BLEND;
#if NUM_LAYERS > 2
  //
  // Create and start graphics surface 0
  //
  layer_id        = VDC5_LAYER_ID_3_RD;
  graphics_type   = VDC5_GR_TYPE_GR3;
  gr_format       = GRAPHICS_FORMAT_2;
  switch (ch) {
  case VDC5_CHANNEL_0:
    period_rect.vs  = (uint16_t)YPOS_2 + LCD_CH0_DISP_VS;
    period_rect.vw  = (uint16_t)YSIZE_PHYS_2;
    period_rect.hs  = (uint16_t)XPOS_2 + LCD_CH0_DISP_HS;
    period_rect.hw  = (uint16_t)XSIZE_PHYS_2;
    break;
  case VDC5_CHANNEL_1:
    period_rect.vs  = (uint16_t)YPOS_2 + LCD_CH1_DISP_VS;
    period_rect.vw  = (uint16_t)YSIZE_PHYS_2;
    period_rect.hs  = (uint16_t)XPOS_2 + LCD_CH1_DISP_HS;
    period_rect.hw  = (uint16_t)XSIZE_PHYS_2;
    break;
  default:
    return 1;
  }
  error = GrpDrv_GraphicsCreateSurface(ch, layer_id, VRAM_ADDR_2, FRAME_BUFFER_STRIDE_2, gr_format, &period_rect);
  if (error != VDC5_OK) {
    return 1;
  }
  gr_disp_sel[graphics_type] = VDC5_DISPSEL_BLEND;
    
#endif
#endif
  error = GrpDrv_StartSurfaces(ch, gr_disp_sel);
  if (error != VDC5_OK) {
    return 1;
  }
  //
  // Configure interrupt generation of VDC5
  //
  VDC50.SYSCNT_INT4  |= 0x1000;                // Enable GR3 interrupts
  VDC50.SYSCNT_INT1  |= 0x1000;                // Accept GR3 interrupts
  VDC50.GR3_CLUT_INT &= ~0x000007FFuL;         // Clear Line Interrupt Set
  VDC50.GR3_CLUT_INT |= LINE_INDEX_INTERRUPT;  // Set line after which an interrupt should be triggered
  VDC50.GR3_UPDATE   |= 0x10;                  // Update register on rising edge of VSYNC
  //
  // Enable ISR by embOS functions
  //
  OS_ARM_InstallISRHandler(INTC_ID_GR3_VLINE0, _ISR_VSync);
  OS_ARM_ISRSetPrio(INTC_ID_GR3_VLINE0, VDC5_INT_PRIORITY);
  OS_ARM_EnableISR(INTC_ID_GR3_VLINE0);
  return 0;
}

/*********************************************************************
*
*       _SetVis
*/
static void _SetVis(unsigned LayerIndex, unsigned OnOff) {
  switch (LayerIndex) {
  case 0:
    if (OnOff) {
      VDC50.GR0_AB1    = 2;
      VDC50.GR0_FLM_RD = 1;
    } else {
      VDC50.GR0_AB1    = 1;
      VDC50.GR0_FLM_RD = 0;
    }
    VDC50.GR0_UPDATE = 0x111;
    break;
#if NUM_LAYERS > 1
  case 1:
    if (OnOff) {
      VDC50.GR2_AB1    = 3;
      VDC50.GR2_FLM_RD = 1;
    } else {
      VDC50.GR2_AB1    = 1;
      VDC50.GR2_FLM_RD = 0;
    }
    VDC50.GR2_UPDATE = 0x111;
    break;
#if NUM_LAYERS > 2
  case 2:
    if (OnOff) {
      VDC50.GR3_AB1    = 3;
      VDC50.GR3_FLM_RD = 1;
    } else {
      VDC50.GR3_AB1    = 1;
      VDC50.GR3_FLM_RD = 0;
    }
    VDC50.GR3_UPDATE = 0x111;
    break;
#endif
#endif
  }
}

/*********************************************************************
*
*       _SetPos
*/
static void _SetPos(unsigned LayerIndex, int xPos, int yPos) {
  switch (LayerIndex) {
  case 0:
    VDC50.GR0_AB2   &= 0x0000FFFF;
    VDC50.GR0_AB2   |= (yPos + LCD_CH0_DISP_VS) << 16;
    VDC50.GR0_AB3   &= 0x0000FFFF;
    VDC50.GR0_AB3   |= (xPos + LCD_CH0_DISP_HS) << 16;
    VDC50.GR0_UPDATE = 0x111;
    break;
#if NUM_LAYERS > 1
  case 1:
    VDC50.GR2_AB2   &= 0x0000FFFF;
    VDC50.GR2_AB2   |= (yPos + LCD_CH0_DISP_VS) << 16;
    VDC50.GR2_AB3   &= 0x0000FFFF;
    VDC50.GR2_AB3   |= (xPos + LCD_CH0_DISP_HS) << 16;
    VDC50.GR2_UPDATE = 0x111;
    break;
#if NUM_LAYERS > 2
  case 2:
    VDC50.GR3_AB2   &= 0x0000FFFF;
    VDC50.GR3_AB2   |= (yPos + LCD_CH0_DISP_VS) << 16;
    VDC50.GR3_AB3   &= 0x0000FFFF;
    VDC50.GR3_AB3   |= (xPos + LCD_CH0_DISP_HS) << 16;
    VDC50.GR3_UPDATE = 0x111;
    break;
#endif
#endif
  }
}

/*********************************************************************
*
*       _SetSize
*/
static void _SetSize(unsigned LayerIndex, int xSize, int ySize) {
  switch (LayerIndex) {
  case 0:
    VDC50.GR0_AB2   &= 0x07FF0000;
    VDC50.GR0_AB2   |= ySize;
    VDC50.GR0_AB3   &= 0x07FF0000;
    VDC50.GR0_AB3   |= xSize;
    VDC50.GR0_UPDATE = 0x111;
    break;
#if NUM_LAYERS > 1
  case 1:
    VDC50.GR2_AB2   &= 0x07FF0000;
    VDC50.GR2_AB2   |= ySize;
    VDC50.GR2_AB3   &= 0x07FF0000;
    VDC50.GR2_AB3   |= xSize;
    VDC50.GR2_UPDATE = 0x111;
    break;
#if NUM_LAYERS > 2
  case 2:
    VDC50.GR3_AB2   &= 0x07FF0000;
    VDC50.GR3_AB2   |= ySize;
    VDC50.GR3_AB3   &= 0x07FF0000;
    VDC50.GR3_AB3   |= xSize;
    VDC50.GR3_UPDATE = 0x111;
    break;
#endif
#endif
  }
}

/*********************************************************************
*
*       _SetAlpha
*/
static void _SetAlpha(unsigned LayerIndex,int Alpha) {
  vdc5_pd_disp_rect_t        gr_arc;
  vdc5_alpha_rect_t          alpha_rect;
  vdc5_scl_und_sel_t         scl_und_sel = {0};
  vdc5_alpha_blending_rect_t Param;
  vdc5_layer_id_t            Layer_ID;
  
  Layer_ID = 0;  // Init with 0 to avoid compiler warnings
  switch (LayerIndex) {
  case 0:
    gr_arc.vs_rel = 0; 
    gr_arc.vw_rel = YSIZE_PHYS_0; 
    gr_arc.hs_rel = 0; 
    gr_arc.hw_rel = XSIZE_PHYS_0;
    Layer_ID      = VDC5_LAYER_ID_0_RD;
    break;
#if NUM_LAYERS > 1
  case 1:
    gr_arc.vs_rel = 0; 
    gr_arc.vw_rel = YSIZE_PHYS_1; 
    gr_arc.hs_rel = 0; 
    gr_arc.hw_rel = XSIZE_PHYS_1;
    Layer_ID      = VDC5_LAYER_ID_2_RD;
    break;
#if NUM_LAYERS > 2
  case 2:
    gr_arc.vs_rel = 0; 
    gr_arc.vw_rel = YSIZE_PHYS_2; 
    gr_arc.hs_rel = 0; 
    gr_arc.hw_rel = XSIZE_PHYS_2;
    Layer_ID      = VDC5_LAYER_ID_3_RD;
    break;
#endif
#endif
  default:
    return;
  }  
  alpha_rect.gr_arc_coef = 0; 
  alpha_rect.gr_arc_rate = 0;
  alpha_rect.gr_arc_def  = Alpha; 
  alpha_rect.gr_arc_mul  = 1;
  Param.gr_arc = &gr_arc; 
  Param.alpha_rect = &alpha_rect; 
  Param.scl_und_sel = & scl_und_sel;
  R_VDC5_AlphaBlendingRect(0, Layer_ID, 1, &Param);
}

/*********************************************************************
*
*       _InitLCD
*/
static void _InitLCD(void) {
  _InitController();
  Board_Init();     // Initialize board related stuff (MCU GPIOs, ADC, DMA, ...)
  LCD_On();         // Turn LCD on, backlight is still off
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
void LCD_X_Config(void) {
  //
  // *** Layer 0 ***
  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS_0 > 1)
    GUI_MULTIBUF_ConfigEx(0, NUM_BUFFERS_0);
  #endif
  //
  // Calculate 32 byte aligned frame buffer address
  //
  _apFrameBuffer[0] = (U32 *)(((U32)(&_aFrameBuffer0[0] + 8) & ~0x1fUL) + RAM_RELOCATION_OFFS);
  //
  // Set display driver and color conversion
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  //
  // Layer size configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS_0, XSIZE_PHYS_0);
    LCD_SetVSizeEx(0, YSIZE_PHYS_0, XSIZE_PHYS_0);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS_0, YSIZE_PHYS_0);
    LCD_SetVSizeEx(0, XSIZE_PHYS_0, YSIZE_PHYS_0);
  }
  //
  // Set VRAM address
  //
  LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR_0 - VRAM_UNCACHED_ADDR_OFFS); // Uncached area _NC_NB
#if NUM_LAYERS > 1
  //
  // *** Layer 1 ***
  //
  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS_1 > 1)
    GUI_MULTIBUF_ConfigEx(1, NUM_BUFFERS_1);
  #endif
  //
  // Calculate 32 byte aligned frame buffer address
  //
  _apFrameBuffer[1] = (U32 *)(((U32)(&_aFrameBuffer1[0] + 8) & ~0x1fUL) + RAM_RELOCATION_OFFS);
  //
  // Set display driver and color conversion
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 1);
  //
  // Layer size configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (1, YSIZE_PHYS_1, XSIZE_PHYS_1);
    LCD_SetVSizeEx(1, YSIZE_PHYS_1, XSIZE_PHYS_1);
  } else {
    LCD_SetSizeEx (1, XSIZE_PHYS_1, YSIZE_PHYS_1);
    LCD_SetVSizeEx(1, XSIZE_PHYS_1, YSIZE_PHYS_1);
  }
  //
  // Set VRAM address
  //
  LCD_SetVRAMAddrEx(1, (void *)VRAM_ADDR_1 - VRAM_UNCACHED_ADDR_OFFS); // Uncached area _NC_NB
#if NUM_LAYERS > 2
  //
  // *** Layer 2 ***
  //
  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS_2 > 1)
    GUI_MULTIBUF_ConfigEx(2, NUM_BUFFERS_2);
  #endif
  //
  // Calculate 32 byte aligned frame buffer address
  //
  _apFrameBuffer[2] = (U32 *)(((U32)(&_aFrameBuffer2[0] + 8) & ~0x1fUL) + RAM_RELOCATION_OFFS);
  //
  // Set display driver and color conversion
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_2, COLOR_CONVERSION_2, 0, 2);
  //
  // Layer size configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (2, YSIZE_PHYS_2, XSIZE_PHYS_2);
    LCD_SetVSizeEx(2, YSIZE_PHYS_2, XSIZE_PHYS_2);
  } else {
    LCD_SetSizeEx (2, XSIZE_PHYS_2, YSIZE_PHYS_2);
    LCD_SetVSizeEx(2, XSIZE_PHYS_2, YSIZE_PHYS_2);
  }
  //
  // Set VRAM address
  //
  LCD_SetVRAMAddrEx(2, (void *)VRAM_ADDR_2 - VRAM_UNCACHED_ADDR_OFFS); // Uncached area _NC_NB
#endif
#endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    if (LayerIndex == 0) {
      _InitLCD();
    }
    return 0;
  }
  case LCD_X_SHOWBUFFER: {
    LCD_X_SHOWBUFFER_INFO * p;

    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    _aPendingBuffer[LayerIndex] = p->Index;
    return 0;
  }
  case LCD_X_SETALPHA: {
    LCD_X_SETALPHA_INFO * p;

    p = (LCD_X_SETALPHA_INFO *)pData;    
    _SetAlpha(LayerIndex, p->Alpha);
    return 0;
  }
  case LCD_X_SETPOS: {
    LCD_X_SETPOS_INFO * p;

    p = (LCD_X_SETPOS_INFO *)pData;
    _SetPos(LayerIndex, p->xPos, p->yPos);
    break;
  }
  case LCD_X_SETVIS: {
    LCD_X_SETVIS_INFO * p;

    p = (LCD_X_SETVIS_INFO *)pData;
    _SetVis(LayerIndex, p->OnOff);
    break;
  }
  case LCD_X_SETSIZE: {
    LCD_X_SETSIZE_INFO * p;

    p = (LCD_X_SETSIZE_INFO *)pData;
    _SetSize(LayerIndex, p->xSize, p->ySize);
    }
    return 0;
  case LCD_X_ON: {
    //
    // Set layer to its designated position.
    // Somehow it doesn't work on creation of these layers.
    //
    _SetPos(0, XPOS_0, YPOS_0);
#if NUM_LAYERS > 1
    _SetPos(1, XPOS_1, YPOS_1);
#if NUM_LAYERS > 2
    _SetPos(2, XPOS_2, YPOS_2);
#endif
#endif
    GUI_X_Delay(150);        // Wait to avoid flickering when turning on the backlight
    LCD_SetBrightness(128);  // Set the brightness to 50%
    BL_On();                 // Turn on the backlight
    return 0;
  }
  case LCD_X_OFF: {
    BL_Off();
    return 0;
  }
  default:
    r = -1;
    break;
  }
  return r;
}

/*************************** End of file ****************************/
