################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include STM32F4xx_HAL_Driver/Src/Legacy/subdir.mk
-include STM32F4xx_HAL_Driver/Src/subdir.mk
-include STM32F429I-Discovery/subdir.mk
-include CMSIS/ST/STM32F4xx/Source/Templates/iar/subdir.mk
-include CMSIS/ST/STM32F4xx/Source/Templates/gcc/subdir.mk
-include CMSIS/ST/STM32F4xx/Source/Templates/arm/subdir.mk
-include CMSIS/ST/STM32F4xx/Source/Templates/subdir.mk
-include CMSIS/Core/Template/ARMv8-M/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := GPIO_EXTI_Example
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
GPIO_EXTI_Example.elf \

MAP_FILES += \
GPIO_EXTI_Example.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
GPIO_EXTI_Example.list \


# All Target
all: main-build

# Main-build Target
main-build: GPIO_EXTI_Example.elf secondary-outputs

# Tool invocations
GPIO_EXTI_Example.elf GPIO_EXTI_Example.map: $(OBJS) $(USER_OBJS) D:\Users\soumy\GitHub\STM32_MCU_Peripherals\STM32CubeF4\ package\GPIO_EXTI_Example\STM32F429ZITX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "GPIO_EXTI_Example.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\Users\soumy\GitHub\STM32_MCU_Peripherals\STM32CubeF4 package\GPIO_EXTI_Example\STM32F429ZITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="GPIO_EXTI_Example.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

GPIO_EXTI_Example.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "GPIO_EXTI_Example.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) GPIO_EXTI_Example.elf GPIO_EXTI_Example.list GPIO_EXTI_Example.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
