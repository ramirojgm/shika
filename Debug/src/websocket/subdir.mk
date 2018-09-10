################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/websocket/gwebsocket.c \
../src/websocket/gwebsocketmessage.c \
../src/websocket/gwebsocketservice.c \
../src/websocket/httppackage.c \
../src/websocket/httprequest.c \
../src/websocket/httpresponse.c 

OBJS += \
./src/websocket/gwebsocket.o \
./src/websocket/gwebsocketmessage.o \
./src/websocket/gwebsocketservice.o \
./src/websocket/httppackage.o \
./src/websocket/httprequest.o \
./src/websocket/httpresponse.o 

C_DEPS += \
./src/websocket/gwebsocket.d \
./src/websocket/gwebsocketmessage.d \
./src/websocket/gwebsocketservice.d \
./src/websocket/httppackage.d \
./src/websocket/httprequest.d \
./src/websocket/httpresponse.d 


# Each subdirectory must supply rules for building sources it contributes
src/websocket/%.o: ../src/websocket/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=c11 -I/usr/include/glib-2.0 -I"/home/ramiro/git/shika/src" -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O3 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


