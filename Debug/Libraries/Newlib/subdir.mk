################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/Newlib/syscalls.c 

OBJS += \
./Libraries/Newlib/syscalls.o 

C_DEPS += \
./Libraries/Newlib/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/Newlib/%.o: ../Libraries/Newlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"E:\DAVEv4\DAVE-4.1.4\eclipse\ARM-GCC-49/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC4700_F144x2048 -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Libraries\XMCLib\inc" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode/Libraries/CMSIS/Include" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode/Libraries/CMSIS/Infineon/XMC4700_series/Include" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Dave\Generated" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Libraries" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo.

