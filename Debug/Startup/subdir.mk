################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Startup/system_XMC4700.c 

S_UPPER_SRCS += \
../Startup/startup_XMC4700.S 

OBJS += \
./Startup/startup_XMC4700.o \
./Startup/system_XMC4700.o 

S_UPPER_DEPS += \
./Startup/startup_XMC4700.d 

C_DEPS += \
./Startup/system_XMC4700.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/%.o: ../Startup/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC Assembler'
	"E:\DAVEv4\DAVE-4.1.4\eclipse\ARM-GCC-49/bin/arm-none-eabi-gcc" -MMD -MT "$@" -x assembler-with-cpp -DXMC4700_F144x2048 -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Libraries\XMCLib\inc" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Dave\Generated" -Wall -Wa,-adhlns="$@.lst" -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo.
Startup/%.o: ../Startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"E:\DAVEv4\DAVE-4.1.4\eclipse\ARM-GCC-49/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC4700_F144x2048 -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Libraries\XMCLib\inc" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode/Libraries/CMSIS/Include" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode/Libraries/CMSIS/Infineon/XMC4700_series/Include" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Dave\Generated" -I"D:\dholler\Dokumente\uni\bus\xmc\usbCanNode\Libraries" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo.

