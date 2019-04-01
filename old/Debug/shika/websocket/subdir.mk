################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shika/websocket/gwebsocket.c \
../shika/websocket/gwebsocketmessage.c \
../shika/websocket/gwebsocketservice.c \
../shika/websocket/httppackage.c \
../shika/websocket/httprequest.c \
../shika/websocket/httpresponse.c 

OBJS += \
./shika/websocket/gwebsocket.o \
./shika/websocket/gwebsocketmessage.o \
./shika/websocket/gwebsocketservice.o \
./shika/websocket/httppackage.o \
./shika/websocket/httprequest.o \
./shika/websocket/httpresponse.o 

C_DEPS += \
./shika/websocket/gwebsocket.d \
./shika/websocket/gwebsocketmessage.d \
./shika/websocket/gwebsocketservice.d \
./shika/websocket/httppackage.d \
./shika/websocket/httprequest.d \
./shika/websocket/httpresponse.d 


# Each subdirectory must supply rules for building sources it contributes
shika/websocket/%.o: ../shika/websocket/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=c11 -I/usr/include/glib-2.0 -I"/home/ramiro/git/shika/shika" -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O3 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


