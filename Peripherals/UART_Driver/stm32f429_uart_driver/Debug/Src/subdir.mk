################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main.c \
../Src/stm32f429xx_gpio_driver.c \
../Src/stm32f429xx_rcc_driver.c \
../Src/stm32f429xx_uart_driver.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/main.o \
./Src/stm32f429xx_gpio_driver.o \
./Src/stm32f429xx_rcc_driver.o \
./Src/stm32f429xx_uart_driver.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/main.d \
./Src/stm32f429xx_gpio_driver.d \
./Src/stm32f429xx_rcc_driver.d \
./Src/stm32f429xx_uart_driver.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429I_DISC1 -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"D:/Users/soumy/GitHub/STM32_MCU_Peripherals/Peripherals/UART_Driver/stm32f429_uart_driver/Drivers/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/stm32f429xx_gpio_driver.cyclo ./Src/stm32f429xx_gpio_driver.d ./Src/stm32f429xx_gpio_driver.o ./Src/stm32f429xx_gpio_driver.su ./Src/stm32f429xx_rcc_driver.cyclo ./Src/stm32f429xx_rcc_driver.d ./Src/stm32f429xx_rcc_driver.o ./Src/stm32f429xx_rcc_driver.su ./Src/stm32f429xx_uart_driver.cyclo ./Src/stm32f429xx_uart_driver.d ./Src/stm32f429xx_uart_driver.o ./Src/stm32f429xx_uart_driver.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

