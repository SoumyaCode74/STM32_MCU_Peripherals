################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Library/microtbx/tbx_comp.s 

C_SRCS += \
../Library/microtbx/tbx_aes256.c \
../Library/microtbx/tbx_assert.c \
../Library/microtbx/tbx_checksum.c \
../Library/microtbx/tbx_critsect.c \
../Library/microtbx/tbx_crypto.c \
../Library/microtbx/tbx_heap.c \
../Library/microtbx/tbx_list.c \
../Library/microtbx/tbx_mempool.c \
../Library/microtbx/tbx_platform.c \
../Library/microtbx/tbx_port.c \
../Library/microtbx/tbx_random.c 

OBJS += \
./Library/microtbx/tbx_aes256.o \
./Library/microtbx/tbx_assert.o \
./Library/microtbx/tbx_checksum.o \
./Library/microtbx/tbx_comp.o \
./Library/microtbx/tbx_critsect.o \
./Library/microtbx/tbx_crypto.o \
./Library/microtbx/tbx_heap.o \
./Library/microtbx/tbx_list.o \
./Library/microtbx/tbx_mempool.o \
./Library/microtbx/tbx_platform.o \
./Library/microtbx/tbx_port.o \
./Library/microtbx/tbx_random.o 

S_DEPS += \
./Library/microtbx/tbx_comp.d 

C_DEPS += \
./Library/microtbx/tbx_aes256.d \
./Library/microtbx/tbx_assert.d \
./Library/microtbx/tbx_checksum.d \
./Library/microtbx/tbx_critsect.d \
./Library/microtbx/tbx_crypto.d \
./Library/microtbx/tbx_heap.d \
./Library/microtbx/tbx_list.d \
./Library/microtbx/tbx_mempool.d \
./Library/microtbx/tbx_platform.d \
./Library/microtbx/tbx_port.d \
./Library/microtbx/tbx_random.d 


# Each subdirectory must supply rules for building sources it contributes
Library/microtbx/%.o Library/microtbx/%.su Library/microtbx/%.cyclo: ../Library/microtbx/%.c Library/microtbx/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Library/microtbx -I../Library/microtbx-modbus -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Library/microtbx/%.o: ../Library/microtbx/%.s Library/microtbx/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Library-2f-microtbx

clean-Library-2f-microtbx:
	-$(RM) ./Library/microtbx/tbx_aes256.cyclo ./Library/microtbx/tbx_aes256.d ./Library/microtbx/tbx_aes256.o ./Library/microtbx/tbx_aes256.su ./Library/microtbx/tbx_assert.cyclo ./Library/microtbx/tbx_assert.d ./Library/microtbx/tbx_assert.o ./Library/microtbx/tbx_assert.su ./Library/microtbx/tbx_checksum.cyclo ./Library/microtbx/tbx_checksum.d ./Library/microtbx/tbx_checksum.o ./Library/microtbx/tbx_checksum.su ./Library/microtbx/tbx_comp.d ./Library/microtbx/tbx_comp.o ./Library/microtbx/tbx_critsect.cyclo ./Library/microtbx/tbx_critsect.d ./Library/microtbx/tbx_critsect.o ./Library/microtbx/tbx_critsect.su ./Library/microtbx/tbx_crypto.cyclo ./Library/microtbx/tbx_crypto.d ./Library/microtbx/tbx_crypto.o ./Library/microtbx/tbx_crypto.su ./Library/microtbx/tbx_heap.cyclo ./Library/microtbx/tbx_heap.d ./Library/microtbx/tbx_heap.o ./Library/microtbx/tbx_heap.su ./Library/microtbx/tbx_list.cyclo ./Library/microtbx/tbx_list.d ./Library/microtbx/tbx_list.o ./Library/microtbx/tbx_list.su ./Library/microtbx/tbx_mempool.cyclo ./Library/microtbx/tbx_mempool.d ./Library/microtbx/tbx_mempool.o ./Library/microtbx/tbx_mempool.su ./Library/microtbx/tbx_platform.cyclo ./Library/microtbx/tbx_platform.d ./Library/microtbx/tbx_platform.o ./Library/microtbx/tbx_platform.su ./Library/microtbx/tbx_port.cyclo ./Library/microtbx/tbx_port.d ./Library/microtbx/tbx_port.o ./Library/microtbx/tbx_port.su ./Library/microtbx/tbx_random.cyclo ./Library/microtbx/tbx_random.d ./Library/microtbx/tbx_random.o ./Library/microtbx/tbx_random.su

.PHONY: clean-Library-2f-microtbx

