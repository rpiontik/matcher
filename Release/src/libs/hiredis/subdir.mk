################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/libs/hiredis/async.c \
../src/libs/hiredis/dict.c \
../src/libs/hiredis/hiredis.c \
../src/libs/hiredis/net.c \
../src/libs/hiredis/read.c \
../src/libs/hiredis/sds.c 

OBJS += \
./src/libs/hiredis/async.o \
./src/libs/hiredis/dict.o \
./src/libs/hiredis/hiredis.o \
./src/libs/hiredis/net.o \
./src/libs/hiredis/read.o \
./src/libs/hiredis/sds.o 

C_DEPS += \
./src/libs/hiredis/async.d \
./src/libs/hiredis/dict.d \
./src/libs/hiredis/hiredis.d \
./src/libs/hiredis/net.d \
./src/libs/hiredis/read.d \
./src/libs/hiredis/sds.d 


# Each subdirectory must supply rules for building sources it contributes
src/libs/hiredis/%.o: ../src/libs/hiredis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


