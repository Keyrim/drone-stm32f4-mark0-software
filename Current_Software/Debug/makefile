################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/Src/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Core/Sensors/subdir.mk
-include Core/OS/tasks/subdir.mk
-include Core/OS/scheduler/subdir.mk
-include Core/OS/events/subdir.mk
-include Core/OS/debug/subdir.mk
-include Core/OS/subdir.mk
-include subdir.mk
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

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
F4_flight_controller.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
F4_flight_controller.list \

OBJCOPY_BIN += \
F4_flight_controller.bin \


# All Target
all: F4_flight_controller.elf secondary-outputs

# Tool invocations
F4_flight_controller.elf: $(OBJS) $(USER_OBJS) C:\WorkSpace\GitHub\F4_Flight_Controller\Current_Software\STM32F405RGTX_FLASH.ld
	arm-none-eabi-gcc -o "F4_flight_controller.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\WorkSpace\GitHub\F4_Flight_Controller\Current_Software\STM32F405RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="F4_flight_controller.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

F4_flight_controller.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "F4_flight_controller.list"
	@echo 'Finished building: $@'
	@echo ' '

F4_flight_controller.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "F4_flight_controller.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
