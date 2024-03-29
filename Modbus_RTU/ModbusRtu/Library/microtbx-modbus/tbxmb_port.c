/************************************************************************************//**
* \file         tbxmb_port.c
* \brief        Modbus hardware specific port source file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX library                   */
#include "microtbxmodbus.h"                      /* MicroTBX-Modbus library            */
#include "stm32f4xx_hal.h"                       /* STM32 HAL drivers                  */


/****************************************************************************************
* External data declarations
****************************************************************************************/
extern TIM_HandleTypeDef htim10;
extern UART_HandleTypeDef huart2;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static uint8_t rxByte;


/************************************************************************************//**
** \brief     Initializes the UART channel.
** \param     port The serial port to use. The actual meaning of the serial port is
**            hardware dependent. It typically maps to the UART peripheral number. E.g. 
**            TBX_MB_UART_PORT1 = USART1 on an STM32.
** \param     baudrate The desired communication speed.
** \param     databits Number of databits for a character.
** \param     stopbits Number of stop bits at the end of a character.
** \param     parity Parity bit type to use.
**
****************************************************************************************/
void TbxMbPortUartInit(tTbxMbUartPort     port, 
                       tTbxMbUartBaudrate baudrate,
                       tTbxMbUartDatabits databits, 
                       tTbxMbUartStopbits stopbits,
                       tTbxMbUartParity   parity)
{
  /* Kick off first byte reception. */
  HAL_UART_Receive_IT(&huart2, &rxByte, 1U);
} /*** end of TbxMbPortUartInit ***/


/************************************************************************************//**
** \brief     Starts the transfer of len bytes from the data array on the specified 
**            serial port.
** \attention This function has mutual exclusive access to the bytes in the data[] array,
**            until this port module calls TbxMbUartTransmitComplete(). This means that
**            you do not need to copy the data bytes to a local buffer. This approach 
**            keeps RAM requirements low and benefits the run-time performance. Just make
**            sure to call TbxMbUartTransmitComplete() once all bytes are transmitted or
**            an error was detected, to release access to the data[] array.
** \param     port The serial port to start the data transfer on.
** \param     data Byte array with data to transmit.
** \param     len Number of bytes to transmit.
** \return    TBX_OK if successful, TBX_ERROR otherwise.  
**
****************************************************************************************/
uint8_t TbxMbPortUartTransmit(tTbxMbUartPort         port, 
                              uint8_t        const * data, 
                              uint16_t               len)
{
  uint8_t result = TBX_ERROR;

  if (HAL_UART_Transmit_IT(&huart2, (uint8_t *)data, len) == HAL_OK)
  {
    result = TBX_OK;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbPortUartTransmit ***/


/************************************************************************************//**
** \brief     Obtains the free running counter value of a timer that runs at 20 kHz.
** \details   The Modbus RTU communication makes use of 1.5 (T1_5) and 3.5 (T3_5)
**            character timeouts. Time between packet characters should be <= T1_5 and
**            the time between packets should be >= T3_5. 
** 
**            To get these timings right a free running counter, incrementing every 50
**            microseconds, provides a time reference. This function obtains the current
**            value of this counter.
**
**            Ideally a timer is initialized to have its free running counter running at
**            20 kHz for this. The benefit of this approach is that the timer can still
**            be reused for other purposes (input capture, pwm, output compare, etc.) and
**            does not need a periodic timer interrupt.
**
**            Timers are a scarce resource on microcontrollers. Therefore it is also
**            possible to use the free running counter of a timer that runs at a
**            different frequency. Just make sure to adjust the counter value in this
**            function accordingly. For example, if you choose to reuse your RTOS'
**            1 millisecond SysTick, you need to multiply its tick counter value by 20 to
**            simulate a 20 kHz timer. This does of course have a negative impact on the
**            accuracy of the T1_5 and T3_5 timings, so there's a trade off involved.
** \return    Free running counter value.
**
****************************************************************************************/
uint16_t TbxMbPortTimerCount(void)
{
  static uint8_t counterStarted = TBX_FALSE;

  /* Start the counter the first time this function is called. */
  if (counterStarted == TBX_FALSE)
  {
    counterStarted = TBX_TRUE;
    __HAL_TIM_ENABLE(&htim10);
  }
  /* Read out the current value of counter.. */
  return (uint16_t)__HAL_TIM_GET_COUNTER(&htim10);
} /*** end of TbxMbPortTimerCount ***/


/****************************************************************************************
*            C A L L B A C K   R O U T I N E S
****************************************************************************************/
/************************************************************************************//**
** \brief     UART transmit complete callback.
** \param     handle Pointer to the channel's handle.
**
****************************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef * handle)
{
  /* Inform the Modbus UART module about the transmission completed event. */
  TbxMbUartTransmitComplete(TBX_MB_UART_PORT1);
} /*** end of HAL_UART_TxCpltCallback ***/


/************************************************************************************//**
** \brief     UART reception complete callback.
** \param     handle Pointer to the channel's handle.
**
****************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * handle)
{
  uint32_t errorCode = HAL_UART_GetError(&huart2);
  /* Only process the byte if no noise, framing or parity error was detected. */
  if ((errorCode & (HAL_UART_ERROR_NE | HAL_UART_ERROR_PE | HAL_UART_ERROR_FE)) == 0U)
  {
    /* Inform the Modbus UART module about the newly received data byte. */
    TbxMbUartDataReceived(TBX_MB_UART_PORT1, &rxByte, 1U);
  }
  /* Restart reception for the next byte. */
  HAL_UART_Receive_IT(&huart2, &rxByte, 1U);
} /*** end of HAL_UART_RxCpltCallback ***/


/*********************************** end of tbxmb_port.c *******************************/
