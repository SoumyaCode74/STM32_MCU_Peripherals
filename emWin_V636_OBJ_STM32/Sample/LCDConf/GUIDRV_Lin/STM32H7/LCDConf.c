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

The part between 'DISPLAY CONFIGURATION START' and 'DISPLAY CONFIGURA-
TION END' can be used to configure the following for each layer:

- Color mode
- Layer size
- Layer orientation

Further the background color used on positions without a valid layer
can be set here.

This file was created to be used with the STM32F746 - Discovery board.
But it can be used with almost any other STM32F7 when the GPIO settings
are getting adapted.

---------------------------END-OF-HEADER------------------------------
*/
#include "GUI.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"
#include "PIDConf.h"
#include "RTOS.h"
#include "stm32h743xx.h"
//#include "stm32h743i_eval.h"
#include "System/stm32h743i_eval.h"

/*********************************************************************
*
*       Supported orientation modes (not to be changed)
*/
#define ROTATION_0   0
#define ROTATION_CW  1
#define ROTATION_180 2
#define ROTATION_CCW 3

/*********************************************************************
*
*       Supported color modes (not to be changed)
*/
//      Color mode       Conversion     Driver (default orientation)
//      ---------------------------------------------
#define CM_ARGB8888 1 // GUICC_M8888I - GUIDRV_LIN_32
#define CM_RGB888   2 // GUICC_M888   - GUIDRV_LIN_24
#define CM_RGB565   3 // GUICC_M565   - GUIDRV_LIN_16
#define CM_ARGB1555 4 // GUICC_M1555I - GUIDRV_LIN_16
#define CM_ARGB4444 5 // GUICC_M4444I - GUIDRV_LIN_16
#define CM_L8       6 // GUICC_8666   - GUIDRV_LIN_8
#define CM_AL44     7 // GUICC_1616I  - GUIDRV_LIN_8
#define CM_AL88     8 // GUICC_88666I - GUIDRV_LIN_16

/*********************************************************************
**********************************************************************
*
*       DISPLAY CONFIGURATION START (TO BE MODIFIED)
*
**********************************************************************
**********************************************************************
*/

#define MB_1063 0
#define MB_1046 1

#define LCD_PANEL MB_1063

/*********************************************************************
*
*       Common
*/
//
// Physical display size
//
#if   (LCD_PANEL == MB_1046)
  #define XSIZE_PHYS 480
  #define YSIZE_PHYS 272
#elif (LCD_PANEL == MB_1063)
  #define XSIZE_PHYS 640
  #define YSIZE_PHYS 480
#endif

//
// Buffers / VScreens
//
#define NUM_BUFFERS  3 // Number of multiple buffers to be used (at least 1 buffer)
#define NUM_VSCREENS 1 // Number of virtual  screens to be used (at least 1 screen)

//
// Redefine number of layers for this configuration file. Must be equal or less than in GUIConf.h!
//
#undef  NUM_LAYERS
#define NUM_LAYERS 1

/*********************************************************************
*
*       Layer 0
*/
//
// Color mode layer 0. Should be one of the above defined color modes
//
//#define COLOR_MODE_0 CM_RGB888
#define COLOR_MODE_0 CM_ARGB8888
//#define COLOR_MODE_0 CM_RGB565
//#define COLOR_MODE_0 CM_L8

//
// Size of layer 0. Ignored and replaced by XSIZE_PHYS and YSIZE_PHYS if NUM_LAYERS == 1
//
#define XSIZE_0 XSIZE_PHYS
#define YSIZE_0 YSIZE_PHYS
//#define XSIZE_0 10
//#define YSIZE_0 10

//
// Orientation of layer 0. Should be one of the above defined display orientations.
//
#define ORIENTATION_0 ROTATION_0

/*********************************************************************
*
*       Layer 1
*/
//
// Color mode layer 1. Should be one of the above defined color modes
//
//#define COLOR_MODE_1 CM_RGB888
#define COLOR_MODE_1 CM_ARGB8888
//#define COLOR_MODE_1 CM_RGB565
//#define COLOR_MODE_1 CM_L8

//
// Size of layer 1
//
//#define XSIZE_1 100
//#define YSIZE_1 100
#define XSIZE_1 50
#define YSIZE_1 50

//
// Orientation of layer 1. Should be one of the above defined display orientations.
//
#define ORIENTATION_1 ROTATION_0

/*********************************************************************
*
*       Background color shown where no layer is active
*/
#define BK_COLOR GUI_DARKBLUE

/*********************************************************************
**********************************************************************
*
*       DISPLAY CONFIGURATION END
*
**********************************************************************
**********************************************************************
*/
/*********************************************************************
*
*       Automatic selection of driver and color conversion
*/
#if   (COLOR_MODE_0 == CM_ARGB8888)
  #define COLOR_CONVERSION_0 GUICC_M8888I
#elif (COLOR_MODE_0 == CM_RGB888)
  #define COLOR_CONVERSION_0 GUICC_M888
#elif (COLOR_MODE_0 == CM_RGB565)
  #define COLOR_CONVERSION_0 GUICC_M565
#elif (COLOR_MODE_0 == CM_ARGB1555)
  #define COLOR_CONVERSION_0 GUICC_M1555I
#elif (COLOR_MODE_0 == CM_ARGB4444)
  #define COLOR_CONVERSION_0 GUICC_M4444I
#elif (COLOR_MODE_0 == CM_L8)
  #define COLOR_CONVERSION_0 GUICC_8666
#elif (COLOR_MODE_0 == CM_AL44)
  #define COLOR_CONVERSION_0 GUICC_1616I
#elif (COLOR_MODE_0 == CM_AL88)
  #define COLOR_CONVERSION_0 GUICC_88666I
#else
  #error Illegal color mode 0!
#endif

#if   (COLOR_MODE_0 == CM_ARGB8888)
  #if   (ORIENTATION_0 == ROTATION_0)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_32
  #elif (ORIENTATION_0 == ROTATION_CW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSX_32
  #elif (ORIENTATION_0 == ROTATION_180)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OXY_32
  #elif (ORIENTATION_0 == ROTATION_CCW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSY_32
  #endif
#elif (COLOR_MODE_0 == CM_RGB888)
  #if   (ORIENTATION_0 == ROTATION_0)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_24
  #elif (ORIENTATION_0 == ROTATION_CW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSX_24
  #elif (ORIENTATION_0 == ROTATION_180)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OXY_24
  #elif (ORIENTATION_0 == ROTATION_CCW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSY_24
  #endif
#elif (COLOR_MODE_0 == CM_RGB565)   \
   || (COLOR_MODE_0 == CM_ARGB1555) \
   || (COLOR_MODE_0 == CM_ARGB4444) \
   || (COLOR_MODE_0 == CM_AL88)
  #if   (ORIENTATION_0 == ROTATION_0)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_16
  #elif (ORIENTATION_0 == ROTATION_CW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSX_16
  #elif (ORIENTATION_0 == ROTATION_180)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OXY_16
  #elif (ORIENTATION_0 == ROTATION_CCW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSY_16
  #endif
#elif (COLOR_MODE_0 == CM_L8)   \
   || (COLOR_MODE_0 == CM_AL44)
  #if   (ORIENTATION_0 == ROTATION_0)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_8
  #elif (ORIENTATION_0 == ROTATION_CW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSX_8
  #elif (ORIENTATION_0 == ROTATION_180)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OXY_8
  #elif (ORIENTATION_0 == ROTATION_CCW)
    #define DISPLAY_DRIVER_0   GUIDRV_LIN_OSY_8
  #endif
#endif


#if (NUM_LAYERS > 1)

#if   (COLOR_MODE_1 == CM_ARGB8888)
  #define COLOR_CONVERSION_1 GUICC_M8888I
#elif (COLOR_MODE_1 == CM_RGB888)
  #define COLOR_CONVERSION_1 GUICC_M888
#elif (COLOR_MODE_1 == CM_RGB565)
  #define COLOR_CONVERSION_1 GUICC_M565
#elif (COLOR_MODE_1 == CM_ARGB1555)
  #define COLOR_CONVERSION_1 GUICC_M1555I
#elif (COLOR_MODE_1 == CM_ARGB4444)
  #define COLOR_CONVERSION_1 GUICC_M4444I
#elif (COLOR_MODE_1 == CM_L8)
  #define COLOR_CONVERSION_1 GUICC_8666
#elif (COLOR_MODE_1 == CM_AL44)
  #define COLOR_CONVERSION_1 GUICC_1616I
#elif (COLOR_MODE_1 == CM_AL88)
  #define COLOR_CONVERSION_1 GUICC_88666I
#else
  #error Illegal color mode 0!
#endif

#if   (COLOR_MODE_1 == CM_ARGB8888)
  #if   (ORIENTATION_1 == ROTATION_0)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_32
  #elif (ORIENTATION_1 == ROTATION_CW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSX_32
  #elif (ORIENTATION_1 == ROTATION_180)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OXY_32
  #elif (ORIENTATION_1 == ROTATION_CCW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSY_32
  #endif
#elif (COLOR_MODE_1 == CM_RGB888)
  #if   (ORIENTATION_1 == ROTATION_0)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_24
  #elif (ORIENTATION_1 == ROTATION_CW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSX_24
  #elif (ORIENTATION_1 == ROTATION_180)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OXY_24
  #elif (ORIENTATION_1 == ROTATION_CCW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSY_24
  #endif
#elif (COLOR_MODE_1 == CM_RGB565)   \
   || (COLOR_MODE_1 == CM_ARGB1555) \
   || (COLOR_MODE_1 == CM_ARGB4444) \
   || (COLOR_MODE_1 == CM_AL88)
  #if   (ORIENTATION_1 == ROTATION_0)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_16
  #elif (ORIENTATION_1 == ROTATION_CW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSX_16
  #elif (ORIENTATION_1 == ROTATION_180)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OXY_16
  #elif (ORIENTATION_1 == ROTATION_CCW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSY_16
  #endif
#elif (COLOR_MODE_1 == CM_L8)   \
   || (COLOR_MODE_1 == CM_AL44)
  #if   (ORIENTATION_1 == ROTATION_0)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_8
  #elif (ORIENTATION_1 == ROTATION_CW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSX_8
  #elif (ORIENTATION_1 == ROTATION_180)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OXY_8
  #elif (ORIENTATION_1 == ROTATION_CCW)
    #define DISPLAY_DRIVER_1   GUIDRV_LIN_OSY_8
  #endif
#endif

#else

/*********************************************************************
*
*       Use complete display automatically in case of only one layer
*/
#undef XSIZE_0
#undef YSIZE_0
#define XSIZE_0 XSIZE_PHYS
#define YSIZE_0 YSIZE_PHYS

#endif

/*********************************************************************
*
*       Redirect bulk conversion to DMA2D routines
*/
#define DEFINE_DMA2D_COLORCONVERSION(PFIX, PIXELFORMAT)                                                        \
static void _Color2IndexBulk_##PFIX##_DMA2D(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) { \
  _DMA_Color2IndexBulk(pColor, pIndex, NumItems, SizeOfIndex, PIXELFORMAT);                                    \
}                                                                                                              \
static void _Index2ColorBulk_##PFIX##_DMA2D(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) { \
  _DMA_Index2ColorBulk(pIndex, pColor, NumItems, SizeOfIndex, PIXELFORMAT);                                    \
}

/*********************************************************************
*
*       H/V front/backporch and synchronization width/height
*/
#define  LCD_HSYNC            ((U16)30)      // Horizontal synchronization
#define  LCD_HBP              ((U16)114)     // Horizontal back porch
#define  LCD_HFP              ((U16)16)      // Horizontal front porch
#define  LCD_VSYNC            ((U16)3)       // Vertical synchronization
#define  LCD_VBP              ((U16)32)      // Vertical back porch
#define  LCD_VFP              ((U16)10)      // Vertical front porch

/*********************************************************************
*
*       Configuration checking
*/
#if NUM_BUFFERS > 3
  #error More than 3 buffers make no sense and are not supported in this configuration file!
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   NUM_BUFFERS
  #define NUM_BUFFERS 1
#else
  #if (NUM_BUFFERS <= 0)
    #error At least one buffer needs to be defined!
  #endif
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens together with multiple buffers are not allowed!
#endif

/*********************************************************************
*
*       Frame buffer base address
*/
#define LCD_FRAME_BUFFER ((U32)0xd1C00000)

/*********************************************************************
*
*       SFRs
*/

#define SCS_BASE                   (0xE000E000UL)                 // System Control Space Base Address
#define NVIC_BASE                  (SCS_BASE + 0x0100UL)          // NVIC Base Address      

#define GPIO_Pin_0                 ((U16)0x0001)
#define GPIO_Pin_1                 ((U16)0x0002)
#define GPIO_Pin_2                 ((U16)0x0004)
#define GPIO_Pin_3                 ((U16)0x0008)
#define GPIO_Pin_4                 ((U16)0x0010)
#define GPIO_Pin_5                 ((U16)0x0020)
#define GPIO_Pin_6                 ((U16)0x0040)
#define GPIO_Pin_7                 ((U16)0x0080)
#define GPIO_Pin_8                 ((U16)0x0100)
#define GPIO_Pin_9                 ((U16)0x0200)
#define GPIO_Pin_10                ((U16)0x0400)
#define GPIO_Pin_11                ((U16)0x0800)
#define GPIO_Pin_12                ((U16)0x1000)
#define GPIO_Pin_13                ((U16)0x2000)
#define GPIO_Pin_14                ((U16)0x4000)
#define GPIO_Pin_15                ((U16)0x8000)
#define GPIO_Pin_All               ((U16)0xFFFF)

#define GPIO_PinSource0            ((U8)0x00)
#define GPIO_PinSource1            ((U8)0x01)
#define GPIO_PinSource2            ((U8)0x02)
#define GPIO_PinSource3            ((U8)0x03)
#define GPIO_PinSource4            ((U8)0x04)
#define GPIO_PinSource5            ((U8)0x05)
#define GPIO_PinSource6            ((U8)0x06)
#define GPIO_PinSource7            ((U8)0x07)
#define GPIO_PinSource8            ((U8)0x08)
#define GPIO_PinSource9            ((U8)0x09)
#define GPIO_PinSource10           ((U8)0x0A)
#define GPIO_PinSource11           ((U8)0x0B)
#define GPIO_PinSource12           ((U8)0x0C)
#define GPIO_PinSource13           ((U8)0x0D)
#define GPIO_PinSource14           ((U8)0x0E)
#define GPIO_PinSource15           ((U8)0x0F)      

#define RCC_AHB1Periph_GPIOA       ((U32)0x00000001)
#define RCC_AHB1Periph_GPIOB       ((U32)0x00000002)
#define RCC_AHB1Periph_GPIOC       ((U32)0x00000004)
#define RCC_AHB1Periph_GPIOD       ((U32)0x00000008)
#define RCC_AHB1Periph_GPIOE       ((U32)0x00000010)
#define RCC_AHB1Periph_GPIOF       ((U32)0x00000020)
#define RCC_AHB1Periph_GPIOG       ((U32)0x00000040)
#define RCC_AHB1Periph_GPIOH       ((U32)0x00000080)
#define RCC_AHB1Periph_GPIOI       ((U32)0x00000100)
#define RCC_AHB1Periph_GPIOJ       ((U32)0x00000200)
#define RCC_AHB1Periph_GPIOK       ((U32)0x00000400)
#define RCC_AHB1Periph_DMA2D       ((U32)0x00800000)
#define RCC_APB2Periph_LTDC        ((U32)0x04000000)

#define RCC_DCKCFGR_PLLSAIDIVR     ((U32)0x00030000)

#define RCC_PLLSAIDivR_Div8        ((U32)0x00020000)

#define RCC_FLAG_PLLSAIRDY         ((U8)0x3D)



// Bit definition for DMA2D_IFSR register
#define DMA2D_IFSR_CTEIF           ((U32)0x00000001)              // Clears Transfer Error Interrupt Flag
#define DMA2D_IFSR_CTCIF           ((U32)0x00000002)              // Clears Transfer Complete Interrupt Flag
#define DMA2D_IFSR_CTWIF           ((U32)0x00000004)              // Clears Transfer Watermark Interrupt Flag
#define DMA2D_IFSR_CCAEIF          ((U32)0x00000008)              // Clears CLUT Access Error Interrupt Flag
#define DMA2D_IFSR_CCTCIF          ((U32)0x00000010)              // Clears CLUT Transfer Complete Interrupt Flag
#define DMA2D_IFSR_CCEIF           ((U32)0x00000020)              // Clears Configuration Error Interrupt Flag

// Common LTDC
#define GCR_MASK                   ((U32)0x0FFE888F)              // LTDC GCR Mask

#define LTDC_Pixelformat_ARGB8888  ((U32)0x00000000)
#define LTDC_Pixelformat_RGB888    ((U32)0x00000001)
#define LTDC_Pixelformat_RGB565    ((U32)0x00000002)
#define LTDC_Pixelformat_ARGB1555  ((U32)0x00000003)
#define LTDC_Pixelformat_ARGB4444  ((U32)0x00000004)
#define LTDC_Pixelformat_L8        ((U32)0x00000005)
#define LTDC_Pixelformat_AL44      ((U32)0x00000006)
#define LTDC_Pixelformat_AL88      ((U32)0x00000007)
#define LTDC_Pixelformat_L4        ((U32)0x00000008)

// Common
#define __NVIC_PRIO_BITS           4                              // STM32F4XX uses 4 Bits for the Priority Levels

#define LTDC_IRQn                  88                             // LTDC global Interrupt
#define DMA2D_IRQn                 90                             // DMA2D global Interrupt

#define PLLSAION_BitNumber         0x1C

#define PERIPH_BB_BASE             ((U32)0x42000000)              // Peripheral base address in the bit-band region

#define RCC_OFFSET                 (RCC_BASE - PERIPH_BASE)
#define CR_OFFSET                  (RCC_OFFSET + 0x00)
#define CR_PLLSAION_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLSAION_BitNumber * 4))

#define FLAG_MASK                  ((U8)0x1F)

#define BUFFER_OFFSET              ((U32)0x130000)

#define DISABLE 0
#define ENABLE  1
#define RESET   0
#define SET     1

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

static LTDC_HandleTypeDef _hLTDC;
static U32                PCLK_profile = 0;

//static LTDC_Layer_TypeDef * _apLayer[] = { LTDC_Layer1, LTDC_Layer2 };
static LTDC_LayerCfgTypeDef _aLayer[NUM_LAYERS];
static const U32      _aAddr[]   = { LCD_FRAME_BUFFER, LCD_FRAME_BUFFER + XSIZE_PHYS * YSIZE_PHYS * sizeof(U32) * NUM_VSCREENS * NUM_BUFFERS };

static int _aPendingBuffer[2] = { -1, -1 };
static int _aBufferIndex[NUM_LAYERS];
static int _axSize[NUM_LAYERS] = {XSIZE_0, XSIZE_1};
static int _aySize[NUM_LAYERS] = {YSIZE_0, YSIZE_1};
static int _aBytesPerPixels[NUM_LAYERS];

static int _SupressCopyBuffer;

static volatile int _WaitForDMA2D;

//
// Prototypes of DMA2D color conversion routines
//
static void _DMA_Index2ColorBulk(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex, U32 PixelFormat);
static void _DMA_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, U32 PixelFormat);

//
// Color conversion routines using DMA2D
//
DEFINE_DMA2D_COLORCONVERSION(M8888I, LTDC_Pixelformat_ARGB8888)
DEFINE_DMA2D_COLORCONVERSION(M888,   LTDC_Pixelformat_ARGB8888) // Internal pixel format of emWin is 32 bit, because of that ARGB8888
//DEFINE_DMA2D_COLORCONVERSION(M565,   LTDC_Pixelformat_RGB565)
DEFINE_DMA2D_COLORCONVERSION(M1555I, LTDC_Pixelformat_ARGB1555)
DEFINE_DMA2D_COLORCONVERSION(M4444I, LTDC_Pixelformat_ARGB4444)

//
// Buffer for DMA2D color conversion, required because hardware does not support overlapping regions
//

#if (GUI_USE_ARGB == 0)
static U32 _aBuffer[XSIZE_PHYS * sizeof(U32) * 3];

static U32 * _pBuffer_DMA2D = &_aBuffer[XSIZE_PHYS * sizeof(U32) * 0];
static U32 * _pBuffer_FG    = &_aBuffer[XSIZE_PHYS * sizeof(U32) * 1];
static U32 * _pBuffer_BG    = &_aBuffer[XSIZE_PHYS * sizeof(U32) * 2];
#else
static U32 _aBuffer[40 * 40];  // Only required for drawing AA4 characters
#endif

//
// Array of color conversions for each layer
//
static const LCD_API_COLOR_CONV * _apColorConvAPI[] = {
  COLOR_CONVERSION_0,
#if NUM_LAYERS > 1
  COLOR_CONVERSION_1,
#endif
};

//
// Array of orientations for each layer
//
static const int _aOrientation[] = {
  ORIENTATION_0,
#if NUM_LAYERS > 1
  ORIENTATION_1,
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
*       _GetPixelformat
*/
static U32 _GetPixelformat(unsigned LayerIndex) {
  const LCD_API_COLOR_CONV * pColorConvAPI;

  if (LayerIndex >= GUI_COUNTOF(_apColorConvAPI)) {
    return 0;
  }
  pColorConvAPI = _apColorConvAPI[LayerIndex];
  if        (pColorConvAPI == GUICC_M8888I) {
    return LTDC_Pixelformat_ARGB8888;
  } else if (pColorConvAPI == GUICC_M888  ) {
    return LTDC_Pixelformat_RGB888;
  } else if (pColorConvAPI == GUICC_M565  ) {
    return LTDC_Pixelformat_RGB565;
  } else if (pColorConvAPI == GUICC_M1555I) {
    return LTDC_Pixelformat_ARGB1555;
  } else if (pColorConvAPI == GUICC_M4444I) {
    return LTDC_Pixelformat_ARGB4444;
  } else if (pColorConvAPI == GUICC_8666  ) {
    return LTDC_Pixelformat_L8;
  } else if (pColorConvAPI == GUICC_1616I ) {
    return LTDC_Pixelformat_AL44;
  } else if (pColorConvAPI == GUICC_88666I) {
    return LTDC_Pixelformat_AL88;
  }
  while (1); // Error
}

/*********************************************************************
*
*       _GetBytesPerLine
*/
static int _GetBytesPerLine(int LayerIndex, int xSize) {
  int BitsPerPixel, BytesPerLine;

  BitsPerPixel  = LCD_GetBitsPerPixelEx(LayerIndex);
  BytesPerLine = (BitsPerPixel * xSize + 7) / 8;
  return BytesPerLine;
}

/*********************************************************************
*
*       _LTDC_LayerCmd
*/
static void _LTDC_LayerCmd(LTDC_Layer_TypeDef * LTDC_Layerx, int NewState) {
  if (NewState != DISABLE) {
    LTDC_Layerx->CR |= (U32)LTDC_LxCR_LEN;
  } else {
    LTDC_Layerx->CR &= ~(U32)LTDC_LxCR_LEN;
  }
  LTDC->SRCR = LTDC_SRCR_VBR; // Reload on next blanking period
}

/*********************************************************************
*
*       _LTDC_LayerEnableColorKeying
*/
static void _LTDC_LayerEnableColorKeying(LTDC_Layer_TypeDef * LTDC_Layerx, int NewState) {
  if (NewState != DISABLE) {
    LTDC_Layerx->CR |= (U32)LTDC_LxCR_COLKEN;
  } else {
    LTDC_Layerx->CR &= ~(U32)LTDC_LxCR_COLKEN;
  }
  LTDC->SRCR = LTDC_SRCR_VBR; // Reload on next blanking period
}

/*********************************************************************
*
*       _LTDC_LayerEnableLUT
*/
static void _LTDC_LayerEnableLUT(LTDC_Layer_TypeDef * LTDC_Layerx, int NewState) {
  if (NewState != DISABLE) {
    LTDC_Layerx->CR |= (U32)LTDC_LxCR_CLUTEN;
  } else {
    LTDC_Layerx->CR &= ~(U32)LTDC_LxCR_CLUTEN;
  }
  LTDC->SRCR = LTDC_SRCR_IMR;//LTDC_SRCR_VBR; // Reload on next blanking period
}

/*********************************************************************
*
*       _LTDC_ITConfig
*/
static void _LTDC_ITConfig(U32 LTDC_IT, int NewState) {
  if (NewState != DISABLE) {
    LTDC->IER |= LTDC_IT;
  } else {
    LTDC->IER &= (U32)~LTDC_IT;
  }
}

/*********************************************************************
*
*       _LTDC_SetLUTEntry
*/
static void _LTDC_SetLUTEntry(int LayerIndex, U32 Color, int Pos) {
//  U32 r, g, b, a;
//
//  r = ( Color        & 0xff) << 16;
//  g = ((Color >>  8) & 0xff) <<  8;
//  b = ((Color >> 16) & 0xff);
//  a = Pos << 24;
//  _apLayer[LayerIndex]->CLUTWR &= ~(LTDC_LxCLUTWR_BLUE | LTDC_LxCLUTWR_GREEN | LTDC_LxCLUTWR_RED | LTDC_LxCLUTWR_CLUTADD);
//  _apLayer[LayerIndex]->CLUTWR  = r | g | b | a;
//  //
//  // Reload configuration
//  //
//  _LTDC_ReloadConfig(LTDC_SRCR_IMR);
//  HAL_LTDC_ConfigCLUT()
//  HAL_LTDC
}

/*********************************************************************
*
*       _NVIC_SetPriority
*/
static void _NVIC_SetPriority(int IRQn, U32 priority) {
  if(IRQn < 0) {
    while (1); // Not supported here, stop execution.
  } else {
    NVIC->IP[(U32)(IRQn)] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff); /* Set Priority for device specific Interrupts */
  }
}

/*********************************************************************
*
*       _NVIC_EnableIRQ
*/
static void _NVIC_EnableIRQ(int IRQn) {
  NVIC->ISER[(U32)((I32)IRQn) >> 5] = (U32)(1 << ((U32)((I32)IRQn) & (U32)0x1F)); /* Enable interrupt */
}

/*********************************************************************
*
*       _DMA_ExecOperation
*/
static void _DMA_ExecOperation(void) {
  int Timeout;
  SCB_CleanInvalidateDCache();
  //
  // Set Flag which gets cleared when DMA2D transfer is completed
  //
  _WaitForDMA2D = 1;
  //
  // Execute operation
  //
  DMA2D->CR |= 1  | (0x3F << 8);          // Control Register (Start operation)
  //
  // Wait until transfer is done
  //
  Timeout = 10000000;
  while (_WaitForDMA2D && Timeout) {
   Timeout--;
  }
  if (Timeout == 0) {
    DMA2D->CR |= (1 << 2);             // Abort
  }
}

/*********************************************************************
*
*       _DMA_Copy
*/
static void _DMA_Copy(int LayerIndex, const void * pSrc, void * pDst, int xSize, int ySize, int OffLineSrc, int OffLineDst) {
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00000000UL;         // Control Register (Memory to memory and TCIE)
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
*       _DMA_DrawAlphaBitmap
*/
/**/
static void _DMA_DrawAlphaBitmap(void * pDst, const void * pSrc, int xSize, int ySize, int OffLineSrc, int OffLineDst, int PixelFormat) {
  DMA2D->CR      = 0x00020000UL;         // Control Register (Memory to memory with blending of FG and BG and TCIE)
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->BGMAR   = (U32)pDst;                       // Background Memory Address Register (Destination address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  DMA2D->FGOR    = OffLineSrc;                      // Foreground Offset Register (Source line offset)
  DMA2D->BGOR    = OffLineDst;                      // Background Offset Register (Destination line offset)
  DMA2D->OOR     = OffLineDst;                      // Output Offset Register (Destination line offset)
  DMA2D->FGPFCCR = LTDC_Pixelformat_ARGB8888;       // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->BGPFCCR = PixelFormat;                     // Background PFC Control Register (Defines the destination pixel format)
  DMA2D->OPFCCR  = PixelFormat;                     // Output     PFC Control Register (Defines the output pixel format)
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  _DMA_ExecOperation();
}

#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW
/*********************************************************************
*
*       _DMA_CopyRGB565
*/
static void _DMA_CopyRGB565(const void * pSrc, void * pDst, int xSize, int ySize, int OffLineSrc, int OffLineDst) {
  DMA2D->CR      = 0x00000000UL;         // Control Register (Memory to memory and TCIE)
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  DMA2D->FGOR    = OffLineSrc;                      // Foreground Offset Register (Source line offset)
  DMA2D->OOR     = OffLineDst;                      // Output Offset Register (Destination line offset)
  DMA2D->FGPFCCR = LTDC_Pixelformat_RGB565;         // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  _DMA_ExecOperation();
}
#endif

/*********************************************************************
*
*       _DMA_Fill
*/
static void _DMA_Fill(int LayerIndex, void * pDst, int xSize, int ySize, int OffLine, U32 ColorIndex) {
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00030000UL;         // Control Register (Register to memory and TCIE)
  DMA2D->OCOLR   = ColorIndex;                      // Output Color Register (Color to be used)
  //
  // Set up pointers
  //
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  //
  // Set up offsets
  //
  DMA2D->OOR     = OffLine;                         // Output Offset Register (Destination line offset)
  //
  // Set up pixel format
  //
  DMA2D->OPFCCR  = PixelFormat;                     // Output PFC Control Register (Defines the output pixel format)
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
  _DMA_ExecOperation();
}

/*********************************************************************
*
*       _DMA_AlphaBlendingBulk
*/
static void _DMA_AlphaBlendingBulk(LCD_COLOR * pColorFG, LCD_COLOR * pColorBG, LCD_COLOR * pColorDst, U32 NumItems) {
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00020000UL;         // Control Register (Memory to memory with blending of FG and BG and TCIE)
  //
  // Set up pointers
  //
  DMA2D->FGMAR   = (U32)pColorFG;                   // Foreground Memory Address Register
  DMA2D->BGMAR   = (U32)pColorBG;                   // Background Memory Address Register
  DMA2D->OMAR    = (U32)pColorDst;                  // Output Memory Address Register (Destination address)
  //
  // Set up offsets
  //
  DMA2D->FGOR    = 0;                               // Foreground Offset Register
  DMA2D->BGOR    = 0;                               // Background Offset Register
  DMA2D->OOR     = 0;                               // Output Offset Register
  //
  // Set up pixel format
  //
  DMA2D->FGPFCCR = LTDC_Pixelformat_ARGB8888;       // Foreground PFC Control Register (Defines the FG pixel format)
  DMA2D->BGPFCCR = LTDC_Pixelformat_ARGB8888;       // Background PFC Control Register (Defines the BG pixel format)
  DMA2D->OPFCCR  = LTDC_Pixelformat_ARGB8888;       // Output     PFC Control Register (Defines the output pixel format)
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(NumItems << 16) | 1;       // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
  _DMA_ExecOperation();
}

/*********************************************************************
*
*       _DMA_MixColors
*
* Purpose:
*   Function for mixing up 2 colors with the given intensity.
*   If the background color is completely transparent the
*   foreground color should be used unchanged.
*/
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
  U32 ColorDst;

#if (GUI_USE_ARGB)
  if ((BkColor & 0xFF000000) == 0x00000000) {
    return Color;
  }
#else
  if ((BkColor & 0xFF000000) == 0xFF000000) {
    return Color;
  }
  Color   ^= 0xFF000000;
  BkColor ^= 0xFF000000;
#endif
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00020000UL;         // Control Register (Memory to memory with blending of FG and BG and TCIE)
  //
  // Set up pointers
  //
  DMA2D->FGMAR   = (U32)&Color;                   // Foreground Memory Address Register
  DMA2D->BGMAR   = (U32)&BkColor;                   // Background Memory Address Register
  DMA2D->OMAR    = (U32)&ColorDst;                  // Output Memory Address Register (Destination address)
  //
  // Set up pixel format
  //
  DMA2D->FGPFCCR = LTDC_Pixelformat_ARGB8888
                 | (1UL << 16)
                 | ((U32)Intens << 24);
  DMA2D->BGPFCCR = LTDC_Pixelformat_ARGB8888
                 | (0UL << 16)
                 | ((U32)(255 - Intens) << 24);
  DMA2D->OPFCCR  = LTDC_Pixelformat_ARGB8888;
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(1 << 16) | 1;              // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
  _DMA_ExecOperation();
#if (GUI_USE_ARGB == 0)
  ColorDst ^= 0xFF000000;
#endif
  return ColorDst;
}

/*********************************************************************
*
*       _DMA_MixColorsBulk
*/
static void _DMA_MixColorsBulk(LCD_COLOR * pColorFG, LCD_COLOR * pColorBG, LCD_COLOR * pColorDst, U8 Intens, U32 NumItems) {
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00020000UL;         // Control Register (Memory to memory with blending of FG and BG and TCIE)
  //
  // Set up pointers
  //
  DMA2D->FGMAR   = (U32)pColorFG;                   // Foreground Memory Address Register
  DMA2D->BGMAR   = (U32)pColorBG;                   // Background Memory Address Register
  DMA2D->OMAR    = (U32)pColorDst;                  // Output Memory Address Register (Destination address)
  //
  // Set up pixel format
  //
  DMA2D->FGPFCCR = LTDC_Pixelformat_ARGB8888
                 | (1UL << 16)
                 | ((U32)Intens << 24);
  DMA2D->BGPFCCR = LTDC_Pixelformat_ARGB8888
                 | (0UL << 16)
                 | ((U32)(255 - Intens) << 24);
  DMA2D->OPFCCR  = LTDC_Pixelformat_ARGB8888;
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(NumItems << 16) | 1;              // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
  _DMA_ExecOperation();
}

/*********************************************************************
*
*       _DMA_ConvertColor
*/
static void _DMA_ConvertColor(void * pSrc, void * pDst,  U32 PixelFormatSrc, U32 PixelFormatDst, U32 NumItems) {
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00010000UL;         // Control Register (Memory to memory with pixel format conversion and TCIE)
  //
  // Set up pointers
  //
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  //
  // Set up offsets
  //
  DMA2D->FGOR    = 0;                               // Foreground Offset Register (Source line offset)
  DMA2D->OOR     = 0;                               // Output Offset Register (Destination line offset)
  //
  // Set up pixel format
  //
  DMA2D->FGPFCCR = PixelFormatSrc;                  // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->OPFCCR  = PixelFormatDst;                  // Output PFC Control Register (Defines the output pixel format)
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(NumItems << 16) | 1;       // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
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
  DMA2D->CR      = 0x00010000UL;         // Control Register (Memory to memory with pixel format conversion and TCIE)
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
  DMA2D->FGPFCCR = LTDC_Pixelformat_L8;             // Foreground PFC Control Register (Defines the input pixel format)
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
  U8 * pRD;
  U8 * pWR;
  U32 NumBytes, Color, Index;

  //
  // Check size of conversion buffer
  //
  NumBytes = ((xSize + 1) & (U32)~1) * ySize;
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
*       _DMA_LoadLUT
*/
static void _DMA_LoadLUT(LCD_COLOR * pColor, U32 NumItems) {
  DMA2D->FGCMAR  = (U32)pColor;                     // Foreground CLUT Memory Address Register
  //
  // Foreground PFC Control Register
  //
  DMA2D->FGPFCCR  = LTDC_Pixelformat_RGB888         // Pixel format
                  | ((NumItems - 1) & 0xFF) << 8;   // Number of items to load
  DMA2D->FGPFCCR |= (1 << 5);                       // Start loading
  //
  // Waiting not required here...
  //
}

#if (GUI_USE_ARGB == 0)

/*********************************************************************
*
*       _InvertAlpha_SwapRB_CPY
*
* Purpose:
*   Color format of DMA2D is different to emWin color format. This routine
*   swaps red and blue and inverts alpha that it is compatible to emWin
*   and vice versa. Result is located in destination buffer.
*
* Performance:
*   10.8 MPixel/s
*/
static void _InvertAlpha_SwapRB_CPY(LCD_COLOR * pColorSrc, LCD_COLOR * pColorDst, U32 NumItems) {
  U32 Color;
  while (NumItems >= 4) {
    Color = *(pColorSrc + 0);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColorDst + 0) = Color;
    Color = *(pColorSrc + 1);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColorDst + 1) = Color;
    Color = *(pColorSrc + 2);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColorDst + 2) = Color;
    Color = *(pColorSrc + 3);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColorDst + 3) = Color;
    pColorSrc += 4;
    pColorDst += 4;
    NumItems -= 4;
  };
  while (NumItems--) {
    Color = *pColorSrc++;
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *pColorDst++ = Color;
  };
}

/*********************************************************************
*
*       _InvertAlpha_SwapRB_MOD
*
* Purpose:
*   Color format of DMA2D is different to emWin color format. This routine
*   swaps red and blue and inverts alpha that it is compatible to emWin
*   and vice versa. Conversion is done in given buffer.
*
* Performance:
*   10.9 MPixel/s
*/
static void _InvertAlpha_SwapRB_MOD(LCD_COLOR * pColor, U32 NumItems) {
  U32 Color;
  while (NumItems >= 4) {
    Color = *(pColor + 0);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColor + 0) = Color;
    Color = *(pColor + 1);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColor + 1) = Color;
    Color = *(pColor + 2);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColor + 2) = Color;
    Color = *(pColor + 3);
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *(pColor + 3) = Color;
    pColor += 4;
    NumItems -= 4;
  }
  while (NumItems--) {
    Color = *pColor;
    Color = ((Color << 24) >>  8)  // Red
          |  (Color & 0xFF00)      // Green
          | ((Color <<  8) >> 24)  // Blue
          | (~Color & 0xFF000000); // Alpha
    *pColor++ = Color;
  };
}

/*********************************************************************
*
*       _InvertAlpha_CPY
*
* Purpose:
*   Color format of DMA2D is different to emWin color format. This routine
*   inverts alpha that it is compatible to emWin and vice versa.
*   Changes are done in the destination memory location.
*
* Performance:
*   17.5 MPixel/s
*/
static void _InvertAlpha_CPY(LCD_COLOR * pColorSrc, LCD_COLOR * pColorDst, U32 NumItems) {
  U32 Color;

  while (NumItems >= 4) {
    Color = *(pColorSrc + 0);
    Color ^= 0xFF000000;      // Invert alpha
    *(pColorDst + 0) = Color;
    Color = *(pColorSrc + 1);
    Color ^= 0xFF000000;      // Invert alpha
    *(pColorDst + 1) = Color;
    Color = *(pColorSrc + 2);
    Color ^= 0xFF000000;      // Invert alpha
    *(pColorDst + 2) = Color;
    Color = *(pColorSrc + 3);
    Color ^= 0xFF000000;      // Invert alpha
    *(pColorDst + 3) = Color;
    pColorSrc += 4;
    pColorDst += 4;
    NumItems -= 4;
  }
  while (NumItems--) {
    Color = *pColorSrc++;
    *pColorDst++ = Color ^ 0xFF000000; // Invert alpha
  };
}

/*********************************************************************
*
*       _InvertAlpha_MOD
*
* Purpose:
*   Color format of DMA2D is different to emWin color format. This routine
*   inverts alpha that it is compatible to emWin and vice versa.
*   Changes are done in the given buffer.
*
* Performance:
*   18.0 MPixel/s
*/
static void _InvertAlpha_MOD(LCD_COLOR * pColor, U32 NumItems) {
  U32 Color;

  while (NumItems >= 4) {
    Color = *(pColor + 0);
    Color ^= 0xFF000000; // Invert alpha
    *(pColor + 0) = Color;
    Color = *(pColor + 1);
    Color ^= 0xFF000000; // Invert alpha
    *(pColor + 1) = Color;
    Color = *(pColor + 2);
    Color ^= 0xFF000000; // Invert alpha
    *(pColor + 2) = Color;
    Color = *(pColor + 3);
    Color ^= 0xFF000000; // Invert alpha
    *(pColor + 3) = Color;
    pColor += 4;
    NumItems -= 4;
  }
  while (NumItems--) {
    Color = *pColor;
    Color ^= 0xFF000000; // Invert alpha
    *pColor++ = Color;
  };
}

#endif

/*********************************************************************
*
*       _DMA_AlphaBlending
*/
static void _DMA_AlphaBlending(LCD_COLOR * pColorFG, LCD_COLOR * pColorBG, LCD_COLOR * pColorDst, U32 NumItems) {
#if (GUI_USE_ARGB)
  //
  // Use DMA2D for mixing
  //
  _DMA_AlphaBlendingBulk(pColorFG, pColorBG, pColorDst, NumItems);
#else
  //
  // Invert alpha values
  //
  _InvertAlpha_CPY(pColorFG, _pBuffer_FG, NumItems);
  _InvertAlpha_CPY(pColorBG, _pBuffer_BG, NumItems);
  //
  // Use DMA2D for mixing
  //
  _DMA_AlphaBlendingBulk(_pBuffer_FG, _pBuffer_BG, pColorDst, NumItems);
  //
  // Invert alpha values
  //
  _InvertAlpha_MOD(pColorDst, NumItems);
#endif
}

/*********************************************************************
*
*       _DMA_Index2ColorBulk
*
* Purpose:
*   This routine is used by the emWin color conversion routines to use DMA2D for
*   color conversion. It converts the given index values to 32 bit colors.
*   Because emWin uses ABGR internally and 0x00 and 0xFF for opaque and fully
*   transparent the color array needs to be converted after DMA2D has been used.
*/
static void _DMA_Index2ColorBulk(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex, U32 PixelFormat) {
  GUI_USE_PARA(SizeOfIndex);

#if (GUI_USE_ARGB)
  //
  // Use DMA2D for the conversion
  //
  _DMA_ConvertColor(pIndex, pColor, PixelFormat, LTDC_Pixelformat_ARGB8888, NumItems);
#else
  //
  // Use DMA2D for the conversion
  //
  _DMA_ConvertColor(pIndex, pColor, PixelFormat, LTDC_Pixelformat_ARGB8888, NumItems);
  //
  // Convert colors from ARGB to ABGR and invert alpha values
  //
  _InvertAlpha_SwapRB_MOD(pColor, NumItems);
#endif
}

/*********************************************************************
*
*       _DMA_Color2IndexBulk
*
* Purpose:
*   This routine is used by the emWin color conversion routines to use DMA2D for
*   color conversion. It converts the given 32 bit color array to index values.
*   Because emWin uses ABGR internally and 0x00 and 0xFF for opaque and fully
*   transparent the given color array needs to be converted before DMA2D can be used.
*/
static void _DMA_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, U32 PixelFormat) {
  GUI_USE_PARA(SizeOfIndex);

#if (GUI_USE_ARGB)
  //
  // Use DMA2D for the conversion
  //
  _DMA_ConvertColor(pColor, pIndex, LTDC_Pixelformat_ARGB8888, PixelFormat, NumItems);
#else
  //
  // Convert colors from ABGR to ARGB and invert alpha values
  //
  _InvertAlpha_SwapRB_CPY(pColor, _pBuffer_DMA2D, NumItems);
  //
  // Use DMA2D for the conversion
  //
  _DMA_ConvertColor(_pBuffer_DMA2D, pIndex, LTDC_Pixelformat_ARGB8888, PixelFormat, NumItems);
#endif
}

/*********************************************************************
*
*       _LCD_MixColorsBulk
*/
static void _LCD_MixColorsBulk(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens) {
#if (GUI_USE_ARGB)
  unsigned int y;

  GUI_USE_PARA(OffFG);
  GUI_USE_PARA(OffDest);
  for (y = 0; y < ySize; y++) {
    //
    // Use DMA2D for mixing up
    //
    _DMA_MixColorsBulk(pFG, pBG, pDst, Intens, xSize);
    pFG  += xSize + OffFG;
    pBG  += xSize + OffBG;
    pDst += xSize + OffDest;
  }
#else
  unsigned int y;

  GUI_USE_PARA(OffFG);
  GUI_USE_PARA(OffDest);
  for (y = 0; y < ySize; y++) {
    //
    // Invert alpha values
    //
    _InvertAlpha_CPY(pFG, _pBuffer_FG, xSize);
    _InvertAlpha_CPY(pBG, _pBuffer_BG, xSize);
    //
    // Use DMA2D for mixing up
    //
    _DMA_MixColorsBulk(_pBuffer_FG, _pBuffer_BG, pDst, Intens, xSize);
    //
    // Invert alpha values
    //
    _InvertAlpha_MOD(pDst, xSize);
    pFG  += xSize + OffFG;
    pBG  += xSize + OffBG;
    pDst += xSize + OffDest;
  }
#endif
}

/*********************************************************************
*
*       _GetBufferSize
*/
static U32 _GetBufferSize(int LayerIndex) {
  U32 BufferSize;

  BufferSize = _axSize[LayerIndex] * _aySize[LayerIndex] * _aBytesPerPixels[LayerIndex];
  return BufferSize;
}

/*********************************************************************
*
*       _LCD_CopyBuffer
*/
static void _LCD_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = _GetBufferSize(LayerIndex);
  AddrSrc    = _aAddr[LayerIndex] + BufferSize * IndexSrc;
  AddrDst    = _aAddr[LayerIndex] + BufferSize * IndexDst;
  if (_SupressCopyBuffer == 0) {
    _DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, _axSize[LayerIndex], _aySize[LayerIndex], 0, 0);
  }
  _aBufferIndex[LayerIndex] = IndexDst; // After this function has been called all drawing operations are routed to Buffer[IndexDst]!
}

/*********************************************************************
*
*       _LCD_CopyRect
*/
static void _LCD_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) {
  U32 BufferSize, AddrSrc, AddrDst;
  int OffLine;

  BufferSize = _GetBufferSize(LayerIndex);
  AddrSrc = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y0 * _axSize[LayerIndex] + x0) * _aBytesPerPixels[LayerIndex];
  AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y1 * _axSize[LayerIndex] + x1) * _aBytesPerPixels[LayerIndex];
  OffLine = _axSize[LayerIndex] - xSize;
  _DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, xSize, ySize, OffLine, OffLine);
}

/*********************************************************************
*
*       _LCD_FillRect
*/
static void _LCD_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  U32 BufferSize, AddrDst;
  int xSize, ySize;
  U32 NumPixels;

  xSize = x1 - x0 + 1;
  ySize = y1 - y0 + 1;
  NumPixels = xSize * ySize;
  if ((GUI_GetDrawMode() == GUI_DM_XOR) || (NumPixels <= 256)) {
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_FillRect);
  } else {
    BufferSize = _GetBufferSize(LayerIndex);
    AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y0 * _axSize[LayerIndex] + x0) * _aBytesPerPixels[LayerIndex];
    _DMA_Fill(LayerIndex, (void *)AddrDst, xSize, ySize, _axSize[LayerIndex] - xSize, PixelIndex);
  }
}

/*********************************************************************
*
*       _LCD_DrawBitmapAlpha
*/
static void _LCD_DrawBitmapAlpha(int LayerIndex, int x, int y, U32 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y * _axSize[LayerIndex] + x) * _aBytesPerPixels[LayerIndex];
  OffLineSrc = (BytesPerLine / 4) - xSize;
  OffLineDst = _axSize[LayerIndex] - xSize;
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
  AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y * _axSize[LayerIndex] + x) * _aBytesPerPixels[LayerIndex];
  OffLineSrc = (BytesPerLine / 4) - xSize;
  OffLineDst = _axSize[LayerIndex] - xSize;
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
  AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y * _axSize[LayerIndex] + x) * _aBytesPerPixels[LayerIndex];
  OffLineSrc = (BytesPerLine / 2) - xSize;
  OffLineDst = _axSize[LayerIndex] - xSize;
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
  AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y * _axSize[LayerIndex] + x) * _aBytesPerPixels[LayerIndex];
  OffLineSrc = BytesPerLine - xSize;
  OffLineDst = _axSize[LayerIndex] - xSize;
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
  if (PixelFormat > LTDC_Pixelformat_ARGB4444) {
    return 1;
  }
  BufferSize = _GetBufferSize(LayerIndex);
  AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y * _axSize[LayerIndex] + x) * _aBytesPerPixels[LayerIndex];
  OffLineSrc = (BytesPerLine * 2) - xSize;
  OffLineDst = _axSize[LayerIndex] - xSize;
  
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
  _DMA_DrawAlphaBitmap(pDst, pSrc, xSize, ySize, OffLineSrc, OffLineDst, LTDC_Pixelformat_ARGB8888);
}

#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW
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
*       _LCD_GetpPalConvTable
*
* Purpose:
*   The emWin function LCD_GetpPalConvTable() normally translates the given colors into
*   index values for the display controller. In case of index based bitmaps without
*   transparent pixels we load the palette only to the DMA2D LUT registers to be
*   translated (converted) during the process of drawing via DMA2D.
*/
static LCD_PIXELINDEX * _LCD_GetpPalConvTable(const LCD_LOGPALETTE GUI_UNI_PTR * pLogPal, const GUI_BITMAP GUI_UNI_PTR * pBitmap, int LayerIndex) {
#if (GUI_USE_ARGB)
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
  return (LCD_PIXELINDEX *)pLogPal->pPalEntries;//_pBuffer_DMA2D;/**/
#else
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
  // Convert palette colors from ARGB to ABGR
  //
  _InvertAlpha_SwapRB_CPY((U32 *)pLogPal->pPalEntries, _pBuffer_DMA2D, pLogPal->NumEntries);
  //
  // Load LUT using DMA2D
  //
  _DMA_LoadLUT(_pBuffer_DMA2D, pLogPal->NumEntries);
  //
  // Return something not NULL
  //
  return _pBuffer_DMA2D;
#endif
}

#define GPV_BASE  ((U32)0x51000000)
#define AXI_INI5_READ_QOS   (*(volatile U32 *)(GPV_BASE + 0x46100))  // DMA2D
#define AXI_INI5_WRITE_QOS  (*(volatile U32 *)(GPV_BASE + 0x46104))
#define AXI_INI6_READ_QOS   (*(volatile U32 *)(GPV_BASE + 0x47100))  // LTDC
#define AXI_INI6_WRITE_QOS  (*(volatile U32 *)(GPV_BASE + 0x47104))

/*********************************************************************
*
*       _InitLCD
*/
static void _InitLCD(void) {
  GPIO_InitTypeDef         gpio_init_structure;
  RCC_PeriphCLKInitTypeDef periph_clk_init_struct;
  
  _hLTDC.Init.HorizontalSync = (LCD_HSYNC - 1);
  _hLTDC.Init.VerticalSync = (LCD_VSYNC - 1);
  _hLTDC.Init.AccumulatedHBP = (LCD_HSYNC + LCD_HBP - 1);
  _hLTDC.Init.AccumulatedVBP = (LCD_VSYNC + LCD_VBP - 1);  
  _hLTDC.Init.AccumulatedActiveH = (YSIZE_PHYS + LCD_VSYNC + LCD_VBP - 1);
  _hLTDC.Init.AccumulatedActiveW = (XSIZE_PHYS + LCD_HSYNC + LCD_HBP - 1);
  _hLTDC.Init.TotalHeigh = (YSIZE_PHYS + LCD_VSYNC + LCD_VBP + LCD_VFP - 1);
  _hLTDC.Init.TotalWidth = (XSIZE_PHYS + LCD_HSYNC + LCD_HBP + LCD_HFP - 1); 
  
  /* Initialize the LCD pixel width and pixel height */
  _hLTDC.LayerCfg->ImageWidth  = XSIZE_PHYS;
  _hLTDC.LayerCfg->ImageHeight = YSIZE_PHYS;
  
  /* Background value */
  _hLTDC.Init.Backcolor.Blue = 0;
  _hLTDC.Init.Backcolor.Green = 0;
  _hLTDC.Init.Backcolor.Red = 0;
  
  /* Polarity */
  _hLTDC.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  _hLTDC.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
  _hLTDC.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
  _hLTDC.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  _hLTDC.Instance = LTDC;
  
  periph_clk_init_struct.PLL3.PLL3R = 32;
  periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  periph_clk_init_struct.PLL3.PLL3M = 5;		
  periph_clk_init_struct.PLL3.PLL3N = 160;
  periph_clk_init_struct.PLL3.PLL3P = 2;
  periph_clk_init_struct.PLL3.PLL3Q = 2;	
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
  //
  // Enable the LTDC and DMA2D clocks
  //
  __HAL_RCC_LTDC_CLK_ENABLE();
  __HAL_RCC_DMA2D_CLK_ENABLE();
  //
  // Enable GPIOs clock
  //
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  //
  // LTDC Pins configuration
  //
  // GPIOI configuration
  //
  gpio_init_structure.Pin       = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15; 
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;  
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);
  //
  // GPIOJ configuration
  //
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15; 
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;  
  HAL_GPIO_Init(GPIOJ, &gpio_init_structure);  
  //
  // GPIOK configuration
  //
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; 
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;  
  HAL_GPIO_Init(GPIOK, &gpio_init_structure);
  //
  // Initialize LTDC
  //
  HAL_LTDC_Init(&_hLTDC);
  //
  // Enable line interrupt
  //
  HAL_LTDC_ProgramLineEvent(&_hLTDC, YSIZE_PHYS);
  HAL_NVIC_SetPriority(LTDC_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);
  //
  // Enable DMA2D transfer complete interrupt
  //
  HAL_NVIC_SetPriority(DMA2D_IRQn, 0x00, 0);
  HAL_NVIC_EnableIRQ(DMA2D_IRQn);
  //
  // Clear transfer complete interrupt flag
  //
  DMA2D->IFCR = (U32)DMA2D_IFSR_CTCIF;
}

/*********************************************************************
*
*       _InitLayer
*/
static void _InitLayer(int LayerIndex) {
  //
  // Layer Init
  //
  _aLayer[LayerIndex].WindowX0 = 0;
  _aLayer[LayerIndex].WindowX1 = _axSize[LayerIndex];
  _aLayer[LayerIndex].WindowY0 = 0;
  _aLayer[LayerIndex].WindowY1 = _aySize[LayerIndex];
  _aLayer[LayerIndex].PixelFormat = _GetPixelformat(LayerIndex);
  _aLayer[LayerIndex].FBStartAdress = _aAddr[LayerIndex];
  _aLayer[LayerIndex].Alpha = 255;
  _aLayer[LayerIndex].Alpha0 = 0;
  _aLayer[LayerIndex].Backcolor.Blue = 0;
  _aLayer[LayerIndex].Backcolor.Green = 0;
  _aLayer[LayerIndex].Backcolor.Red = 0;
  _aLayer[LayerIndex].BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  _aLayer[LayerIndex].BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  _aLayer[LayerIndex].ImageWidth  = _axSize[LayerIndex];
  _aLayer[LayerIndex].ImageHeight = _aySize[LayerIndex];
  
  HAL_LTDC_ConfigLayer(&_hLTDC, &_aLayer[LayerIndex], LayerIndex);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       DMA2D_IRQHandler
*
* Purpose:
*   Transfer-complete-interrupt of DMA2D
*/
void DMA2D_IRQHandler(void);
void DMA2D_IRQHandler(void) {
  U32 ISR_Flags;

  ISR_Flags = DMA2D->ISR;
  if (ISR_Flags & DMA2D_ISR_CEIF) {
    DMA2D->IFCR |= DMA2D_IFCR_CCEIF;
  }
  if (ISR_Flags & DMA2D_ISR_CTCIF) {
    DMA2D->IFCR |= DMA2D_IFCR_CCTCIF;
  }
  if (ISR_Flags & DMA2D_ISR_CAEIF) {
    DMA2D->IFCR |= DMA2D_IFCR_CAECIF;
  }
  if (ISR_Flags & DMA2D_ISR_TWIF) {
    DMA2D->IFCR |= DMA2D_IFCR_CTWIF;
  }
  if (ISR_Flags & DMA2D_ISR_TCIF) {
    DMA2D->IFCR |= DMA2D_IFCR_CTCIF;
  }
  if (ISR_Flags & DMA2D_ISR_TEIF) {
    DMA2D->IFCR |= DMA2D_IFSR_CTEIF;
  }
  _WaitForDMA2D = 0;
}

/*********************************************************************
*
*       LTDC_ISR_Handler
*
* Purpose:
*   End-Of-Frame-Interrupt for managing multiple buffering
*/
#ifdef __ICCARM__
void LTDC_IRQHandler(void);
void LTDC_IRQHandler(void) {
#elif defined __GNUC__
void LCD_TFT_IRQHandler(void);
void LCD_TFT_IRQHandler(void) {
#else
static void LTDC_ISR_Handler(void) {
#endif
  U32 Addr;
  int i;

  LTDC->ICR = (U32)LTDC_IER_LIE;
  for (i = 0; i < NUM_LAYERS; i++) {
    if (_aPendingBuffer[i] >= 0) {
      //
      // Calculate address of buffer to be used  as visible frame buffer
      //
      Addr = _aAddr[i] + _axSize[i] * _aySize[i] * _aPendingBuffer[i] * _aBytesPerPixels[i];
      //
      // Store address into SFR
      //
      HAL_LTDC_SetAddress(&_hLTDC, Addr, i);
      //
      // Tell emWin that buffer is used
      //
      GUI_MULTIBUF_ConfirmEx(i, _aPendingBuffer[i]);
      //
      // Clear pending buffer flag of layer
      //
      _aPendingBuffer[i] = -1;
    }
  }
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
  int r = 0;
  static int Once;

  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    if (Once == 0) {
      //
      // Called during the initialization process in order to set up the display controller and put it into operation.
      //
      _InitLCD();
      PID_X_Init();
      Once = 1;
    }
    _InitLayer(LayerIndex);
    __HAL_LTDC_LAYER_ENABLE(&_hLTDC, LayerIndex);
    break;
  }
  case LCD_X_SETORG: {
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETORG_INFO * p;

    p = (LCD_X_SETORG_INFO *)pData;
    HAL_LTDC_SetAddress(&_hLTDC, (U32)(_aAddr[LayerIndex] + p->yPos * _axSize[LayerIndex] * _aBytesPerPixels[LayerIndex]), LayerIndex);
    break;
  }
  case LCD_X_SHOWBUFFER: {
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.
    //
    LCD_X_SHOWBUFFER_INFO * p;

    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    _aPendingBuffer[LayerIndex] = p->Index;
    break;
  }
  case LCD_X_SETLUTENTRY: {
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
    LCD_X_SETLUTENTRY_INFO * p;
    U32 Color = 0x00000000;

    p = (LCD_X_SETLUTENTRY_INFO *)pData;
    Color |= p->Color;
    Color |= p->Pos << 24;
    HAL_LTDC_ConfigCLUT(&_hLTDC, &Color, 1, LayerIndex);
    break;
  }
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on and off
    //
    __HAL_LTDC_ENABLE(&_hLTDC);
    break;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching on and off
    //
    __HAL_LTDC_DISABLE(&_hLTDC);
    break;
  }
  case LCD_X_SETVIS: {
    //
    // Required for setting the layer visibility which is passed in the 'OnOff' element of pData
    //
    LCD_X_SETVIS_INFO * p;

    p = (LCD_X_SETVIS_INFO *)pData;
    if (p->OnOff) {
      __HAL_LTDC_LAYER_ENABLE(&_hLTDC, LayerIndex);
    } else {
      __HAL_LTDC_LAYER_DISABLE(&_hLTDC, LayerIndex);
    }
    break;
  }
  case LCD_X_SETPOS: {
    //
    // Required for setting the layer position which is passed in the 'xPos' and 'yPos' element of pData
    //
    LCD_X_SETPOS_INFO * p;

    p = (LCD_X_SETPOS_INFO *)pData;
    HAL_LTDC_SetWindowPosition(&_hLTDC, p->xPos, p->yPos, LayerIndex);
    break;
  }
  case LCD_X_SETSIZE: {
    //
    // Required for setting the layer position which is passed in the 'xPos' and 'yPos' element of pData
    //
    LCD_X_SETSIZE_INFO * p;
    p = (LCD_X_SETSIZE_INFO *)pData;
    if (LCD_GetSwapXYEx(LayerIndex)) {
      _axSize[LayerIndex] = p->ySize;
      _aySize[LayerIndex] = p->xSize;
    } else {
      _axSize[LayerIndex] = p->xSize;
      _aySize[LayerIndex] = p->ySize;
    }
    HAL_LTDC_SetWindowSize(&_hLTDC, _axSize[LayerIndex], _aySize[LayerIndex], LayerIndex);

    break;
  }
  case LCD_X_SETALPHA: {
    //
    // Required for setting the alpha value which is passed in the 'Alpha' element of pData
    //
    LCD_X_SETALPHA_INFO * p;

    p = (LCD_X_SETALPHA_INFO *)pData;
    HAL_LTDC_SetAlpha(&_hLTDC, p->Alpha, LayerIndex);
    break;
  }
  case LCD_X_SETCHROMAMODE: {
    //
    // Required for setting the chroma mode which is passed in the 'ChromaMode' element of pData
    //
    LCD_X_SETCHROMAMODE_INFO * p;

    p = (LCD_X_SETCHROMAMODE_INFO *)pData;
    if (p->ChromaMode != 0) {
      HAL_LTDC_EnableColorKeying(&_hLTDC, LayerIndex);
    } else {
      HAL_LTDC_DisableColorKeying(&_hLTDC, LayerIndex);
    }
    break;
  }
  case LCD_X_SETCHROMA: {
    //
    // Required for setting the chroma value which is passed in the 'ChromaMin' and 'ChromaMax' element of pData
    //
    LCD_X_SETCHROMA_INFO * p;
    U32 Color;

    p = (LCD_X_SETCHROMA_INFO *)pData;
    Color = ((p->ChromaMin & 0xFF0000) >> 16) | (p->ChromaMin & 0x00FF00) | ((p->ChromaMin & 0x0000FF) << 16);
    HAL_LTDC_ConfigColorKeying(&_hLTDC, Color, LayerIndex);
    break;
  }
  default:
    r = -1;
  }
  return r;
}

/*********************************************************************
*
*       LCDConf_SupressCopyBuffer
*/
void LCDConf_SupressCopyBuffer(int SupressCopyBuffer);
void LCDConf_SupressCopyBuffer(int SupressCopyBuffer) {
  _SupressCopyBuffer = SupressCopyBuffer;
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
  int i, NoRotationUsed;
  U32 PixelFormat;
  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS > 1)
    for (i = 0; i < NUM_LAYERS; i++) {
      GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
    }
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  //
  // Set size of 1st layer
  //
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx (0, YSIZE_0, XSIZE_0);
    LCD_SetVSizeEx(0, YSIZE_0 * NUM_VSCREENS, XSIZE_0);
  } else {
    LCD_SetSizeEx (0, XSIZE_0, YSIZE_0);
    LCD_SetVSizeEx(0, XSIZE_0, YSIZE_0 * NUM_VSCREENS);
  }
  #if (NUM_LAYERS > 1)
    //
    // Set display driver and color conversion for 2nd layer
    //
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 1);
    //
    // Set size of 2nd layer
    //
    if (LCD_GetSwapXYEx(1)) {
      LCD_SetSizeEx (1, YSIZE_1, XSIZE_1);
      LCD_SetVSizeEx(1, YSIZE_1 * NUM_VSCREENS, XSIZE_1);
    } else {
      LCD_SetSizeEx (1, XSIZE_1, YSIZE_1);
      LCD_SetVSizeEx(1, XSIZE_1, YSIZE_1 * NUM_VSCREENS);
    }
  #endif
  //
  // Setting up VRam address and remember pixel size
  //
  for (i = 0; i < NUM_LAYERS; i++) {
    LCD_SetVRAMAddrEx(i, (void *)(_aAddr[i]));                                                 // Setting up VRam address
    _aBytesPerPixels[i] = LCD_GetBitsPerPixelEx(i) >> 3;                                       // Remember pixel size
  }
  //
  // Setting up custom functions
  //
  NoRotationUsed = 1;
  for (i = 0; i < NUM_LAYERS; i++) {
    if (_aOrientation[i] == ROTATION_0) {
      LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_LCD_CopyBuffer);               // Set custom function for copying complete buffers (used by multiple buffering) using DMA2D
      LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT, (void(*)(void))_LCD_CopyRect);                   // Set custom function for copy recxtangle areas (used by GUI_CopyRect()) using DMA2D
      //
      // Set functions for direct color mode layers. Won't work with indexed color modes because of missing LUT for DMA2D destination
      //
      PixelFormat = _GetPixelformat(i);
      if (PixelFormat <= LTDC_Pixelformat_ARGB4444) {
        LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_FillRect);                 // Set custom function for filling operations using DMA2D
        LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_8BPP, (void(*)(void))_LCD_DrawBitmap8bpp);       // Set up custom drawing routine for index based bitmaps using DMA2D
      }
      //
      // Set up drawing routine for 16bpp bitmap using DMA2D
      //
      if (PixelFormat == LTDC_Pixelformat_RGB565) {
        LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))_LCD_DrawBitmap16bpp);     // Set up drawing routine for 16bpp bitmap using DMA2D. Makes only sense with RGB565
      }
      //
      // Set up drawing routine for 32bpp bitmap using DMA2D
      //
      if (PixelFormat == LTDC_Pixelformat_ARGB8888) {
        LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_32BPP, (void(*)(void))_LCD_DrawBitmap32bpp);     // Set up drawing routine for 32bpp bitmap using DMA2D. Makes only sense with ARGB8888
      }
    } else {
      NoRotationUsed = 0;
    }
  }
  //
  // Set up custom color conversion using DMA2D, works only for direct color modes because of missing LUT for DMA2D destination
  //
  GUICC_M1555I_SetCustColorConv(_Color2IndexBulk_M1555I_DMA2D, _Index2ColorBulk_M1555I_DMA2D); // Set up custom bulk color conversion using DMA2D for ARGB1555
  //GUICC_M565_SetCustColorConv  (_Color2IndexBulk_M565_DMA2D,   _Index2ColorBulk_M565_DMA2D);   // Set up custom bulk color conversion using DMA2D for RGB565 (This is commented because it does not speed up conversion, default method is slightly faster!)
  GUICC_M4444I_SetCustColorConv(_Color2IndexBulk_M4444I_DMA2D, _Index2ColorBulk_M4444I_DMA2D); // Set up custom bulk color conversion using DMA2D for ARGB4444
  GUICC_M888_SetCustColorConv  (_Color2IndexBulk_M888_DMA2D,   _Index2ColorBulk_M888_DMA2D);   // Set up custom bulk color conversion using DMA2D for RGB888
  GUICC_M8888I_SetCustColorConv(_Color2IndexBulk_M8888I_DMA2D, _Index2ColorBulk_M8888I_DMA2D); // Set up custom bulk color conversion using DMA2D for ARGB8888
  //
  // Set up custom alpha blending function using DMA2D
  //
  GUI_SetFuncAlphaBlending(_DMA_AlphaBlending);
  //
  // Set up custom function for translating a bitmap palette into index values.
  // Required to load a bitmap palette into DMA2D CLUT in case of a 8bpp indexed bitmap
  //
  //GUI_SetFuncGetpPalConvTable(_LCD_GetpPalConvTable);
  //
  // Set up custom function for mixing up single colors using DMA2D
  //
  //GUI_SetFuncMixColors(_DMA_MixColors);  // Does not make sense for single pixels
  //
  // Set up custom function for mixing up arrays of colors using DMA2D
  //
  GUI_SetFuncMixColorsBulk(_LCD_MixColorsBulk);
  if (NoRotationUsed) {
    //
    // Set up custom function for drawing AA4 characters
    //
    GUI_AA_SetpfDrawCharAA4(_LCD_DrawBitmap4bpp);
    //
    // Set up custom function for drawing 16bpp memory devices
    //
#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW
    GUI_MEMDEV_SetDrawMemdev16bppFunc(_LCD_DrawMemdev16bpp);
#endif
    //
    // Set up custom functions for drawing 32bpp bitmaps and 32bpp memory devices
    //
    GUI_SetFuncDrawAlpha(_LCD_DrawMemdevAlpha, _LCD_DrawBitmapAlpha);
  }
}

/*************************** End of file ****************************/
