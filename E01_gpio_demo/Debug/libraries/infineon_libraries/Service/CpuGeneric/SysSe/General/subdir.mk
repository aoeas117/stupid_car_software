################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/Ifx_GlobalResources.c 

OBJS += \
./libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/Ifx_GlobalResources.o 

COMPILED_SRCS += \
./libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/Ifx_GlobalResources.src 

C_DEPS += \
./libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/Ifx_GlobalResources.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/%.src: ../libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/%.c libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fF:/ADS/E01_gpio_demo/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --io-streams --exceptions --rtti --anachronisms --g++ --fp-model=1 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<" -lcpsx_fpu -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/%.o: ./libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/%.src libraries/infineon_libraries/Service/CpuGeneric/SysSe/General/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '

