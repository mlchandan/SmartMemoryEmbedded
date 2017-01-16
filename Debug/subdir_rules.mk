################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
PLL.obj: ../PLL.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/TivaWare_C_Series-2.1.2.111/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" -g --gcc --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="PLL.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

diskio.obj: ../diskio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/TivaWare_C_Series-2.1.2.111/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" -g --gcc --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="diskio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ff.obj: ../ff.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/TivaWare_C_Series-2.1.2.111/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" -g --gcc --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="ff.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/TivaWare_C_Series-2.1.2.111/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" -g --gcc --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c123gh6pm_startup_ccs.obj: ../tm4c123gh6pm_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/TivaWare_C_Series-2.1.2.111/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" -g --gcc --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="tm4c123gh6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


