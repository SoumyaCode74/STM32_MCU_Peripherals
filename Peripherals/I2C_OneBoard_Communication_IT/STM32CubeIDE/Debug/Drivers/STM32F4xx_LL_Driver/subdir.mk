################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Users/soumy/GitHub/EmbeddedSystems/EmbeddedSystems/FastBit\ Courses/Peripherals/I2C_OneBoard_Communication_IT/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c \
D:/Users/soumy/GitHub/EmbeddedSystems/EmbeddedSystems/FastBit\ Courses/Peripherals/I2C_OneBoard_Communication_IT/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c 

OBJS += \
./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.o \
./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.o 

C_DEPS += \
./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.d \
./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.o: D:/Users/soumy/GitHub/EmbeddedSystems/EmbeddedSystems/FastBit\ Courses/Peripherals/I2C_OneBoard_Communication_IT/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c Drivers/STM32F4xx_LL_Driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429xx -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000U -c -I../../Inc -I../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.o: D:/Users/soumy/GitHub/EmbeddedSystems/EmbeddedSystems/FastBit\ Courses/Peripherals/I2C_OneBoard_Communication_IT/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c Drivers/STM32F4xx_LL_Driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429xx -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000U -c -I../../Inc -I../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-STM32F4xx_LL_Driver

clean-Drivers-2f-STM32F4xx_LL_Driver:
	-$(RM) ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.cyclo ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.d ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.o ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_rcc.su ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.cyclo ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.d ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.o ./Drivers/STM32F4xx_LL_Driver/stm32f4xx_ll_utils.su

.PHONY: clean-Drivers-2f-STM32F4xx_LL_Driver

