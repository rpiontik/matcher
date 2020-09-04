################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/modules/workers/subdir.mk
-include src/modules/markets/subdir.mk
-include src/modules/gateways/subdir.mk
-include src/modules/subdir.mk
-include src/libs/hiredis/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: matcher

# Tool invocations
matcher: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -Wl,-rpath=/usr/local/lib/x86_64-linux-gnu -L/usr/local/lib/x86_64-linux-gnu -L. -o "matcher" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) matcher
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
