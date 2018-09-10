################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shika/http/httppackage.c \
../shika/http/httprequest.c \
../shika/http/httpresponse.c 

OBJS += \
./shika/http/httppackage.o \
./shika/http/httprequest.o \
./shika/http/httpresponse.o 

C_DEPS += \
./shika/http/httppackage.d \
./shika/http/httprequest.d \
./shika/http/httpresponse.d 


# Each subdirectory must supply rules for building sources it contributes
shika/http/%.o: ../shika/http/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=c11 -I/usr/include/glib-2.0 -I"/home/ramiro/git/shika/shika" -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O3 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


