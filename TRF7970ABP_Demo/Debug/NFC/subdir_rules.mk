################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
NFC/iso14443a.obj: ../NFC/iso14443a.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=coffabi -Ooff --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.2.1/include" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo/Hardware" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo/NFC" -g --define=__MSP430G2553__ --define=ENABLE15693 --define=ENABLE14443A --define=ENABLE14443B --define=TRF7970A --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="NFC/iso14443a.pp" --obj_directory="NFC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

NFC/iso14443b.obj: ../NFC/iso14443b.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=coffabi -Ooff --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.2.1/include" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo/Hardware" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo/NFC" -g --define=__MSP430G2553__ --define=ENABLE15693 --define=ENABLE14443A --define=ENABLE14443B --define=TRF7970A --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="NFC/iso14443b.pp" --obj_directory="NFC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

NFC/iso15693.obj: ../NFC/iso15693.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=coffabi -Ooff --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.2.1/include" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo/Hardware" --include_path="C:/Users/Alex/SkyDrive/WRIST_project/TRF7970ABP_Demo/NFC" -g --define=__MSP430G2553__ --define=ENABLE15693 --define=ENABLE14443A --define=ENABLE14443B --define=TRF7970A --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="NFC/iso15693.pp" --obj_directory="NFC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


