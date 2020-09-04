################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modules/executers/depthtree.c \
../src/modules/executers/executer.c \
../src/modules/executers/matcher.c \
../src/modules/executers/orders.c \
../src/modules/executers/ordproducer.c 

OBJS += \
./src/modules/executers/depthtree.o \
./src/modules/executers/executer.o \
./src/modules/executers/matcher.o \
./src/modules/executers/orders.o \
./src/modules/executers/ordproducer.o 

C_DEPS += \
./src/modules/executers/depthtree.d \
./src/modules/executers/executer.d \
./src/modules/executers/matcher.d \
./src/modules/executers/orders.d \
./src/modules/executers/ordproducer.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/executers/%.o: ../src/modules/executers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


