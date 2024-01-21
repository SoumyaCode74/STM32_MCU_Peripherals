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

#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "LCDConf.h"
#include "PIDConf.h"

#include "MIMXRT1176_cm7.h"

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_clock.h"
#include "fsl_mipi_dsi.h"
#include "fsl_pxp.h"
#include "clock_config.h"

#include "fsl_mipi_dsi.h"
#include "fsl_rm68200.h"

#include "fsl_dc_fb_lcdifv2.h"

/*********************************************************************
*
*       OS configuration
*
**********************************************************************
*/
#define USE_OS  (1)

#if (USE_OS == 1)
  #define EMBOS     (1)
  #define FREERTOS  (2)
  #define OS EMBOS

  #if   (OS == EMBOS)
    #include "RTOS.h"
    #define  SEMAPHORE_TYPE                OS_SEMAPHORE
    #define  SEMAPHORE_HANDLE_TYPE         OS_SEMAPHORE *
    #define  MAKE_SEMAPHORE_HANDLE(sema)   &sema
  #elif (OS == FREERTOS)
    #include "FreeRTOS.h"
    #include "semphr.h"
    #define  SEMAPHORE_TYPE                SemaphoreHandle_t
    #define  SEMAPHORE_HANDLE_TYPE         SemaphoreHandle_t
    #define  MAKE_SEMAPHORE_HANDLE(sema)   sema
  #else
    #error "No RTOS selected"
  #endif
#endif

/*********************************************************************
*
*       Supported orientation modes (not to be changed)
*/
#define ROTATION_0         0
#define ROTATION_CW        1
#define ROTATION_180       2
#define ROTATION_CCW       3

/*********************************************************************
*
*       LCD configuration (TO BE MODIFIED)
*
**********************************************************************
*/
//
// 16 or 32 bpp supported.
//
#define LCD_BITS_PER_PIXEL   32

//
// Buffers / VScreens
//
#define NUM_BUFFERS  2 // Number of multiple buffers to be used (at least 1 buffer)
#define NUM_VSCREENS 1 // Number of virtual  screens to be used (at least 1 screen)

//
// Define number of layers for this configuration file. Must be equal or less than in GUIConf.h!
//
#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 1

//
// Default display orientation. Should be one of the above defined display orientations.
//
#define ORIENTATION     ROTATION_CCW

//
// Allow run-time display rotation. Necessary for AppWizard ROTATEDISPLAY job.
//
#define ENABLE_DISPLAY_ROTATION   1

//
// Some emWin operations require running the PXP multiple times. For those cases,
// a temporary buffer is needed. Set the below define to 1 to use a persistent buffer
// as the temporary buffer without the need for dynamic memory allocations (improves performance).
// It is not recommended to turn this, given that enough memory is available.
//
#define USE_PXP_BUFFER    1

/*********************************************************************
*
*       LCD configuration (NOT TO BE MODIFIED)
*
**********************************************************************
*/
#define LCD_BYTES_PER_PIXEL    (LCD_BITS_PER_PIXEL / 8)
//
// Physical display size (not to be changed).
//
#define XSIZE_PHYS 720
#define YSIZE_PHYS 1280
//
// Porch values
//
#define HSW    8
#define HFP    32
#define HBP    32
#define VSW    2
#define VFP    16
#define VBP    14

#define LCDIF_POL_FLAGS  (kLCDIFV2_DataEnableActiveHigh | kLCDIFV2_VsyncActiveLow | kLCDIFV2_HsyncActiveLow | \
                               kLCDIFV2_DriveDataOnFallingClkEdge)

#define MIPI_DSI          (&g_mipiDsi)
#define MIPI_DSI_LANE_NUM 2
//
// Mipi pins
//
#define MIPI_PANEL_RST_GPIO   GPIO9
#define MIPI_PANEL_RST_PIN    1
#define MIPI_PANEL_POWER_GPIO GPIO11
#define MIPI_PANEL_POWER_PIN  16
//
// Backlight
//
#define MIPI_PANEL_BL_GPIO GPIO9
#define MIPI_PANEL_BL_PIN  29

#define MIPI_DPHY_BIT_CLK_ENLARGE(origin) (((origin) / 8) * 9)
//
// Master read priority
//
#define PXP_READ_QOS    *((U32 *)0x41043100)

/*********************************************************************
*
*       Automatic selection of driver and color conversion
*/
#if LCD_BITS_PER_PIXEL == 16
  #define COLOR_CONVERSION    GUICC_M565
  #define PIXEL_FORMAT        kVIDEO_PixelFormatRGB565
  #define LCD_DATA_BUS_WIDTH  kLCDIFV2_DataBus16Bit
  #define PXP_OUT_FORMAT      kPXP_OutputPixelFormatRGB565
  #define PXP_PS_FORMAT       kPXP_PsPixelFormatRGB565
  #define PXP_AS_FORMAT       kPXP_AsPixelFormatRGB565
  //
  #if   (ORIENTATION == ROTATION_0)
    #define DISPLAY_DRIVER      GUIDRV_LIN_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSX_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OXY_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSY_16
  #elif (ORIENTATION == ROTATION_CW)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OSX_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OXY_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OSY_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_16
  #elif (ORIENTATION == ROTATION_180)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OXY_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSY_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSX_16
  #elif (ORIENTATION == ROTATION_CCW)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OSY_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OSX_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OXY_16
  #endif
#else
  #define COLOR_CONVERSION    GUICC_M8888I
  #define PIXEL_FORMAT        kVIDEO_PixelFormatXRGB8888
  #define LCD_DATA_BUS_WIDTH  kLCDIFV2_DataBus16Bit
  #define PXP_OUT_FORMAT      kPXP_OutputPixelFormatARGB8888
  #define PXP_PS_FORMAT       kPXP_PsPixelFormatRGB888
  #define PXP_AS_FORMAT       kPXP_AsPixelFormatARGB8888
  //
  #if   (ORIENTATION == ROTATION_0)
    #define DISPLAY_DRIVER      GUIDRV_LIN_32
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSX_32
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OXY_32
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSY_32
  #elif (ORIENTATION == ROTATION_CW)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OSX_32
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OXY_32
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OSY_32
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_32
  #elif (ORIENTATION == ROTATION_180)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OXY_32
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSY_32
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_32
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSX_32
  #elif (ORIENTATION == ROTATION_CCW)
    #define DISPLAY_DRIVER     GUIDRV_LIN_OSY_32
    #define DISPLAY_DRIVER_CW  GUIDRV_LIN_32
    #define DISPLAY_DRIVER_180 GUIDRV_LIN_OSX_32
    #define DISPLAY_DRIVER_CCW GUIDRV_LIN_OXY_32
  #endif
#endif

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VRAM_ADDR
  #define VRAM_ADDR (U32)(&_aVRAM0[0])
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
#if ((LCD_BITS_PER_PIXEL != 16) && (LCD_BITS_PER_PIXEL != 32))
  #error Invalid display bit depth defined!
#endif

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
static void _LCD_PXP_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat);
static void _LCD_PXP_Index2ColorBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat);
static void _PullPanelResetPin      (bool pullUp);
static void _PullPanelPowerPin      (bool pullUp);
static int  _DSI_Transfer           (dsi_transfer_t *xfer);

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Buffer related
//
#define LCD_XY2BufferPtr(x, y)     (U32)((U8 *)_aBufferAddress[_BackBuffer] + ((y * XSIZE_PHYS) + x) * LCD_BYTES_PER_PIXEL)
//
// PXP macros
//
#define PXP_PS_InvalidFormat           kPXP_PsPixelFormatY4     // Unused format, used here as invalid format flag
#define PXP_DisableSurface(surf)       PXP_Set##surf##SurfacePosition(PXP, 0xFFFFu, 0xFFFFu, 0u, 0u)
#define PXP_DisableProcessSurface()    PXP_DisableSurface(Process)
#define PXP_DisableAlphaSurface()      PXP_DisableSurface(Alpha)

#define DEFINE_PXP_COLOR2INDEX(SUFFIX, FORMAT)                                                                     \
  static void _LCD_PXP_Color2IndexBulk_##SUFFIX(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) { \
    _LCD_PXP_Color2IndexBulk(pColor, pIndex, NumItems, SizeOfIndex, FORMAT);                                       \
  }

#define DEFINE_PXP_INDEX2COLOR(SUFFIX, FORMAT)                                                                     \
  static void _LCD_PXP_Index2ColorBulk_##SUFFIX(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) { \
    _LCD_PXP_Index2ColorBulk(pIndex, pColor, NumItems, SizeOfIndex, FORMAT);                                       \
  }

#define DEFINE_PXP_COLORCONV(SUFFIX, FORMAT)  \
  DEFINE_PXP_COLOR2INDEX(SUFFIX, FORMAT)      \
  DEFINE_PXP_INDEX2COLOR(SUFFIX, FORMAT)

#define GUI_PXP_SETCOLORCONV(FORMAT)   GUICC_##FORMAT##_SetCustColorConv(_LCD_PXP_Color2IndexBulk_##FORMAT, _LCD_PXP_Index2ColorBulk_##FORMAT)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef enum {
  BUFFER_TYPE_FRAMEBUFFER = 0,
  BUFFER_TYPE_PROCESS_SURFACE,
  BUFFER_TYPE_ALPHA_SURFACE,
} BUFFER_TYPE;

typedef enum {
  BLEND_MODE_NORMAL     = (1 << 0),
  BLEND_MODE_MULTIPLY   = (1 << 1),
  BLEND_MODE_PORTERDUFF = (1 << 2),
} BLEND_MODE;

typedef struct {
  void      * p;
  int         xSize, ySize;
  int         BytesPerLine;
  bool        OverwriteAlpha;
  BUFFER_TYPE Type;
  union {
    pxp_output_pixel_format_t Out;
    pxp_ps_pixel_format_t     Ps;
    pxp_as_pixel_format_t     As;
  } Format;
} BUFFER_OBJ;

typedef struct {
  BLEND_MODE                   Mode;
  U8                           Alpha;
  pxp_porter_duff_blend_mode_t PorterDuffMode;
} PXP_BLEND_INFO;

typedef struct {
  int              x, y;
  int              xSize, ySize;
  BUFFER_OBJ     * pOutputBuffer;
  BUFFER_OBJ     * pProcessSurface;
  BUFFER_OBJ     * pAlphaSurface;
  PXP_BLEND_INFO * pBlend;
} LCD_PXP_DRAW_CONTEXT;

typedef struct {
  int * pxSizeRoundUp;
  int * pySizeRoundUp;
  int * pxSizeRoundDown;
  int * pySizeRoundDown;
} LCD_PXP_SIZE_CONTEXT;

/*********************************************************************
*
*       Static data: Display init
*
**********************************************************************
*/
static U32 mipiDsiTxEscClkFreq_Hz;
static U32 mipiDsiDphyBitClkFreq_Hz;
static U32 mipiDsiDphyRefClkFreq_Hz;
static U32 mipiDsiDpiClkFreq_Hz;

const MIPI_DSI_Type g_mipiDsi = {
  .host = DSI_HOST,
  .apb  = DSI_HOST_APB_PKT_IF,
  .dpi  = DSI_HOST_DPI_INTFC,
  .dphy = DSI_HOST_DPHY_INTFC,
};

static mipi_dsi_device_t dsiDevice = {
  .virtualChannel = 0,
  .xferFunc       = _DSI_Transfer,
};

static const rm68200_resource_t rm68200Resource = {
  .dsiDevice    = &dsiDevice,
  .pullResetPin = _PullPanelResetPin,
  .pullPowerPin = _PullPanelPowerPin,
};

static display_handle_t rm68200Handle = {
  .resource = &rm68200Resource,
  .ops      = &rm68200_ops,
};

static dc_fb_lcdifv2_handle_t s_dcFbLcdifv2Handle = {0};

static const dc_fb_lcdifv2_config_t s_dcFbLcdifv2Config = {
  .lcdifv2       = LCDIFV2,
  .width         = XSIZE_PHYS,
  .height        = YSIZE_PHYS,
  .hsw           = HSW,
  .hfp           = HFP,
  .hbp           = HBP,
  .vsw           = VSW,
  .vfp           = VFP,
  .vbp           = VBP,
  .polarityFlags = LCDIF_POL_FLAGS,
  .lineOrder     = kLCDIFV2_LineOrderRGB,
  // CM4 is domain 1, CM7 is domain 0.
#if (__CORTEX_M <= 4)
  .domain = 1,
#else
  .domain = 0,
#endif
};

const dc_fb_t g_dc = {
  .ops     = &g_dcFbOpsLcdifv2,
  .prvData = &s_dcFbLcdifv2Handle,
  .config  = &s_dcFbLcdifv2Config,
};

#define BUFFER_ALIGNMENT     __attribute__((aligned(32)))

//
// Framebuffer
//
U8                  _aVRAM0[XSIZE_PHYS * YSIZE_PHYS * NUM_BUFFERS * NUM_VSCREENS * LCD_BYTES_PER_PIXEL] BUFFER_ALIGNMENT;
static volatile int _PendingBuffer = -1;
static volatile int _BackBuffer    =  0;
//
// Buffer used for various PXP operations requiring two buffers, as well as CopyRect().
//
#if USE_PXP_BUFFER
  U8 _aBufferPXP[XSIZE_PHYS * YSIZE_PHYS * LCD_BYTES_PER_PIXEL] BUFFER_ALIGNMENT;
#endif
//
// Temporarily allocated buffer
//
static void * _pBuffer;
//
// Config objects used for PXP operations 
//
static LCD_PXP_DRAW_CONTEXT _PXP_Context;
static PXP_BLEND_INFO       _PXP_Blend;
static BUFFER_OBJ           _PXP_aBuffer[3]; // Output buffer, process surface, alpha surface
//
// Currently used display rotation for PXP.
//
static pxp_rotate_degree_t _PXP_Rotation;

static const GUI_PXP_BITMAP_DATA _acBitmapData[] = {
  { kPXP_AsPixelFormatARGB8888, 32, GUI_DRAW_BMPM8888I },
};

static U32 _aBufferAddress[NUM_BUFFERS] = {
  VRAM_ADDR,
  #if (NUM_BUFFERS > 1)
    VRAM_ADDR + XSIZE_PHYS * YSIZE_PHYS * LCD_BYTES_PER_PIXEL,
  #endif
  #if (NUM_BUFFERS > 2)
    VRAM_ADDR + XSIZE_PHYS * YSIZE_PHYS * LCD_BYTES_PER_PIXEL * 2
  #endif
};

//
// Semaphores
//
#if (USE_OS == 1)
  //
  // Semaphore definitions.
  //
  static SEMAPHORE_TYPE _Sema_WaitForPXP;
  static SEMAPHORE_TYPE _Sema_WaitForVSYNC;
  //
  // Handles/Pointers to above semaphores.
  //
  static SEMAPHORE_HANDLE_TYPE _hSema_WaitForPXP   = MAKE_SEMAPHORE_HANDLE(_Sema_WaitForPXP);
  static SEMAPHORE_HANDLE_TYPE _hSema_WaitForVSYNC = MAKE_SEMAPHORE_HANDLE(_Sema_WaitForVSYNC);
#endif

/*********************************************************************
*
*       Static code: General helper functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Malloc
*/
static void * _Malloc(U32 NumBytes) {
#if USE_PXP_BUFFER
  if (NumBytes > sizeof(_aBufferPXP)) {
    _pBuffer = NULL;
  }
  _pBuffer = _aBufferPXP;
#else
  GUI_HMEM hMem;

  hMem = GUI_ALLOC_AllocNoInit(NumBytes);
  if (hMem == 0) {
    return NULL;
  }
  GUI_LOCK();
  _pBuffer = GUI_ALLOC_LockH(hMem);
#endif
  return _pBuffer;
}

/*********************************************************************
*
*       _Free
*/
static void _Free(void * p) {
#if USE_PXP_BUFFER
  GUI_USE_PARA(p);
#else
  GUI_HMEM hMem;
  
  if (p) {
    hMem = GUI_ALLOC_p2h(p);
    GUI_ALLOC_UnlockH(p);
    GUI_UNLOCK();
    GUI_ALLOC_Free(hMem);
  }
#endif
}

/*********************************************************************
*
*       _GetMemdevParams
*/
static void _GetMemdevParams(GUI_MEMDEV_Handle hMem, void ** pp, int * pxSize, int * pySize, int * pBPP) {
  *pp     = GUI_MEMDEV_GetDataPtr(hMem);
  *pxSize = GUI_MEMDEV_GetXSize(hMem);
  *pySize = GUI_MEMDEV_GetYSize(hMem);
  *pBPP   = GUI_MEMDEV_GetBitsPerPixel(hMem);
}

/*********************************************************************
*
*       Static code: Semaphore handling
*
**********************************************************************
*/
#if (USE_OS == 1)
/*********************************************************************
*
*       _Semaphore_Take
*/
static void _Semaphore_Take(SEMAPHORE_HANDLE_TYPE hSema) {
#if (OS == EMBOS)
  OS_SEMAPHORE_TakeBlocked(hSema);
#endif
#if (OS == FREERTOS)
  xSemaphoreTake(hSema, portMAX_DELAY);
#endif
}

/*********************************************************************
*
*       _Semaphore_Create
*/
static void _Semaphore_Create(SEMAPHORE_HANDLE_TYPE * phSema) {
#if (OS == EMBOS)
  OS_SEMAPHORE_Create(*phSema, 0);
#endif
#if (OS == FREERTOS)
  *phSema = xSemaphoreCreateBinary();
#endif
}

/*********************************************************************
*
*       _Semaphore_Give
*/
static void _Semaphore_Give(SEMAPHORE_HANDLE_TYPE hSema) {
#if (OS == EMBOS)
  OS_EnterInterrupt();
  OS_SEMAPHORE_Give(hSema);
  OS_LeaveInterrupt();
#endif
#if (OS == FREERTOS)
  xSemaphoreGive(hSema);
#endif
}
#endif

/*********************************************************************
*
*       Static code: PXP helper functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Swap
*/
static void _Swap(int * p0, int * p1) {
  int Temp;

  Temp = *p0;
  *p0  = *p1;
  *p1  = Temp;
}

/*********************************************************************
*
*       _SwapSizesOnDemand
*/
static void _SwapSizesOnDemand(int * pxSize, int * pySize) {
  if (LCD_GetSwapXY()) {
    _Swap(pxSize, pySize);
  }
}

/*********************************************************************
*
*       _PXP_InitOperation
*/
static void _PXP_InitOperation(void) {
  //
  // Clear data cache before operation to avoid artefacts on the screen.
  //
  GUI_DCACHE_Clear(0);
  PXP_Reset(PXP);
  //
  // Use interrupt with semaphores to decrease CPU load.
  //
#if (USE_OS == 1)
  PXP_EnableInterrupts(PXP, kPXP_CompleteInterruptEnable);
#endif
  //
  // Disable CSC module, enabled by default.
  // 16x16 blocks for better performance.
  //
  PXP_EnableCsc1(PXP, false);
  PXP_SetProcessBlockSize(PXP, kPXP_BlockSize16);
}

/*********************************************************************
*
*       _PXP_StartOperation
*/
static void _PXP_StartOperation(void) {
  //
  // Start and wait for process to complete.
  //
  PXP_Start(PXP);
#if (USE_OS == 1)
  _Semaphore_Take(_hSema_WaitForPXP);
#else
  while (!(kPXP_CompleteFlag & PXP_GetStatusFlags(PXP)));
  PXP_ClearStatusFlags(PXP, kPXP_CompleteFlag);
#endif
  //
  // Free temporary buffer if there is one.
  //
  if (_pBuffer) {
    _Free(_pBuffer);
    _pBuffer = NULL;
  }
}

/*********************************************************************
*
*       _PXP_ScalingParamValid
*/
static bool _PXP_ScalingParamValid(int inputDimension, int outputDimension) {
  U32 scaleFact = ((U32)inputDimension << 12U) / outputDimension;
  
  if ((scaleFact >= (16UL << 12U)) || scaleFact == 0) {
    return false;
  }
  return true;
}

/*********************************************************************
*
*       _PXP_SetPorterDuffMode
*/
static void _PXP_SetPorterDuffMode(pxp_porter_duff_blend_mode_t Mode) {
  pxp_porter_duff_config_t pd_cfg;

  PXP_GetPorterDuffConfig(Mode, &pd_cfg);
  PXP_SetPorterDuffConfig(PXP, &pd_cfg);
}

/*********************************************************************
*
*       _Log2PhysCoords
*/
static void _Log2PhysCoords(int x, int y, int xSize, int ySize, int * pxPhys, int * pyPhys) {
  *pxPhys = 0;
  *pyPhys = 0;
  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
    *pxPhys = x;
    *pyPhys = y;
    break;
  case kPXP_Rotate90:
    *pxPhys = XSIZE_PHYS - y - ySize;
    *pyPhys = x;
    break;
  case kPXP_Rotate180:
    *pxPhys = XSIZE_PHYS - x - xSize;
    *pyPhys = YSIZE_PHYS - y - ySize;
    break;
  case kPXP_Rotate270:
    *pxPhys = y;
    *pyPhys = YSIZE_PHYS - x - xSize;
    break;
  }
}

/*********************************************************************
*
*       _Phys2LogCoords
*/
static void _Phys2LogCoords(int * px0, int * py0, int * px1, int * py1) {
  int Temp;

  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
    break;
  case kPXP_Rotate90:
    Temp = *px0;
    *px0 = XSIZE_PHYS - 1 - *px1;
    *px1 = XSIZE_PHYS - 1 - Temp;
    _Swap(px0, py0);
    _Swap(px1, py1);
    break;
  case kPXP_Rotate180:
    *px0 = XSIZE_PHYS - 1 - *px0;
    *px1 = XSIZE_PHYS - 1 - *px1;
    *py0 = YSIZE_PHYS - 1 - *py0;
    *py1 = YSIZE_PHYS - 1 - *py1;
    if (*px0 > *px1) {
      _Swap(px0, px1);
    }
    if (*py0 > *py1) {
      _Swap(py0, py1);
    }
    break;
  case kPXP_Rotate270:
    Temp = *py1;
    *py1 = (YSIZE_PHYS - 1 - *py0);
    *py0 = (YSIZE_PHYS - 1 - Temp);
    _Swap(px0, py0);
    _Swap(px1, py1);
    break;
  }
}

/*********************************************************************
*
*       _Phys2LogCoordsEx
*/
static void _Phys2LogCoordsEx(GUI_RECT * pRect) {
  int x0 = pRect->x0;
  int y0 = pRect->y0;
  int x1 = pRect->x1;
  int y1 = pRect->y1;
  _Phys2LogCoords(&x0, &y0, &x1, &y1);
  pRect->x0 = x0;
  pRect->y0 = y0;
  pRect->x1 = x1;
  pRect->y1 = y1;
}

/*********************************************************************
*
*       _GetBufferPtr
*/
static U32 _GetBufferPtr(int x, int y, int xSize, int ySize) {
  U8 * pBuffer;
  int  xPhys, yPhys;

  //
  // Convert logical coordinates to physical coordinates
  //
  _Log2PhysCoords(x, y, xSize, ySize, &xPhys, &yPhys);
  pBuffer  = (U8 *)_aBufferAddress[_BackBuffer];
  pBuffer += ((yPhys * XSIZE_PHYS) + xPhys) * LCD_BYTES_PER_PIXEL;
  return (U32)pBuffer;
}

/*********************************************************************
*
*       _PXP_ConfigOutBufferEx
*/
static void _PXP_ConfigOutBufferEx(void * pAddress, U32 Pitch, int xSize, int ySize, bool OverwriteAlpha, pxp_output_pixel_format_t PixelFmt) {
  pxp_output_buffer_config_t out_cfg;

  out_cfg.pixelFormat    = PixelFmt;
  out_cfg.interlacedMode = kPXP_OutputProgressive;
  out_cfg.buffer0Addr    = (U32)pAddress;
  out_cfg.buffer1Addr    = 0U;
  out_cfg.pitchBytes     = Pitch;
  out_cfg.width          = xSize;
  out_cfg.height         = ySize;
  PXP_SetOutputBufferConfig(PXP, &out_cfg);
  //
  // Make sure alpha channel is overwritten with 0xFF after the composite has been written to frame buffer.
  //
  if (OverwriteAlpha) {
    PXP->OUT_CTRL |= (0xFFu << 24) | (0x1u << 23);
  }
}

/*********************************************************************
*
*       _PXP_ConfigOutBuffer
*/
static void _PXP_ConfigOutBuffer(void * pAddress, U32 Pitch, int xSize, int ySize, bool OverwriteAlpha) {
  _PXP_ConfigOutBufferEx(pAddress, Pitch, xSize, ySize, OverwriteAlpha, PXP_OUT_FORMAT);
}

/*********************************************************************
*
*       _PXP_ConfigOutBufferAuto
*/
static int _PXP_ConfigOutBufferAuto(int x0, int y0, int xSize, int ySize) {
  GUI_MEMDEV_Handle hMem;
  U32               Addr;
  U32               AddrMem;
  void            * pData;
  int               xSizeDev, ySizeDev, BppDev;
  int               xPosDev, yPosDev;

  //
  // Check if there currently is a MEMDEV selected so that we can write into the device.
  //
  hMem = GUI_MEMDEV_GetSelMemdev();
  if (hMem) {
    _GetMemdevParams(hMem, &pData, &xSizeDev, &ySizeDev, &BppDev);
    if (BppDev != 32) {
      return 1;
    }
    xPosDev = GUI_MEMDEV_GetXPos(hMem);
    yPosDev = GUI_MEMDEV_GetYPos(hMem);
    x0     -= xPosDev;
    y0     -= yPosDev;
    AddrMem = ((U32)pData) + (((y0 * xSizeDev) + x0) * (BppDev / 8));
    _PXP_ConfigOutBuffer((void *)AddrMem, xSizeDev * (BppDev / 8), xSizeDev, ySizeDev, true);
  } else {
    Addr = _GetBufferPtr(x0, y0, xSize, ySize);
    _SwapSizesOnDemand(&xSize, &ySize);
    _PXP_ConfigOutBuffer((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSize, ySize, true);
  }
  return 0;
}

/*********************************************************************
*
*       _PXP_ConfigProcessSurfaceEx
*/
static void _PXP_ConfigProcessSurfaceEx(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_ps_pixel_format_t PixelFmt) {
  pxp_ps_buffer_config_t ps_cfg;

  ps_cfg.bufferAddr  = (U32)pAddress;
  ps_cfg.bufferAddrU = 0;
  ps_cfg.bufferAddrV = 0;
  ps_cfg.pitchBytes  = Pitch;
  ps_cfg.pixelFormat = PixelFmt;
  ps_cfg.swapByte    = 0;
  PXP_SetProcessSurfaceBufferConfig(PXP, &ps_cfg);
  PXP_SetProcessSurfacePosition(PXP, 0, 0, xSize - 1, ySize - 1);
}

/*********************************************************************
*
*       _PXP_ConfigProcessSurface
*/
static void _PXP_ConfigProcessSurface(void * pAddress, U32 Pitch, int xSize, int ySize) {
  _PXP_ConfigProcessSurfaceEx(pAddress, Pitch, xSize, ySize, PXP_PS_FORMAT);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaSurfaceEx
*/
static void _PXP_ConfigAlphaSurfaceEx(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_as_pixel_format_t PixelFmt) {
  pxp_as_buffer_config_t as_cfg;

  as_cfg.pixelFormat = PixelFmt;
  as_cfg.bufferAddr  = (U32)pAddress;
  as_cfg.pitchBytes  = Pitch;
  PXP_SetAlphaSurfaceBufferConfig(PXP, &as_cfg);
  PXP_SetAlphaSurfacePosition(PXP, 0, 0, xSize - 1, ySize - 1);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaSurface
*/
static void _PXP_ConfigAlphaSurface(void * pAddress, U32 Pitch, int xSize, int ySize) {
  _PXP_ConfigAlphaSurfaceEx(pAddress, Pitch, xSize, ySize, PXP_AS_FORMAT);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaBlendNormal
*/
static void _PXP_ConfigAlphaBlendNormal(void) {
  pxp_as_blend_config_t blend_cfg;

  blend_cfg.alpha       = 0;
  blend_cfg.invertAlpha = 0;
  blend_cfg.alphaMode   = kPXP_AlphaEmbedded;
  PXP_SetAlphaSurfaceBlendConfig(PXP, &blend_cfg);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaBlendMultiply
*/
static void _PXP_ConfigAlphaBlendMultiply(U8 Alpha) {
  pxp_as_blend_config_t blend_cfg;

  blend_cfg.alpha       = Alpha;
  blend_cfg.invertAlpha = 0;
  blend_cfg.alphaMode   = kPXP_AlphaMultiply;
  PXP_SetAlphaSurfaceBlendConfig(PXP, &blend_cfg);
}

/*********************************************************************
*
*       Static code: PXP related emWin functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _LCD_PXP_SetProcessSurface
*/
static void _LCD_PXP_SetProcessSurface(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_ps_pixel_format_t Fmt) {
  BUFFER_OBJ * pPS;

  _PXP_Context.pProcessSurface = pPS = &_PXP_aBuffer[BUFFER_TYPE_PROCESS_SURFACE];
  pPS->p            = pAddress;
  pPS->Type         = BUFFER_TYPE_PROCESS_SURFACE;
  pPS->xSize        = xSize;
  pPS->ySize        = ySize;
  pPS->BytesPerLine = Pitch;
  pPS->Format.Ps    = Fmt;
}

/*********************************************************************
*
*       _LCD_PXP_SetAlphaSurface
*/
static void _LCD_PXP_SetAlphaSurface(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_as_pixel_format_t Fmt) {
  BUFFER_OBJ * pAS;

  _PXP_Context.pAlphaSurface = pAS = &_PXP_aBuffer[BUFFER_TYPE_ALPHA_SURFACE];
  pAS->p            = pAddress;
  pAS->Type         = BUFFER_TYPE_ALPHA_SURFACE;
  pAS->xSize        = xSize;
  pAS->ySize        = ySize;
  pAS->BytesPerLine = Pitch;
  pAS->Format.As    = Fmt;
}

/*********************************************************************
*
*       _LCD_PXP_GetFrameBufferConfig
*/
static BUFFER_OBJ * _LCD_PXP_GetFrameBufferConfig(LCD_PXP_DRAW_CONTEXT * pCtx) {
  BUFFER_OBJ * p = &_PXP_aBuffer[BUFFER_TYPE_FRAMEBUFFER];

  p->p              = (void *)_GetBufferPtr(pCtx->x, pCtx->y, pCtx->xSize, pCtx->ySize);
  p->xSize          = pCtx->xSize;
  p->ySize          = pCtx->ySize;
  p->Type           = BUFFER_TYPE_FRAMEBUFFER;
  p->Format.Out     = PXP_OUT_FORMAT;
  p->BytesPerLine   = XSIZE_PHYS * LCD_BYTES_PER_PIXEL;
  p->OverwriteAlpha = true;
  return p;
}

/*********************************************************************
*
*       _LCD_PXP_AlphaBlending
*/
static PXP_BLEND_INFO * _LCD_PXP_AlphaBlending(void) {
  LCD_PXP_DRAW_CONTEXT * pCtx   = &_PXP_Context;
  PXP_BLEND_INFO       * pBlend = &_PXP_Blend;

  pCtx->pBlend = pBlend;
  return pBlend;
}

/*********************************************************************
*
*       _LCD_PXP_InitContext
*/
static void _LCD_PXP_InitContext(int x, int y, int xSize, int ySize) {
  LCD_PXP_DRAW_CONTEXT * pCtx = &_PXP_Context;
  BUFFER_OBJ           * pOB;

  //
  // Clear all previous parameters.
  //
  memset(pCtx, 0, sizeof(LCD_PXP_DRAW_CONTEXT));
  pCtx->x     = x;
  pCtx->y     = y;
  pCtx->xSize = xSize;
  pCtx->ySize = ySize;
  //
  // Set frame buffer as output buffer.
  //
  pOB                 = _LCD_PXP_GetFrameBufferConfig(pCtx);
  pCtx->pOutputBuffer = pOB;
}

#define ROUND_UP_BY(div, n)    ((n % div) ? (n + (div - (n % div))) : n)
#define ROUND_DOWN_BY(div, n)  ((n % div) ? (n -        (n % div) ) : n)
/*********************************************************************
*
*       _PXP_CheckSizes
*/
static void _PXP_CheckSizes(int xSize, int ySize, LCD_PXP_SIZE_CONTEXT * pSize) {
  //
  // In case a size is not divisible by 16 (the block size set to the PXP),
  // we need the sizes rounded up and rounded down.
  // The rounded down size is the part of the bitmap that is drawn by the PXP.
  // The rounded up size is to incorporate the background of the bitmap will be
  // blended into. It is crucial for rotated configs that all sizes are divisible by 16.
  //
  *pSize->pxSizeRoundUp   = ROUND_UP_BY(16, xSize);
  *pSize->pySizeRoundUp   = ROUND_UP_BY(16, ySize);
  *pSize->pxSizeRoundDown = ROUND_DOWN_BY(16, xSize);
  *pSize->pySizeRoundDown = ROUND_DOWN_BY(16, ySize);
}

/*********************************************************************
*
*       _PXP_ClipSizes
*/
static void _PXP_ClipSizes(int x, int y, LCD_PXP_SIZE_CONTEXT * pSize) {
  int * pxSize = pSize->pxSizeRoundUp;
  int * pySize = pSize->pySizeRoundUp;

  if (x + *pxSize > (LCD_GetXSize())) {
    *pxSize = LCD_GetXSize() - x;
  }
  if (y + *pySize > (LCD_GetYSize())) {
    *pySize = LCD_GetYSize() - y;
  }
}

/*********************************************************************
*
*       _PXP_InvertRotation
*/
static pxp_rotate_degree_t _PXP_InvertRotation(void) {
  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
    return kPXP_Rotate0;
  case kPXP_Rotate90:
    return kPXP_Rotate270;
  case kPXP_Rotate180:
    return kPXP_Rotate180;
  case kPXP_Rotate270:
    return kPXP_Rotate90;
  }
  return kPXP_Rotate0;
}

/*********************************************************************
*
*       _LCD_PXP_ConfigOutputBuffer
*/
static void _LCD_PXP_ConfigOutputBuffer(BUFFER_OBJ * pBuffer) {
  _PXP_ConfigOutBufferEx(pBuffer->p, pBuffer->BytesPerLine, pBuffer->xSize, pBuffer->ySize, pBuffer->OverwriteAlpha, pBuffer->Format.Out);
}

/*********************************************************************
*
*       _LCD_PXP_ConfigProcessSurface
*/
static void _LCD_PXP_ConfigProcessSurface(BUFFER_OBJ * pBuffer) {
  _PXP_ConfigProcessSurfaceEx(pBuffer->p, pBuffer->BytesPerLine, pBuffer->xSize, pBuffer->ySize, pBuffer->Format.Ps);
}

/*********************************************************************
*
*       _LCD_PXP_ConfigAlphaSurface
*/
static void _LCD_PXP_ConfigAlphaSurface(BUFFER_OBJ * pBuffer) {
  _PXP_ConfigAlphaSurfaceEx(pBuffer->p, pBuffer->BytesPerLine, pBuffer->xSize, pBuffer->ySize, pBuffer->Format.As);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaBlend
*/
static void _LCD_PXP_ConfigAlphaBlend(PXP_BLEND_INFO * pBlend) {
  if (!pBlend) {
    return;
  }
  if (pBlend->Mode & BLEND_MODE_NORMAL) {
    _PXP_ConfigAlphaBlendNormal();
  }
  if (pBlend->Mode & BLEND_MODE_MULTIPLY) {
    _PXP_ConfigAlphaBlendMultiply(pBlend->Alpha);
  }
  if (pBlend->Mode & BLEND_MODE_PORTERDUFF) {
    _PXP_SetPorterDuffMode(pBlend->PorterDuffMode);
  }
}

/*********************************************************************
*
*       _GetSizeUnmirrored
*/
static void _GetSizeUnmirrored(int xSize, int ySize, int * pOutXSize, int * pOutYSize) {
  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
  case kPXP_Rotate180:
    *pOutXSize = xSize;
    *pOutYSize = ySize;
    break;
  case kPXP_Rotate90:
  case kPXP_Rotate270:
    *pOutXSize = ySize;
    *pOutYSize = xSize;
    break;
  }
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmap
*
*  Used by all functions which need to observe the current rotation
*  (i.e. most bitmap functions that output directly to the screen).
*  Functions drawing into memory devices do not need to call this function.
*/
static int _LCD_PXP_DrawBitmap(LCD_PXP_DRAW_CONTEXT * pCtx) {
  U32                  Addr;
  int                  xSizeRD, ySizeRD, xSizeRU, ySizeRU; // x/ySize rounded down and up
  LCD_PXP_SIZE_CONTEXT Sizes =  { &xSizeRU, &ySizeRU, &xSizeRD, &ySizeRD };
  void               * pBuffer = NULL;
  int                  x = pCtx->x, y = pCtx->y;
  bool                 IsRotated = (_PXP_Rotation != kPXP_Rotate0);
  BUFFER_OBJ         * pOut, *pAS;
  int                  xSizeUM, ySizeUM;
  int                  xDiff = 0, yDiff = 0;

  _PXP_InitOperation();
  //
  // Non-rotated config: entire operation is done with one PXP iteration.
  //
  if (!IsRotated  || !pCtx->pAlphaSurface) {
    //
    // Set rotation if required.
    //
    if (IsRotated) {
      PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, _PXP_Rotation, kPXP_FlipDisable);
    }
    //
    // Configure output buffer. If a specific buffer has been set, use it (e.g. memdev).
    // If not, use the default which is the backbuffer.
    //
    pOut = (pCtx->pOutputBuffer) ? pCtx->pOutputBuffer : _LCD_PXP_GetFrameBufferConfig(pCtx);
    _LCD_PXP_ConfigOutputBuffer(pOut);
    //
    // Enable/disable PS.
    //
    if (!pCtx->pProcessSurface) {
      PXP_DisableProcessSurface();
    } else {
      _LCD_PXP_ConfigProcessSurface(pCtx->pProcessSurface);
    }
    //
    // Enable/disable AS and alpha blending.
    //
    if (!pCtx->pAlphaSurface) {
      PXP_DisableAlphaSurface();
    } else {
      _LCD_PXP_ConfigAlphaSurface(pCtx->pAlphaSurface);
      _LCD_PXP_ConfigAlphaBlend(pCtx->pBlend);
    }
    _PXP_StartOperation();
  }
  //
  // In rotated configs with semi-transparent bitmaps, two steps are required.
  //
  else {
    //
    // Step 1: Copy affected frame buffer area and rotate it against the current angle into the temp. buffer.
    // We rotate here in the opposite direction to have the frame buffer content unrotated, just like the bitmap is.
    //
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, _PXP_InvertRotation(), kPXP_FlipDisable);
    _PXP_CheckSizes(pCtx->xSize, pCtx->ySize, &Sizes);
    xDiff = xSizeRU;
    yDiff = ySizeRU;
    _PXP_ClipSizes(x, y, &Sizes);
    xDiff -= xSizeRU;
    yDiff -= ySizeRU;
    pBuffer = _Malloc(ySizeRU * xSizeRU * LCD_BYTES_PER_PIXEL);
    if (!pBuffer) {
      return 1;
    }
    _GetSizeUnmirrored(xSizeRU, ySizeRU, &xSizeUM, &ySizeUM);
    _PXP_ConfigOutBuffer(pBuffer, xSizeRU * LCD_BYTES_PER_PIXEL, xSizeUM, ySizeUM, true);
    PXP_DisableAlphaSurface();
    Addr = _GetBufferPtr(x, y, xSizeRU, ySizeRU);
    _PXP_ConfigProcessSurface((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSizeUM, ySizeUM);
    _PXP_StartOperation();
    //
    // Step 2: Blend alpha bitmap over copied area in temp. buffer into the frame buffer.
    //
    _PXP_InitOperation();
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, _PXP_Rotation, kPXP_FlipDisable);
    //
    // Make sure size of output buffer is aligned.
    //
    pOut = (pCtx->pOutputBuffer) ? pCtx->pOutputBuffer : _LCD_PXP_GetFrameBufferConfig(pCtx);
    pOut->xSize = xSizeRU;
    pOut->ySize = ySizeRU;
    pOut->p     = (void *)Addr;
    _LCD_PXP_ConfigOutputBuffer(pOut);
    //
    // Blend bitmap as AS over PS into frame buffer.
    //
    _PXP_ConfigProcessSurface(pBuffer, xSizeRU * LCD_BYTES_PER_PIXEL, xSizeRU, ySizeRU);
    pAS = pCtx->pAlphaSurface;
    _LCD_PXP_ConfigAlphaSurface(pAS);
    _LCD_PXP_ConfigAlphaBlend(pCtx->pBlend);
    if (_PXP_Rotation == kPXP_Rotate180) {
      PXP_SetAlphaSurfacePosition(PXP, xDiff, yDiff, pAS->xSize + xDiff - 1, pAS->ySize + yDiff - 1);
    }
    _PXP_StartOperation();
  }
  return 0;
}

/*********************************************************************
*
*       _LCD_PXP_CopyBuffer
*/
static void _LCD_PXP_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  GUI_USE_PARA(LayerIndex);

  _PXP_InitOperation();
  //
  // Configure destination buffer as output buffer.
  //
  _PXP_ConfigOutBuffer((void *)_aBufferAddress[IndexDst], XSIZE_PHYS * LCD_BYTES_PER_PIXEL, XSIZE_PHYS, YSIZE_PHYS, true);
  //
  // Configure source buffer as PS.
  //
  _PXP_ConfigProcessSurface((void *)_aBufferAddress[IndexSrc], XSIZE_PHYS * LCD_BYTES_PER_PIXEL, XSIZE_PHYS, YSIZE_PHYS);
  //
  // AS not needed.
  //
  PXP_DisableAlphaSurface();
  _PXP_StartOperation();
  //
  // Clear cache to avoid artefacts on the screen.
  //
  GUI_DCACHE_Clear(0);
  //
  // Buffer switch done.
  //
  _BackBuffer = IndexDst;
}

/*********************************************************************
*
*       _LCD_PXP_FillRect
*/
static void _LCD_PXP_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  pxp_porter_duff_config_t pd_cfg;
  U8                       Alpha;
  U32                      Addr;
  U32                      w, h;
  GUI_COLOR                Color;

  GUI_USE_PARA(LayerIndex);
  //
  // Do XOR filling by software.
  //
  if (GUI_GetDrawMode() & GUI_DM_XOR) {
    LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))NULL);
    _Phys2LogCoords(&x0, &y0, &x1, &y1);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_PXP_FillRect);
    return;
  }
  //
  // Non-XOR filling done by PXP.
  //
  _PXP_InitOperation();
  w = x1 - x0 + 1;
  h = y1 - y0 + 1;
  //
  // Configure output buffer.
  // This is a low-level driver function, coordinates are physical not logical.
  //
  Addr = LCD_XY2BufferPtr(x0, y0);
  _PXP_ConfigOutBuffer((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, w, h, true);
  //
  Color = GUI_Index2Color(PixelIndex);
  Alpha = GUI_GetAlpha();
  if (Alpha == 0xFF) {
    //
    // Opaque fill, AS not needed.
    //
    PXP_DisableAlphaSurface();
  } else {
    //
    // Transparency fill, output buffer is used as AS.
    //
    _PXP_ConfigAlphaSurface((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, w, h);
  }
  //
  // Disable PS, instead generate color.
  //
  PXP_DisableProcessSurface();
  PXP_SetProcessSurfaceBackGroundColor(PXP, Color);
  //
  // Configure Porter-Duff blending.
  //
  pd_cfg.enable             = 1;
  pd_cfg.dstColorMode       = kPXP_PorterDuffColorNoAlpha;
  pd_cfg.srcColorMode       = kPXP_PorterDuffColorNoAlpha;
  pd_cfg.dstGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
  pd_cfg.srcGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
  pd_cfg.dstFactorMode      = kPXP_PorterDuffFactorStraight;
  pd_cfg.srcFactorMode      = (Alpha == 0xFF) ? kPXP_PorterDuffFactorStraight : kPXP_PorterDuffFactorInversed;
  pd_cfg.dstGlobalAlpha     = Alpha;
  pd_cfg.srcGlobalAlpha     = Alpha;
  pd_cfg.dstAlphaMode       = kPXP_PorterDuffAlphaStraight;
  pd_cfg.srcAlphaMode       = kPXP_PorterDuffAlphaStraight;
  PXP_SetPorterDuffConfig(PXP, &pd_cfg);
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawMemdevAlpha
*/
static void _LCD_PXP_DrawMemdevAlpha(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  _PXP_InitOperation();
  //
  // Destination device is output buffer.
  //
  _PXP_ConfigOutBuffer(pDst, BytesPerLineDst, xSize, ySize, false);
  //
  // Configure destination device as PS.
  //
  _PXP_ConfigProcessSurface(pDst, BytesPerLineDst, xSize, ySize);
  //
  // Configure bitmap as AS.
  //
  _PXP_ConfigAlphaSurfaceEx((void *)pSrc, BytesPerLineSrc, xSize, ySize, kPXP_AsPixelFormatARGB8888);
  _PXP_ConfigAlphaBlendNormal();
  //
  // Set Porter-Duff blending to SrcOver.
  //
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmap32bpp
*/
static void _LCD_PXP_DrawBitmap32bpp(int LayerIndex, int x, int y, U16 const * p, int xSize, int ySize, int BytesPerLine) {
  LCD_PXP_DRAW_CONTEXT * pCtx = &_PXP_Context;
  BUFFER_OBJ           * pPS;

  GUI_USE_PARA(LayerIndex);
  //
  // Use actual xSize according to pitch of buffer.
  //
  _LCD_PXP_InitContext(x, y, xSize, ySize);
  //
  // Set bitmap as AS and blend normally.
  //
  _LCD_PXP_SetAlphaSurface((void *)p, BytesPerLine, xSize, ySize, kPXP_AsPixelFormatARGB8888);
  _LCD_PXP_AlphaBlending()->Mode = BLEND_MODE_NORMAL;
  //
  // Use frame buffer as PS, copy output buffer config to PS config.
  //
  pCtx->pProcessSurface = pPS = &_PXP_aBuffer[BUFFER_TYPE_PROCESS_SURFACE];
  *pPS                  = *pCtx->pOutputBuffer;
  pPS->Format.Ps        = kPXP_PsPixelFormatRGB888;
  //
  // Draw 32bpp alpha bitmap into frame buffer.
  //
  _LCD_PXP_DrawBitmap(pCtx);
}

/*********************************************************************
*
*       _LCD_PXP_CopyRect
*/
static void _LCD_PXP_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) {
  U32      AddrSrc, AddrDst;
  void   * pBuffer = NULL;
  GUI_RECT rSrc    = { x0, y0, x0 + xSize - 1, y0 + ySize - 1 };
  GUI_RECT rDst    = { x1, y1, x1 + xSize - 1, y1 + ySize - 1 };
  int      PitchSrc;

  GUI_USE_PARA(LayerIndex);
  _PXP_InitOperation();
  AddrSrc = _GetBufferPtr(x0, y0, xSize, ySize);
  AddrDst = _GetBufferPtr(x1, y1, xSize, ySize);
  //
  // Swap if X/Y axis are mirrored.
  //
  _SwapSizesOnDemand(&xSize, &ySize);
  //
  // Use buffer if the rectangles intersect
  //
  PitchSrc = XSIZE_PHYS * LCD_BYTES_PER_PIXEL;
  if (GUI_RectsIntersect(&rSrc, &rDst)) {
    pBuffer = _Malloc(LCD_GetXSize() * LCD_GetYSize() * LCD_BYTES_PER_PIXEL);
    if (!pBuffer) {
      //
      // Fallback to software.
      // Coordinates are physical, they need to be converted to logical coords.
      //
      _Phys2LogCoordsEx(&rSrc);
      _Phys2LogCoordsEx(&rDst);
      LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))NULL);
      GUI_CopyRect(rSrc.x0, rSrc.y0, rDst.x0, rDst.y0, rSrc.x1 - rSrc.x0 + 1, rSrc.y1 - rSrc.y0 + 1);
      LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_LCD_PXP_CopyBuffer);
      return;
    }
    PitchSrc = xSize * LCD_BYTES_PER_PIXEL;
    //
    // Configure output buffer.
    //
    _PXP_ConfigOutBuffer(pBuffer, PitchSrc, xSize, ySize, true);
    //
    // Configure rect to be copied as processing surface.
    //
    _PXP_ConfigProcessSurface((void *)AddrSrc, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSize, ySize);
    //
    // AS not needed.
    //
    PXP_DisableAlphaSurface();
    _PXP_StartOperation();
    //
    // Configure address for final copy.
    //
    AddrSrc = (U32)pBuffer;
  }
  //
  // Configure output buffer.
  //
  _PXP_ConfigOutBuffer((void *)AddrDst, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSize, ySize, true);
  //
  // Configure rect to be copied as processing surface.
  //
  _PXP_ConfigProcessSurface((void *)AddrSrc, PitchSrc, xSize, ySize);
  //
  // AS not needed.
  //
  PXP_DisableAlphaSurface();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_MixColorsBulk
*/
static void _LCD_PXP_MixColorsBulk(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens) {
  GUI_USE_PARA(OffFG);
  GUI_USE_PARA(OffDest);
  _PXP_InitOperation();
  //
  // Configure output buffer.
  //
  _PXP_ConfigOutBuffer(pDst, xSize * LCD_BYTES_PER_PIXEL, xSize, ySize, false);
  //
  // Configure BG as PS.
  //
  _PXP_ConfigProcessSurface(pBG, (xSize + OffBG) * LCD_BYTES_PER_PIXEL, xSize, ySize);
  //
  // Configure FG as AS.
  //
  _PXP_ConfigAlphaSurface(pFG, xSize * LCD_BYTES_PER_PIXEL, xSize, ySize);
  _PXP_ConfigAlphaBlendMultiply(Intens);
  //
  // Configure Porter-Duff mode.
  //
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawMemdevM565
*/
static void _LCD_PXP_DrawMemdevM565(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  _PXP_InitOperation();
  //
  // 32bpp memdev is output buffer.
  //
  _PXP_ConfigOutBufferEx(pDst, BytesPerLineDst, xSize, ySize, false, kPXP_OutputPixelFormatARGB8888);
  //
  // Destination memdev is configured as PS.
  //
  _PXP_ConfigProcessSurfaceEx(pDst, BytesPerLineDst, xSize, ySize, kPXP_PsPixelFormatRGB888);
  //
  // Configure source device as AS.
  //
  _PXP_ConfigAlphaSurfaceEx((void *)pSrc, BytesPerLineSrc, xSize, ySize, kPXP_AsPixelFormatRGB565);
  _PXP_ConfigAlphaBlendNormal();
  //
  // Set Porter-Duff blending to SrcOver.
  //
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawMemdev16bpp
*/
static void _LCD_PXP_DrawMemdev16bpp(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  GUI_USE_PARA(BytesPerLineSrc);
  _PXP_InitOperation();
  //
  // Output buffer is 16bpp memdev.
  //
  _PXP_ConfigOutBufferEx(pDst, BytesPerLineDst, xSize, ySize, false, kPXP_OutputPixelFormatRGB565);
  //
  // Source memdev is configured as PS.
  //
  _PXP_ConfigProcessSurfaceEx((void *)pSrc, BytesPerLineDst, xSize, ySize, kPXP_PsPixelFormatRGB565);
  //
  // Bitmap does not have alpha, AS not needed.
  //
  PXP_DisableAlphaSurface();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmapM565
*/
static void _LCD_PXP_DrawBitmapM565(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  LCD_PXP_DRAW_CONTEXT * pCtx = &_PXP_Context;

  GUI_USE_PARA(LayerIndex);
  //
  // Use actual xSize according to pitch of buffer.
  //
  _LCD_PXP_InitContext(x, y, xSize, ySize);
  //
  // Set 16bpp bitmap as PS.
  //
  _LCD_PXP_SetProcessSurface((void *)p, BytesPerLine, xSize, ySize, kPXP_PsPixelFormatRGB565);
  //
  // Draw 16bpp opaque bitmap into frame buffer.
  //
  _LCD_PXP_DrawBitmap(pCtx);
}

/*********************************************************************
*
*       _LCD_PXP_BlendMemdev
*/
static int _LCD_PXP_BlendMemdev(GUI_MEMDEV_Handle hMem, GUI_COLOR Color, U8 BlendIntens) {
  U32  * pBuffer;
  U32    NumBytes;
  void * pDst;
  int    BitsPerPixel;
  int    BytesPerPixel;
  int    xSize, ySize;
  
  _GetMemdevParams(hMem, &pDst, &xSize, &ySize, &BitsPerPixel);
  BytesPerPixel = BitsPerPixel / 8;
  if (BitsPerPixel != 32) {
    return 1;
  }
  //
  // Allocate temp. buffer
  //
  NumBytes = xSize * ySize * BytesPerPixel;
  pBuffer  = (U32 *)_Malloc(NumBytes);
  if (pBuffer == NULL) {
    return 1;
  }
  //
  // Step 1: Fill buffer with given color and given alpha value
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBuffer(pBuffer, xSize * BytesPerPixel, xSize, ySize, true);
  //
  // No AS needed.
  //
  PXP_DisableAlphaSurface();
  //
  // Disable PS, generate the color instead.
  //
  PXP_DisableProcessSurface();
  PXP_SetProcessSurfaceBackGroundColor(PXP, Color);
  //
  _PXP_StartOperation();
  //
  // Step 2: Blend temp. buffer over memory device.
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBufferEx(pDst, xSize * BytesPerPixel, xSize, ySize, false, kPXP_OutputPixelFormatARGB8888);
  //
  // Configure memdev as PS.
  //
  _PXP_ConfigProcessSurfaceEx(pDst, xSize * BytesPerPixel, xSize, ySize, kPXP_PsPixelFormatRGB888);
  //
  // Use temp. buffer as AS.
  //
  _PXP_ConfigAlphaSurfaceEx(pBuffer, xSize * BytesPerPixel, xSize, ySize, kPXP_AsPixelFormatARGB8888);
  //
  // Set Porter-Duff blending to SrcOver.
  //
  _PXP_ConfigAlphaBlendMultiply(BlendIntens);
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
  return 0;
}

/*********************************************************************
*
*       _LCD_PXP_GetPsFormatFromBitmap
*/
static pxp_ps_pixel_format_t _LCD_PXP_GetPsFormatFromBitmap(const GUI_BITMAP * pBitmap) {
  if (pBitmap->pMethods == GUI_DRAW_BMPM8888I) {
    return kPXP_PsPixelFormatRGB888;
  }
  if (pBitmap->pMethods == GUI_DRAW_BMPM565) {
    return kPXP_PsPixelFormatRGB565;
  }
  if (pBitmap->pMethods == GUI_DRAW_BMPM444_12) {
    return kPXP_PsPixelFormatRGB444;
  }
  if (pBitmap->pMethods == GUI_DRAW_BMP555) {
    return kPXP_PsPixelFormatRGB555;
  }
  return PXP_PS_InvalidFormat;
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmapEx
*/
static int _LCD_PXP_DrawBitmapEx(const GUI_BITMAP * pBitmap, int x0, int y0, int xMag, int yMag) {
  pxp_ps_pixel_format_t Fmt;
  pxp_flip_mode_t       FlipMode;
  int                   xSizeScaled;
  int                   ySizeScaled;
  int                   DevClass;
  bool                  TwoSteps;
  U32                   NumBytes;
  void                * pBuffer;
  pxp_rotate_degree_t   Rotation = _PXP_Rotation;
  bool                  Scaling;
  int                   xSizeRD, ySizeRD, xSizeRU, ySizeRU;
  LCD_PXP_SIZE_CONTEXT  Ctx = { &xSizeRU, &ySizeRU, &xSizeRD, &ySizeRD };
  int                   Pitch;

  //
  // We only want to write into the frame buffer or the current MEMDEV.
  //
  DevClass = GUI_DEVICE_GetDeviceClass();
  if ((DevClass != DEVICE_CLASS_DRIVER) && (DevClass != DEVICE_CLASS_MEMDEV)) {
    return 1;
  }
  //
  // Convert bitmap format to processing surface format. If the format is not supported by PXP we abort.
  //
  Fmt = _LCD_PXP_GetPsFormatFromBitmap(pBitmap);
  if (Fmt == PXP_PS_InvalidFormat) {
    return 1;
  }
  //
  // Retrieve flip mode to be used.
  // We also always need positive scaling factors.
  //
  FlipMode  = kPXP_FlipDisable;
  if (xMag < 0) {
    FlipMode |= kPXP_FlipHorizontal;
    xMag      = -xMag;
  }
  if (yMag < 0) {
    FlipMode |= kPXP_FlipVertical;
    yMag      = -yMag;
  }
  xSizeScaled = (pBitmap->XSize * xMag) / 1000;
  ySizeScaled = (pBitmap->YSize * yMag) / 1000;
  //
  // Do operation by software if desired size is too small or too large.
  //
  if (!(_PXP_ScalingParamValid(pBitmap->XSize, xSizeScaled) && _PXP_ScalingParamValid(pBitmap->YSize, ySizeScaled))) {
    return 1;
  }
  //
  // In case we output into a memory device, it must not be rotated.
  //
  if (DevClass == DEVICE_CLASS_MEMDEV) {
    Rotation = kPXP_Rotate0;
  }
  //
  // Vertical flipping (same as rotation) and scaling cannot be done in one step.
  //
  Scaling  = ((xMag != 1000) || (yMag != 1000)) ? true : false;
  TwoSteps = ((Scaling && (FlipMode & kPXP_FlipVertical)) || (Rotation != kPXP_Rotate0)) ? true : false;
  _PXP_InitOperation();
  if (!TwoSteps) {
    //
    // Set flipping mode and rotation.
    //
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, Rotation, FlipMode);
    //
    // Config scaling params for PS.
    //
    PXP_SetProcessSurfaceScaler(PXP, pBitmap->XSize, pBitmap->YSize, xSizeScaled, ySizeScaled);
    //
    // Configure bitmap as PS.
    //
    PXP_DisableAlphaSurface();
    _PXP_ConfigProcessSurfaceEx((void *)pBitmap->pData, pBitmap->BytesPerLine, xSizeScaled, ySizeScaled, Fmt);
    //
    // Configure output buffer.
    //
    _PXP_ConfigOutBufferAuto(x0, y0, xSizeScaled, ySizeScaled);
    //
    // Execute and done.
    //
    _PXP_StartOperation();
  } else {
    _PXP_CheckSizes(pBitmap->XSize, pBitmap->YSize, &Ctx);
    _PXP_ClipSizes(x0, y0, &Ctx);
    //
    // Step 1: Do flipping and/or rotation operation to temp. buffer.
    // Output will be converted into 32bpp pixels.
    //
    NumBytes = (sizeof(U32) * xSizeRU * ySizeRU);
    pBuffer  = _Malloc(NumBytes);
    if (pBuffer == NULL) {
      return 1;
    }
    Pitch = (LCD_GetSwapXY()) ? (sizeof(U32) * ySizeRU) : (sizeof(U32) * xSizeRU);
    //
    // Set flipping mode but no rotation.
    //
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, Rotation, FlipMode);
    //
    // Use temp. buffer as output buffer for this step.
    //
    _PXP_ConfigOutBufferEx(pBuffer, Pitch, pBitmap->XSize, pBitmap->YSize, true, kPXP_OutputPixelFormatARGB8888);
    //
    // Configure bitmap as PS.
    //
    PXP_DisableAlphaSurface();
    _PXP_ConfigProcessSurfaceEx((void *)pBitmap->pData, pBitmap->BytesPerLine, pBitmap->XSize, pBitmap->YSize, Fmt);
    _PXP_StartOperation();
    //
    // Step 2: Scale the flipped bitmap.
    //
    _PXP_InitOperation();
    PXP_SetProcessSurfaceScaler(PXP, pBitmap->XSize, pBitmap->YSize, xSizeScaled, ySizeScaled);
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, kPXP_Rotate0, kPXP_FlipDisable);
    //
    // Configure output buffer.
    //
    _PXP_ConfigOutBufferAuto(x0, y0, xSizeScaled, ySizeScaled);
    //
    // Configure temp. buffer as PS.
    //
    PXP_DisableAlphaSurface();
    _SwapSizesOnDemand(&xSizeScaled, &ySizeScaled); // Swap sizes if required for PS.
    _PXP_ConfigProcessSurfaceEx(pBuffer, Pitch, xSizeScaled, ySizeScaled, kPXP_PsPixelFormatRGB888);
    //
    // We are done, start PXP and free memory.
    //
    _PXP_StartOperation();
  }
  return 0;
}

/*********************************************************************
*
*       Custom color conversions
*/
DEFINE_PXP_COLORCONV(M1555I, kPXP_OutputPixelFormatARGB1555)
DEFINE_PXP_COLORCONV(M565,   kPXP_OutputPixelFormatRGB565)
DEFINE_PXP_COLORCONV(M4444I, kPXP_OutputPixelFormatARGB4444)
DEFINE_PXP_COLORCONV(M8888I, kPXP_OutputPixelFormatARGB8888)
DEFINE_PXP_COLOR2INDEX(M888, kPXP_OutputPixelFormatRGB888P)

/*********************************************************************
*
*       _LCD_PXP_Color2IndexBulk
*/
static void _LCD_PXP_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat) {
  //
  // Set input color buffer as AS so that alpha values are also converted.
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBufferEx(pIndex, NumItems * SizeOfIndex, NumItems, 1, false, PixelFormat);
  _PXP_ConfigAlphaSurface(pColor, NumItems * LCD_BYTES_PER_PIXEL, NumItems, 1);
  _PXP_ConfigAlphaBlendNormal();
  PXP_DisableProcessSurface();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_Index2ColorBulk
*/
static void _LCD_PXP_Index2ColorBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat) {
  pxp_as_pixel_format_t InputFormat;

  //
  // Convert given parameter to input format.
  //
  switch (PixelFormat) {
  case kPXP_OutputPixelFormatARGB1555:
    InputFormat = kPXP_AsPixelFormatARGB1555;
    break;
  case kPXP_OutputPixelFormatRGB565:
    InputFormat = kPXP_AsPixelFormatRGB565;
    break;
  case kPXP_OutputPixelFormatARGB4444:
    InputFormat = kPXP_AsPixelFormatARGB4444;
    break;
  default:
  case kPXP_OutputPixelFormatARGB8888:
    InputFormat = kPXP_AsPixelFormatARGB8888;
    break;
  }
  //
  // No AS needed.
  // Configure input colors as PS.
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBuffer(pColor, NumItems * LCD_BYTES_PER_PIXEL, NumItems, 1, true);
  PXP_DisableProcessSurface();
  _PXP_ConfigAlphaSurfaceEx(pIndex, NumItems * SizeOfIndex, NumItems, 1, InputFormat);
  _PXP_ConfigAlphaBlendNormal();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       Static code: Display init
*
**********************************************************************
*/
/*********************************************************************
*
*       _VSyncCallback
*/
static void _VSyncCallback(void *p, void *SwitchOffBuffer) {
  GUI_USE_PARA(p);
  GUI_USE_PARA(SwitchOffBuffer);
  if (_PendingBuffer >= 0) {
    GUI_MULTIBUF_Confirm(_PendingBuffer);
#if (USE_OS == 1)
      _Semaphore_Give(_hSema_WaitForVSYNC);
#endif
    _PendingBuffer = -1;
  } else if (_PendingBuffer == -2) {
    _PendingBuffer = -1;
  }
}

/*********************************************************************
*
*       _InitPins
*/
static void _InitPins(void) {
  CLOCK_EnableClock(kCLOCK_Iomuxc);           /* LPCG on: LPCG is ON. */
  CLOCK_EnableClock(kCLOCK_Iomuxc_Lpsr);      /* LPCG on: LPCG is ON. */

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_00_GPIO8_IO31,           /* GPIO_AD_00 is configured as GPIO8_IO31 */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_AD_00 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_01_GPIO9_IO00,           /* GPIO_AD_01 is configured as GPIO9_IO00 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_02_GPIO9_IO01,           /* GPIO_AD_02 is configured as GPIO9_IO01 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_30_GPIO9_IO29,           /* GPIO_AD_30 is configured as GPIO9_IO29 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_05_GPIO11_IO06,     /* GPIO_DISP_B2_05 is configured as GPIO11_IO06 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_LPSR_04_LPI2C5_SDA,         /* GPIO_LPSR_04 is configured as LPI2C5_SDA */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_LPSR_04 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_LPSR_05_LPI2C5_SCL,         /* GPIO_LPSR_05 is configured as LPI2C5_SCL */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_LPSR_05 */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_00_GPIO8_IO31,           /* GPIO_AD_00 PAD functional properties : */
      0x0EU);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Enable
                                                 Pull Up / Down Config. Field: Weak pull up
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_01_GPIO9_IO00,           /* GPIO_AD_01 PAD functional properties : */
      0x0EU);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Enable
                                                 Pull Up / Down Config. Field: Weak pull up
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_02_GPIO9_IO01,           /* GPIO_AD_02 PAD functional properties : */
      0x06U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Enable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_30_GPIO9_IO29,           /* GPIO_AD_30 PAD functional properties : */
      0x06U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Enable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_05_GPIO11_IO06,     /* GPIO_DISP_B2_05 PAD functional properties : */
      0x02U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Disable, Highz
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_LPSR_04_LPI2C5_SDA,         /* GPIO_LPSR_04 PAD functional properties : */
      0x20U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: normal driver
                                                 Pull / Keep Select Field: Pull Disable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain LPSR Field: Enabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_LPSR_05_LPI2C5_SCL,         /* GPIO_LPSR_05 PAD functional properties : */
      0x20U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: normal driver
                                                 Pull / Keep Select Field: Pull Disable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain LPSR Field: Enabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
}

/*********************************************************************
*
*       _PullPanelResetPin
*/
static void _PullPanelResetPin(bool pullUp) {
  if (pullUp) {
    GPIO_PinWrite(MIPI_PANEL_RST_GPIO, MIPI_PANEL_RST_PIN, 1);
  }
  else {
    GPIO_PinWrite(MIPI_PANEL_RST_GPIO, MIPI_PANEL_RST_PIN, 0);
  }
}

/*********************************************************************
*
*       _PullPanelPowerPin
*/
static void _PullPanelPowerPin(bool pullUp) {
  if (pullUp) {
    GPIO_PinWrite(MIPI_PANEL_POWER_GPIO, MIPI_PANEL_POWER_PIN, 1);
  }
  else {
    GPIO_PinWrite(MIPI_PANEL_POWER_GPIO, MIPI_PANEL_POWER_PIN, 0);
  }
}

/*********************************************************************
*
*       _DSI_Transfer
*/
static int _DSI_Transfer(dsi_transfer_t *xfer) {
  return DSI_TransferBlocking(MIPI_DSI, xfer);
}

/*********************************************************************
*
*       _InitLcdPanel
*/
static int _InitLcdPanel(void) {
  int status;
  const gpio_pin_config_t pinConfig = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
  const display_config_t displayConfig = {
    .resolution   = FSL_VIDEO_RESOLUTION(XSIZE_PHYS, YSIZE_PHYS),
    .hsw          = HSW,
    .hfp          = HFP,
    .hbp          = HBP,
    .vsw          = VSW,
    .vfp          = VFP,
    .vbp          = VBP,
    .controlFlags = 0,
    .dsiLanes     = MIPI_DSI_LANE_NUM,
  };

  GPIO_PinInit(MIPI_PANEL_POWER_GPIO, MIPI_PANEL_POWER_PIN, &pinConfig);
  GPIO_PinInit(MIPI_PANEL_BL_GPIO, MIPI_PANEL_BL_PIN, &pinConfig);
  GPIO_PinInit(MIPI_PANEL_RST_GPIO, MIPI_PANEL_RST_PIN, &pinConfig);
  status = RM68200_Init(&rm68200Handle, &displayConfig);
  if (status == kStatus_Success) {
    GPIO_PinWrite(MIPI_PANEL_BL_GPIO, MIPI_PANEL_BL_PIN, 1);
  }
  return status;
}

/*********************************************************************
*
*       _SetMipiDsiConfig
*/
static void _SetMipiDsiConfig(void) {
  dsi_config_t           dsiConfig;
  dsi_dphy_config_t      dphyConfig;
  const dsi_dpi_config_t dpiConfig = {
    .pixelPayloadSize = XSIZE_PHYS,
    .dpiColorCoding   = kDSI_Dpi24Bit,
    .pixelPacket      = kDSI_PixelPacket24Bit,
    .videoMode        = kDSI_DpiBurst,
    .bllpMode         = kDSI_DpiBllpLowPower,
    .polarityFlags    = kDSI_DpiVsyncActiveLow | kDSI_DpiHsyncActiveLow,
    .hfp              = HFP,
    .hbp              = HBP,
    .hsw              = HSW,
    .vfp              = VFP,
    .vbp              = VBP,
    .panelHeight      = YSIZE_PHYS,
    .virtualChannel   = 0
  };
  DSI_GetDefaultConfig(&dsiConfig);
  dsiConfig.numLanes       = MIPI_DSI_LANE_NUM;
  dsiConfig.autoInsertEoTp = true;
  //
  /// Init the DSI module.
  //
  DSI_Init(MIPI_DSI, &dsiConfig);

  // Init DPHY.
  //
  // The DPHY bit clock must be fast enough to send out the pixels, it should be
  // larger than:
  //
  //         (Pixel clock * bit per output pixel) / number of MIPI data lane
  //
  // Here the desired DPHY bit clock multiplied by ( 9 / 8 = 1.125) to ensure
  // it is fast enough.
  //
  // Note that the DSI output pixel is 24bit per pixel.
  //
  mipiDsiDphyBitClkFreq_Hz = mipiDsiDpiClkFreq_Hz * (24 / MIPI_DSI_LANE_NUM);
  mipiDsiDphyBitClkFreq_Hz = MIPI_DPHY_BIT_CLK_ENLARGE(mipiDsiDphyBitClkFreq_Hz);
  DSI_GetDphyDefaultConfig(&dphyConfig, mipiDsiDphyBitClkFreq_Hz, mipiDsiTxEscClkFreq_Hz);
  mipiDsiDphyBitClkFreq_Hz = DSI_InitDphy(MIPI_DSI, &dphyConfig, mipiDsiDphyRefClkFreq_Hz);
  //
  // Init DPI interface.
  //
  DSI_SetDpiConfig(MIPI_DSI, &dpiConfig, MIPI_DSI_LANE_NUM, mipiDsiDpiClkFreq_Hz, mipiDsiDphyBitClkFreq_Hz);
}

/*********************************************************************
*
*       _InitMipiDsiClock
*/
static void _InitMipiDsiClock(void) {
  U32 mipiDsiEscClkFreq_Hz;

  //
  // RxClkEsc max 60MHz, TxClkEsc 12 to 20MHz.
  // RxClkEsc = 528MHz / 11 = 48MHz.
  // TxClkEsc = 528MHz / 11 / 4 = 16MHz.
  //
  const clock_root_config_t mipiEscClockConfig = {
    .clockOff = false,
    .mux      = 4,  // PLL_528
    .div      = 11,
  };
  const clock_group_config_t mipiEscClockGroupConfig = {
    .clockOff = false,
    .resetDiv = 2,
    .div0     = 2,
  };
  //
  // DPHY reference clock, use OSC 24MHz clock.
  //
  const clock_root_config_t mipiDphyRefClockConfig = {
      .clockOff = false,
      .mux      = 1,  // OSC_24M.
      .div      = 1,
  };

  CLOCK_SetRootClock(kCLOCK_Root_Mipi_Esc, &mipiEscClockConfig);
  mipiDsiEscClkFreq_Hz = CLOCK_GetRootClockFreq(kCLOCK_Root_Mipi_Esc);
  CLOCK_SetGroupConfig(kCLOCK_Group_MipiDsi, &mipiEscClockGroupConfig);
  mipiDsiTxEscClkFreq_Hz = mipiDsiEscClkFreq_Hz / 3;
  CLOCK_SetRootClock(kCLOCK_Root_Mipi_Ref, &mipiDphyRefClockConfig);
  mipiDsiDphyRefClkFreq_Hz = BOARD_XTAL0_CLK_HZ;
}

/*********************************************************************
*
*       _InitDisplayInterface
*/
static int _InitDisplayInterface(void) {
  CLOCK_EnableClock(kCLOCK_Video_Mux);

  //
  // LCDIF v2 output to MIPI DSI.
  //
  VIDEO_MUX->VID_MUX_CTRL.SET = VIDEO_MUX_VID_MUX_CTRL_MIPI_DSI_SEL_MASK;
  //
  // 1. Power on and isolation off.
  //
  PGMC_BPC4->BPC_POWER_CTRL |= (PGMC_BPC_BPC_POWER_CTRL_PSW_ON_SOFT_MASK | PGMC_BPC_BPC_POWER_CTRL_ISO_OFF_SOFT_MASK);
  //
  // 2. Assert MIPI reset.
  //
  IOMUXC_GPR->GPR62 &= ~(IOMUXC_GPR_GPR62_MIPI_DSI_PCLK_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_ESC_SOFT_RESET_N_MASK |
                         IOMUXC_GPR_GPR62_MIPI_DSI_BYTE_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_DPI_SOFT_RESET_N_MASK);
  //
  // 3. Setup clock.
  //
  _InitMipiDsiClock();
  //
  // 4. Deassert PCLK and ESC reset.
  //
  IOMUXC_GPR->GPR62 |= (IOMUXC_GPR_GPR62_MIPI_DSI_PCLK_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_ESC_SOFT_RESET_N_MASK);
  //
  // 5. Configures peripheral.
  //
  _SetMipiDsiConfig();
  //
  // 6. Deassert BYTE and DBI reset.
  //
  IOMUXC_GPR->GPR62 |= (IOMUXC_GPR_GPR62_MIPI_DSI_BYTE_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_DPI_SOFT_RESET_N_MASK);
  //
  // 7. Configure the panel.
  //
  return _InitLcdPanel();
}

/*********************************************************************
*
*       _VerifyDisplayClockSource
*/
static int _VerifyDisplayClockSource(void) {
  int status;
  U32 srcClkFreq;

  // 
  // In this implementation, the SYSPLL2 (528M) clock is used as the source
  // of LCDIFV2 pixel clock and MIPI DSI ESC clock. The OSC24M clock is used
  // as the MIPI DSI DPHY PLL reference clock. This function checks the clock
  // source are valid. OSC24M is always valid, so only verify the SYSPLL2.
  // 
  srcClkFreq = CLOCK_GetPllFreq(kCLOCK_PllSys2);
  if (528 != (srcClkFreq / 1000000)) {
    status = kStatus_Fail;
  } else {
    status = kStatus_Success;
  }
  return status;
}

/*********************************************************************
*
*       _InitLcdifClock
*/
static void _InitLcdifClock(void) {
  // 
  // The pixel clock is (height + VSW + VFP + VBP) * (width + HSW + HFP + HBP) * frame rate.
  // 
  // For 60Hz frame rate, the RK055IQH091 pixel clock should be 36MHz.
  // the RK055AHD091 pixel clock should be 62MHz.
  // 
  const clock_root_config_t lcdifClockConfig = {
    .clockOff = false,
    .mux      = 4, /*!< PLL_528. */
    .div = 9,
  };
  CLOCK_SetRootClock(kCLOCK_Root_Lcdifv2, &lcdifClockConfig);
  mipiDsiDpiClkFreq_Hz = CLOCK_GetRootClockFreq(kCLOCK_Root_Lcdifv2);
}

/*********************************************************************
*
*       _PrepareDisplayController
*/
static int _PrepareDisplayController(void) {
  int status;

  status = _VerifyDisplayClockSource();
  if (status != kStatus_Success) {
    return status;
  }
  _InitLcdifClock();
  status = _InitDisplayInterface();
  if (kStatus_Success == status) {
    NVIC_ClearPendingIRQ(LCDIFv2_IRQn);
    NVIC_SetPriority(LCDIFv2_IRQn, 8);
    EnableIRQ(LCDIFv2_IRQn);
  }
  return kStatus_Success;
}

/*********************************************************************
*
*       _InitController
*/
static void _InitController(void) {
  static U8    Done;
  int          status;
  dc_fb_info_t fbInfo;

  if (Done == 0) {
#if (USE_OS == 1)
    _Semaphore_Create(&_hSema_WaitForPXP);
    _Semaphore_Create(&_hSema_WaitForVSYNC);
#endif
    //
    // Set up GPIOs
    //
    _InitPins();
    //
    // Set up LCD controller
    //
    _PrepareDisplayController();

    status = g_dc.ops->init(&g_dc);
    if (kStatus_Success != status) {
      while (1);
    }
    g_dc.ops->getLayerDefaultConfig(&g_dc, 0, &fbInfo);
    fbInfo.pixelFormat = PIXEL_FORMAT;
    fbInfo.width       = XSIZE_PHYS;
    fbInfo.height      = YSIZE_PHYS;
    fbInfo.strideBytes = XSIZE_PHYS * LCD_BYTES_PER_PIXEL;
    g_dc.ops->setLayerConfig(&g_dc, 0, &fbInfo);
    g_dc.ops->setCallback(&g_dc, 0, _VSyncCallback, NULL);
    //
    // Init PXP
    //
    PXP_Init(PXP);
    PXP_READ_QOS = 15;
#if (USE_OS == 1)
    NVIC_EnableIRQ(PXP_IRQn);
    NVIC_SetPriority(PXP_IRQn, 0xf);
    PXP_EnableInterrupts( PXP, kPXP_CompleteInterruptEnable );
#endif
    Done = 1;
  }
}

/*********************************************************************
*
*       _ClearCache
*/
static void _ClearCache(U32 v) {
  GUI_USE_PARA(v);
  SCB_CleanInvalidateDCache();
}

/*********************************************************************
*
*       _ConfigDriver
*
* Purpose:
*   Called from LCD_ROTATE_SetSelEx() after a new driver has been selected in order to
*   configure the new driver.
*/
static void _ConfigDriver(GUI_DEVICE * pDevice, int Index, int LayerIndex) {
  int EnablePXP;

  GUI_USE_PARA(Index);
  GUI_USE_PARA(LayerIndex);
  //
  // Display driver configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, (void *)_aBufferAddress[0]);
  //
  // Enable PXP accelerator
  //
  LCD_PXP_Enable(1);
  //
  // Update the current display rotation angle for PXP.
  //
  if      ((pDevice->pDeviceAPI == GUIDRV_LIN_32)     || (pDevice->pDeviceAPI == GUIDRV_LIN_16))     {
    _PXP_Rotation = kPXP_Rotate0;
  }
  else if ((pDevice->pDeviceAPI == GUIDRV_LIN_OSX_32) || (pDevice->pDeviceAPI == GUIDRV_LIN_OSX_16)) {
    _PXP_Rotation = kPXP_Rotate90;
  }
  else if ((pDevice->pDeviceAPI == GUIDRV_LIN_OXY_32) || (pDevice->pDeviceAPI == GUIDRV_LIN_OXY_16)) {
    _PXP_Rotation = kPXP_Rotate180;
  }
  else if ((pDevice->pDeviceAPI == GUIDRV_LIN_OSY_32) || (pDevice->pDeviceAPI == GUIDRV_LIN_OSY_16)) {
    _PXP_Rotation = kPXP_Rotate270;
  }
}

/*********************************************************************
*
*       Interrupt handlers
*
**********************************************************************
*/
#if (USE_OS == 1)
/*********************************************************************
*
*       PXP_IRQHandler
*/
void PXP_IRQHandler(void);
void PXP_IRQHandler(void) {
  if (PXP_GetStatusFlags( PXP ) & kPXP_CompleteFlag ) {
    PXP_ClearStatusFlags( PXP, kPXP_CompleteFlag );
    _Semaphore_Give(_hSema_WaitForPXP);
  }
}
#endif

/*********************************************************************
*
*       LCDIFv2_IRQHandler
*/
void LCDIFv2_IRQHandler(void);
void LCDIFv2_IRQHandler(void) {
  DC_FB_LCDIFV2_IRQHandler(&g_dc);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_PXP_Enable
*/
void LCD_PXP_Enable(int Enable) {
  if (Enable) {
    GUI_AlphaEnableFillRectHW(1);
    LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT,   (void(*)(void))_LCD_PXP_FillRect);
    LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_LCD_PXP_CopyBuffer);
    LCD_SetDevFunc(0, LCD_DEVFUNC_COPYRECT,   (void(*)(void))_LCD_PXP_CopyRect);
    //
    // Set up functions for drawing bitmaps and memory devices with PXP
    //
    GUI_SetFuncDrawBitmapEx   (_LCD_PXP_DrawBitmapEx);
    GUI_SetFuncMixColorsBulk  (_LCD_PXP_MixColorsBulk);
    GUI_SetFuncDrawAlpha      (_LCD_PXP_DrawMemdevAlpha,
        (GUI_DRAWBITMAP_FUNC *)_LCD_PXP_DrawBitmap32bpp);
#if GUI_SUPPORT_MEMDEV
#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW
    GUI_MEMDEV_SetDrawMemdev16bppFunc(_LCD_PXP_DrawMemdev16bpp);
#endif
    GUI_SetFuncDrawM565       (_LCD_PXP_DrawMemdevM565,
                               _LCD_PXP_DrawBitmapM565);
    //
    // Memdev manupulation functions with PXP
    //
    GUI_MEMDEV_SetBlendFunc   (_LCD_PXP_BlendMemdev);
#endif
    //
    // Color conversion functions
    //
    GUI_PXP_SETCOLORCONV(M1555I);
    GUI_PXP_SETCOLORCONV(M565);
    GUI_PXP_SETCOLORCONV(M4444I);
    GUI_PXP_SETCOLORCONV(M8888I);
    GUICC_M888_SetCustColorConv(_LCD_PXP_Color2IndexBulk_M888, NULL);
  } else {
    GUI_AlphaEnableFillRectHW(0);
    LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT,   (void(*)(void))NULL);
    LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))NULL);
    LCD_SetDevFunc(0, LCD_DEVFUNC_COPYRECT,   (void(*)(void))NULL);
    //
    // Clear functions for drawing bitmaps and memory devices with PXP
    //
    GUI_SetFuncDrawBitmapEx   (NULL);
    GUI_SetFuncMixColorsBulk  (NULL);
    GUI_SetFuncDrawAlpha      (NULL,
        (GUI_DRAWBITMAP_FUNC *)NULL);
#if GUI_SUPPORT_MEMDEV
#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW
    GUI_MEMDEV_SetDrawMemdev16bppFunc(NULL);
#endif
    GUI_SetFuncDrawM565       (NULL,
                               NULL);
    //
    // Memdev manupulation functions with PXP
    //
    GUI_MEMDEV_SetBlendFunc   (NULL);
#endif
    //
    // Color conversion functions
    //
    GUICC_M1555I_SetCustColorConv(NULL, NULL);
    GUICC_M565_SetCustColorConv  (NULL, NULL);
    GUICC_M4444I_SetCustColorConv(NULL, NULL);
    GUICC_M8888I_SetCustColorConv(NULL, NULL);
    GUICC_M888_SetCustColorConv  (NULL, NULL);
  }
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
  GUI_DEVICE * pDevice;

#if (NUM_BUFFERS > 1)
  GUI_MULTIBUF_Config(NUM_BUFFERS);
#endif
  //
  // Set display driver and color conversion for 1st layer
  //
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  //
  // Configure the initial driver
  //
  _ConfigDriver(pDevice, 0, GUI_ROTATION_0);
  //
  // If screen rotation is not required the following section could be switched off to spare ROM
  //
#if ENABLE_DISPLAY_ROTATION
  //
  // Create additional drivers and add them to the display rotation module
  //
  LCD_ROTATE_AddDriverExOrientation(DISPLAY_DRIVER_CCW, 0, GUI_ROTATION_CCW);
  LCD_ROTATE_AddDriverExOrientation(DISPLAY_DRIVER_180, 0, GUI_ROTATION_180);
  LCD_ROTATE_AddDriverExOrientation(DISPLAY_DRIVER_CW,  0, GUI_ROTATION_CW);
  //
  // Set callback function to be used for layer 0
  //
  LCD_ROTATE_SetCallback(_ConfigDriver, 0);
#endif
  //
  // Cache management
  //
  GUI_DCACHE_SetClearCacheHook(_ClearCache);
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
  LCD_X_SHOWBUFFER_INFO *pInfo;
  GUI_USE_PARA(LayerIndex);
  r = 0;
  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    _InitController();
    PID_X_Init();
    break;
  }
  case LCD_X_ON: {
    _PendingBuffer = -2;
    g_dc.ops->setFrameBuffer(&g_dc, 0, (void *)_aBufferAddress[_BackBuffer]);
    //
    // Wait for frame buffer sent to display controller video memory.
    //
    if ((g_dc.ops->getProperty(&g_dc) & kDC_FB_ReserveFrameBuffer) == 0) {
      while (_PendingBuffer == -2);
    }
    g_dc.ops->enableLayer(&g_dc, 0);
    break;
  }
  case LCD_X_SHOWBUFFER: {
    pInfo          = (LCD_X_SHOWBUFFER_INFO *)pData;
    _PendingBuffer = pInfo->Index;
    //
    // Make the given buffer visible
    //
    g_dc.ops->setFrameBuffer(&g_dc, 0, (void *)_aBufferAddress[pInfo->Index]);
    //
    // Wait for VSYNC ISR to restore '_PendingBuffer'
    //
#if (USE_OS == 1)
    _Semaphore_Take(_hSema_WaitForVSYNC);
#else
    while (_PendingBuffer >= 0);
#endif
    return 0;
  }
  default:
    r = -1;
    break;
  }
  return r;
}

/*************************** End of file ****************************/
