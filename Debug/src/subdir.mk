################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/can_config.c \
../src/restbus.c 

OBJS += \
./src/can_config.o \
./src/restbus.o 

C_DEPS += \
./src/can_config.d \
./src/restbus.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-cortexa8-linux-gnueabi-gcc -I/home/benny/podbox/OSELAS.BSP-TP-TBA53/platform-TBa53/sysroot-target/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


