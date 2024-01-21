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
Purpose     : Display controller configuration

This LCDConf.c has been written to be used on a Renesas EK-RA6M3G.
It can't be used standalone since it relies on libraries provided by
Renesas, but can be used as a reference.

It shows how to drive the Dave2D drawing engine in combination with
emWin.

---------------------------END-OF-HEADER------------------------------
*/
#include "gpx_thread.h"
#include "bsp_pin_cfg.h"
#include "graphics.h"

#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "LCDConf.h"

#include "GUI_Private.h"

#include <stdlib.h>
#include <string.h>

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS LCD_HPIX
#define YSIZE_PHYS LCD_VPIX

#define BITS_PER_PIXEL DISPLAY_BITS_PER_PIXEL_INPUT0  // Allowed values: 1, 4, 8, 16, 32

#define ORIENTATION_0    0
#define ORIENTATION_CW   1
#define ORIENTATION_180  2
#define ORIENTATION_CCW  3

#define DISPLAY_ORIENTATION  ORIENTATION_0
//#define DISPLAY_ORIENTATION  ORIENTATION_CW
//#define DISPLAY_ORIENTATION  ORIENTATION_180
//#define DISPLAY_ORIENTATION  ORIENTATION_CCW

/*********************************************************************
*
*       Determine driver, color conversion
*
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
#elif (BITS_PER_PIXEL == 8)
  #define COLOR_CONVERSION GUICC_M233
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
#endif

//
// Check if the Dave2D engine can be used
//
#if (DISPLAY_ORIENTATION == 0)
  #define USE_DAVE2D  1
#else
  #define USE_DAVE2D  0
#endif

#define USE_FLAG_FOR_MB 1

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VRAM_ADDR
  #define VRAM_ADDR (void *)&fb_background[0][0]
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#if (USE_FLAG_FOR_MB == 1)
static volatile int         _PendingBuffer = -1;
#endif
#if (BITS_PER_PIXEL < 16)
  #define NUM_COLORS (1 << BITS_PER_PIXEL)
  static U32 _aClut[NUM_COLORS];
#else
  #define NUM_COLORS  256
#endif
//
// Dave2D
//
static U32               _DaveActive;
static U32               _WriteBufferIndex;
static d2_renderbuffer * renderbuffer;
static U32               _FB_BufferSize;
static U32               _aBufferAddress[NUM_BUFFERS];
//
// D2 driver handle
//
static d2_device * d2_handle;

//
// Forward declaration
//
static void _LCD_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex);

volatile int TimeA, TimeD;

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

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       vsync_callback
*/
void vsync_callback(display_callback_args_t *p_args) {
  GUI_USE_PARA(p_args);
#if USE_FLAG_FOR_MB
  _PendingBuffer = 0;
#else
  BaseType_t context_switch;
  //
  // Set Vsync semaphore
  //
  if (p_args->event == DISPLAY_EVENT_LINE_DETECTION) {
    xSemaphoreGiveFromISR(g_vsync_semaphore, &context_switch);
  }
  //
  // Return to the highest priority available task
  //
  portYIELD_FROM_ISR(context_switch);
#endif
}

/*********************************************************************
*
*       _SwitchBuffersOnVSYNC
*/
#if (NUM_BUFFERS == 2)

static void _SwitchBuffersOnVSYNC(int Index) {
#if USE_FLAG_FOR_MB
  TimeA = GUI_GetTime();
  _PendingBuffer = 1;
  while (_PendingBuffer);
#else
  xSemaphoreTake(g_vsync_semaphore, portMAX_DELAY);
#endif
  TimeD = GUI_GetTime() - TimeA;
  R_GLCDC_BufferChange(g_display0.p_ctrl, fb_background[Index], 0);
  GUI_MULTIBUF_ConfirmEx(0, Index);  // Tell emWin that buffer is used
}
#endif

/*********************************************************************
*
*       graphics_init
*/
static void graphics_init (void) {
  //
  // Initialize D/AVE 2D driver
  //
  d2_handle = d2_opendevice(0);
  d2_inithw(d2_handle, 0);
  //
  // Clear both buffers
  //
  d2_framebuffer(d2_handle, fb_background, LCD_HPIX, LCD_STRIDE, LCD_VPIX * 2, d2_mode_rgb565);
  d2_clear(d2_handle, 0x000000);
  //
  // Set various D2 parameters
  //
  d2_setblendmode(d2_handle, d2_bm_alpha, d2_bm_one_minus_alpha);
  d2_setalphamode(d2_handle, d2_am_constant);
  d2_setalpha(d2_handle, 0xff);
  d2_setantialiasing(d2_handle, 1);
  d2_setlinecap(d2_handle, d2_lc_butt);
  d2_setlinejoin(d2_handle, d2_lj_none);
  //
  // Start graphics LCD controller
  //
  R_GLCDC_Open(g_display0.p_ctrl, g_display0.p_cfg);
  R_GLCDC_Start(g_display0.p_ctrl);
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
  display_clut_cfg_t p_clut_cfg;
  int err;

  _aClut[Pos] = Color;
  if (Pos == NUM_COLORS - 1) {
    p_clut_cfg.p_base = _aClut;
    p_clut_cfg.size   = NUM_COLORS;
    p_clut_cfg.start  = 0;
    GUI_Delay(100);
    err = R_GLCDC_ClutUpdate(g_display0.p_ctrl, &p_clut_cfg, DISPLAY_FRAME_LAYER_1);
    if (err != FSP_SUCCESS) {
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
*       Dave2D related code
*
**********************************************************************
*/
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
    d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
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
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
}

/*********************************************************************
*
*       _DrawBitmapAlpha
*/
static void _DrawBitmapAlpha(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  U32 Mode;

  GUI_USE_PARA(LayerIndex);
  Mode     = _GetD2Mode();
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  d2_setblitsrc(d2_handle, (void *)p, BytesPerLine / 4, xSize, ySize, d2_mode_argb8888);
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
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
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
  d2_framebuffer(d2_handle, (void *) _aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, (d2_s32) Mode);
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
  d2_framebuffer(d2_handle, (void *) _aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, (d2_s32) Mode);
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
  U32      Mode;
  int      ret;
  d2_color Color;
  GUI_RECT Rect;

  Mode = _GetD2Mode();
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  Color = GUI_GetColor();
  d2_setcolor(d2_handle, 0, Color);
  _ModifyRect(GUI__GetClipRect(), &Rect);
  d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
  ret = d2_rendercircle(d2_handle, x0 * 16, y0 * 16, r * 16, w * 16);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  while (d2_commandspending(d2_handle));
  return ret;
}

/*********************************************************************
*
*       _FillCircleAA
*/
static int _FillCircleAA(int x0, int y0, int r) {
  return _CircleAA(x0, y0, r, 0);
}

/*********************************************************************
*
*       _DrawCircleAA
*/
static int _DrawCircleAA(int x0, int y0, int r) {
  U16 PenSize;

  PenSize = GUI_GetPenSize();
  return _CircleAA(x0, y0, r, PenSize);
}

/*********************************************************************
*
*       _FillPolygonAA
*/
static int _FillPolygonAA(const GUI_POINT * pPoints, int NumPoints, int x0, int y0) {
  U32        Mode;
  d2_point * pData;
  d2_point * pDataI;
  int        i;
  int        ret;
  d2_color   Color;
  GUI_RECT   Rect;

  Mode = _GetD2Mode();
  pData = malloc(sizeof(d2_point) * NumPoints * 2);
  ret = 1;
  _ModifyCoord(&x0, &y0);
  if (pData) {
    pDataI = pData;
    for (i = 0; i < NumPoints; i++) {
      *pDataI++ = (d2_point)(pPoints->x + x0) * 16;
      *pDataI++ = (d2_point)(pPoints->y + y0) * 16;
      pPoints++;
    }
    //
    // Generate render operations
    //
    d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
    d2_selectrenderbuffer(d2_handle, renderbuffer);
    Color = GUI_GetColor();
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
  U32        Mode;
  d2_point * pData;
  d2_point * pDataI;
  int        i;
  int        ret;
  d2_color   Color;
  GUI_RECT   Rect;

  Mode = _GetD2Mode();
  pData = malloc(sizeof(d2_point) * NumPoints * 2);
  ret = 1;
  if (pData) {
    pDataI = pData;
    for (i = 0; i < NumPoints; i++) {
      *pDataI++ = (d2_point)(pPoints->x + x) * 16;
      *pDataI++ = (d2_point)(pPoints->y + y) * 16;
      pPoints++;
    }
    //
    // Generate render operations
    //
    d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
    d2_selectrenderbuffer(d2_handle, renderbuffer);
    Color = GUI_GetColor();
    d2_setcolor(d2_handle, 0, Color);
    _ModifyRect(GUI_GetClipRect(), &Rect);
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

/*********************************************************************
*
*       _DrawLineAA
*/
static int _DrawLineAA(int x0, int y0, int x1, int y1) {
  U32      Mode;
  int      ret;
  d2_color Color;
  GUI_RECT Rect;
  U16      PenSize;

  Mode    = _GetD2Mode();
  PenSize = GUI_GetPenSize();
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  Color = GUI_GetColor();
  d2_setcolor(d2_handle, 0, Color);
  _ModifyRect(GUI_GetClipRect(), &Rect);
  d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
  ret = d2_renderline(d2_handle, x0 * 16, y0 * 16, x1 * 16, y1 * 16, PenSize * 16, d2_le_exclude_none);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  return ret;
}

/*********************************************************************
*
*       _DrawArcAA
*/
static int _DrawArcAA(int x0, int y0, int rx, int ry, int a0, int a1) {
  U32      Mode;
  int      ret;
  I32      nx0;
  I32      ny0;
  I32      nx1;
  I32      ny1;
  d2_color Color;
  GUI_RECT Rect;
  U16      PenSize;

  GUI_USE_PARA(ry);
  PenSize = GUI_GetPenSize();
  Mode = _GetD2Mode();
  while (a1 < a0) {
    a1 += 360000;
  }
  nx0  = GUI__SinHQ(a0);
  ny0  = GUI__CosHQ(a0);
  nx1  = GUI__SinHQ(a1);
  ny1  = GUI__CosHQ(a1);
  if ((a1 - a0) <= 180000) {
    nx0 *= -1;
    ny0 *= -1;
  }
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  Color = GUI_GetColor();
  d2_setcolor(d2_handle, 0, Color);
  _ModifyRect(GUI_GetClipRect(), &Rect);
  d2_cliprect(d2_handle, Rect.x0, Rect.y0, Rect.x1, Rect.y1);
  ret = d2_renderwedge(d2_handle, x0 * 16, y0 * 16, rx * 16, PenSize * 16, nx0, ny0, nx1, ny1, 0);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  return ret;
}

/*********************************************************************
*
*       _CopyBuffer
*/
static void _CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  U32 Mode;

  GUI_USE_PARA(LayerIndex);
  Mode     = _GetD2Mode();
  //
  // Generate render operations
  //
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[IndexDst], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_selectrenderbuffer(d2_handle, renderbuffer);
  d2_setblitsrc(d2_handle, (void *)_aBufferAddress[IndexSrc], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
  d2_blitcopy(d2_handle, XSIZE_PHYS, YSIZE_PHYS, 0, 0, XSIZE_PHYS * 16, YSIZE_PHYS * 16, 0, 0, 0);
  //
  // Execute render operations
  //
  d2_executerenderbuffer(d2_handle, renderbuffer, 0);
  d2_flushframe(d2_handle);
  d2_framebuffer(d2_handle, (void *)_aBufferAddress[_WriteBufferIndex], XSIZE_PHYS, XSIZE_PHYS, YSIZE_PHYS, Mode);
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
*       LCDCONF_EnableDave2D
*/
void LCDCONF_EnableDave2D(void) {
#if (BITS_PER_PIXEL > 8)
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  GUI_SetFuncDrawAlpha         (_DrawMemdevAlpha, _DrawBitmapAlpha);
  GUI_SetFuncDrawM565          (_DrawMemdevAlpha, _DrawBitmapAlpha);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))_DrawBitmap16bpp);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_8BPP, (void (*)(void))_DrawBitmap8bpp);
#endif
  GUI_AA_SetFuncFillCircle     (_FillCircleAA);
  GUI_AA_SetFuncFillPolygon    (_FillPolygonAA);
  GUI_AA_SetFuncDrawCircle     (_DrawCircleAA);
  GUI_AA_SetFuncDrawLine       (_DrawLineAA);
  GUI_AA_SetFuncDrawPolyOutline(_DrawPolyOutlineAA);
  GUI_AA_SetFuncDrawArc        (_DrawArcAA);
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
#if (BITS_PER_PIXEL > 8)
#if (DISPLAY_ORIENTATION == ORIENTATION_0)
  GUI_SetFuncDrawAlpha         (NULL, NULL);
  GUI_SetFuncDrawM565          (NULL, NULL);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))NULL);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_8BPP, (void (*)(void))NULL);
#endif
  GUI_AA_SetFuncFillCircle     (NULL);
  GUI_AA_SetFuncFillPolygon    (NULL);
  GUI_AA_SetFuncDrawCircle     (NULL);
  GUI_AA_SetFuncDrawLine       (NULL);
  GUI_AA_SetFuncDrawPolyOutline(NULL);
  GUI_AA_SetFuncDrawArc        (NULL);
  GUI_AA_SetpfDrawCharAA4      (NULL);
  LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))NULL);
  GUI_AlphaEnableFillRectHW(0);
  _DaveActive = 0;
#endif
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
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, g_display0.p_cfg->input->hstride);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, g_display0.p_cfg->input->hstride, YSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, VRAM_ADDR);
  //
  // Set user palette data (only required if no fixed palette is used)
  //
  #if defined(PALETTE)
    LCD_SetLUTEx(0, PALETTE);
  #endif
  renderbuffer = d2_newrenderbuffer(d2_handle, 20, 20);
  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_CopyBuffer);
  LCDCONF_EnableDave2D();
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
    // Controller initialiiation is done before calling GUI_Init() so we simply store the frame buffer addresses
    //
    _FB_BufferSize = sizeof(fb_background[0]);
    _aBufferAddress[0] = (U32)fb_background[0];
#if (NUM_BUFFERS > 1)
    _aBufferAddress[1] = (U32)fb_background[1];
#endif
    return 0;
  }
  case LCD_X_SHOWBUFFER: {
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.
    //
    LCD_X_SHOWBUFFER_INFO * p;

    p = (LCD_X_SHOWBUFFER_INFO *)pData;
#if (NUM_BUFFERS == 2)
    _SwitchBuffersOnVSYNC(p->Index);
#else
    //_PendingBuffer = p->Index;
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
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on and off
    //
    return 0;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching on and off
    //
    // ...
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*********************************************************************
*
*       gpx_thread_entry
*/
void gpx_thread_entry(void *pvParameters) {
  FSP_PARAMETER_NOT_USED (pvParameters);
  //
  // Initialize graphics HW blocks
  //
  graphics_init();
  //
  // End this task
  //
  vTaskDelete(NULL);
}

/*************************** End of file ****************************/
