################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modules/markets/order.c \
../src/modules/markets/transaction.c 

OBJS += \
./src/modules/markets/order.o \
./src/modules/markets/transaction.o 

C_DEPS += \
./src/modules/markets/order.d \
./src/modules/markets/transaction.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/markets/%.o: ../src/modules/markets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


