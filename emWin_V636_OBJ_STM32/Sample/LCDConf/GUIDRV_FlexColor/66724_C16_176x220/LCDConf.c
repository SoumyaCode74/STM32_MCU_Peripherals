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
Purpose     : Sample  configuration file for GUIDRV_FlexColor

Display     : Truly 176x220 with ST7775 controller

---------------------------END-OF-HEADER------------------------------
*/

#ifndef WIN32
#include "AT91SAM7X512.h"
#endif

#include "GUI.h"

#include "GUIDRV_FlexColor.h"

/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 176
#define YSIZE_PHYS 220

//
// Color conversion
//
#define COLOR_CONVERSION GUICC_565

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR

//
// Orientation (choose one of the following options)
//
#define DISPLAY_ORIENTATION (0)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_X)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_Y)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_X | GUI_MIRROR_Y)
//#define DISPLAY_ORIENTATION (GUI_SWAP_XY)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_X | GUI_SWAP_XY)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_Y | GUI_SWAP_XY)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_X | GUI_MIRROR_Y | GUI_SWAP_XY)

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
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
#ifndef   DISPLAY_ORIENTATION
  #define DISPLAY_ORIENTATION 0
#endif

/*********************************************************************
*
*       Definition of IO-lines
*
**********************************************************************
*/

#define SDI_LINE     (1 << 4)    // SDI,    PA4, J6-15
#define SDO_LINE     (1 << 5)    // SDO,    PA5, J6-16

#define CONTROL_LINES ((1 << 0)  | /* CS0,     PA0, J6-11 */ \
                       (1 << 1)  | /* D/C,     PA1, J6-12 */ \
                       (1 << 2)  | /* RESET,   PA2, J6-13 */ \
                       (1 << 3))   /* CLK,     PA3, J6-14 */

#define LCD_CLR_CS()      PIOA_CODR = (1 << 0)
#define LCD_SET_CS()      PIOA_SODR = (1 << 0)
#define LCD_CLR_A0()      PIOA_CODR = (1 << 1)
#define LCD_SET_A0()      PIOA_SODR = (1 << 1)
#define LCD_CLR_RESET()   PIOA_CODR = (1 << 2)
#define LCD_SET_RESET()   PIOA_SODR = (1 << 2)
#define LCD_CLR_CLK()     PIOA_CODR = (1 << 3)
#define LCD_SET_CLK()     PIOA_SODR = (1 << 3)
#define LCD_CLR_DATA()    PIOA_CODR = (1 << 4)
#define LCD_SET_DATA()    PIOA_SODR = (1 << 4)

#define LCD_SDO()       ((PIOA_PDSR & (1 << 5)) != 0)
#define LCD_SET_SDI_OUT() PIOA_OER  = SDI_LINE; \
                          PIOA_OWER = SDI_LINE
#define LCD_SET_SDO_IN()  PIOA_ODR  = SDO_LINE; \
                          PIOA_OWDR = SDO_LINE
#define LCD_DELAY(ms)     GUI_X_Delay(ms)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U8 _Continue;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
#ifndef WIN32
/*********************************************************************
*
*       LCD_X_SPI4_Init
*
* Purpose:
*   Read multiple bytes with A0 = 1
*/
static void _SPI_Init(void) {
  //
  // Enable clock for PIOA, required for reading
  //
  PMC_PCER |= (1 << PIOA_ID);
  //
  //  Enable the PIO to control the corresponding pin (disable peripheral control of the pin)
  //
  PIOA_PER = SDI_LINE | SDO_LINE | CONTROL_LINES; // PIO Enable Register
  //
  //  Assign the I/O lines to the Peripheral A function
  //
  PIOA_ASR = SDI_LINE | SDO_LINE; // Peripheral A Select Register
  //
  // Enable output for controll lines
  //
  PIOA_OER  = CONTROL_LINES;
  PIOA_OWER = CONTROL_LINES;
  //
  // Set write mode per default
  //
  LCD_SET_SDI_OUT();
  LCD_SET_SDO_IN();
  //
  // Set initial values for control lines
  //
  LCD_SET_CS();
  LCD_SET_A0();
  LCD_SET_CLK();
  LCD_SET_DATA();
  LCD_CLR_RESET();
  LCD_DELAY(50);
  LCD_SET_RESET();
  LCD_DELAY(50);
}

/*********************************************************************
*
*       _SendU8
*/
static void _SendU8(U8 Data) {
  unsigned i;

  for (i = 8; i; i--) {
    LCD_CLR_CLK();
    if (Data & 0x80) {
      LCD_SET_DATA();
    } else {
      LCD_CLR_DATA();
    }
    LCD_SET_CLK();
    Data = Data << 1;
  }
}

/*********************************************************************
*
*       _SendU16
*/
static void _SendU16(U16 Data) {
  unsigned i;

  for (i = 16; i; i--) {
    LCD_CLR_CLK();
    if (Data & 0x8000) {
      LCD_SET_DATA();
    } else {
      LCD_CLR_DATA();
    }
    LCD_SET_CLK();
    Data = Data << 1;
  }
}

/*********************************************************************
*
*       _ReadU16
*/
static U16 _ReadU16(void) {
  U16 Data;
  unsigned i;

  Data = 0;
  for (i = 16; i; i--) {
    Data = Data << 1;
    LCD_CLR_CLK();	
    if (LCD_SDO()) {
      Data |= 1;
    }
    LCD_SET_CLK();
  }
  return Data;
}

/*********************************************************************
*
*       _SendStartByte
*/
static void _SendStartByte(U8 Byte) {
  LCD_SET_CS();
  LCD_SET_CLK();
  LCD_SET_DATA();
  LCD_CLR_CS();
  _SendU8(Byte);
}

/*********************************************************************
*
*       _SPI_Write0
*/
static void _SPI_Write0(U16 Data) {
  _SendStartByte(0x70); // 01110000
  _Continue = 0;
  _SendU16(Data);
  LCD_SET_CS();
}

/*********************************************************************
*
*       _SPI_Write1
*/
static void _SPI_Write1(U16 Data) {
  _SendStartByte(0x72); // 01110010
  _Continue = 0;
  _SendU16(Data);
  LCD_SET_CS();
}

/*********************************************************************
*
*       _SPI_WriteM1
*/
static void _SPI_WriteM1(U16 * pData, int NumItems) {
  _SendStartByte(0x72); // 01110010
  _Continue = 0;
  while (NumItems--) {
    _SendU16(*pData++);
  }
  LCD_SET_CS();
}

/*********************************************************************
*
*       _SPI_ReadM1
*/
static void _SPI_ReadM1(unsigned short * pData, int NumItems) {
  if (_Continue == 0) {
    _SendStartByte(0x73); // 01110011
    _Continue = 1;
  } else {
    LCD_SET_CLK();
    LCD_SET_DATA();
    LCD_CLR_CS();
  }
  while (NumItems--) {
    *pData++ = _ReadU16();
  }
  //
  // IMPORTANT: Leave CS active!!!
  //
}

/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the display controller
*/
static void _InitController(void) {
  GUI_X_Delay(10);
  _SPI_Init();
  GUI_X_Delay(10);
  _SPI_Write0(0x01);
  _SPI_Write1(0x011C);
  //
  // DOT inverson
  //
  _SPI_Write0(0x02);
  _SPI_Write1(0x0100);	
  _SPI_Write0(0x03);
  _SPI_Write1(0x1030);
  _SPI_Write0(0x08);
  _SPI_Write1(0x0808);
  //
  // RGB interface control
  //
  _SPI_Write0(0x0c);
  _SPI_Write1(0x0000);
  //
  // Oscillator on/off
  //
  _SPI_Write0(0x0F);
  _SPI_Write1(0x0001);
  //
  // AC pointer set
  //
  _SPI_Write0(0x20);
  _SPI_Write1(0x0000);
  _SPI_Write0(0x21);
  _SPI_Write1(0x0000);
  GUI_X_Delay(10);
  //
  // Step up  circult
  //
  _SPI_Write0(0x11);
  _SPI_Write1(0x1000);
  GUI_X_Delay(50);
  //
  // Display control
  //
  _SPI_Write0(0x30);
  _SPI_Write1(0x0000);
  _SPI_Write0(0x31);
  _SPI_Write1(0x00DB);
  _SPI_Write0(0x32);
  _SPI_Write1(0x0000);
  _SPI_Write0(0x33);
  _SPI_Write1(0x0000);
  _SPI_Write0(0x34);
  _SPI_Write1(0x00DB);
  _SPI_Write0(0x35);
  _SPI_Write1(0x0000);
  //
  // Set NVM
  //
  _SPI_Write0(0xff);
  _SPI_Write1(0x0003);
  //
  // VCOM set
  //
  _SPI_Write0(0xb0);
  _SPI_Write1(0x1701);//VCOM=-1V
  //
  // Set AVDD
  //
  _SPI_Write0(0xb2);
  _SPI_Write1(0x0616);//AVDD=5.1V AVCL=-5.1V
  //
  // Set gamma
  //
  _SPI_Write0(0x50);
  _SPI_Write1(0x0507);
  _SPI_Write0(0x51);
  _SPI_Write1(0x090a);
  _SPI_Write0(0x52);
  _SPI_Write1(0x0207);
  _SPI_Write0(0x53);
  _SPI_Write1(0x1941);
  _SPI_Write0(0x54);
  _SPI_Write1(0x0705);
  _SPI_Write0(0x55);
  _SPI_Write1(0x0607);
  _SPI_Write0(0x56);
  _SPI_Write1(0x0708);
  _SPI_Write0(0x57);
  _SPI_Write1(0x0207);
  _SPI_Write0(0x58);
  _SPI_Write1(0x1941);
  _SPI_Write0(0x59);
  _SPI_Write1(0x0605);
  //
  // Frame rate set
  //
  _SPI_Write0(0x0b);
  _SPI_Write1(0x0004);
  //
  // Display on
  //
  _SPI_Write0(0x07);
  _SPI_Write1(0x1017);
  GUI_X_Delay(50);
}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
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
  GUI_PORT_API PortAPI    = {0};
  CONFIG_FLEXCOLOR Config = {0};

  //
  // Set display driver and color conversion for 1st layer
  //
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  //
  // Display size configuration
  //
  LCD_SetSizeEx (0, XSIZE_PHYS,  YSIZE_PHYS);
  LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
  //
  // Function selection, hardware routines (PortAPI) and operation mode (bus, bpp and cache)
  //
#ifndef WIN32
  PortAPI.pfWrite16_A0  = _SPI_Write0;
  PortAPI.pfWrite16_A1  = _SPI_Write1;
  PortAPI.pfWriteM16_A1 = _SPI_WriteM1;
  PortAPI.pfReadM16_A1  = _SPI_ReadM1;
#endif
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66724, GUIDRV_FLEXCOLOR_M16C0B16);
  //
  // Orientation
  //
  Config.Orientation   = DISPLAY_ORIENTATION;
  GUIDRV_FlexColor_Config(pDevice, &Config);
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
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(pData);
  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    #ifndef WIN32
      _InitController();
    #endif
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
