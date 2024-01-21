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
File        : GUI_X_Touch_StoreState.c
Purpose     : Template function which can be used as a starting point
              for functions storing the state of a controller driven
              touch screen
---------------------------END-OF-HEADER------------------------------
*/

/*********************************************************************
*
*       TouchTask
*
* Function decription:
*   Handles a controller driven touch screen and uses GUI_TOUCH_StoreState()
*   to pass the values to emWin. Please note that multitasking for
*   emWin is not required at this point.
*/
void TouchTask(void) {
  static U16 xOld;
  static U16 yOld;
  static U8  PressedOld;
  U16 x, y, xDiff, yDiff;
  U8  Pressed;

  do {
    Pressed = // TBD: Insert function which returns:
              //      1, if the touch screen is pressed
              //      0, if the touch screen is released
    //
    // Touch screen is pressed
    //
    if (Pressed) {
      x = // TBD: Insert function which reads current x value
      y = // TBD: Insert function which reads current y value
      //
      // The touch has already been pressed
      //
      if (PressedOld == 1) {
        //
        // Calculate difference between new and old position
        //
        xDiff = (x > xOld) ? (x - xOld) : (xOld - x);
        yDiff = (y > yOld) ? (y - yOld) : (yOld - y);
        //
        // Store state if new position differs significantly from old position
        //
        if (xDiff + yDiff > 2) {
          xOld = x;
          yOld = y;
          GUI_TOUCH_StoreState(x, y);
        }
      }
      //
      // The touch was previously released
      // Store state regardless position
      //
      } else {
        if ((x != 0) && (y != 0)) {
          xOld = x;
          yOld = y;
          PressedOld = 1;
          GUI_TOUCH_StoreState(x, y);
        }
      }
    //
    // Touch screen is not pressed
    // Store state if it was released recently
    //
    } else {
      if (PressedOld == 1) {
        PressedOld = 0;
        GUI_TOUCH_StoreState(-1, -1);
      }
    }
    //
    // Make sure 
    //
    GUI_X_Delay(20);
  } while (1);
}

