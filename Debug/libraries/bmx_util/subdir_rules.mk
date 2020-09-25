################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
libraries/bmx_util/%.obj: ../libraries/bmx_util/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/alealfaro/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/alealfaro/bmx_bike" --include_path="/home/alealfaro/Embedded/tivaware" --include_path="/home/alealfaro/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --include_path="/home/alealfaro/bmx_bike/config" --include_path="/home/alealfaro/bmx_bike/drivers/lsm9ds1" --include_path="/home/alealfaro/bmx_bike/libraries" --include_path="/home/alealfaro/bmx_bike/libraries/bmx_bluetooth" --include_path="/home/alealfaro/bmx_bike/libraries/bmx_encoder" --include_path="/home/alealfaro/bmx_bike/libraries/bmx_imu" --include_path="/home/alealfaro/bmx_bike/libraries/bmx_util" --include_path="/home/alealfaro/bmx_bike/libraries/bmx_esc" --include_path="/home/alealfaro/bmx_bike/libraries/bmx_adc" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="libraries/bmx_util/$(basename $(<F)).d_raw" --obj_directory="libraries/bmx_util" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


