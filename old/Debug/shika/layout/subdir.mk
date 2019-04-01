################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shika/layout/shikaapplication.c \
../shika/layout/shikafilesystem.c \
../shika/layout/shikasecurity.c 

OBJS += \
./shika/layout/shikaapplication.o \
./shika/layout/shikafilesystem.o \
./shika/layout/shikasecurity.o 

C_DEPS += \
./shika/layout/shikaapplication.d \
./shika/layout/shikafilesystem.d \
./shika/layout/shikasecurity.d 


# Each subdirectory must supply rules for building sources it contributes
shika/layout/%.o: ../shika/layout/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=c11 -I/usr/include/glib-2.0 -I"/home/ramiro/git/shika/shika" -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O3 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


