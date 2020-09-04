################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modules/gateways/rabbitmq.c \
../src/modules/gateways/redis.c 

OBJS += \
./src/modules/gateways/rabbitmq.o \
./src/modules/gateways/redis.o 

C_DEPS += \
./src/modules/gateways/rabbitmq.d \
./src/modules/gateways/redis.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/gateways/%.o: ../src/modules/gateways/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


