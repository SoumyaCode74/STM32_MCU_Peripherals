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
File        : JPEGConf.h
Purpose     : Configuration header file to draw JPEG with hardware
----------------------------------------------------------------------
*/

#ifndef JPEGCONF_H
#define JPEGCONF_H

#define JPEG_ARGB8888 0
#define JPEG_RGB565   1

typedef struct {
  U32                 WorkBufferSize;
  void              * (* pfMalloc)(size_t size);
  void                (* pfFree)  (void * ptr);
  int                 ColorFormat;
  U8                * pWorkBuffer;
  GUI_HMEM            hWorkBuffer;
  U8                * pOutBuffer;
  GUI_HMEM            hOutBuffer;
  U32                 OutBufferSize;
  U8                * pInBuffer;
  U32                 NumBytesInBuffer;
  int                 BitsPerPixel;
  int                 BytesPerLine;
  GUI_GET_DATA_FUNC * pfGetData;
  void              * pVoid;
  U32                 Off;
  U32                 xPos;
  U32                 yPos;
  U32                 xSize;
  U32                 ySize;
  U32                 BlockSize;
  U32                 TotalMCUs;
  U32                 IndexMCU;
  U32                 (* pfConvert)(U32 BlockIndex);
  //U8                  HFactor;
  U8                  VFactor;
  U32                 xSizeExtended;
  U8                  Error;
  U8                  IRQFlag;
#if GUI_SUPPORT_MEMDEV
  GUI_MEMDEV_Handle   hMem;
#endif
} JPEG_X_CONTEXT;

int  JPEG_X_Draw        (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
void JPEG_X_DeInit      (void);
void JPEG_X_Init(JPEG_X_CONTEXT * pContext);
void JPEG_X_SetStayAlive(int OnOff);
void JPEG_X_IncStayAlive(void);
void JPEG_X_DecStayAlive(void);

void MOVIE_X_cbNotify    (GUI_MOVIE_HANDLE hMovie, int Notification, U32 CurrentFrame);

#endif /* LCDCONF_H */

/*************************** End of file ****************************/
