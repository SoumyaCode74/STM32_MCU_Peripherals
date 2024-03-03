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


BUILD_ARTIFACT_NAME := SysTick_STM32CubeMonitor
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SysTick_STM32CubeMonitor.elf \

MAP_FILES += \
SysTick_STM32CubeMonitor.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SysTick_STM32CubeMonitor.list \


# All Target
all: main-build

# Main-build Target
main-build: SysTick_STM32CubeMonitor.elf secondary-outputs

# Tool invocations
SysTick_STM32CubeMonitor.elf SysTick_STM32CubeMonitor.map: $(OBJS) $(USER_OBJS) D:\Users\soumy\GitHub\EmbeddedSystems\EmbeddedSystems\Israel_Gbati\ courses\Bare-Metal\ Programming\ Ground\ Up\ (STM32F4)\9a_OutputCompare\STM32F429ZITX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "SysTick_STM32CubeMonitor.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\Users\soumy\GitHub\EmbeddedSystems\EmbeddedSystems\Israel_Gbati courses\Bare-Metal Programming Ground Up (STM32F4)\9a_OutputCompare\STM32F429ZITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="SysTick_STM32CubeMonitor.map" -Wl,--cref -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SysTick_STM32CubeMonitor.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SysTick_STM32CubeMonitor.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) SysTick_STM32CubeMonitor.elf SysTick_STM32CubeMonitor.list SysTick_STM32CubeMonitor.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
