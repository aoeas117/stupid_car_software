################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/IfxPsi5s.c 

OBJS += \
./libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/IfxPsi5s.o 

COMPILED_SRCS += \
./libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/IfxPsi5s.src 

C_DEPS += \
./libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/IfxPsi5s.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/%.src: ../libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/%.c libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fF:/ADS/E01_gpio_demo/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --io-streams --exceptions --rtti --anachronisms --g++ --fp-model=1 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<" -lcpsx_fpu -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/%.o: ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/%.src libraries/infineon_libraries/iLLD/TC26B/Tricore/Psi5s/Std/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


