# STM32_MCU_Peripherals
## Common steps to keep in mind when writing bare-metal peripheral drivers for STM32 MCUs
* Clock configuration:
  1. System clock source choice
  2. Bus prescalers choice
  3. Enable the clock access to the bus which has the GPIO port for the peripheral. This can be identified from the Alternate function mappings for the peripheral in the MCU's datasheet.
  4. Enable the clock access to the bus which supplies to your peripheral.
* GPIO configuration:
  1. Configure the GPIO pins required by the peripheral to Alternate Function mode.
  2. Set the required Alternate function number that multiplexes the required peripheral. This number to be found from the Alternate function mappings in the MCU's datasheet.
  3. Configure any other special settings as required for the operation.
* Peripheral configuration:
  1. Always remember, for any peripheral, there is a Control Register(s) which contains most of the configuration settings for the peripheral, to allow it to serve different purposes.
  2. Then comes the Clock Configuration Register for some peripherals.
  3. Then comes the other configuration registers specific to the peripheral under use.
  4. Then pay attention to whether Interrupt mode needs to be activated for the peripheral's operation.
* Peripheral operation:
  1. Some registers are required to use the peripheral as set through the configuration.
  2. Most commonly found and used are the Data Register, the Status Register, and any other register which needs interaction for the operation of the peripheral.
  3. The Status register must be used if the peripheral is to be used in Blocking mode (or Polling mode).
* Peripheral operation under Interrupt:
  1. In these cases, remember two things, besides setting the configuration in the Control Registers. The first is that, the interrupt is enabled in the NVIC_ISER register, as per the interrupt number and the priority number. These can be found from the Interrupt Vector Table found in the MCU's reference manual.
  2. The second thing is: Re-define the particular peripheral's Interrupt Handler function, and the Interrupt Callback function. The Callback function is to be called from the Handler function, and should perform the operations required after the interrupt is raised. After doing all the work, clear the interrupt for further interrupts to be received and raised.
* System exceptions:
  1. This is optional. If you wish to write separate logic to process if any system exceptions occur, then follow the same procedure for the interrupts, for the ARM system exceptions.
