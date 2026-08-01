################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/bme680/Src/bme68x.c \
../Core/bme680/Src/bme68x_necessary_functions.c 

OBJS += \
./Core/bme680/Src/bme68x.o \
./Core/bme680/Src/bme68x_necessary_functions.o 

C_DEPS += \
./Core/bme680/Src/bme68x.d \
./Core/bme680/Src/bme68x_necessary_functions.d 


# Each subdirectory must supply rules for building sources it contributes
Core/bme680/Src/%.o Core/bme680/Src/%.su Core/bme680/Src/%.cyclo: ../Core/bme680/Src/%.c Core/bme680/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-bme680-2f-Src

clean-Core-2f-bme680-2f-Src:
	-$(RM) ./Core/bme680/Src/bme68x.cyclo ./Core/bme680/Src/bme68x.d ./Core/bme680/Src/bme68x.o ./Core/bme680/Src/bme68x.su ./Core/bme680/Src/bme68x_necessary_functions.cyclo ./Core/bme680/Src/bme68x_necessary_functions.d ./Core/bme680/Src/bme68x_necessary_functions.o ./Core/bme680/Src/bme68x_necessary_functions.su

.PHONY: clean-Core-2f-bme680-2f-Src

