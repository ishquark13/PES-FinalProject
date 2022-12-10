################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/analog_peripherals.c \
../source/dsp_fft.c \
../source/leds.c \
../source/main.c \
../source/mtb.c \
../source/semihost_hardfault.c \
../source/test_dsp_fft.c \
../source/touch_sensor.c \
../source/tpm_sync.c 

C_DEPS += \
./source/analog_peripherals.d \
./source/dsp_fft.d \
./source/leds.d \
./source/main.d \
./source/mtb.d \
./source/semihost_hardfault.d \
./source/test_dsp_fft.d \
./source/touch_sensor.d \
./source/tpm_sync.d 

OBJS += \
./source/analog_peripherals.o \
./source/dsp_fft.o \
./source/leds.o \
./source/main.o \
./source/mtb.o \
./source/semihost_hardfault.o \
./source/test_dsp_fft.o \
./source/touch_sensor.o \
./source/tpm_sync.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DARM_MATH_CM0PLUS -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/Users/ip/MCUX/ECEN5813_FinalProject/board" -I"/Users/ip/MCUX/ECEN5813_FinalProject/source" -I"/Users/ip/MCUX/ECEN5813_FinalProject" -I"/Users/ip/MCUX/ECEN5813_FinalProject/drivers" -I"/Users/ip/MCUX/ECEN5813_FinalProject/CMSIS" -I"/Users/ip/MCUX/ECEN5813_FinalProject/utilities" -I"/Users/ip/MCUX/ECEN5813_FinalProject/startup" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/analog_peripherals.d ./source/analog_peripherals.o ./source/dsp_fft.d ./source/dsp_fft.o ./source/leds.d ./source/leds.o ./source/main.d ./source/main.o ./source/mtb.d ./source/mtb.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/test_dsp_fft.d ./source/test_dsp_fft.o ./source/touch_sensor.d ./source/touch_sensor.o ./source/tpm_sync.d ./source/tpm_sync.o

.PHONY: clean-source

