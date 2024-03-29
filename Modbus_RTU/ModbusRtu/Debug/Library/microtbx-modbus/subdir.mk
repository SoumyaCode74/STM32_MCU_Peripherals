################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/microtbx-modbus/tbxmb_client.c \
../Library/microtbx-modbus/tbxmb_event.c \
../Library/microtbx-modbus/tbxmb_port.c \
../Library/microtbx-modbus/tbxmb_rtu.c \
../Library/microtbx-modbus/tbxmb_server.c \
../Library/microtbx-modbus/tbxmb_superloop.c \
../Library/microtbx-modbus/tbxmb_uart.c 

OBJS += \
./Library/microtbx-modbus/tbxmb_client.o \
./Library/microtbx-modbus/tbxmb_event.o \
./Library/microtbx-modbus/tbxmb_port.o \
./Library/microtbx-modbus/tbxmb_rtu.o \
./Library/microtbx-modbus/tbxmb_server.o \
./Library/microtbx-modbus/tbxmb_superloop.o \
./Library/microtbx-modbus/tbxmb_uart.o 

C_DEPS += \
./Library/microtbx-modbus/tbxmb_client.d \
./Library/microtbx-modbus/tbxmb_event.d \
./Library/microtbx-modbus/tbxmb_port.d \
./Library/microtbx-modbus/tbxmb_rtu.d \
./Library/microtbx-modbus/tbxmb_server.d \
./Library/microtbx-modbus/tbxmb_superloop.d \
./Library/microtbx-modbus/tbxmb_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Library/microtbx-modbus/%.o Library/microtbx-modbus/%.su Library/microtbx-modbus/%.cyclo: ../Library/microtbx-modbus/%.c Library/microtbx-modbus/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Library/microtbx -I../Library/microtbx-modbus -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Library-2f-microtbx-2d-modbus

clean-Library-2f-microtbx-2d-modbus:
	-$(RM) ./Library/microtbx-modbus/tbxmb_client.cyclo ./Library/microtbx-modbus/tbxmb_client.d ./Library/microtbx-modbus/tbxmb_client.o ./Library/microtbx-modbus/tbxmb_client.su ./Library/microtbx-modbus/tbxmb_event.cyclo ./Library/microtbx-modbus/tbxmb_event.d ./Library/microtbx-modbus/tbxmb_event.o ./Library/microtbx-modbus/tbxmb_event.su ./Library/microtbx-modbus/tbxmb_port.cyclo ./Library/microtbx-modbus/tbxmb_port.d ./Library/microtbx-modbus/tbxmb_port.o ./Library/microtbx-modbus/tbxmb_port.su ./Library/microtbx-modbus/tbxmb_rtu.cyclo ./Library/microtbx-modbus/tbxmb_rtu.d ./Library/microtbx-modbus/tbxmb_rtu.o ./Library/microtbx-modbus/tbxmb_rtu.su ./Library/microtbx-modbus/tbxmb_server.cyclo ./Library/microtbx-modbus/tbxmb_server.d ./Library/microtbx-modbus/tbxmb_server.o ./Library/microtbx-modbus/tbxmb_server.su ./Library/microtbx-modbus/tbxmb_superloop.cyclo ./Library/microtbx-modbus/tbxmb_superloop.d ./Library/microtbx-modbus/tbxmb_superloop.o ./Library/microtbx-modbus/tbxmb_superloop.su ./Library/microtbx-modbus/tbxmb_uart.cyclo ./Library/microtbx-modbus/tbxmb_uart.d ./Library/microtbx-modbus/tbxmb_uart.o ./Library/microtbx-modbus/tbxmb_uart.su

.PHONY: clean-Library-2f-microtbx-2d-modbus

