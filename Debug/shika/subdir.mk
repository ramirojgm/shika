################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shika/shika.c \
../shika/shikabroadway.c 

OBJS += \
./shika/shika.o \
./shika/shikabroadway.o 

C_DEPS += \
./shika/shika.d \
./shika/shikabroadway.d 


# Each subdirectory must supply rules for building sources it contributes
shika/%.o: ../shika/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=c11 -I/usr/include/glib-2.0 -I"/home/ramiro/git/shika/shika" -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O3 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


