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

#include <string.h>

#include "GUI.h"
#include "GUIDRV_FlexColor.h"

#include "stm32f10x.h"                  /* STM32F10x.h definitions            */

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 240
#define YSIZE_PHYS 320

//
// Color conversion
//
#define COLOR_CONVERSION GUICC_565

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR

//
// Display orientation
//
#define DISPLAY_ORIENTATION GUI_SWAP_XY | GUI_MIRROR_Y

/*********************************************************************
*
*       Hardware access
*
**********************************************************************
*/
#define LCD_BASE   0x6C000000UL
#define LCD_REG16  (*((volatile unsigned short *)(LCD_BASE    ))) 
#define LCD_DAT16  (*((volatile unsigned short *)(LCD_BASE + 2)))

void GLCD_Init(void);

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
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
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Write00_16
*
* Purpose:
*   Write to controller, with A0 = 0
*/
static void _Write00_16(U16 c) {
  LCD_REG16 = c;
}

/*********************************************************************
*
*       _Write01_16
*
* Purpose:
*   Write to controller, with A0 = 1
*/
static void _Write01_16(U16 c) {
  LCD_DAT16 = c;
}

/*********************************************************************
*
*       _WriteM01_16
*
* Purpose:
*   Write multiple bytes to controller, with A0 = 1
*/
static void _WriteM01_16(U16 * pData, int NumWords) {
  for (; NumWords; NumWords--) {
    LCD_DAT16 = *pData++;
  }
}

/*********************************************************************
*
*       _Read01_16
*
* Purpose:
*   Read from controller, with A0 = 1
*/
static U16 _Read01_16(void) {
  U16 c;

  c = LCD_DAT16;
  return c;
}

/*********************************************************************
*
*       _ReadM01_16
*
* Purpose:
*   Read multiple bytes from controller, with A0 = 1
*/
static void _ReadM01_16(U16 * pData, int NumWords) {
  for (; NumWords; NumWords--) {
    *pData++ = LCD_DAT16;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_ManageJoystick
*
* Purpose:
*   Manages joysick input, called via ISR
*/
void LCD_X_ManageJoystick(void) {
  GUI_PID_STATE State;
  GUI_PID_STATE StateJoy = {0};
  static GUI_PID_STATE StatePrev;
  static int TimeAcc;
  static int Cnt;
  int Max, IsDifferent;

  //if (_IsInitialized == 0) {
  //  return;
  //}
  //
  // Routine is called each 10ms. So only execute each 4th call
  //
  if (Cnt++ < 4) {
    return;
  }
  Cnt = 0;
  //
  // Joystick input
  //
  if ((GPIOG->IDR & (1 << 14)) == 0) StateJoy.x = -1;      // Joystick left
  if ((GPIOG->IDR & (1 << 13)) == 0) StateJoy.x = +1;      // Joystick right
  if ((GPIOG->IDR & (1 << 15)) == 0) StateJoy.y = -1;      // Joystick up
  if ((GPIOD->IDR & (1 <<  3)) == 0) StateJoy.y = +1;      // Joystick down
  if ((GPIOG->IDR & (1 <<  7)) == 0) StateJoy.Pressed = 1; // Joystick pressed
  IsDifferent = memcmp(&StateJoy, &StatePrev, sizeof(GUI_PID_STATE));
  //
  // Handle dynamic pointer acceleration
  //
  if (IsDifferent == 0) {
    if (TimeAcc < 10) {
      TimeAcc++;
    }
  } else {
    TimeAcc = 1;
  }
  if ((StateJoy.x || StateJoy.y || StateJoy.Pressed) || IsDifferent) {
    //
    // Compute the new coordinates
    //
    GUI_PID_GetState(&State);
    if (StateJoy.x < 0) {
      State.x -= TimeAcc;
    }
    if (StateJoy.x > 0) {
      State.x += TimeAcc;
    }
    if (StateJoy.y < 0) {
      State.y -= TimeAcc;
    }
    if (StateJoy.y > 0) {
      State.y += TimeAcc;
    }
    State.Pressed = StateJoy.Pressed;
    //
    // Make sure coordinates are still in bounds
    //
    if (State.x < 0) {
      State.x = 0;
    }
    if (State.y < 0) {
      State.y = 0;
    }
    Max = YSIZE_PHYS - 1;
    if (State.x >= Max) {
      State.x = Max;
    }
    Max = XSIZE_PHYS - 1;
    if (State.y > Max) {
      State.y = Max;
    }
    //
    // Inform emWin
    //
    GUI_PID_StoreState(&State);
    StatePrev = StateJoy;
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
  CONFIG_FLEXCOLOR Config = {0};
  GUI_PORT_API PortAPI = {0}; 

  //
  // Set display driver and color conversion for 1st layer
  //
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
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
  //
  // Orientation
  //
  Config.Orientation   = DISPLAY_ORIENTATION;
  GUIDRV_FlexColor_Config(pDevice, &Config);
  //
  // Select hardware interface
  //
  GUIDRV_FlexColor_SetInterface66712_B16(pDevice, GUIDRV_FLEXCOLOR_IF_TYPE_II);
  //
  // Set controller and operation mode
  //
  PortAPI.pfWrite16_A0  = _Write00_16;
  PortAPI.pfWrite16_A1  = _Write01_16;
  PortAPI.pfWriteM16_A1 = _WriteM01_16;
  PortAPI.pfRead16_A1   = _Read01_16;
  PortAPI.pfReadM16_A1  = _ReadM01_16;
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66712, GUIDRV_FLEXCOLOR_M16C0B16);
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
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    GLCD_Init();
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
