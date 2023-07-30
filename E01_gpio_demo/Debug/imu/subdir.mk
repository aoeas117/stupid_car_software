################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../imu/Ekf.cpp \
../imu/cpp_api.cpp \
../imu/sta_update.cpp 

OBJS += \
./imu/Ekf.o \
./imu/cpp_api.o \
./imu/sta_update.o 

COMPILED_SRCS += \
./imu/Ekf.src \
./imu/cpp_api.src \
./imu/sta_update.src 

CPP_DEPS += \
./imu/Ekf.d \
./imu/cpp_api.d \
./imu/sta_update.d 


# Each subdirectory must supply rules for building sources it contributes
imu/%.src: ../imu/%.cpp imu/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fF:/ADS/E01_gpio_demo/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --io-streams --exceptions --rtti --anachronisms --g++ --fp-model=1 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<" -lcpsx_fpu -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

imu/%.o: ./imu/%.src imu/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


