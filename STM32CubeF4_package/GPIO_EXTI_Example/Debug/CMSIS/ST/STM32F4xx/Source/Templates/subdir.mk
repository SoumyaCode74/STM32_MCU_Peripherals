################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c 

OBJS += \
./CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.o 

C_DEPS += \
./CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/ST/STM32F4xx/Source/Templates/%.o CMSIS/ST/STM32F4xx/Source/Templates/%.su CMSIS/ST/STM32F4xx/Source/Templates/%.cyclo: ../CMSIS/ST/STM32F4xx/Source/Templates/%.c CMSIS/ST/STM32F4xx/Source/Templates/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429xx -DSTM32F429I_DISC1 -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/BSP/STM32F429I-Discovery" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/GPIO_EXTI_Example/CMSIS/Core/Include" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/GPIO_EXTI_Example/CMSIS/ST/STM32F4xx/Include" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CMSIS-2f-ST-2f-STM32F4xx-2f-Source-2f-Templates

clean-CMSIS-2f-ST-2f-STM32F4xx-2f-Source-2f-Templates:
	-$(RM) ./CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.cyclo ./CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.d ./CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.o ./CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.su

.PHONY: clean-CMSIS-2f-ST-2f-STM32F4xx-2f-Source-2f-Templates

