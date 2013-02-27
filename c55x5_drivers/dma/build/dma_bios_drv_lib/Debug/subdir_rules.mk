################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
C:/Users/Alex/USB\ Audio\ Class/c55x5_drivers/dma/build/Debug/dda_dma.obj: C:/Users/Alex/USB\ Audio\ Class/c55x5_drivers/dma/src/dda_dma.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/Users/Alex/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -vcpu:3.3 -g --define="_DEBUG" --define="CSL_AC_TEST" --define="PLAY_RECORD" --define="SAMP_24BIT" --define="USE_THREE_CODEC" --include_path="C:/Users/Alex/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/build/dma_bios_drv_lib/Debug" --include_path="C:/Users/Alex/bios_5_42_00_07/packages/ti/bios/include" --include_path="C:/Users/Alex/bios_5_42_00_07/packages/ti/rtdx/include/c5500" --include_path="C:/Users/Alex/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/src" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/inc" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/inc" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/soc/DA225/dsp/inc" --diag_warning=225 --large_memory_model --obj_directory="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/build/Debug" --preproc_with_compile --preproc_dependency="dda_dma.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

C:/Users/Alex/USB\ Audio\ Class/c55x5_drivers/dma/build/Debug/ddc_dma.obj: C:/Users/Alex/USB\ Audio\ Class/c55x5_drivers/dma/src/ddc_dma.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/Users/Alex/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -vcpu:3.3 -g --define="_DEBUG" --define="CSL_AC_TEST" --define="PLAY_RECORD" --define="SAMP_24BIT" --define="USE_THREE_CODEC" --include_path="C:/Users/Alex/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/build/dma_bios_drv_lib/Debug" --include_path="C:/Users/Alex/bios_5_42_00_07/packages/ti/bios/include" --include_path="C:/Users/Alex/bios_5_42_00_07/packages/ti/rtdx/include/c5500" --include_path="C:/Users/Alex/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/src" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/inc" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/inc" --include_path="C:/Users/Alex/USB Audio Class/c55x5_drivers/soc/DA225/dsp/inc" --diag_warning=225 --large_memory_model --obj_directory="C:/Users/Alex/USB Audio Class/c55x5_drivers/dma/build/Debug" --preproc_with_compile --preproc_dependency="ddc_dma.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


