################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
A51_UPPER_SRCS += \
../src/SILABS_STARTUP.A51 

C_SRCS += \
../src/Dali.c \
../src/Dali__main.c \
../src/InitDevice.c \
../src/Interrupts.c 

OBJS += \
./src/Dali.OBJ \
./src/Dali__main.OBJ \
./src/InitDevice.OBJ \
./src/Interrupts.OBJ \
./src/SILABS_STARTUP.OBJ 


# Each subdirectory must supply rules for building sources it contributes
src/%.OBJ: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/Dali.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/InitDevice.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/Dali.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

src/Dali__main.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/InitDevice.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/Dali.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

src/InitDevice.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/InitDevice.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/Dali.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

src/Interrupts.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/InitDevice.h C:/Users/clfilho/Dropbox\ (Silicon\ Labs)/Code_Workspace_Dali/Dali_/src/Dali.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

src/%.OBJ: ../src/%.A51
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Assembler'
	AX51 "@$(patsubst %.OBJ,%.__ia,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '


