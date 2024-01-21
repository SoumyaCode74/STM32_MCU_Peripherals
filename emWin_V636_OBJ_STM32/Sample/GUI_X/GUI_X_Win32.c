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
File        : GUI_X_Win32.c
Purpose     : System dependent externals for GUI under Win32
---------------------------END-OF-HEADER------------------------------
*/

#include "LCD_SIM.h"
#include "GUI.h"
#include "windows.h"


/*********************************************************************
*
*       GUI_X_Init()
*
*  Function description
*    This function is being called on initialization of emWin.
*
*  Additional information
*    Typically used to initialize a hardware timer to provide a time
*    base used by the timing related GUI_X functions.
*    This is not required when using an underlaying RTOS.
*/
void GUI_X_Init(void) {
};

/*********************************************************************
*
*       GUI_X_ExecIdle()
*
*  Function description
*    Called only from non-blocking functions of the Window Manager.
*
*  Additional information
*    Called when there are no longer any messages which require processing. In this case
*    the GUI is up to date.
*/
void GUI_X_ExecIdle(void) {
  SIM_GUI_ExecIdle();
};


/*********************************************************************
*
*       GUI_X_GetTime()
*
*  Function description
*    Used by \c{GUI_GetTime()} to return the current system time in milliseconds.
*
*  Return value
*    The current system time in milliseconds, of type integer.
*/
GUI_TIMER_TIME GUI_X_GetTime(void) {
  return SIM_GUI_GetTime();
}

/*********************************************************************
*
*       GUI_X_Delay()
*
*  Function description
*    Returns after a specified time period in milliseconds.
*
*  Parameters
*    Period: Period in milliseconds.
*/
void GUI_X_Delay(int Period) {
  SIM_GUI_Delay(Period);
}

/*********************************************************************
*
*      Multitask interface for Win32
*
*  The folling section consisting of 4 routines is used to make
*  the GUI software thread safe with WIN32
*/

static HANDLE hMutex;
static int _EntranceCnt;   // For debugging only ... Not required

/*********************************************************************
*
*       GUI_X_InitOS()
*
*  Function description
*    Creates the resource semaphore or mutex typically used by GUI_X_Lock()
*    and GUI_X_Unlock().
*/
void GUI_X_InitOS(void) {
  hMutex = CreateMutex(NULL, 0, "GUI Simulation - Mutex");
}

/*********************************************************************
*
*       GUI_X_GetTaskId()
*
*  Function description
*    Returns a unique ID for the current task.
*
*  Return value
*    ID of the current task as a 32-bit integer.
*
*  Additional information
*    Used with a real-time operating system.
*    It does not matter which value is returned, as long as it is unique for each task/
*    thread using the emWin API and as long as the value is always the same for each
*    particular thread.
*/
U32 GUI_X_GetTaskId(void) {
  return GetCurrentThreadId();
}

/*********************************************************************
*
*       GUI_X_Lock()
*
*  Function description
*    Locks the GUI.
*
*  Additional information
*    This routine is called by the GUI before it accesses the display or before using a
*    critical internal data structure. It blocks other threads from entering the same critical
*    section using a resource semaphore/mutex until GUI_X_Unlock() has been called.
*    When using a real time operating system, you normally have to increment a counting
*    resource semaphore.
*/
void GUI_X_Lock(void) {
  WaitForSingleObject(hMutex, INFINITE);
  if (++_EntranceCnt > 1) {
    SIM_ErrorOut("Error in GUITASK.c module ...");
  }
}

/*********************************************************************
*
*       GUI_X_Unlock()
*
*  Function description
*    Unlocks the GUI.
*
*  Additional information
*    This routine is called by the GUI after accessing the display or after using a critical
*    internal data structure.
*    When using a real time operating system, you normally have to decrement a
*    counting resource semaphore.
*/
void GUI_X_Unlock(void) {
  _EntranceCnt--;
  ReleaseMutex(hMutex);
}


/*********************************************************************
*
*      Text output for Logging, warnings and errors

  Logging - required only for higher debug levels
*/
void GUI_X_Log     (const char *s)     { SIM_Log(s); }
void GUI_X_Warn    (const char *s)     { SIM_Warn(s); }
void GUI_X_ErrorOut(const char *s)     { SIM_ErrorOut(s); }







