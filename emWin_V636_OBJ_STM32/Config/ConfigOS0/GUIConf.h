/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2019  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : GUIConf.h
Purpose     : ...
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUICONF_H
#define GUICONF_H

#define GUI_OS                         0  // Multitasking support
#define GUI_SUPPORT_TOUCH              1  // Support a touch screen (req. win-manager)
#define GUI_SUPPORT_MOUSE              1  // Support a mouse
#define GUI_SUPPORT_MEMDEV             1  // Memory devices available
#define GUI_WINSUPPORT                 1  // Window manager package available
#define GUI_MEMDEV_SUPPORT_CUSTOMDRAW  1  // Allow to set custom draw functions for memory devices

#endif /* GUICONF_H */

/*************************** End of file ****************************/
