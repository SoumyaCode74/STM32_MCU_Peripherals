################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4\ package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.c \
D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4\ package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.c 

OBJS += \
./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.o \
./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.o 

C_DEPS += \
./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.d \
./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.d 


# Each subdirectory must supply rules for building sources it contributes
STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.o: D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4\ package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.c STM32F4xx_HAL_Driver/Src/Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429xx -DSTM32F429I_DISC1 -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cec.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cryp_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cryp.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dcmi_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dcmi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dfsdm.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dsi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpi2c_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpi2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpsmbus_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpsmbus.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hash_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hash.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_irda.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_lptim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_mmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_msp_template.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_nand.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_nor.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pccard.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_qspi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_smartcard.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_smbus.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spdifrx.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sram.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_wwdg.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_crc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dac.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma2d.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_exti.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmpi2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fsmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_i2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_lptim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_pwr.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rng.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rtc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_spi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_tim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usart.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" @"STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.c_includes.args"
STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.o: D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4\ package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.c STM32F4xx_HAL_Driver/Src/Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429xx -DSTM32F429I_DISC1 -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cec.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cryp_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cryp.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dcmi_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dcmi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dfsdm.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dsi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpi2c_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpi2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpsmbus_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_fmpsmbus.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hash_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hash.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_irda.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_lptim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_mmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_msp_template.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_nand.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_nor.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pccard.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_qspi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_smartcard.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_smbus.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spdifrx.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sram.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_wwdg.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_crc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dac.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma2d.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_exti.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmpi2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fsmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_i2c.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_lptim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_pwr.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rng.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rtc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_spi.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_tim.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usart.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c" -include"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" @"STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.c_includes.args"

clean: clean-STM32F4xx_HAL_Driver-2f-Src-2f-Legacy

clean-STM32F4xx_HAL_Driver-2f-Src-2f-Legacy:
	-$(RM) ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.cyclo ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.d ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.o ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_can.su ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.cyclo ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.d ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.o ./STM32F4xx_HAL_Driver/Src/Legacy/stm32f4xx_hal_eth.su

.PHONY: clean-STM32F4xx_HAL_Driver-2f-Src-2f-Legacy

