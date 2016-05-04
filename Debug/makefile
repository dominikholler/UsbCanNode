################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Libraries/XMCLib/src/subdir.mk
-include Libraries/Newlib/subdir.mk
-include Dave/Generated/USBD_VCOM/virtual_com/subdir.mk
-include Dave/Generated/USBD_VCOM/class_driver/device/subdir.mk
-include Dave/Generated/USBD_VCOM/subdir.mk
-include Dave/Generated/USBD/usb/core/xmc4000/subdir.mk
-include Dave/Generated/USBD/usb/core/subdir.mk
-include Dave/Generated/USBD/subdir.mk
-include Dave/Generated/INTERRUPT/subdir.mk
-include Dave/Generated/GLOBAL_CAN/subdir.mk
-include Dave/Generated/CPU_CTRL_XMC4/subdir.mk
-include Dave/Generated/CLOCK_XMC4/subdir.mk
-include Dave/Generated/CAN_NODE/subdir.mk
-include Dave/Generated/subdir.mk
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
SECONDARY_FLASH += \
usbCanNode.hex \

SECONDARY_LIST += \
usbCanNode.lst \

SECONDARY_SIZE += \
usbCanNode.siz \


LIST_FILES = $(addsuffix .o.lst, $(basename $(OBJS))) 
MAP_FILE = usbCanNode.map

# All Target
all: usbCanNode.elf secondary-outputs

# Tool invocations
usbCanNode.elf: $(OBJS) ../linker_script.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM-GCC C Linker'
	$(file >objects.rsp,$(OBJS))
	"E:\DAVEv4\DAVE-4.1.4\eclipse\ARM-GCC-49/bin/arm-none-eabi-gcc" -T"../linker_script.ld" -nostartfiles -Xlinker --gc-sections -specs=nano.specs -specs=nosys.specs -Wl,-Map,$(MAP_FILE) -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "usbCanNode.elf" "@objects.rsp" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo.
usbCanNode.hex: usbCanNode.elf
	@echo 'Invoking: ARM-GCC Create Flash Image'
	"E:\DAVEv4\DAVE-4.1.4\eclipse\ARM-GCC-49/bin/arm-none-eabi-objcopy" -O ihex "usbCanNode.elf"  "usbCanNode.hex"
	@echo 'Finished building: $@'
	@echo.
usbCanNode.lst: usbCanNode.elf
	@echo 'Invoking: ARM-GCC Create Listing'
	"E:\DAVEv4\DAVE-4.1.4\eclipse\ARM-GCC-49/bin/arm-none-eabi-objdump" -h -S "usbCanNode.elf" > "usbCanNode.lst"
	@echo 'Finished building: $@'
	@echo.
usbCanNode.siz: usbCanNode.elf
	@echo 'Invoking: ARM-GCC Print Size'
	"E:\DAVEv4\DAVE-4.1.4\eclipse\ARM-GCC-49/bin/arm-none-eabi-size"  --format=berkeley "usbCanNode.elf"
	@echo 'Finished building: $@'
	@echo.
# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ARM_GCC_FOR_XMC_LINKER_OUTPUT_TYPE_OUTPUTS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) usbCanNode.elf $(LIST_FILES) $(MAP_FILE) objects.rsp
	-@echo.
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
