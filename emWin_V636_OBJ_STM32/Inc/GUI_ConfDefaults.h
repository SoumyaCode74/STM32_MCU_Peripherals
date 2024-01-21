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
File        : GUI_ConfDefaults.h
Purpose     : Defaults for GUI config switches.
---------------------------END-OF-HEADER------------------------------

Attention : Do not modify this file ! If you do, you will not
            be able do update to a later GUI version !

*/

#ifndef  GUI_CONFDEFAULTS_H
#define  GUI_CONFDEFAULTS_H

#include "GUIConf.h"

#ifndef   GUI_SUPPORT_BIDI
  #define GUI_SUPPORT_BIDI 1
#endif

#ifndef   GUI_USE_BIDI2
  #define GUI_USE_BIDI2 1
#endif

#ifndef   LCD_MAX_LOG_COLORS
  #define LCD_MAX_LOG_COLORS 256
#else
  #if (LCD_MAX_LOG_COLORS > 256)
    #error The value of LCD_MAX_LOG_COLORS must be <= 256!
  #endif
#endif

#define LCD_PIXELINDEX U32

#ifndef LCD_YMAG
  #define LCD_YMAG 1
#endif
#ifndef LCD_XMAG
  #define LCD_XMAG 1
#endif

#ifndef   GUI_SIM_SUPPORT_EMBOS
  #define GUI_SIM_SUPPORT_EMBOS 1
#endif

/**********************************************************************
*
*       Defaults for config switches
*
***********************************************************************
*/

/* ATTENTION: This define swaps the meaning of a logical color from
   ABGR to ARGB.
   
   It further swaps the meaning of a transparent pixel:
   ABGR: 0x00 means opaque, 0xFF means transparent
   ARGB: 0x00 means transparent, 0xFF means opaque (default)
*/
#ifndef GUI_USE_ARGB
  #define GUI_USE_ARGB 1
#endif

/* Define "universal pointer". Normally, this is not needed (define will expand to nothing)
   However, on some systems (AVR - IAR compiler) it can be necessary ( -> __generic),
   since a default pointer can access RAM only, not the built-in Flash
*/
#ifndef GUI_UNI_PTR
  #define GUI_UNI_PTR /* Remains only for compatibility purpose, no longer used in emWin */
#endif

/* Define const storage. Normally, this is not needed (define will expand to const)
   However, on some systems (AVR - IAR compiler) it can be necessary ( -> __flash const),
   since otherwise constants are copied into RAM
*/
#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

#ifndef GUI_USE_MEMDEV_1BPP_FOR_SCREEN
  #define GUI_USE_MEMDEV_1BPP_FOR_SCREEN 1
#endif

#ifndef GUI_BIDI_MAX_CHARS_PER_LINE
  #if GUI_USE_BIDI2
    #define GUI_BIDI_MAX_CHARS_PER_LINE 200
  #else
    #define GUI_BIDI_MAX_CHARS_PER_LINE  80
  #endif
#endif

#ifndef GUI_SUPPORT_TOUCH
  #define GUI_SUPPORT_TOUCH   0
#endif

#ifndef GUI_SUPPORT_MOUSE
  #define GUI_SUPPORT_MOUSE   0
#endif

#ifndef GUI_SUPPORT_MEMDEV
  #define GUI_SUPPORT_MEMDEV  0
#endif

#ifndef GUI_SUPPORT_AA
  #define GUI_SUPPORT_AA      1
#endif

#ifndef GUI_OS
  #define GUI_OS              0
#endif

#ifndef GUI_NUM_LAYERS
  #define GUI_NUM_LAYERS      1
#endif

#ifndef GUI_SUPPORT_CURSOR
  #define GUI_SUPPORT_CURSOR  (GUI_SUPPORT_MOUSE | GUI_SUPPORT_TOUCH)
#endif

#ifndef   GUI_CURSOR_LAYER
  #define GUI_CURSOR_LAYER 0
#endif

#ifndef GUI_SUPPORT_ROTATION
  #define GUI_SUPPORT_ROTATION 1
#endif

/* In order to avoid warnings for undefined parameters */
#ifndef GUI_USE_PARA
  #if defined (__BORLANDC__) || defined(NC30) || defined(NC308)
    #define GUI_USE_PARA(para)
  #else
    #define GUI_USE_PARA(para) (void)para
  #endif
#endif

/* Default for types */
#ifndef GUI_TIMER_TIME
  #define GUI_TIMER_TIME int  /* default is to use 16 bits for 16 bit CPUs,
	                           32 bits on 32 bit CPUs for timing */
#endif

/* Types used for memory allocation */
#define GUI_ALLOC_DATATYPE   I32
#define GUI_ALLOC_DATATYPE_U U32

#ifndef   GUI_MAX_XBF_BYTES
  #define GUI_MAX_XBF_BYTES 200
#endif

#ifndef   GUI_MEMSET
  #define GUI_MEMSET memset
#endif

#ifndef GUI_MEMCPY
  #define GUI_MEMCPY memcpy
#endif

#ifndef GUI_CEIL
  #define GUI_CEIL ceilf
#endif

#ifndef GUI_FLOOR
  #define GUI_FLOOR floorf
#endif

#ifndef GUI_SQRT
  #define GUI_SQRT sqrtf
#endif

/* Optional custom drawing of memory devices */
#ifndef   GUI_MEMDEV_SUPPORT_CUSTOMDRAW
  #define GUI_MEMDEV_SUPPORT_CUSTOMDRAW 0
#endif

#endif   /* ifdef GUI_CONFDEFAULTS_H */

/*************************** End of file ****************************/
