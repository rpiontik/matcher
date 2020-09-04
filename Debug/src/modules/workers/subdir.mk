################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modules/workers/accounts.c \
../src/modules/workers/depthtree.c \
../src/modules/workers/manager.c \
../src/modules/workers/matchcore.c \
../src/modules/workers/ordproducer.c \
../src/modules/workers/ordreceiver.c \
../src/modules/workers/trtransmit.c 

OBJS += \
./src/modules/workers/accounts.o \
./src/modules/workers/depthtree.o \
./src/modules/workers/manager.o \
./src/modules/workers/matchcore.o \
./src/modules/workers/ordproducer.o \
./src/modules/workers/ordreceiver.o \
./src/modules/workers/trtransmit.o 

C_DEPS += \
./src/modules/workers/accounts.d \
./src/modules/workers/depthtree.d \
./src/modules/workers/manager.d \
./src/modules/workers/matchcore.d \
./src/modules/workers/ordproducer.d \
./src/modules/workers/ordreceiver.d \
./src/modules/workers/trtransmit.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/workers/%.o: ../src/modules/workers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


