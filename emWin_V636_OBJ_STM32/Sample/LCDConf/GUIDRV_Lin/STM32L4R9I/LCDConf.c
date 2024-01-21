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
File    : LCDConf.c
Purpose : Configuration file for emWin.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "LCDConf.h"
#include "GUI.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"
#include "stm32L4xx_hal.h"
#include "stm32l4r9i_discovery_io.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       Common
*/
#define LCD_BITS_PER_PIXEL  32
//
// Physical display size
//
#define LCD_SIZE      390 

//
// Buffers and virtual screens
//
#define NUM_BUFFERS     1 // Number of multiple buffers to be used
#define NUM_VSCREENS    1 // Number of virtual screens to be used

//
// Number of a layers
//
#define NUM_LAYERS   1



#include "stm32l4r9i_discovery_gfxmmu_lut.h"

#if (LCD_BITS_PER_PIXEL == 32)
#define DISPLAY_DRIVER_0     GUIDRV_LIN_32
#define COLOR_CONVERSION_0   GUICC_M8888I
#elif (LCD_BITS_PER_PIXEL == 24)
  #define DISPLAY_DRIVER_0     GUIDRV_LIN_24
  #define COLOR_CONVERSION_0   GUICC_M888
#elif (LCD_BITS_PER_PIXEL == 16)
  #define DISPLAY_DRIVER_0     GUIDRV_LIN_16
  #define COLOR_CONVERSION_0   GUICC_M565
#endif

#if defined ( __ICCARM__ )  /* IAR Compiler */
  #pragma data_alignment = 16
static U8              _aVRAM[LCD_PHYS_FB_SIZE];
#elif defined (__GNUC__)    /* GNU Compiler */

static U32              _aVRAM[LCD_PHYS_FB_SIZE / 4] __attribute__ ((aligned (16)));
#else                       /* ARM Compiler */
__align(16) static U8  _aVRAM[LCD_PHYS_FB_SIZE];
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  U32                 address;          
  I32                 pending_buffer;   
  I32                 buffer_index;     
  U32                 xSize;            
  U32                 ySize;            
  U32                 BytesPerPixel;
  LCD_API_COLOR_CONV *pColorConvAPI;
} LCD_LayerPropTypedef;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
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

extern U32 bsp_psram_initialized;
static LCD_LayerPropTypedef layer_prop[NUM_LAYERS];
static volatile int LCD_ActiveRegion;
static int LCD_IsInitialized;
static GFXMMU_HandleTypeDef hgfxmmu;
static LTDC_HandleTypeDef   hltdc;
static DSI_HandleTypeDef    hdsi;
static int _PendingBuffer;
static void * _apBufferAddress[] = {
  (void *)GFXMMU_VIRTUAL_BUFFER0_BASE,
  (void *)GFXMMU_VIRTUAL_BUFFER0_BASE
};

static const LCD_API_COLOR_CONV * _apColorConvAPI[] = {
  COLOR_CONVERSION_0,
#if NUM_LAYERS == 2
  COLOR_CONVERSION_1,
#endif
};

static volatile U32 _aBuffer[40 * 40];  // Only required for drawing AA4 characters
//
// DMA2D transfer is ready flag
//
static volatile int _WaitForDMA2D;

#if (defined(__GNUC__))
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

/*********************************************************************
*
*       Local code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetPixelformat
*/
static U32 _GetPixelformat(int LayerIndex) {
  const LCD_API_COLOR_CONV * pColorConvAPI;

  if ((unsigned)LayerIndex >= GUI_COUNTOF(_apColorConvAPI)) {
    return 0;
  }
  pColorConvAPI = _apColorConvAPI[LayerIndex];
  if        (pColorConvAPI == GUICC_M8888I) {
    return LTDC_PIXEL_FORMAT_ARGB8888;
  } else if (pColorConvAPI == GUICC_M888  ) {
    return LTDC_PIXEL_FORMAT_RGB888;
  } else if (pColorConvAPI == GUICC_M565  ) {
    return LTDC_PIXEL_FORMAT_RGB565;
  } else if (pColorConvAPI == GUICC_M1555I) {
    return LTDC_PIXEL_FORMAT_ARGB1555;
  } else if (pColorConvAPI == GUICC_M4444I) {
    return LTDC_PIXEL_FORMAT_ARGB4444;
  } else if (pColorConvAPI == GUICC_8666  ) {
    return LTDC_PIXEL_FORMAT_L8;
  } else if (pColorConvAPI == GUICC_1616I ) {
    return LTDC_PIXEL_FORMAT_AL44;
  } else if (pColorConvAPI == GUICC_88666I) {
    return LTDC_PIXEL_FORMAT_AL88;
  }
  while (1); // Error
}

/*********************************************************************
*
*       _DMA_ExecOperation
*/
static void _DMA_ExecOperation(void) {
  //
  // Set Flag which gets cleared when DMA2D transfer is completed
  //
  _WaitForDMA2D = 1;
  //
  // Execute operation
  //
  DMA2D->CR     |= 1;                               // Control Register (Start operation)
  //
  // Wait until transfer is done
  //
  while (_WaitForDMA2D) {
  }
}

/*********************************************************************
*
*       _DMA_Fill
*/
static void _DMA_Fill(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex) {
#if 1
  //
  // Fills with observing alpha value in ColorIndex. Works only (as far as I can see)
  // with STM32L4S whose ChromART accelerator supports 6 instead of 4 operating modes
  //
  U32 PixelFormat;
  
  PixelFormat    = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00040000UL | (1 << 9);        
  DMA2D->FGCOLR  = ColorIndex;
  DMA2D->FGPFCCR = PixelFormat | (ColorIndex & 0xFF000000) | (1 << 16);
  DMA2D->BGMAR   = (U32)pDst;
  DMA2D->BGOR    = OffLine;
  DMA2D->OMAR    = (U32)pDst;
  DMA2D->OOR     = OffLine;
  DMA2D->OPFCCR  = PixelFormat;
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize;  
  _DMA_ExecOperation();
#else
  //
  // Simply filling with ColorIndex. Works with all versions of ChromART accelerator
  //
  U32 PixelFormat;
  
  PixelFormat    = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00030000UL | (1 << 9);        
  DMA2D->OCOLR   = ColorIndex;                     
  DMA2D->OMAR    = (U32)pDst;                      
  DMA2D->OOR     = OffLine;
  DMA2D->OPFCCR  = PixelFormat;
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize;  
  _DMA_ExecOperation();
#endif
}

/*********************************************************************
*
*       _DMA_DrawAlphaBitmap
*/
static void _DMA_DrawAlphaBitmap(void * pDst, const void * pSrc, int xSize, int ySize, int OffLineSrc, int OffLineDst, int PixelFormat) {
  DMA2D->CR      = 0x00020000UL | (1 << 9);         // Control Register (Memory to memory with blending of FG and BG and TCIE)
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->BGMAR   = (U32)pDst;                       // Background Memory Address Register (Destination address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  DMA2D->FGOR    = OffLineSrc;                      // Foreground Offset Register (Source line offset)
  DMA2D->BGOR    = OffLineDst;                      // Background Offset Register (Destination line offset)
  DMA2D->OOR     = OffLineDst;                      // Output Offset Register (Destination line offset)
  DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;      // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->BGPFCCR = PixelFormat;                     // Background PFC Control Register (Defines the destination pixel format)
  DMA2D->OPFCCR  = PixelFormat;                     // Output     PFC Control Register (Defines the output pixel format)
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  _DMA_ExecOperation();
}

/*********************************************************************
*
*       _DMA_DrawBitmap
*/
static void _DMA_DrawBitmap(void * pDst, const void * pSrc, int xSize, int ySize, int OffLineSrc, int OffLineDst, int PixelFormatSrc, int PixelFormatDst) {
  DMA2D->CR      = 0x00010000UL | (1 << 9);         // Control Register (Memory-to-memory with PFC and TCIE)
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->BGMAR   = (U32)pDst;                       // Background Memory Address Register (Destination address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  DMA2D->FGOR    = OffLineSrc;                      // Foreground Offset Register (Source line offset)
  DMA2D->BGOR    = OffLineDst;                      // Background Offset Register (Destination line offset)
  DMA2D->OOR     = OffLineDst;                      // Output Offset Register (Destination line offset)
  DMA2D->FGPFCCR = PixelFormatSrc;                  // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->OPFCCR  = PixelFormatDst;                  // Output     PFC Control Register (Defines the output pixel format)
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  _DMA_ExecOperation();
}

/*********************************************************************
*
*       _DMA_Copy
*/
static void _DMA_Copy(int LayerIndex, const void * pSrc, void * pDst, int xSize, int ySize, int OffLineSrc, int OffLineDst) {
  U32 PixelFormat;

  PixelFormat    = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00000000UL | (1 << 9);         // Control Register (Memory to memory and TCIE)
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  DMA2D->FGOR    = OffLineSrc;                      // Foreground Offset Register (Source line offset)
  DMA2D->OOR     = OffLineDst;                      // Output Offset Register (Destination line offset)
  DMA2D->FGPFCCR = PixelFormat;                     // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  _DMA_ExecOperation();
}


/*********************************************************************
*
*       _DMA_DrawBitmapL8
*/
static void _DMA_DrawBitmapL8(void * pSrc, void * pDst,  U32 OffSrc, U32 OffDst, U32 PixelFormatDst, U32 xSize, U32 ySize) {
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00010000UL | (1 << 9);         // Control Register (Memory to memory with pixel format conversion and TCIE)
  //
  // Set up pointers
  //
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  //
  // Set up offsets
  //
  DMA2D->FGOR    = OffSrc;                          // Foreground Offset Register (Source line offset)
  DMA2D->OOR     = OffDst;                          // Output Offset Register (Destination line offset)
  //
  // Set up pixel format
  //
  DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_L8;             // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->OPFCCR  = PixelFormatDst;                  // Output PFC Control Register (Defines the output pixel format)
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(xSize << 16) | ySize;      // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
  _DMA_ExecOperation();
}

/*********************************************************************
*
*       _DMA_DrawBitmapA4
*/
static int _DMA_DrawBitmapA4(void * pSrc, void * pDst,  U32 OffSrc, U32 OffDst, U32 PixelFormatDst, U32 xSize, U32 ySize) {
  volatile U8 * pRD;
  volatile U8 * pWR;
  volatile U32 NumBytes, Color, Index;

  //
  // Check size of conversion buffer
  //
  NumBytes = (((xSize + 1) & ~1) * ySize) >> 1;
  if ((NumBytes > sizeof(_aBuffer)) || (NumBytes == 0)) {
    return 1;
  }
  //
  // Conversion (swapping nibbles)
  //
  pWR = (U8 *)_aBuffer;
  pRD = (U8 *)pSrc;
  do {
    *pWR++ = _aMirror[*pRD++];
  } while (--NumBytes);
  //
  // Get current drawing color (ABGR)
  //
  Index = LCD_GetColorIndex();
  Color = LCD_Index2Color(Index);
  //
  // Set up operation mode
  //
  DMA2D->CR = 0x00020000UL;
  //
  // Set up source
  //
#if (GUI_USE_ARGB == 0)
  DMA2D->FGCOLR  = ((Color & 0xFF) << 16)  // Red
                 |  (Color & 0xFF00)       // Green
                 | ((Color >> 16) & 0xFF); // Blue
#else
  DMA2D->FGCOLR  = Color;
#endif
  DMA2D->CR      = 0x00020000UL | (1 << 9);         // Control Register (Memory to memory with pixel format conversion and TCIE)
  DMA2D->FGMAR   = (U32)_aBuffer;
  DMA2D->FGOR    = 0;
  DMA2D->FGPFCCR = 0xA;                    // A4 bitmap
  DMA2D->NLR     = (U32)((xSize + OffSrc) << 16) | ySize;
  DMA2D->BGMAR   = (U32)pDst;
  DMA2D->BGOR    = OffDst - OffSrc;
  DMA2D->BGPFCCR = PixelFormatDst;
  DMA2D->OMAR    = DMA2D->BGMAR;
  DMA2D->OOR     = DMA2D->BGOR;
  DMA2D->OPFCCR  = DMA2D->BGPFCCR;
  //
  // Execute operation
  //
  _DMA_ExecOperation();
  return 0;
}

/*********************************************************************
*
*       _GetBufferSize
*/
static U32 _GetBufferSize(U32 LayerIndex) {
  GUI_USE_PARA(LayerIndex);
  return sizeof(_aVRAM);
}

/*********************************************************************
*
*       _LCD_CopyBuffer
*/
static void _LCD_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {

#if NUM_BUFFERS < 2
  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(IndexSrc);
  GUI_USE_PARA(IndexDst);
#else
  U32 BufferSize, AddrSrc, AddrDst;
  
  BufferSize = _GetBufferSize(LayerIndex);
  AddrSrc    = layer_prop[LayerIndex].address + BufferSize * IndexSrc;
  AddrDst    = layer_prop[LayerIndex].address + BufferSize * IndexDst;
  _DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, layer_prop[LayerIndex].xSize, layer_prop[LayerIndex].ySize, 0, 0);
  layer_prop[LayerIndex].buffer_index = IndexDst;
#endif
}

/*********************************************************************
*
*       _LCD_CopyRect
*/
static void _LCD_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) {
  U32 BufferSize, AddrSrc, AddrDst;
  
  BufferSize = _GetBufferSize(LayerIndex);
  AddrSrc = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].pending_buffer + (y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].pending_buffer + (y1 * layer_prop[LayerIndex].xSize + x1) * layer_prop[LayerIndex].BytesPerPixel;
  _DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, xSize, ySize, layer_prop[LayerIndex].xSize - xSize, 0);
}

/*********************************************************************
*
*       _LCD_FillRect
*/
static void _LCD_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  U32 BufferSize, AddrDst;
  int xSize, ySize;
  
  if (GUI_GetDrawMode() == GUI_DM_XOR) {		
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_FillRect);
  } else {
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    BufferSize = _GetBufferSize(LayerIndex);
    AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;
    _DMA_Fill(LayerIndex, (void *)AddrDst, xSize, ySize, layer_prop[LayerIndex].xSize - xSize, PixelIndex);
  }	
}

/*********************************************************************
*
*       _LCD_DrawBitmapM565
*/
static void _LCD_DrawBitmapM565(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;
  U32 PixelFormatDst;

  PixelFormatDst = _GetPixelformat(LayerIndex);
  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst    = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 2) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  _DMA_DrawBitmap((void *)AddrDst, p, xSize, ySize, OffLineSrc, OffLineDst, LTDC_PIXEL_FORMAT_RGB565, PixelFormatDst);
}

/*********************************************************************
*
*       _LCD_DrawBitmapAlpha
*/
static void _LCD_DrawBitmapAlpha(int LayerIndex, int x, int y, void const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 4) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  _DMA_DrawAlphaBitmap((void *)AddrDst, p, xSize, ySize, OffLineSrc, OffLineDst, PixelFormat);
}

/*********************************************************************
*
*       _LCD_DrawBitmap32bpp
*/
static void _LCD_DrawBitmap32bpp(int LayerIndex, int x, int y, U16 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 4) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  _DMA_Copy(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}

/*********************************************************************
*
*       _LCD_DrawBitmap16bpp
*/
static void _LCD_DrawBitmap16bpp(int LayerIndex, int x, int y, U16 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 2) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  _DMA_Copy(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}

/*********************************************************************
*
*       _LCD_DrawBitmap8bpp
*/
static void _LCD_DrawBitmap8bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = BytesPerLine - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  _DMA_DrawBitmapL8((void *)p, (void *)AddrDst, OffLineSrc, OffLineDst, PixelFormat, xSize, ySize);
}

/*********************************************************************
*
*       _LCD_DrawBitmap4bpp
*/
static int _LCD_DrawBitmap4bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;
  U32 PixelFormat;

  if (x < GUI_pContext->ClipRect.x0) {
    return 1;
  }
  if ((x + xSize - 1) > GUI_pContext->ClipRect.x1) {
    return 1;
  }
  if (y < GUI_pContext->ClipRect.y0) {
    return 1;
  }
  if ((y + ySize - 1) > GUI_pContext->ClipRect.y1) {
    return 1;
  }
  PixelFormat = _GetPixelformat(LayerIndex);
  //
  // Check if destination has direct color mode
  //
  if (PixelFormat > LTDC_PIXEL_FORMAT_ARGB4444) {
    return 1;
  }
  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine * 2) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  return _DMA_DrawBitmapA4((void *)p, (void *)AddrDst, OffLineSrc, OffLineDst, PixelFormat, xSize, ySize);;
}

/*********************************************************************
*
*       _LCD_DrawMemdevAlpha
*/
static void _LCD_DrawMemdevAlpha(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  int OffLineSrc, OffLineDst;

  OffLineSrc = (BytesPerLineSrc / 4) - xSize;
  OffLineDst = (BytesPerLineDst / 4) - xSize;
  _DMA_DrawAlphaBitmap(pDst, pSrc, xSize, ySize, OffLineSrc, OffLineDst, LTDC_PIXEL_FORMAT_ARGB8888);
}

/*********************************************************************
*
*       _LCD_DrawMemdevM565
*
* Purpose:
*   Copy data with conversion
*/
static void _LCD_DrawMemdevM565(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  int OffLineSrc, OffLineDst;

  OffLineSrc = (BytesPerLineSrc / 2) - xSize;
  OffLineDst = (BytesPerLineDst / 4) - xSize;
  _DMA_DrawBitmap(pDst, pSrc, xSize, ySize, OffLineSrc, OffLineDst, LTDC_PIXEL_FORMAT_RGB565, LTDC_PIXEL_FORMAT_ARGB8888);
}

#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW

/*********************************************************************
*
*       _DMA_CopyRGB565
*/
static void _DMA_CopyRGB565(const void * pSrc, void * pDst, int xSize, int ySize, int OffLineSrc, int OffLineDst) {
  DMA2D->CR      = 0x00000000UL | (1 << 9);         // Control Register (Memory to memory and TCIE)
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  DMA2D->FGOR    = OffLineSrc;                      // Foreground Offset Register (Source line offset)
  DMA2D->OOR     = OffLineDst;                      // Output Offset Register (Destination line offset)
  DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;        // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  DMA2D->CR     |= DMA2D_CR_START;   
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
  }
}

/*********************************************************************
*
*       _LCD_DrawMemdev16bpp
*/
static void _LCD_DrawMemdev16bpp(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  int OffLineSrc, OffLineDst;

  OffLineSrc = (BytesPerLineSrc / 2) - xSize;
  OffLineDst = (BytesPerLineDst / 2) - xSize;
  _DMA_CopyRGB565(pSrc, pDst, xSize, ySize, OffLineSrc, OffLineDst);
}
#endif

/*********************************************************************
*
*       _DMA_MixColors
*
* Purpose:
*   Function for mixing up 2 colors with the given intensity.
*   If the background color is completely transparent the
*   foreground color should be used unchanged.
*/
static LCD_COLOR _DMA_MixColors(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens) {
  volatile U32 ColorDst;

#if (GUI_USE_ARGB == 0)
  Color   ^= 0xFF000000;
  BkColor ^= 0xFF000000;
#endif
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00020000UL | (1 << 9);         // Control Register (Memory to memory with blending of FG and BG and TCIE)
  //
  // Set up pointers
  //
  DMA2D->FGMAR   = (U32)&Color;                   // Foreground Memory Address Register
  DMA2D->BGMAR   = (U32)&BkColor;                   // Background Memory Address Register
  DMA2D->OMAR    = (U32)&ColorDst;                  // Output Memory Address Register (Destination address)
  //
  // Set up pixel format
  //
  DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_ARGB8888
                 | (1UL << 16)
                 | ((U32)Intens << 24);
  DMA2D->BGPFCCR = LTDC_PIXEL_FORMAT_ARGB8888
                 | (0UL << 16)
                 | ((U32)(255 - Intens) << 24);
  DMA2D->OPFCCR  = LTDC_PIXEL_FORMAT_ARGB8888;
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(1 << 16) | 1;              // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
  DMA2D->CR     |= 1;                               // Control Register (Start operation)
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
  }
#if (GUI_USE_ARGB == 0)
  ColorDst ^= 0xFF000000;
#endif
  return ColorDst;
}

/*********************************************************************
*
*       _DMA_LoadLUT
*/
static void _DMA_LoadLUT(LCD_COLOR * pColor, U32 NumItems) {
  DMA2D->FGCMAR  = (U32)pColor;                     // Foreground CLUT Memory Address Register
  //
  // Foreground PFC Control Register
  //
  DMA2D->FGPFCCR  = LTDC_PIXEL_FORMAT_RGB888        // Pixel format
                  | ((NumItems - 1) & 0xFF) << 8;   // Number of items to load
  DMA2D->FGPFCCR |= (1 << 5);                       // Start loading
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
  }
}

/*********************************************************************
*
*       _DMA2D_ITConfig
*/
static void _DMA2D_ITConfig(U32 DMA2D_IT, int NewState) {
  if (NewState != DISABLE) {
    DMA2D->CR |= DMA2D_IT;
  } else {
    DMA2D->CR &= (U32)~DMA2D_IT;
  }
}

/*********************************************************************
*
*       _LCD_MspInit
*/
static void _LCD_MspInit(void) {
  __HAL_RCC_GFXMMU_CLK_ENABLE();
  __HAL_RCC_GFXMMU_FORCE_RESET();
  __HAL_RCC_GFXMMU_RELEASE_RESET();
  __HAL_RCC_LTDC_CLK_ENABLE();
  __HAL_RCC_LTDC_FORCE_RESET();
  __HAL_RCC_LTDC_RELEASE_RESET();
  __HAL_RCC_DMA2D_CLK_ENABLE();
  __HAL_RCC_DMA2D_FORCE_RESET();
  __HAL_RCC_DMA2D_RELEASE_RESET();
  __HAL_RCC_DSI_CLK_ENABLE();
  __HAL_RCC_DSI_FORCE_RESET();
  __HAL_RCC_DSI_RELEASE_RESET();

  // Configure the clock for the LTDC
  // We want DSI PHI at 500MHz
  // We have only one line => 500Mbps
  // With 24bits per pixel, equivalent PCLK is 500/24 = 20.8MHz
  // We will set PCLK at 15MHz
  // Following values are OK with HSI = 16MHz
  // (4*60)/(1*4*4) = 15MHz
  RCC_PeriphCLKInitTypeDef  PeriphClkInit;
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInit.PLLSAI2.PLLSAI2Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI2.PLLSAI2M = 1;
  PeriphClkInit.PLLSAI2.PLLSAI2N = 15;
  PeriphClkInit.PLLSAI2.PLLSAI2R = RCC_PLLR_DIV4;
  PeriphClkInit.LtdcClockSelection = RCC_LTDCCLKSOURCE_PLLSAI2_DIV4;
  PeriphClkInit.PLLSAI2.PLLSAI2ClockOut = RCC_PLLSAI2_LTDCCLK;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    while(1);
  }
  //
  // Enable HSE used for DSI PLL
  //
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  if(RCC_OscInitStruct.HSEState == RCC_HSE_OFF) {
    GPIO_InitTypeDef  GPIO_InitStruct;
    __HAL_RCC_GPIOH_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pin       = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_0, GPIO_PIN_RESET);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
      while(1);
    }
  }
  HAL_NVIC_SetPriority(LTDC_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);
  HAL_NVIC_SetPriority(LTDC_ER_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(LTDC_ER_IRQn);
  HAL_NVIC_SetPriority(DSI_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DSI_IRQn);
}

/*********************************************************************
*
*       _LCD_PowerOn
*/
static void _LCD_PowerOn(void) {
  if (bsp_psram_initialized == 0) {
    BSP_IO_Init();
    BSP_IO_WritePin(IO_PIN_8, GPIO_PIN_RESET);
    BSP_IO_ConfigPin(IO_PIN_8, IO_MODE_OUTPUT);
    HAL_Delay(1);
    BSP_IO_WritePin(AGPIO_PIN_2, GPIO_PIN_RESET);
    BSP_IO_ConfigPin(AGPIO_PIN_2, IO_MODE_OUTPUT);
    HAL_Delay(15);
  }
  BSP_IO_ConfigPin(IO_PIN_10, IO_MODE_OUTPUT);
  BSP_IO_WritePin(IO_PIN_10, GPIO_PIN_SET);
  HAL_Delay(120);
}

/*********************************************************************
*
*       _LCD_Init
*/
static U8 _LCD_Init(void) {
  U8 InitParam1[4]= {0x00, 0x04, 0x01, 0x89}; // MODIF OFe: adjusted w/ real image
  U8 InitParam2[4]= {0x00, 0x00, 0x01, 0x85};
  LTDC_LayerCfgTypeDef    LayerCfg;
  DSI_PLLInitTypeDef      dsiPllInit;
  DSI_PHY_TimerTypeDef    PhyTimings;
  DSI_HOST_TimeoutTypeDef HostTimeouts;
  DSI_LPCmdTypeDef        LPCmd;
  DSI_CmdCfgTypeDef       CmdCfg;
  U8 ScanLineParams[2];
  U16 scanline = (390 - 10);

  if(LCD_IsInitialized == 0) {

    _LCD_PowerOn();
    _LCD_MspInit();
    //
    // Init GFXMMU
    //
    hgfxmmu.Instance = GFXMMU;
    __HAL_GFXMMU_RESET_HANDLE_STATE(&hgfxmmu);
    hgfxmmu.Init.BlocksPerLine                     = GFXMMU_192BLOCKS;
    hgfxmmu.Init.DefaultValue                      = 0x0;
    hgfxmmu.Init.Buffers.Buf0Address               = (U32) _aVRAM;
    hgfxmmu.Init.Buffers.Buf1Address               = 0; /* NU */
    hgfxmmu.Init.Buffers.Buf2Address               = 0; /* NU */
    hgfxmmu.Init.Buffers.Buf3Address               = 0; /* NU */
    hgfxmmu.Init.Interrupts.Activation             = DISABLE;
    hgfxmmu.Init.Interrupts.UsedInterrupts         = GFXMMU_AHB_MASTER_ERROR_IT; /* NU */
    if(HAL_OK != HAL_GFXMMU_Init(&hgfxmmu)) {
      return(1);
    }
    if(HAL_OK != HAL_GFXMMU_ConfigLut(&hgfxmmu, 0, 390 * 2, (U32) gfxmmu_lut_config)) {
      return(1);
    }
    if(HAL_OK != HAL_GFXMMU_DisableLutLines(&hgfxmmu, 390, 634)) {
      return(1);
    }
    //
    // Init LTDC
    //
    hltdc.Instance = LTDC;
    __HAL_LTDC_RESET_HANDLE_STATE(&hltdc);
    hltdc.Init.HSPolarity         = LTDC_HSPOLARITY_AL;
    hltdc.Init.VSPolarity         = LTDC_VSPOLARITY_AL;
    hltdc.Init.DEPolarity         = LTDC_DEPOLARITY_AL;
    hltdc.Init.PCPolarity         = LTDC_PCPOLARITY_IPC;
    hltdc.Init.HorizontalSync     = 0;   // HSYNC width - 1
    hltdc.Init.VerticalSync       = 0;   // VSYNC width - 1
    hltdc.Init.AccumulatedHBP     = 1;   // HSYNC width + HBP - 1
    hltdc.Init.AccumulatedVBP     = 1;   // VSYNC width + VBP - 1
    hltdc.Init.AccumulatedActiveW = 391; // HSYNC width + HBP + Active width - 1
    hltdc.Init.AccumulatedActiveH = 391; // VSYNC width + VBP + Active height - 1
    hltdc.Init.TotalWidth         = 392; // HSYNC width + HBP + Active width + HFP - 1
    hltdc.Init.TotalHeigh         = 392; // VSYNC width + VBP + Active height + VFP - 1
    hltdc.Init.Backcolor.Red      = 0;
    hltdc.Init.Backcolor.Green    = 0;
    hltdc.Init.Backcolor.Blue     = 0;
    hltdc.Init.Backcolor.Reserved = 0xFF;
    if(HAL_LTDC_Init(&hltdc) != HAL_OK) {
      return(1);
    }
    //
    // Init layer
    //
    LayerCfg.WindowX0        = 0;
    LayerCfg.WindowX1        = 390;
    LayerCfg.WindowY0        = 0;
    LayerCfg.WindowY1        = 390;
#if (LCD_BITS_PER_PIXEL == 16)
    LayerCfg.PixelFormat     = LTDC_PIXEL_FORMAT_RGB565;
#elif (LCD_BITS_PER_PIXEL == 24)
    LayerCfg.PixelFormat     = LTDC_PIXEL_FORMAT_RGB888;
#elif (LCD_BITS_PER_PIXEL == 32)
    LayerCfg.PixelFormat     = LTDC_PIXEL_FORMAT_ARGB8888;
#endif
    LayerCfg.ImageWidth      = LCD_IMAGE_WIDTH;
    LayerCfg.Alpha           = 0xFF; /* NU default value */
    LayerCfg.Alpha0          = 0; /* NU default value */
    LayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA; /* NU default value */
    LayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA; /* NU default value */
    LayerCfg.FBStartAdress   = GFXMMU_VIRTUAL_BUFFER0_BASE;
    LayerCfg.ImageHeight     = 390;
    LayerCfg.Backcolor.Red   = 0; /* NU default value */
    LayerCfg.Backcolor.Green = 0; /* NU default value */
    LayerCfg.Backcolor.Blue  = 0; /* NU default value */
    LayerCfg.Backcolor.Reserved = 0xFF;
    if(HAL_LTDC_ConfigLayer(&hltdc, &LayerCfg, 0) != HAL_OK)
    {
      return(1);
    }
    __HAL_LTDC_LAYER_ENABLE(&hltdc, 0);
    //
    // Init DSI
    //
    hdsi.Instance = DSI;
    __HAL_DSI_RESET_HANDLE_STATE(&hdsi);
    hdsi.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
    // We have 1 data lane at 500Mbps => lane byte clock at 500/8 = 62,5 MHZ
    // We want TX escape clock at arround 20MHz and under 20MHz so clock division is set to 4
    hdsi.Init.TXEscapeCkdiv = 4;
    hdsi.Init.NumberOfLanes = DSI_ONE_DATA_LANE;
    // We have HSE value at 16 Mhz and we want data lane at 500Mbps
    dsiPllInit.PLLNDIV = 125;
    dsiPllInit.PLLIDF  = DSI_PLL_IN_DIV4;
    dsiPllInit.PLLODF  = DSI_PLL_OUT_DIV1;
    if(HAL_DSI_Init(&hdsi, &dsiPllInit) != HAL_OK) {
      return(1);
    }

    PhyTimings.ClockLaneHS2LPTime  = 33; // Tclk-post + Tclk-trail + Ths-exit = [(60ns + 52xUI) + (60ns) + (300ns)]/16ns
    PhyTimings.ClockLaneLP2HSTime  = 30; // Tlpx + (Tclk-prepare + Tclk-zero) + Tclk-pre = [150ns + 300ns + 8xUI]/16ns
    PhyTimings.DataLaneHS2LPTime   = 11; // Ths-trail + Ths-exit = [(60ns + 4xUI) + 100ns]/16ns
    PhyTimings.DataLaneLP2HSTime   = 21; // Tlpx + (Ths-prepare + Ths-zero) + Ths-sync = [150ns + (145ns + 10xUI) + 8xUI]/16ns
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime        = 7;
    if(HAL_DSI_ConfigPhyTimer(&hdsi, &PhyTimings) != HAL_OK) {
      return(1);
    }

    HostTimeouts.TimeoutCkdiv                 = 1;
    HostTimeouts.HighSpeedTransmissionTimeout = 0;
    HostTimeouts.LowPowerReceptionTimeout     = 0;
    HostTimeouts.HighSpeedReadTimeout         = 0;
    HostTimeouts.LowPowerReadTimeout          = 0;
    HostTimeouts.HighSpeedWriteTimeout        = 0;
    HostTimeouts.HighSpeedWritePrespMode      = 0;
    HostTimeouts.LowPowerWriteTimeout         = 0;
    HostTimeouts.BTATimeout                   = 0;
    if(HAL_DSI_ConfigHostTimeouts(&hdsi, &HostTimeouts) != HAL_OK) {
      return(1);
    }

    LPCmd.LPGenShortWriteNoP  = DSI_LP_GSW0P_ENABLE;
    LPCmd.LPGenShortWriteOneP = DSI_LP_GSW1P_ENABLE;
    LPCmd.LPGenShortWriteTwoP = DSI_LP_GSW2P_ENABLE;
    LPCmd.LPGenShortReadNoP   = DSI_LP_GSR0P_ENABLE;
    LPCmd.LPGenShortReadOneP  = DSI_LP_GSR1P_ENABLE;
    LPCmd.LPGenShortReadTwoP  = DSI_LP_GSR2P_ENABLE;
    LPCmd.LPGenLongWrite      = DSI_LP_GLW_DISABLE;
    LPCmd.LPDcsShortWriteNoP  = DSI_LP_DSW0P_ENABLE;
    LPCmd.LPDcsShortWriteOneP = DSI_LP_DSW1P_ENABLE;
    LPCmd.LPDcsShortReadNoP   = DSI_LP_DSR0P_ENABLE;
    LPCmd.LPDcsLongWrite      = DSI_LP_DLW_DISABLE;
    LPCmd.LPMaxReadPacket     = DSI_LP_MRDP_DISABLE;
    LPCmd.AcknowledgeRequest  = DSI_ACKNOWLEDGE_DISABLE;
    if(HAL_DSI_ConfigCommand(&hdsi, &LPCmd) != HAL_OK) {
      return(1);
    }

    CmdCfg.VirtualChannelID      = 0;
    CmdCfg.ColorCoding           = DSI_RGB888;
    CmdCfg.CommandSize           = 390;
    CmdCfg.TearingEffectSource   = DSI_TE_DSILINK;
    CmdCfg.TearingEffectPolarity = DSI_TE_FALLING_EDGE;
    CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_LOW;
    CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_LOW;
    CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
    CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
    CmdCfg.AutomaticRefresh      = DSI_AR_ENABLE;
    CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_ENABLE;
    if(HAL_DSI_ConfigAdaptedCommandMode(&hdsi, &CmdCfg) != HAL_OK) {
      return(1);
    }
    if(HAL_DSI_ConfigFlowControl(&hdsi, DSI_FLOW_CONTROL_BTA) != HAL_OK) {
      return(1);
    }
    __HAL_DSI_ENABLE(&hdsi);
    //
    // Init sequence
    //
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x01);
    // IC Frame rate control, set power, sw mapping, mux swithc timing command
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x06, 0x62);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0E, 0x80);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0F, 0x80);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x10, 0x71);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x13, 0x81);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x14, 0x81);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x15, 0x82);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x16, 0x82);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x18, 0x88);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x19, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1A, 0x10);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1C, 0x99);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1D, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1E, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1F, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x20, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x25, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x26, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2A, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2B, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x36, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x37, 0x10);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3A, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3B, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3D, 0x20);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3F, 0x3A);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x40, 0x30);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x41, 0x1A);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x42, 0x33);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x43, 0x22);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x44, 0x11);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x45, 0x66);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x46, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x47, 0x44);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4C, 0x33);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4D, 0x22);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4E, 0x11);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4F, 0x66);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x50, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x51, 0x44);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x57, 0x33);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x6B, 0x1B);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x70, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x74, 0x0C);
    // Go to command 3
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x02);
    // Set the VGMP/VGSP coltage control
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9B, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9C, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9D, 0x20);
    // Go to command 4
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x03);
    // Set the VGMP/VGSP coltage control
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9B, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9C, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9D, 0x20);
    // Go to command 5
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x04);
    // VSR command
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x5D, 0x10);
    // VSR1 timing set
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x01, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x02, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x03, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x04, 0x10);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x05, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x06, 0xA7);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x07, 0x20);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x08, 0x00);
    // VSR2 timing set
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x09, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0A, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0B, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0C, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0D, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0E, 0x06);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0F, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x10, 0xA7);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x11, 0x00);
    // VSR3 timing set
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x12, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x13, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x14, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x15, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x16, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x17, 0x07);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x18, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x19, 0xA7);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1A, 0x00);
    // VSR4 timing set
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1B, 0x82);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1C, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1D, 0xFF);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1E, 0x05);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1F, 0x60);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x20, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x21, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x22, 0x7C);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x23, 0x00);
    // VSR5 timing set
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x24, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x25, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x26, 0x04);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x27, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x28, 0x70);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x29, 0x05);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2A, 0x74);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2B, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2D, 0x00);
    // VSR6 timing set
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2F, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x30, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x31, 0x04);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x32, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x33, 0x70);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x34, 0x07);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x35, 0x74);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x36, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x37, 0x00);
    // VSR marping command
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x5E, 0x20);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x5F, 0x31);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x60, 0x54);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x61, 0x76);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x62, 0x98);
    // Go to command 6
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x05);
    // Set the ELVSS voltage
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x05, 0x17);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2A, 0x04);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x91, 0x00);
    // Go back in standard commands
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x00);
    // Set tear off
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, DSI_SET_TEAR_OFF, 0x0);
    // Set DSI mode to internal timing added vs ORIGINAL for Command mode
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC2, 0x0);
    // Set memory address MODIFIED vs ORIGINAL
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_COLUMN_ADDRESS, InitParam1);
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_PAGE_ADDRESS, InitParam2);
    // Sleep out
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, DSI_EXIT_SLEEP_MODE, 0x0);
    HAL_Delay(120);
    // Set display on
    if(HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, DSI_SET_DISPLAY_ON, 0x0) != HAL_OK) {
      return(1);
    }
    // Enable DSI Wrapper
    __HAL_DSI_WRAPPER_ENABLE(&hdsi);
    ScanLineParams[0] = scanline >> 8;
    ScanLineParams[1] = scanline & 0x00FF;
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 2, DSI_SET_TEAR_SCANLINE, ScanLineParams);
    // Set Tearing On
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, DSI_SET_TEAR_ON, 0x00);
    LCD_IsInitialized = 1;
    _DMA2D_ITConfig(DMA2D_CR_TCIE | DMA2D_CR_TEIE, ENABLE);
    HAL_NVIC_SetPriority(DMA2D_IRQn, 0x8, 0x0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);  
  }
  return(0);
}

/*********************************************************************
*
*       _LCD_DisplayOn
*/
static void _LCD_DisplayOn(void) {
  HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, DSI_SET_DISPLAY_ON, 0x0);
}

/*********************************************************************
*
*       Interrupt handler
*
**********************************************************************
*/
void DMA2D_IRQHandler(void);
void DMA2D_IRQHandler(void) {
  DMA2D->IFCR = (U32)DMA2D_IFCR_CTCIF;
  DMA2D->IFCR = (U32)DMA2D_IFCR_CCTCIF;
  /* Release the DMA2D for the next transfer */
  _WaitForDMA2D = 0;
}

void DSIHSOT_IRQHandler(void);
void DSIHSOT_IRQHandler(void) {
  HAL_DSI_IRQHandler(&(hdsi));
}

void LCD_TFT_IRQHandler(void);
void LCD_TFT_IRQHandler(void) {
  HAL_LTDC_IRQHandler(&(hltdc));
}

void LCD_TFT_ER_IRQHandler(void);
void LCD_TFT_ER_IRQHandler(void) {
  HAL_LTDC_IRQHandler(&(hltdc));
}

/*********************************************************************
*
*       _LCD_GetpPalConvTable
*
* Purpose:
*   The emWin function LCD_GetpPalConvTable() normally translates the given colors into
*   index values for the display controller. In case of index based bitmaps without
*   transparent pixels we load the palette only to the DMA2D LUT registers to be
*   translated (converted) during the process of drawing via DMA2D.
*/
static LCD_PIXELINDEX * _LCD_GetpPalConvTable(const LCD_LOGPALETTE GUI_UNI_PTR * pLogPal, const GUI_BITMAP GUI_UNI_PTR * pBitmap, int LayerIndex) {
  void (* pFunc)(void);
  int DoDefault = 0;

  //
  // Check if we have a non transparent device independent bitmap
  //
  if (pBitmap->BitsPerPixel == 8) {
    pFunc = LCD_GetDevFunc(LayerIndex, LCD_DEVFUNC_DRAWBMP_8BPP);
    if (pFunc) {
      if (pBitmap->pPal) {
        if (pBitmap->pPal->HasTrans) {
          DoDefault = 1;
        }
      } else {
        DoDefault = 1;
      }
    } else {
      DoDefault = 1;
    }
  } else {
    DoDefault = 1;
  }
  //
  // Default palette management for other cases
  //
  if (DoDefault) {
    //
    // Return a pointer to the index values to be used by the controller
    //
    return LCD_GetpPalConvTable(pLogPal);
  }
  //
  // Load LUT using DMA2D
  //
  _DMA_LoadLUT((U32 *)pLogPal->pPalEntries, pLogPal->NumEntries);
  //
  // Return something not NULL
  //
  return (LCD_PIXELINDEX *)pLogPal->pPalEntries;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       HAL_DSI_EndOfRefreshCallback
*/
void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *phdsi) {
  GUI_USE_PARA(phdsi);
  LCD_ActiveRegion = 0;
}

/*********************************************************************
*
*       LCD_X_Config
*/
void LCD_X_Config(void) {
  int i;
  //
  // Configure multibuffering
  //
#if (NUM_BUFFERS > 1)
  for (i = 0; i < NUM_LAYERS; i++) {
    GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
  }
#else
  GUI_MULTIBUF_ConfigEx(0, 2);
  
#endif
  //
  // Create display driver for layer 0 and link it into the device chain
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  //
  // Set size of layer 0
  //
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx (0, LCD_SIZE, LCD_SIZE);
    LCD_SetVSizeEx(0, LCD_SIZE * NUM_VSCREENS, LCD_IMAGE_WIDTH);
  } else {
    LCD_SetSizeEx (0, LCD_SIZE, LCD_SIZE);
    LCD_SetVSizeEx(0, LCD_IMAGE_WIDTH, LCD_SIZE);
  }
  LCD_SetBufferPtrEx(0, _apBufferAddress);
#if (NUM_LAYERS > 1)
  //
  // Create display driver for layer 1 and link it into the device chain
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 1);
  //
  // Set size of layer 1
  //
  if (LCD_GetSwapXYEx(1)) {
    LCD_SetSizeEx (1, LCD_SIZE, LCD_SIZE);
    LCD_SetVSizeEx(1, LCD_SIZE * NUM_VSCREENS, LCD_IMAGE_WIDTH);
  } else {
    LCD_SetSizeEx (1, LCD_SIZE, LCD_SIZE);
    LCD_SetVSizeEx(1, LCD_IMAGE_WIDTH, LCD_SIZE);
  }
#endif
  //
  // Init layer struct for layer 0
  //
  layer_prop[0].address = GFXMMU_VIRTUAL_BUFFER0_BASE;
#if (NUM_LAYERS > 1)    
  //
  // Init layer struct for layer 1
  //
  layer_prop[1].address = GFXMMU_VIRTUAL_BUFFER1_BASE; 
#endif
  //
  // Set up custom functions
  //
  for (i = 0; i < NUM_LAYERS; i++) {
    layer_prop[i].xSize = LCD_IMAGE_WIDTH;
    layer_prop[i].ySize = LCD_SIZE;
    layer_prop[i].pColorConvAPI  = (LCD_API_COLOR_CONV *)_apColorConvAPI[i];    
    layer_prop[i].pending_buffer = -1;    
    layer_prop[i].BytesPerPixel  = LCD_GetBitsPerPixelEx(i) >> 3;
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER,    (void(*)(void))_LCD_CopyBuffer);
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT,      (void(*)(void))_LCD_CopyRect);
    LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT,      (void(*)(void))_LCD_FillRect);
    LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_8BPP,  (void(*)(void))_LCD_DrawBitmap8bpp);
    LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))_LCD_DrawBitmap16bpp);  
    LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_32BPP, (void(*)(void))_LCD_DrawBitmap32bpp); 
  } 
#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW
  GUI_MEMDEV_SetDrawMemdev16bppFunc(_LCD_DrawMemdev16bpp);
#endif
  GUI_SetFuncMixColors(_DMA_MixColors);
  GUI_AA_SetpfDrawCharAA4(_LCD_DrawBitmap4bpp);
  GUI_SetFuncDrawAlpha(_LCD_DrawMemdevAlpha, _LCD_DrawBitmapAlpha);
  GUI_SetFuncDrawM565(_LCD_DrawMemdevM565, _LCD_DrawBitmapM565);
  GUI_AlphaEnableFillRectHW(1);
  //
  // Set up custom function for translating a bitmap palette into index values.
  // Required to load a bitmap palette into DMA2D CLUT in case of a 8bpp indexed bitmap
  //
  GUI_SetFuncGetpPalConvTable(_LCD_GetpPalConvTable);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  LCD_X_SHOWBUFFER_INFO * pInfo;
  int r = 0;

  GUI_USE_PARA(LayerIndex);
  switch (Cmd) {   
  case LCD_X_INITCONTROLLER:
    r = _LCD_Init();
    break;
  case LCD_X_SHOWBUFFER:
    pInfo = (LCD_X_SHOWBUFFER_INFO *)pData;
    LCD_ActiveRegion = 1;
    _PendingBuffer = pInfo->Index;  
    HAL_DSI_Refresh(&hdsi);
    for (;LCD_ActiveRegion;);
    GUI_MULTIBUF_ConfirmEx(0, _PendingBuffer);
    break;
  case LCD_X_ON:
    _LCD_DisplayOn();
    break;
  default:
    r = -1;
  }
  return r;
}

/*********************************************************************
*
*       LCD_RefreshDSI
*/
void LCD_RefreshDSI(void) {
  HAL_DSI_Refresh(&hdsi);
}

#if (defined(__GNUC__))
#pragma GCC pop_options
#endif

/*************************** End of file ****************************/
