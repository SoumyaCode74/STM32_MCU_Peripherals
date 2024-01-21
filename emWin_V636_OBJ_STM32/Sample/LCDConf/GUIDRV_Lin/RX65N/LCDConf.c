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
----------------------------------------------------------------------

MultiBuffering:
  emWin requires 3 buffers to be able to work with efficiently.
  The available On-chip RAM unfortunately restricts use of
  3 buffers for MultiBuffering to 1- and 4bpp mode.
  In 8bpp the line offset is 512, because LNOFF needs to be a
  multiple of 0x40 (see GRnFLM3.LNOFF). In that case
  0x200 * 0x110 * 3 = 0x66000 bytes are required. But
  unfortunately the size of the memory area is only 0x60000.
  In that case DoubleBuffering is used. In that mode the CPU has to
  wait for the next VSYNC interrupt in case of a new frame buffer.

32bpp mode:
  Has not been tested because of a lack of RAM.

---------------------------END-OF-HEADER------------------------------
*/

#include <stdlib.h>
#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "LCDConf.h"

#ifdef __RX
  #include "platform.h"
  #include "r_glcdc_rx_if.h"
  #include "r_pinset.h"
  #include "r_dmaca_rx_if.h"
#endif
#ifdef __ICCRX__
  #include "iorx65n.h"
#endif

#include "dave_base.h"
#include "dave_videomodes.h"
#include "dave_driver.h"

extern void drw_int_isr(void);

/*********************************************************************
**********************************************************************
*
*       LAYER CONFIGURATION START (TO BE MODIFIED)
*
**********************************************************************
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 480
#define YSIZE_PHYS 272

//
// Color depth
//
#define BITS_PER_PIXEL 16  // Allowed values: 1, 4, 8, 16, 32

/*********************************************************************
**********************************************************************
*
*       LAYER CONFIGURATION END
*
**********************************************************************
**********************************************************************
*/

/*********************************************************************
*
*       Determine driver, color conversion and size of frame buffer
*
**********************************************************************
*/
//
// Color format selection
//
#define FORMAT_RGB_565   (GLCDC_IN_FORMAT_16BITS_RGB565)
#define FORMAT_RGB_888   (GLCDC_IN_FORMAT_32BITS_RGB888)
#define FORMAT_ARGB_1555 (GLCDC_IN_FORMAT_16BITS_ARGB1555)
#define FORMAT_ARGB_4444 (GLCDC_IN_FORMAT_16BITS_ARGB4444)
#define FORMAT_ARGB_8888 (GLCDC_IN_FORMAT_32BITS_ARGB8888)
#define FORMAT_CLUT_8    (GLCDC_IN_FORMAT_CLUT8)
#define FORMAT_CLUT_4    (GLCDC_IN_FORMAT_CLUT4)
#define FORMAT_CLUT_1    (GLCDC_IN_FORMAT_CLUT1)

#define ORIENTATION_0    0
#define ORIENTATION_CW   1
#define ORIENTATION_180  2
#define ORIENTATION_CCW  3

/*********************************************************************
**********************************************************************
*
*       DISPLAY ORIENTATION START (TO BE MODIFIED)
*
**********************************************************************
**********************************************************************
*/

#define DISPLAY_ORIENTATION  ORIENTATION_0
//#define DISPLAY_ORIENTATION  ORIENTATION_CW
//#define DISPLAY_ORIENTATION  ORIENTATION_180
//#define DISPLAY_ORIENTATION  ORIENTATION_CCW

/*********************************************************************
**********************************************************************
*
*       DISPLAY ORIENTATION START
*
**********************************************************************
**********************************************************************
*/

//
// Color conversion, display driver and multiple buffers
//
#if   (BITS_PER_PIXEL == 32)
  #define COLOR_CONVERSION GUICC_M8888I
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  #define DISPLAY_DRIVER   GUIDRV_LIN_32      // Default
#elif (DISPLAY_ORIENTATION == ORIENTATION_CW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSX_32  // CW
#elif (DISPLAY_ORIENTATION == ORIENTATION_CCW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSY_32  // CCW
#else
  #define DISPLAY_DRIVER   GUIDRV_LIN_OXY_32  // 180
#endif
  #define NUM_BUFFERS      1
#define COLOR_FORMAT     FORMAT_RGB_ARGB8888
#elif (BITS_PER_PIXEL == 16)
  #define COLOR_CONVERSION GUICC_M565
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  #define DISPLAY_DRIVER   GUIDRV_LIN_16      // Default
#elif (DISPLAY_ORIENTATION == ORIENTATION_CW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSX_16  // CW
#elif (DISPLAY_ORIENTATION == ORIENTATION_CCW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSY_16  // CCW
#else
  #define DISPLAY_DRIVER   GUIDRV_LIN_OXY_16  // 180
#endif
  #define NUM_BUFFERS      2
  #define COLOR_FORMAT     FORMAT_RGB_565
#elif (BITS_PER_PIXEL == 8)
  #define COLOR_CONVERSION GUICC_8666
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  #define DISPLAY_DRIVER   GUIDRV_LIN_8      // Default
#elif (DISPLAY_ORIENTATION == ORIENTATION_CW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSX_8  // CW
#elif (DISPLAY_ORIENTATION == ORIENTATION_CCW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSY_8  // CCW
#else
  #define DISPLAY_DRIVER   GUIDRV_LIN_OXY_8  // 180
#endif
  #define NUM_BUFFERS      2
#define COLOR_FORMAT     FORMAT_CLUT_8
#elif (BITS_PER_PIXEL == 4)
  #define COLOR_CONVERSION GUICC_16
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  #define DISPLAY_DRIVER   GUIDRV_LIN_4      // Default
#elif (DISPLAY_ORIENTATION == ORIENTATION_CW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSX_4  // CW
#elif (DISPLAY_ORIENTATION == ORIENTATION_CCW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSY_4  // CCW
#else
  #define DISPLAY_DRIVER   GUIDRV_LIN_OXY_4  // 180
#endif
  #define NUM_BUFFERS      2
#define COLOR_FORMAT     FORMAT_CLUT_4
#elif (BITS_PER_PIXEL == 1)
  #define COLOR_CONVERSION GUICC_1
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  #define DISPLAY_DRIVER   GUIDRV_LIN_1      // Default
#elif (DISPLAY_ORIENTATION == ORIENTATION_CW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSX_1  // CW
#elif (DISPLAY_ORIENTATION == ORIENTATION_CCW)
  #define DISPLAY_DRIVER   GUIDRV_LIN_OSY_1  // CCW
#else
  #define DISPLAY_DRIVER   GUIDRV_LIN_OXY_1  // 180
#endif
  #define NUM_BUFFERS      2
  #define COLOR_FORMAT     FORMAT_CLUT_1
#endif

//
// Check if the Dave2D engine can be used
//
#if ((BITS_PER_PIXEL > 8) && (DISPLAY_ORIENTATION == 0))
  #define USE_DAVE2D  1
#else
  #define USE_DAVE2D  0
#endif

//
// Buffer size and stride
//
#define BYTES_PER_LINE   ((BITS_PER_PIXEL * XSIZE_PHYS) / 8)
#define LINE_OFFSET      (((BYTES_PER_LINE + 63) / 64) * 64)
#define VXSIZE_PHYS      ((LINE_OFFSET * 8) / BITS_PER_PIXEL)
#define BYTES_PER_BUFFER (LINE_OFFSET * YSIZE_PHYS)
#define MEMORY_OFFSET    (((DISPLAY_ORIENTATION == ORIENTATION_180) || (DISPLAY_ORIENTATION == ORIENTATION_CW)) ? (LINE_OFFSET - BYTES_PER_LINE) : 0)

//
// Timing
//
#define SYNC_H       4
#define SYNC_V       1
#define BGSYNC_HP    2
#define BGSYNC_VP    2
#define BGHSIZE_HP  (BGSYNC_HP + SYNC_H)
#define BGVSIZE_VP  (BGSYNC_VP + SYNC_V)
#define GRC_VS      (BGVSIZE_VP - BGSYNC_VP)
#define GRC_HS      (BGHSIZE_HP - BGSYNC_HP)

//
// Brightness & COntrast
//
#define _BRIGHTNESS 0x200
#define _CONTRAST   0x80

//
// Buffer addresses
//
#define BUFFER_ADDR_0 0x00000100
#define BUFFER_ADDR_1 0x00800000

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const U32 _aBufferPTR[] = {
  BUFFER_ADDR_0,  // Begin of On-Chip RAM
  BUFFER_ADDR_1   // Begin of Expansion RAM
};

static volatile int _PendingBuffer = -1;

static unsigned _WriteBufferIndex;

static glcdc_runtime_cfg_t runtime_cfg;

static unsigned _DaveActive;
//
// Dave2D
//
static d2_device       * d2_handle;
static d2_renderbuffer * renderbuffer;
#if (USE_DAVE2D == 0)
  #define NUM_COLORS (1 << BITS_PER_PIXEL)
  static U32 _aClut[NUM_COLORS];
#else
  #define NUM_COLORS  (256)
#endif

static U32 _aBuffer[40 * 10];

//
// Array for speeding up nibble conversion for A4 bitmaps
//
static const U8 _aMirror[] = {
  0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0,
  0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x81, 0x91, 0xA1, 0xB1, 0xC1, 0xD1, 0xE1, 0xF1,
  0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 0x82, 0x92, 0xA2, 0xB2, 0xC2, 0xD2, 0xE2, 0xF2,
  0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, 0x83, 0x93, 0xA3, 0xB3, 0xC3, 0xD3, 0xE3, 0xF3,
  0x04, 0x14, 0x24, 0x34, 0x44, 0x54, 0x64, 0x74, 0x84, 0x94, 0xA4, 0xB4, 0xC4, 0xD4, 0xE4, 0xF4,
  0x05, 0x15, 0x25, 0x35, 0x45, 0x55, 0x65, 0x75, 0x85, 0x95, 0xA5, 0xB5, 0xC5, 0xD5, 0xE5, 0xF5,
  0x06, 0x16, 0x26, 0x36, 0x46, 0x56, 0x66, 0x76, 0x86, 0x96, 0xA6, 0xB6, 0xC6, 0xD6, 0xE6, 0xF6,
  0x07, 0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87, 0x97, 0xA7, 0xB7, 0xC7, 0xD7, 0xE7, 0xF7,
  0x08, 0x18, 0x28, 0x38, 0x48, 0x58, 0x68, 0x78, 0x88, 0x98, 0xA8, 0xB8, 0xC8, 0xD8, 0xE8, 0xF8,
  0x09, 0x19, 0x29, 0x39, 0x49, 0x59, 0x69, 0x79, 0x89, 0x99, 0xA9, 0xB9, 0xC9, 0xD9, 0xE9, 0xF9,
  0x0A, 0x1A, 0x2A, 0x3A, 0x4A, 0x5A, 0x6A, 0x7A, 0x8A, 0x9A, 0xAA, 0xBA, 0xCA, 0xDA, 0xEA, 0xFA,
  0x0B, 0x1B, 0x2B, 0x3B, 0x4B, 0x5B, 0x6B, 0x7B, 0x8B, 0x9B, 0xAB, 0xBB, 0xCB, 0xDB, 0xEB, 0xFB,
  0x0C, 0x1C, 0x2C, 0x3C, 0x4C, 0x5C, 0x6C, 0x7C, 0x8C, 0x9C, 0xAC, 0xBC, 0xCC, 0xDC, 0xEC, 0xFC,
  0x0D, 0x1D, 0x2D, 0x3D, 0x4D, 0x5D, 0x6D, 0x7D, 0x8D, 0x9D, 0xAD, 0xBD, 0xCD, 0xDD, 0xED, 0xFD,
  0x0E, 0x1E, 0x2E, 0x3E, 0x4E, 0x5E, 0x6E, 0x7E, 0x8E, 0x9E, 0xAE, 0xBE, 0xCE, 0xDE, 0xEE, 0xFE,
  0x0F, 0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F, 0x7F, 0x8F, 0x9F, 0xAF, 0xBF, 0xCF, 0xDF, 0xEF, 0xFF,
};

static const U32 clut_i4[256] = {
  0x00FFFFFF, 0x10FFFFFF, 0x20FFFFFF, 0x30FFFFFF,
  0x40FFFFFF, 0x50FFFFFF, 0x60FFFFFF, 0x70FFFFFF,
  0x80FFFFFF, 0x90FFFFFF, 0xA0FFFFFF, 0xB0FFFFFF,
  0xC0FFFFFF, 0xD0FFFFFF, 0xE0FFFFFF, 0xF0FFFFFF
};

static void _LCD_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex);

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _VSYNC_ISR
*/
static void _VSYNC_ISR(void * p) {
  //
  // Sets a flag to be able to notice the interrupt
  //
  _PendingBuffer = 0;
}

/*********************************************************************
*
*       _SwitchBuffersOnVSYNC
*/
static void _SwitchBuffersOnVSYNC(int Index) {
  U32 Addr;
  glcdc_err_t ret;

  for (_PendingBuffer = 1; _PendingBuffer; );  // Wait until _PendingBuffer is cleared by ISR
  Addr = _aBufferPTR[Index];
  runtime_cfg.input.p_base = (uint32_t *)Addr; // Specify the start address of the frame buffer
  ret = R_GLCDC_LayerChange(GLCDC_FRAME_LAYER_2, &runtime_cfg);	// Graphic 2 Register Value Reflection Enable
  if (ret == GLCDC_SUCCESS) {
    GUI_MULTIBUF_ConfirmEx(0, Index);  // Tell emWin that buffer is used
  } else {
    //
    // You should not end up here. Buffer change has failed, check "ret" to get further information
    // or add code to debug the issue.
    //
  }
}

/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Should initialize the display controller
*/
static void _InitController(void) {
  glcdc_cfg_t glcdc_cfg;
  glcdc_err_t ret;
  //
  // Release stop state of GLCDC
  //
  // R_GLCDC_Open function release stop state of GLCDC.
  //
  // Function select of multiplex pins (Display B)
  //
  R_GLCDC_PinSet();
  //
  // Set DISP signal on P97 (Pin31)
  //
  PORT6.PDR.BIT.B3  = 1;  // Port direction: output
  PORT6.PODR.BIT.B3 = 1;  // DISP on
  //
  // Switch backlight on, no PWM
  //
  PORT6.PDR.BIT.B6  = 1;
  PORT6.PODR.BIT.B6 = 1;
  //
  // Set the BGEN.SWRST bit to 1 to release the GLCDC from a software reset
  //
  // R_GLCDC_Open function release the GLCDC from a software reset.
  //
  // Set the frequency of the LCD_CLK and PXCLK to suit the format and set the PANELCLK.CLKEN bit to 1
  //
  glcdc_cfg.output.clksrc = GLCDC_CLK_SRC_INTERNAL;   			  // Select PLL clock
  glcdc_cfg.output.clock_div_ratio = GLCDC_PANEL_CLK_DIVISOR_24;  // 240 / 24 = 10 MHz
  																  // No frequency division
  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  // Enable LCD_CLK output
  //
  // Definition of Background Screen
  //
  										   	   	   	   	   	   	  // Horizontal cycle (whole control screen)
  	  	  	  	  	  	  	  	  	  	   	   	   	   	   	   	  // Vertical cycle (whole control screen)
  glcdc_cfg.output.htiming.front_porch = 5;         			  // Horizontal Synchronization Signal Assertion Position
  glcdc_cfg.output.vtiming.front_porch = 8;         			  // Vertical Synchronization Assertion Position
  glcdc_cfg.output.htiming.back_porch = 40;                        // Horizontal Active Pixel Start Position (min. 6 pixels)
  glcdc_cfg.output.vtiming.back_porch = 8;
  glcdc_cfg.output.htiming.display_cyc = XSIZE_PHYS;              // Horizontal Active Pixel Width
  glcdc_cfg.output.vtiming.display_cyc = YSIZE_PHYS;              // Vertical Active Display Width
  glcdc_cfg.output.htiming.sync_width = 1;        				  // Vertical Active Display Start Position (min. 3 lines)
  glcdc_cfg.output.vtiming.sync_width = 1;

  //
  // Graphic 1 configuration
  //
  glcdc_cfg.input[GLCDC_FRAME_LAYER_1].p_base = NULL;			  // Disable Graphics 1

  //
  // Graphic 2 configuration
  //
  													  	  	  	  // Enable reading of the frame buffer
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].p_base = (uint32_t *)_aBufferPTR[0];   // Specify the start address of the frame buffer
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].offset = LINE_OFFSET;      // Offset value from the end address of the line to the start address of the next line
  																  // Single Line Data Transfer Count
  																  // Single Frame Line Count
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].format = COLOR_FORMAT;     // Frame Buffer Color Format RGB565
  glcdc_cfg.blend[GLCDC_FRAME_LAYER_2].visible = true;
  glcdc_cfg.blend[GLCDC_FRAME_LAYER_2].blend_control = GLCDC_BLEND_CONTROL_NONE;	// Display Screen Control (current graphics)
  glcdc_cfg.blend[GLCDC_FRAME_LAYER_2].frame_edge = false;        // Rectangular Alpha Blending Area Frame Display Control
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].frame_edge = false;        // Graphics Area Frame Display Control
  																  // Alpha Blending Control (Per-pixel alpha blending)
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].coordinate.y = 0;          // Graphics Area Vertical Start Position
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].vsize = YSIZE_PHYS;        // Graphics Area Vertical Width
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].coordinate.x = 0;          // Graphics Area Horizontal Start Position
  glcdc_cfg.input[GLCDC_FRAME_LAYER_2].hsize = XSIZE_PHYS;        // Graphics Area Horizontal Width
  glcdc_cfg.blend[GLCDC_FRAME_LAYER_2].start_coordinate.x = 0;    // Rectangular Alpha Blending Area Vertical Start Position
  glcdc_cfg.blend[GLCDC_FRAME_LAYER_2].end_coordinate.x= YSIZE_PHYS; // Rectangular Alpha Blending Area Vertical Width
  glcdc_cfg.blend[GLCDC_FRAME_LAYER_2].start_coordinate.y = 0;    // Rectangular Alpha Blending Area Horizontal Start Position
  glcdc_cfg.blend[GLCDC_FRAME_LAYER_2].end_coordinate.y= XSIZE_PHYS; // Rectangular Alpha Blending Area Horizontal Width
  																  // Graphic 2 Register Value Reflection Enable
  //
  // Timing configuration
  //
  //   Horizontal Synchronization Signal Reference Timing Offset (not support)
  //   Signal Reference Timing Select (not support)
  //   STVB Signal Assertion Timing
  //   STVB Signal Pulse Width
  //   STHB Signal Pulse Width
  glcdc_cfg.output.tcon_vsync = GLCDC_TCON_PIN_0;           // TCON0 Output Signal Select STVA (VSYNC)
  glcdc_cfg.output.tcon_hsync = GLCDC_TCON_PIN_2;           // TCON2 Output Signal Select STHA (HSYNC)
  glcdc_cfg.output.tcon_de    = GLCDC_TCON_PIN_3;           // TCON3 Output Signal Select DE (DEN)
  glcdc_cfg.output.data_enable_polarity = GLCDC_SIGNAL_POLARITY_HIACTIVE;
  glcdc_cfg.output.hsync_polarity = GLCDC_SIGNAL_POLARITY_LOACTIVE;
  glcdc_cfg.output.vsync_polarity = GLCDC_SIGNAL_POLARITY_LOACTIVE;
  glcdc_cfg.output.sync_edge = GLCDC_SIGNAL_SYNC_EDGE_RISING;
  //
  // Output interface
  //
  //   Serial RGB Data Output Delay Control (0 cycles) (not support)
  //   Serial RGB Scan Direction Select (forward) (not support)
  //   Pixel Clock Division Control (no division)
  glcdc_cfg.output.format = GLCDC_OUT_FORMAT_16BITS_RGB565; // Output Data Format Select (RGB565)
  glcdc_cfg.output.color_order = GLCDC_COLOR_ORDER_RGB; ///**/GLCDC_COLOR_ORDER_BGR;  	// Pixel Order Control (B-G-R)
  glcdc_cfg.output.endian = GLCDC_ENDIAN_LITTLE;  			// Bit Endian Control (Little endian)
  //
  // Brightness Adjustment
  //
  glcdc_cfg.output.brightness.b = _BRIGHTNESS;  // B
  glcdc_cfg.output.brightness.g = _BRIGHTNESS;  // G
  glcdc_cfg.output.brightness.r = _BRIGHTNESS;  // R
  //
  // Contrast Adjustment Value
  //
  glcdc_cfg.output.contrast.b = _CONTRAST;  // B
  glcdc_cfg.output.contrast.g = _CONTRAST;  // G
  glcdc_cfg.output.contrast.r = _CONTRAST;  // R
  //
  // Disable Gamma
  //
  glcdc_cfg.output.gamma.enable = false;
  //
  // Disable Chromakey
  //
  glcdc_cfg.chromakey[GLCDC_FRAME_LAYER_2].enable = false;
  //
  // Disable Dithering
  //
  glcdc_cfg.output.dithering.dithering_on = false;
  //
  // CLUT Adjustment Value
  //
  glcdc_cfg.clut[GLCDC_FRAME_LAYER_2].enable = false;
  //
  // Enable VPOS ISR
  //
  //   Detecting Scanline Setting
  glcdc_cfg.detection.vpos_detect = true;		         	// Enable detection of specified line notification in graphic 2
  glcdc_cfg.interrupt.vpos_enable = true;		           	// Enable VPOS interrupt request
  //   Interrupt Priority Level (r_glcdc_rx_config.h)
  //   Interrupt Request Enable
  //   Clears the STMON.VPOS flag
  //   VPOS (line detection)
  glcdc_cfg.detection.gr1uf_detect = false;
  glcdc_cfg.detection.gr2uf_detect = false;
  glcdc_cfg.interrupt.gr1uf_enable = false;
  glcdc_cfg.interrupt.gr2uf_enable = false;
  //
  // Set function to be called on VSYNC
  //
  glcdc_cfg.p_callback = (void (*)(void *))_VSYNC_ISR;

  runtime_cfg.blend = glcdc_cfg.blend[GLCDC_FRAME_LAYER_2];
  runtime_cfg.input = glcdc_cfg.input[GLCDC_FRAME_LAYER_2];
  runtime_cfg.chromakey = glcdc_cfg.chromakey[GLCDC_FRAME_LAYER_2];
  //
  // Register Dave2D interrupt
  //
  R_BSP_InterruptWrite(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ, (bsp_int_cb_t)drw_int_isr);
  //
  // Register Reflection
  //
  ret = R_GLCDC_Open(&glcdc_cfg);
  if (ret != GLCDC_SUCCESS) {
	  while(1);
  }
  //
  // Init DMA
  //
  R_DMACA_Init();
  //
  // Extended Bus Master Priority Control Register
  //
  BSC.EBMAPCR.BIT.PR1SEL = 3;
  BSC.EBMAPCR.BIT.PR2SEL = 1;
  BSC.EBMAPCR.BIT.PR3SEL = 2;
  BSC.EBMAPCR.BIT.PR4SEL = 0;
  BSC.EBMAPCR.BIT.PR5SEL = 4;
}

/*********************************************************************
*
*       _SetLUTEntry
*
* Purpose:
*   Should set the desired LUT entry
*/
static void _SetLUTEntry(LCD_COLOR Color, U8 Pos) {
#if (BITS_PER_PIXEL < 16)
glcdc_clut_cfg_t p_clut_cfg;
  int err;

  _aClut[Pos] = Color;
  if (Pos == NUM_COLORS - 1) {
    p_clut_cfg.enable = true;
    p_clut_cfg.p_base = _aClut;
    p_clut_cfg.size = NUM_COLORS;
    p_clut_cfg.start = 0;
    R_GLCDC_Control(GLCDC_CMD_START_DISPLAY, FIT_NO_FUNC);  // Enable background generation block
    GUI_Delay(100);
    err = R_GLCDC_ClutUpdate(GLCDC_FRAME_LAYER_2, &p_clut_cfg);
    if (err != GLCDC_SUCCESS) {
      while(1);
    }
  }
#else
  GUI_USE_PARA(Color);
  GUI_USE_PARA(Pos);
#endif
}

/*********************************************************************
*
*       _DisplayOnOff
*/
static void _DisplayOnOff(int OnOff) {
  if (OnOff) {
    R_GLCDC_Control(GLCDC_CMD_START_DISPLAY, FIT_NO_FUNC);  // Enable background generation block
    PORT6.PODR.BIT.B6 = 1;
  } else {
	  R_GLCDC_Control(GLCDC_CMD_STOP_DISPLAY, FIT_NO_FUNC);  	// Disable background generation block
    PORT6.PODR.BIT.B6 = 0;
  }
}

#if (USE_DAVE2D)
/*********************************************************************
*
*       _GetD2Mode
*/
static U32 _GetD2Mode(void) {
  U32 r;
#if   (BITS_PER_PIXEL == 32)
  r = d2_mode_argb8888;
#elif (BITS_PER_PIXEL == 16)
  r = d2_mode_rgb565;
#elif (BITS_PER_PIXEL == 8)
  r = d2_mode_i8;
#elif (BITS_PER_PIXEL == 4)
  r = d2_mode_i4;
#elif (BITS_PER_PIXEL == 1)
  r = d2_mode_i1;
#endif
  return r;
}

/*********************************************************************
*
*       _ModifyRect
*/
static void _ModifyCoord(int * pX, int * pY) {
#if (DISPLAY_ORIENTATION == ORIENTATION_CW)
  int xTemp;

  xTemp = *pX;
  *pX   = XSIZE_PHYS - 1 - *pY;
  *pY   = xTemp;
#elif (DISPLAY_ORIENTATION == ORIENTATION_180)
  *pX   = XSIZE_PHYS - 1 - *pX;
  *pY   = YSIZE_PHYS - 1 - *pY;
#elif (DISPLAY_ORIENTATION == ORIENTATION_CCW)
  int yTemp;

  yTemp = *pY;
  *pY   = YSIZE_PHYS - 1 - *pX;
  *pX   = yTemp;
#else
  //
  // Nothing to do
  //
  GUI_USE_PARA(pX);
  GUI_USE_PARA(pY);
#endif
}

/*********************************************************************
*
*       _ModifyRect
*/
static void _ModifyRect(const GUI_RECT * pRectOrg, GUI_RECT * pRect) {
#if (DISPLAY_ORIENTATION == ORIENTATION_CW)
  int Temp;
  pRect->x0 = XSIZE_PHYS - 1 - pRectOrg->y0;
  pRect->y0 = pRectOrg->x0;
  pRect->x1 = XSIZE_PHYS - 1 - pRectOrg->y1;
  pRect->y1 = pRectOrg->x1;
  if (pRect->x0 > pRect->x1) {
    Temp = pRect->x0;
    pRect->x0 = pRect->x1;
    pRect->x1 = Temp;
  }
#elif (DISPLAY_ORIENTATION == ORIENTATION_180)
  int Temp;
  pRect->x0 = XSIZE_PHYS - 1 - pRectOrg->x0;
  pRect->y0 = YSIZE_PHYS - 1 - pRectOrg->y0;
  pRect->x1 = XSIZE_PHYS - 1 - pRectOrg->x1;
  pRect->y1 = YSIZE_PHYS - 1 - pRectOrg->y1;
  if (pRect->x0 > pRect->x1) {
    Temp = pRect->x0;
    pRect->x0 = pRect->x1;
    pRect->x1 = Temp;
  }
  if (pRect->y0 > pRect->y1) {
    Temp = pRect->y0;
    pRect->y0 = pRect->y1;
    pRect->y1 = Temp;
  }
#elif (DISPLAY_ORIENTATION == ORIENTATION_CCW)
  int Temp;
  pRect->x0 = pRectOrg->y0;
  pRect->y0 = YSIZE_PHYS - 1 - pRectOrg->x0;
  pRect->x1 = pRectOrg->y1;
  pRect->y1 = YSIZE_PHYS - 1 - pRectOrg->x1;
  if (pRect->y0 > pRect->y1) {
    Temp = pRect->y0;
    pRect->y0 = pRect->y1;
    pRect->y1 = Temp;
  }
#else
  *pRect = *pRectOrg;
#endif
}

/*********************************************************************
*
*       _LCD_FillRectXOR
*/
static void _LCD_FillRectXOR(int LayerIndex, int x0, int y0, int x1, int y1) {
#if (DISPLAY_ORIENTATION != ORIENTATION_0)
  int              Temp;
#endif
  LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
  //
  // The following is required because emWin already mirrors/swaps the coordinates.
  // Since we call the default function we have to revert the mirroring/swapping.
  //
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  LCD_FillRect(x0, y0, x1, y1);
#elif (DISPLAY_ORIENTATION == ORIENTATION_CW)
  Temp = x0;
  x0 = XSIZE_PHYS - 1 - x1;
  x1 = XSIZE_PHYS - 1 - Temp;
  Temp = y0;
  y0 = x0;
  x0 = Temp;
  Temp = y1;
  y1 = x1;
  x1 = Temp;
  LCD_FillRect(x0, y0, x1, y1);
#elif (DISPLAY_ORIENTATION == ORIENTATION_180)
  x0 = XSIZE_PHYS - 1 - x0;
  x1 = XSIZE_PHYS - 1 - x1;
  y0 = YSIZE_PHYS - 1 - y0;
  y1 = YSIZE_PHYS - 1 - y1;
  if (x0 > x1) {
    Temp = x0;
    x0 = x1;
    x1 = Temp;
  }
  if (y0 > y1) {
    Temp = y0;
    y0 = y1;
    y1 = Temp;
  }
  LCD_FillRect(x0, y0, x1, y1);
#else
  Temp = y1;
  y1   = (YSIZE_PHYS - 1 - y0);
  y0   = (YSIZE_PHYS - 1 - Temp);
  LCD_FillRect(y0, x0, y1, x1);
#endif
  LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_FillRect);
}

/*********************************************************************
*
*       _LCD_FillRect
*/
static void _LCD_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  U32      Mode;
  d2_color Color;
  GUI_RECT Rect;
  int      xSize;
  int      ySize;
  U8       Alpha;

  if (GUI_GetDrawMode() == GUI_DM_XOR) {
    _LCD_FillRectXOR(LayerIndex, x0, y0, x1, y1);
  } else {
    GUI_USE_PARA(LayerIndex);
    Mode = _GetD2Mode();
    //
    // Generate render operations
    //
    d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
    d2_selectrenderbuffer(d2_handle, renderbuffer);
    Color = GUI_Index2Color(PixelIndex);
    d2_setcolor(d2_handle, 0, Color);
    Alpha = GUI_GetAlpha();
    d2_setalpha(d2_handle, Alpha);
    _ModifyRect(GUI_GetClipRect(), &Rect);
    d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
    xSize = (x1 - x0) + 1;
    ySize = (y1 - y0) + 1;
    d2_renderbox(d2_handle, x0 * 16, y0 * 16, xSize * 16, ySize * 16);
    //
    // Execute render operations
    //
    d2_executerenderbuffer(d2_handle, renderbuffer, 0);
    d2_flushframe(d2_handle);
    Alpha = 0xFF;
    d2_setalpha(d2_handle, Alpha);
  }
}

#if (DISPLAY_ORIENTATION == ORIENTATION_0)
/*********************************************************************
*
*       _DrawMemdevAlpha
*/
static void _DrawMemdevAlpha(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  U32 PitchSrc, PitchDst, Mode;

  PitchDst = BytesPerLineDst / 4;
  PitchSrc = BytesPerLineSrc / 4;
  Mode = _GetD2Mode();
  //
  // Set address of destination memory device as frame buffer to be used
  //
  d2_framebuffer(d2_handle, pDst, PitchDst, PitchDst, ySize, d2_mode_argb8888);
  //
  // Generate render operations
  //
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  d2_setblitsrc(d2_handle, (void *)pSrc, PitchSrc, xSize, ySize, d2_mode_argb8888);
  d2_blitcopy(d2_handle, xSize, ySize, 0, 0, xSize * 16, ySize * 16, 0, 0, d2_bf_usealpha);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  //
  // Restore frame buffer
  //
  d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
}

/*********************************************************************
*
*       _DrawBitmapAlpha
*/
static void _DrawBitmapAlpha(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  U32 Pitch, Mode;

  Pitch = BytesPerLine / 4;
  Mode = _GetD2Mode();
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  d2_setblitsrc(d2_handle, (void *)p, Pitch, xSize, ySize, d2_mode_argb8888);
  d2_blitcopy(d2_handle, xSize, ySize, 0, 0, xSize * 16, ySize * 16, x * 16, y * 16, d2_bf_usealpha);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
}

/*********************************************************************
*
*       _DrawBitmap16bpp
*/
static void _DrawBitmap16bpp(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  U32 Mode, ModeSrc;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(BytesPerLine);
  Mode    = _GetD2Mode();
  ModeSrc = d2_mode_rgb565;

  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  d2_setblitsrc(d2_handle, (void *)p, BytesPerLine / 2, xSize, ySize, ModeSrc);
  d2_blitcopy(d2_handle, xSize, ySize, 0, 0, xSize * 16, ySize * 16, x * 16, y * 16, 0);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
}

/*********************************************************************
*
*       _CompactPixelIndices
*/
static int _CompactPixelIndices(LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel) {
  unsigned BytesPerPixel;
  if (BitsPerPixel <= 8) {
    BytesPerPixel = 1;
  } else if (BitsPerPixel <= 16) {
    BytesPerPixel = 2;
  } else {
    BytesPerPixel = 4;
  }
  if (BytesPerPixel < sizeof(U32)) {
    switch (BytesPerPixel) {
    case 2: { // Convert from U32 to U16
        U16 * pDest;
        pDest = (U16 *)pBuffer;
        do {
          *pDest++ = (U16)(*pBuffer++);
        } while (--NumPixels);
      }
      break;
    case 1: { // Convert from U32 to U8
        U8 * pDest;
        U8 Byte;
        pDest = (U8 *)pBuffer;
        switch (BitsPerPixel) {
        case 1:
          do {
            Byte    = ((*pBuffer++) & 0x1) << 7; --NumPixels;
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x1) << 6; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x1) << 5; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x1) << 4; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x1) << 3; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x1) << 2; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x1) << 1; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x1);
            }
            *pDest++ = Byte;
          } while (NumPixels);
          break;
        case 2:
          do {
            Byte    = ((*pBuffer++) & 0x3) << 6; --NumPixels;
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x3) << 4; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x3) << 2; --NumPixels;
            }
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0x3);      --NumPixels;
            }
            *pDest++ = Byte;
          } while (NumPixels);
          break;
        case 4:
          do {
            Byte    = ((*pBuffer++) & 0xF) << 4; --NumPixels;
            if (NumPixels) {
              Byte |= ((*pBuffer++) & 0xF);      --NumPixels;
            }
            *pDest++ = Byte;
          } while (NumPixels);
          break;
        default:
          do {
            *pDest++ = (U8)(*pBuffer++);
          } while (--NumPixels);
          break;
        }
      }
      break;
    }
    return 1;
  }
  return 0;
}

/*********************************************************************
*
*       _DrawBitmap8bpp
*/
static void _DrawBitmap8bpp (int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine, const LCD_PIXELINDEX * pTrans) {
  U32         Mode;
  U32         ModeSrc;
  int         NumEntries;
  const GUI_RECT * pRect;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(BytesPerLine);
  Mode       = _GetD2Mode();
  ModeSrc    = d2_mode_i8 | d2_mode_clut;
  NumEntries = NUM_COLORS;

  _CompactPixelIndices((LCD_PIXELINDEX *)pTrans, NumEntries, BITS_PER_PIXEL);
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *) _aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, (d2_s32) Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  pRect = GUI_GetClipRect();
  d2_cliprect(d2_handle, pRect->x0, pRect->y0, pRect->x1, pRect->y1);
  d2_setblitsrc(d2_handle, (void *) p, BytesPerLine, xSize, ySize, ModeSrc);
  d2_settexclut_format(d2_handle, d2_mode_rgb565);
  d2_settexclut(d2_handle, (d2_color *)pTrans);
  d2_blitcopy(d2_handle, xSize, ySize, 0, 0, (d2_width) (xSize << 4), (d2_width) (ySize << 4), (d2_point) (x << 4), (d2_point) (y << 4), 0);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
}

/*********************************************************************
*
*       _DrawChar4bpp
*/
static int _DrawChar4bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 Mode;
  int ret;
  U32 NumBytes;
  U8 * pRD;
  U8 * pWR;
  d2_color Color;
  const GUI_RECT * pRect;

  GUI_USE_PARA(LayerIndex);
  pRect = GUI_GetClipRect();
  if (x < pRect->x0) {
    return 1;
  }
  if ((x + xSize - 1) > pRect->x1) {
    return 1;
  }
  if (y < pRect->y0) {
    return 1;
  }
  if ((y + ySize - 1) > pRect->y1) {
    return 1;
  }

  //
  // Check size of conversion buffer
  //
  NumBytes = (((xSize + 1) & (U32)~1) * ySize) >> 1;
  if ((NumBytes > sizeof(_aBuffer)) || (NumBytes == 0)) {
    return 1;
  }
  //
  // Swap nibbles
  //
  pWR = (U8 *)_aBuffer;
  pRD = (U8 *)p;
  do {
    *pWR++ = _aMirror[*pRD++];
  } while (--NumBytes);

  Mode = _GetD2Mode();
  d2_framebuffer(d2_handle, (void *) _aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, (d2_s32) Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);

  Color = GUI_GetColor();
  d2_setcolor(d2_handle, 0, Color);

  // Set D2 driver to replace RGB channels, preserving only alpha
  d2_settextureoperation(d2_handle, d2_to_copy, d2_to_replace, d2_to_replace, d2_to_replace);
  d2_settexopparam(d2_handle, d2_cc_red,    Color >> 16,              0);
  d2_settexopparam(d2_handle, d2_cc_green, (Color & 0x0000FF00) >> 8, 0);
  d2_settexopparam(d2_handle, d2_cc_blue,   Color & 0x000000FF,       0);

  // Set the graphics fill mode to use texture mapped data

  d2_u8 prevfillmode = d2_getfillmode(d2_handle);
  d2_setfillmode(d2_handle, d2_fm_texture);
  d2_cliprect(d2_handle, pRect->x0, pRect->y0, pRect->x1, pRect->y1);

  // Set texture buffer in D2 driver and assign CLUT
  d2_settexclut_format(d2_handle, d2_mode_i4);
  d2_settexclut(d2_handle, (d2_color *) clut_i4);
  d2_settexture(d2_handle, (void *) _aBuffer, BytesPerLine * 2, xSize, ySize, d2_mode_i4 | d2_mode_clut);

  // Set texture mapping for the following:
  //   - Map on the display starting at x, y
  //   - Start in the texture at 0, 0 (each char his its own texture)
  //   - For each pixel in the X and Y direction on the display move exactly one pixel in the respective
  //     direction in the texture
  //
  d2_settexturemapping(d2_handle, (d2_point)(x << 4), (d2_point)(y << 4), (0 << 16), (0 << 16), (1 << 16), 0, 0, (1 << 16));
  // Render a box the size of the character (it will be filled with the texture based on the configured
  //   parameters)
  //
  ret = d2_renderbox(d2_handle, (d2_point)(x << 4), (d2_point)(y << 4), (d2_width)(xSize << 4), (d2_width)(ySize << 4));
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  // Revert fill mode
  d2_setfillmode(d2_handle, prevfillmode);
  return ret;
}

#endif

/*********************************************************************
*
*       _CircleAA
*/
static int _CircleAA(int x0, int y0, int r, int w) {
  U32 Mode;
  int ret;
  GUI_RECT Rect;
  U32 Color;

  Mode = _GetD2Mode();
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  Color = GUI_Index2Color(GUI_GetColorIndex());
  d2_setcolor(d2_handle, 0, Color);
  _ModifyRect(GUI__GetClipRect(), &Rect);
  d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
  ret = d2_rendercircle(d2_handle, x0 * 16, y0 * 16, r * 16, w * 16);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  return ret;
}

/*********************************************************************
*
*       _FillCircleAA
*/
static int _FillCircleAA(int x0, int y0, int r) {
  _ModifyCoord(&x0, &y0);
  return _CircleAA(x0, y0, r, 0);
}

/*********************************************************************
*
*       _DrawCircleAA
*/
static int _DrawCircleAA(int x0, int y0, int r) {
  _ModifyCoord(&x0, &y0);
  return _CircleAA(x0, y0, r, GUI_GetPenSize());
}

#if (DISPLAY_ORIENTATION == ORIENTATION_0)
/*********************************************************************
*
*       _FillPolygonAA
*/
static int _FillPolygonAA(const GUI_POINT * pPoints, int NumPoints, int x0, int y0) {
  U32 Mode;
  d2_point * pData;
  d2_point * pDataI;
  int i, ret;
  GUI_RECT Rect;
  U32 Color;

  Mode = _GetD2Mode();
  pData = malloc(sizeof(d2_point) * NumPoints * 2);
  ret = 1;
  _ModifyCoord(&x0, &y0);
  if (pData) {
    pDataI = pData;
    for (i = 0; i < NumPoints; i++) {
      _ModifyCoord((int *)&pPoints->x, (int *)&pPoints->y);
      *pDataI++ = (d2_point)(pPoints->x + x0) * 16;
      *pDataI++ = (d2_point)(pPoints->y + y0) * 16;
      pPoints++;
    }
    //
    // Generate render operations
    //
    d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
    d2_selectrenderbuffer(d2_handle, renderbuffer);
    Color = GUI_Index2Color(GUI_GetColorIndex());
    d2_setcolor(d2_handle, 0, Color);
    _ModifyRect(GUI__GetClipRect(), &Rect);
    d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
    ret = d2_renderpolygon(d2_handle, pData, NumPoints, d2_le_closed);
    //
    // Execute render operations
    //
    d2_executerenderbuffer(d2_handle, renderbuffer, 0);
    d2_flushframe(d2_handle);
    free(pData);
  }
  return ret;
}

/*********************************************************************
*
*       _DrawPolyOutlineAA
*/
static int _DrawPolyOutlineAA(const GUI_POINT * pPoints, int NumPoints, int Thickness, int x, int y) {
  U32 Mode;
  d2_point * pData;
  d2_point * pDataI;
  int i, ret;
  GUI_RECT Rect;
  U32 Color;

  Mode = _GetD2Mode();
  pData = malloc(sizeof(d2_point) * NumPoints * 2);
  ret = 1;
  _ModifyCoord(&x, &y);
  if (pData) {
    pDataI = pData;
    for (i = 0; i < NumPoints; i++) {
      _ModifyCoord((int *)&pPoints->x, (int *)&pPoints->y);
      *pDataI++ = (d2_point)(pPoints->x + x) * 16;
      *pDataI++ = (d2_point)(pPoints->y + y) * 16;
      pPoints++;
    }
    //
    // Generate render operations
    //
    d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
    d2_selectrenderbuffer(d2_handle, renderbuffer);
    Color = GUI_Index2Color(GUI_GetColorIndex());
    d2_setcolor(d2_handle, 0, Color);
    _ModifyRect(GUI__GetClipRect(), &Rect);
    d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
    d2_selectrendermode(d2_handle, d2_rm_outline);
    ret = d2_renderpolyline(d2_handle, pData, NumPoints, Thickness * 16, d2_le_closed);
    d2_selectrendermode(d2_handle, d2_rm_solid);
    //
    // Execute render operations
    //
    d2_executerenderbuffer(d2_handle, renderbuffer, 0);
    d2_flushframe(d2_handle);
    free(pData);
  }
  return ret;
}
#endif
/*********************************************************************
*
*       _DrawLineAA
*/
static int _DrawLineAA(int x0, int y0, int x1, int y1) {
  U32 Mode;
  int ret;
  U32 Color;
  GUI_RECT Rect;

  Mode = _GetD2Mode();
  _ModifyCoord(&x0, &y0);
  _ModifyCoord(&x1, &y1);
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  Color = GUI_Index2Color(GUI_GetColorIndex());
  d2_setcolor(d2_handle, 0, Color);
  _ModifyRect(GUI__GetClipRect(), &Rect);
  d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
  ret = d2_renderline(d2_handle, x0 * 16, y0 * 16, x1 * 16, y1 * 16, GUI_GetPenSize() * 16, d2_le_exclude_none);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  return ret;
}
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
/*********************************************************************
*
*       _DrawArcAA
*/
int _DrawArcAA(int x0, int y0, int rx, int ry, int a0, int a1) {
  U32 Mode;
  int ret;
  I32 nx0, ny0, nx1, ny1;
  GUI_RECT Rect;
  U32 Color;
  U32 Flag;

  GUI_USE_PARA(ry);
  //
  // If both angles are equal (e.g. 0 and 0 or 180 and 180) nothing has to be done
  //
  if (a0 == a1) {
    return 0;  // Nothing to do, no angle - no arc
  }
  if (a1 < a0) {
    return 0;  // Nothing to do, emWin doesn't support this one
  }
  //
  // If the angles not equal, but meet at the same position
  // we don't draw an arc but a circle instead.
  //
  if (a1 > (a0 + 360000)) {
    return _DrawCircleAA(x0, y0, rx);  // a1 meets a0 after one round so we have a circle
  }
  if ((a0 % 360000) == (a1 % 360000)) {
    return _DrawCircleAA(x0, y0, rx);  // Both angles are at the same position but not equal, so we have a circle
  }

  Mode = _GetD2Mode();
  _ModifyCoord(&x0, &y0);
  nx0  = -GUI__SinHQ(a0);
  ny0  = -GUI__CosHQ(a0);
  nx1  = GUI__SinHQ(a1);
  ny1  = GUI__CosHQ(a1);
  //
  // If the difference between both is larger than 180 degrees we must use the concave flag
  //
  if (((a1 - a0) % 360000) <= 180000) {
    Flag = 0;
  } else {
    Flag = d2_wf_concave;
  }
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferPTR[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  Color = GUI_Index2Color(GUI_GetColorIndex());
  d2_setcolor(d2_handle, 0, Color);
  _ModifyRect(GUI__GetClipRect(), &Rect);
  d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
  ret = d2_renderwedge(d2_handle, x0 * 16, y0 * 16, rx * 16, GUI_GetPenSize() * 16, nx0, ny0, nx1, ny1, Flag);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  return ret;
}
#endif
#endif

/*********************************************************************
*
*       _CopyBuffer
*/
static void _CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  int ret;
  dmaca_transfer_data_cfg_t td_cfg;
  dmaca_stat_t dmac_status;

  GUI_USE_PARA(LayerIndex);
  ret = R_DMACA_Open(DMACA_CH0);
  if (ret != DMACA_SUCCESS) {
    while (1);  // Error
  }
  td_cfg.transfer_mode        = DMACA_TRANSFER_MODE_BLOCK;
  td_cfg.repeat_block_side    = DMACA_REPEAT_BLOCK_DISABLE;
  td_cfg.data_size            = DMACA_DATA_SIZE_LWORD;
  td_cfg.act_source           = (dmaca_activation_source_t)0;
  td_cfg.request_source       = DMACA_TRANSFER_REQUEST_SOFTWARE;
  td_cfg.dtie_request         = DMACA_TRANSFER_END_INTERRUPT_DISABLE;
  td_cfg.esie_request         = DMACA_TRANSFER_ESCAPE_END_INTERRUPT_DISABLE;
  td_cfg.rptie_request        = DMACA_REPEAT_SIZE_END_INTERRUPT_DISABLE;
  td_cfg.sarie_request        = DMACA_SRC_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
  td_cfg.darie_request        = DMACA_DES_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
  td_cfg.src_addr_mode        = DMACA_SRC_ADDR_INCR;
  td_cfg.src_addr_repeat_area = DMACA_SRC_ADDR_EXT_REP_AREA_NONE;
  td_cfg.des_addr_mode        = DMACA_DES_ADDR_INCR;
  td_cfg.des_addr_repeat_area = DMACA_DES_ADDR_EXT_REP_AREA_NONE;
  td_cfg.offset_value         = BYTES_PER_LINE;
  td_cfg.interrupt_sel        = DMACA_CLEAR_INTERRUPT_FLAG_BEGINNING_TRANSFER;
  td_cfg.p_src_addr           = (void *)_aBufferPTR[IndexSrc];
  td_cfg.p_des_addr           = (void *)_aBufferPTR[IndexDst];
  td_cfg.transfer_count       = YSIZE_PHYS;
  td_cfg.block_size           = XSIZE_PHYS >> 1;
  ret = R_DMACA_Create(DMACA_CH0, &td_cfg);
  if (ret != DMACA_SUCCESS) {
    while (1);  // Error
  }
  ret = R_DMACA_Control(DMACA_CH0, DMACA_CMD_ENABLE, &dmac_status);
  if (ret != DMACA_SUCCESS) {
    while (1);  // Error
  }
  ret = R_DMACA_Control(DMACA_CH0, DMACA_CMD_SOFT_REQ_NOT_CLR_REQ, &dmac_status);
  if (ret != DMACA_SUCCESS) {
    while (1);  // Error
  }
  do {
    ret = R_DMACA_Control(DMACA_CH0, DMACA_CMD_STATUS_GET, &dmac_status);
    if (ret != DMACA_SUCCESS) {
      while (1);  // Error
    }
  } while((dmac_status.dtif_stat) == 0);
  ret = R_DMACA_Close(DMACA_CH0);
  if (ret != DMACA_SUCCESS) {
    while (1);  // Error
  }
  _WriteBufferIndex = IndexDst;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCDCONF_GetBufferAddr
*/
void * LCDCONF_GetBufferAddr(void) {
  return (void *)_aBufferPTR[_WriteBufferIndex];
}

/*********************************************************************
*
*       LCDCONF_GetD2
*/
d2_device * LCDCONF_GetD2(void) {
  return d2_handle;
}

/*********************************************************************
*
*       LCDCONF_EnableDave2D
*/
void LCDCONF_EnableDave2D(void) {
#if (USE_DAVE2D)
  #if (ORIENTATION_0 == DISPLAY_ORIENTATION)
  GUI_SetFuncDrawAlpha         (_DrawMemdevAlpha, _DrawBitmapAlpha);
  GUI_SetFuncDrawM565          (_DrawMemdevAlpha, _DrawBitmapAlpha);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))_DrawBitmap16bpp);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_8BPP, (void (*)(void))_DrawBitmap8bpp);
  GUI_AA_SetFuncDrawPolyOutline(_DrawPolyOutlineAA);
  GUI_AA_SetFuncDrawArc        (_DrawArcAA);
  GUI_AA_SetFuncFillPolygon    (_FillPolygonAA);
#endif
  GUI_AA_SetFuncFillCircle     (_FillCircleAA);
  GUI_AA_SetFuncDrawCircle     (_DrawCircleAA);
  GUI_AA_SetFuncDrawLine       (_DrawLineAA);
  GUI_AA_SetpfDrawCharAA4      (_DrawChar4bpp);
  LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_FillRect);
  GUI_AlphaEnableFillRectHW(1);
  _DaveActive = 1;
#endif
}

/*********************************************************************
*
*       LCDCONF_DisableDave2D
*/
void LCDCONF_DisableDave2D(void) {
#if (USE_DAVE2D)
#if (ORIENTATION_0 == DISPLAY_ORIENTATION)
  GUI_SetFuncDrawAlpha         (NULL, NULL);
  GUI_SetFuncDrawAlpha         (NULL, NULL);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))NULL);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_8BPP, (void (*)(void))NULL);
  GUI_AA_SetFuncDrawPolyOutline(NULL);
  GUI_AA_SetFuncDrawArc        (NULL);
  GUI_AA_SetFuncFillPolygon    (NULL);
#endif
  GUI_AA_SetFuncFillCircle     (NULL);
  GUI_AA_SetFuncDrawCircle     (NULL);
  GUI_AA_SetFuncDrawLine       (NULL);
  GUI_AA_SetpfDrawCharAA4      (NULL);
  LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))NULL);
  GUI_AlphaEnableFillRectHW(0);
  _DaveActive = 0;
#endif
}

/*********************************************************************
*
*       LCDCONF_DisableDave2D
*/
unsigned LCDCONF_GetDaveActive(void) {
  return _DaveActive;
}

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
  static const U32 _aBufferPTRGUI[] = {
    BUFFER_ADDR_0 - MEMORY_OFFSET,
	BUFFER_ADDR_1 - MEMORY_OFFSET
  };

  #if (NUM_BUFFERS > 1)
  GUI_MULTIBUF_ConfigEx(0, NUM_BUFFERS);
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  //
  // Display driver configuration
  //
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, VXSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, VXSIZE_PHYS, YSIZE_PHYS);
  }
  LCD_SetBufferPtrEx(0, (void **)_aBufferPTRGUI);
  //
  // Initialize Dave2D
  //
  d2_handle = d2_opendevice(0);
  d2_inithw(d2_handle, 0);
  renderbuffer = d2_newrenderbuffer(d2_handle, 20, 20);
  //
  // Set function pointers
  //
  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_CopyBuffer);
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

  GUI_USE_PARA(LayerIndex);
  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    _InitController();
#if (USE_DAVE2D)
      LCDCONF_EnableDave2D();
#endif
    return 0;
  }
  case LCD_X_SETLUTENTRY: {
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
    LCD_X_SETLUTENTRY_INFO * p;

    p = (LCD_X_SETLUTENTRY_INFO *)pData;
    _SetLUTEntry(p->Color, p->Pos);
    return 0;
  }
  case LCD_X_SHOWBUFFER: {
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.
    //
    LCD_X_SHOWBUFFER_INFO * p;

    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    _SwitchBuffersOnVSYNC(p->Index);
    break;
  }
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on and off
    //
    _DisplayOnOff(1);
    break;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching on and off
    //
    _DisplayOnOff(0);
    break;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
