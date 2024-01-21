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
File        : PIDConf.c
Purpose     : Touch screen controller configuration
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "PIDConf.h"
#include "RTOS.h"
#include "TaskPrio.h"
#include "stm32746g_discovery_ts.h"
#include "GUIMTDRV_FT5336.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define FT5336_SLAVE_ADDRESS            ((U8)0x70)
#define I2C_TIMING                      ((U32)0x40912732)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U8 _IsInitialized;
static int _LayerIndex;

static OS_STACKPTR int Stack_Touch[128];
static OS_TASK TCB_TOUCH;

static I2C_HandleTypeDef hI2cAudioHandler = {0};

static void TouchTask(void);

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       TouchTask
*/
static void TouchTask(void) {
  while(1) {
    GUIMTDRV_FT5336_Exec();
    OS_Delay(25);
  }
}

/*********************************************************************
*
*       _I2C_InitGPIO
*/
static void _I2C_InitGPIO(void) {
  GPIO_InitTypeDef  gpio_init_structure;

    __HAL_RCC_GPIOH_CLK_ENABLE();
  //
  // Configure I2C Tx as alternate function
  //
  gpio_init_structure.Pin = GPIO_PIN_7;
  gpio_init_structure.Mode = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);
  //
  // Configure I2C Rx as alternate function
  //
  gpio_init_structure.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);
}

/*********************************************************************
*
*       _I2C_Init
*/
static void _I2C_Init(unsigned int SlaveAddr) {
  GUI_USE_PARA(SlaveAddr);
  hI2cAudioHandler.Instance              = I2C3;
  hI2cAudioHandler.Init.Timing           = I2C_TIMING;
  hI2cAudioHandler.Init.OwnAddress1      = 0;
  hI2cAudioHandler.Init.AddressingMode   = 1;           // 7 bit addr. mode
  hI2cAudioHandler.Init.DualAddressMode  = 0;           // No dual address
  hI2cAudioHandler.Init.OwnAddress2      = 0;
  hI2cAudioHandler.Init.GeneralCallMode  = 0;           // Disable general call
  hI2cAudioHandler.Init.NoStretchMode    = 0;           // No stretch
  //
  // Init SDA and SCl
  //
  _I2C_InitGPIO();
  //
  // Enable I2C clock
  //
  __HAL_RCC_I2C3_CLK_ENABLE();
  //
  // Force the I2C peripheral clock reset
  //
  __HAL_RCC_I2C3_FORCE_RESET();
  //
  // Release the I2C peripheral clock reset
  //
  __HAL_RCC_I2C3_RELEASE_RESET();
  //
  // Enable and set I2Cx Interrupt to a lower priority
  //
  HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
  //
  // Enable and set I2Cx Interrupt to a lower priority
  //
  HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);

  HAL_I2C_Init(&hI2cAudioHandler);
}

/*********************************************************************
*
*       _I2C_Write
*/
static void _I2C_Write(unsigned SlaveAddr, const U8 *pData, int Addr, int Len) {
   HAL_I2C_Mem_Write(&hI2cAudioHandler, SlaveAddr, (U16)Addr, 1, (U8 *)pData, Len, 1000);
}

/*********************************************************************
*
*       _I2C_Read
*/
static void _I2C_Read(unsigned SlaveAddr, U8 *pData, int Addr, int Len) {
  HAL_I2C_Mem_Read(&hI2cAudioHandler, SlaveAddr, (U16)Addr, 1, pData, Len, 1000);
}

/*********************************************************************
*
*       _TranslateTouch
*/
static void _TranslateTouch(GUI_MTOUCH_INPUT *pCoord) {
  int Temp;

  Temp      = pCoord->x;
  pCoord->x = pCoord->y;
  pCoord->y = Temp;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       PID_X_SetLayerIndex
*/
void PID_X_SetLayerIndex(int LayerIndex) {
  GUIMTDRV_FT5336_SetLayerIndex(LayerIndex);
}

/*********************************************************************
*
*       PID_X_Init
*/
void PID_X_Init(void) {
  const GUIMTDRV_FT5336_CONFIG TSConfig = {
    0,
    _I2C_Init,
    _I2C_Read,
    _I2C_Write,
    _TranslateTouch,
    FT5336_SLAVE_ADDRESS
  };

  if (_IsInitialized == 0) {
    GUI_MTOUCH_Enable(1);
    GUIMTDRV_FT5336_Init(&TSConfig);
    OS_CREATETASK(&TCB_TOUCH, "TouchTask", TouchTask, TASKPRIO_TOUCH, Stack_Touch);
    _IsInitialized = 1;
  }
}

/*************************** End of file ****************************/
