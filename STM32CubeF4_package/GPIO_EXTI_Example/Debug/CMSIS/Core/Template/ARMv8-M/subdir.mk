################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/Core/Template/ARMv8-M/main_s.c \
../CMSIS/Core/Template/ARMv8-M/tz_context.c 

OBJS += \
./CMSIS/Core/Template/ARMv8-M/main_s.o \
./CMSIS/Core/Template/ARMv8-M/tz_context.o 

C_DEPS += \
./CMSIS/Core/Template/ARMv8-M/main_s.d \
./CMSIS/Core/Template/ARMv8-M/tz_context.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/Core/Template/ARMv8-M/%.o CMSIS/Core/Template/ARMv8-M/%.su CMSIS/Core/Template/ARMv8-M/%.cyclo: ../CMSIS/Core/Template/ARMv8-M/%.c CMSIS/Core/Template/ARMv8-M/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429xx -DSTM32F429I_DISC1 -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/BSP/STM32F429I-Discovery" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/GPIO_EXTI_Example/CMSIS/Core/Include" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/GPIO_EXTI_Example/CMSIS/ST/STM32F4xx/Include" -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/STM32CubeF4 package/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CMSIS-2f-Core-2f-Template-2f-ARMv8-2d-M

clean-CMSIS-2f-Core-2f-Template-2f-ARMv8-2d-M:
	-$(RM) ./CMSIS/Core/Template/ARMv8-M/main_s.cyclo ./CMSIS/Core/Template/ARMv8-M/main_s.d ./CMSIS/Core/Template/ARMv8-M/main_s.o ./CMSIS/Core/Template/ARMv8-M/main_s.su ./CMSIS/Core/Template/ARMv8-M/tz_context.cyclo ./CMSIS/Core/Template/ARMv8-M/tz_context.d ./CMSIS/Core/Template/ARMv8-M/tz_context.o ./CMSIS/Core/Template/ARMv8-M/tz_context.su

.PHONY: clean-CMSIS-2f-Core-2f-Template-2f-ARMv8-2d-M

