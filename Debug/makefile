################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /home/alealfaro/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./drivers/lsm9ds1/lsm9ds1.obj" \
"./libraries/bmx_adc/adcInit.obj" \
"./libraries/bmx_bluetooth/bmx_bluetooth.obj" \
"./libraries/bmx_encoder/bmx_encoder.obj" \
"./libraries/bmx_esc/bmx_esc.obj" \
"./libraries/bmx_imu/i2c.obj" \
"./libraries/bmx_util/bmx_utilities.obj" \
"./libraries/bmx_util/delay.obj" \
"./libraries/uartsdio/uartstdio.obj" \
"/home/alealfaro/Embedded/tivaware/driverlib/ccs/Debug/driverlib.lib" \
"../tm4c123gh6pm.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include drivers/lsm9ds1/subdir_vars.mk
-include libraries/bmx_adc/subdir_vars.mk
-include libraries/bmx_bluetooth/subdir_vars.mk
-include libraries/bmx_encoder/subdir_vars.mk
-include libraries/bmx_esc/subdir_vars.mk
-include libraries/bmx_imu/subdir_vars.mk
-include libraries/bmx_util/subdir_vars.mk
-include libraries/uartsdio/subdir_vars.mk
-include subdir_rules.mk
-include drivers/lsm9ds1/subdir_rules.mk
-include libraries/bmx_adc/subdir_rules.mk
-include libraries/bmx_bluetooth/subdir_rules.mk
-include libraries/bmx_encoder/subdir_rules.mk
-include libraries/bmx_esc/subdir_rules.mk
-include libraries/bmx_imu/subdir_rules.mk
-include libraries/bmx_util/subdir_rules.mk
-include libraries/uartsdio/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
bmx_fall_2020.out \

EXE_OUTPUTS__QUOTED += \
"bmx_fall_2020.out" \

BIN_OUTPUTS += \
bmx_fall_2020.hex \

BIN_OUTPUTS__QUOTED += \
"bmx_fall_2020.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "bmx_fall_2020.out"

# Tool invocations
bmx_fall_2020.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: ARM Linker'
	"/home/alealfaro/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"bmx_fall_2020.map" --heap_size=0 --stack_size=512 -i"/home/alealfaro/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/lib" -i"/home/alealfaro/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="bmx_fall_2020_linkInfo.xml" --rom_model -o "bmx_fall_2020.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

bmx_fall_2020.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: ARM Hex Utility'
	"/home/alealfaro/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armhex"  -o "bmx_fall_2020.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "tm4c123gh6pm_startup_ccs.obj" "drivers/lsm9ds1/lsm9ds1.obj" "libraries/bmx_adc/adcInit.obj" "libraries/bmx_bluetooth/bmx_bluetooth.obj" "libraries/bmx_encoder/bmx_encoder.obj" "libraries/bmx_esc/bmx_esc.obj" "libraries/bmx_imu/i2c.obj" "libraries/bmx_util/bmx_utilities.obj" "libraries/bmx_util/delay.obj" "libraries/uartsdio/uartstdio.obj" 
	-$(RM) "main.d" "tm4c123gh6pm_startup_ccs.d" "drivers/lsm9ds1/lsm9ds1.d" "libraries/bmx_adc/adcInit.d" "libraries/bmx_bluetooth/bmx_bluetooth.d" "libraries/bmx_encoder/bmx_encoder.d" "libraries/bmx_esc/bmx_esc.d" "libraries/bmx_imu/i2c.d" "libraries/bmx_util/bmx_utilities.d" "libraries/bmx_util/delay.d" "libraries/uartsdio/uartstdio.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

