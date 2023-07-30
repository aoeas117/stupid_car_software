################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/adc.c \
../code/control.c \
../code/data_set.c \
../code/gps.c \
../code/gtm_pwm.c \
../code/icm.c \
../code/imu.c \
../code/li_chen_ji.c \
../code/lora.c \
../code/my_math.c \
../code/servo.c 

OBJS += \
./code/adc.o \
./code/control.o \
./code/data_set.o \
./code/gps.o \
./code/gtm_pwm.o \
./code/icm.o \
./code/imu.o \
./code/li_chen_ji.o \
./code/lora.o \
./code/my_math.o \
./code/servo.o 

COMPILED_SRCS += \
./code/adc.src \
./code/control.src \
./code/data_set.src \
./code/gps.src \
./code/gtm_pwm.src \
./code/icm.src \
./code/imu.src \
./code/li_chen_ji.src \
./code/lora.src \
./code/my_math.src \
./code/servo.src 

C_DEPS += \
./code/adc.d \
./code/control.d \
./code/data_set.d \
./code/gps.d \
./code/gtm_pwm.d \
./code/icm.d \
./code/imu.d \
./code/li_chen_ji.d \
./code/lora.d \
./code/my_math.d \
./code/servo.d 


# Each subdirectory must supply rules for building sources it contributes
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fF:/ADS/E01_gpio_demo/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --io-streams --exceptions --rtti --anachronisms --g++ --fp-model=1 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<" -lcpsx_fpu -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


