################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
library/TI_MSP430_spi_USCIB0_5xx.obj: ../library/TI_MSP430_spi_USCIB0_5xx.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="C:/Users/Quest01/Desktop/code/F5xx_F6xx_Core_Lib" --include_path="C:/Users/Quest01/Desktop/code/include" --include_path="C:/Users/Quest01/Desktop/code/driverlib/MSP430F5xx_6xx" --include_path="C:/Users/Quest01/Desktop/code/driverlib/MSP430F5xx_6xx/deprecated" --include_path="C:/Users/Quest01/Desktop/code/USB_API/USB_Common" --include_path="C:/Users/Quest01/Desktop/code/USB_config" --include_path="C:/Users/Quest01/Desktop/code/USB_app" --include_path="C:/Users/Quest01/Desktop/code" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.3/include" --advice:power=all -g --define=__MSP430F5529__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="library/TI_MSP430_spi_USCIB0_5xx.d" --obj_directory="library" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


