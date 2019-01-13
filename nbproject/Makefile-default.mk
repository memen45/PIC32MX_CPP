#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=USB/UsbEP.cpp USB/UsbCdcAcm.cpp USB/UsbCentral.cpp USB/UsbISR.cpp Pins.cpp config_bits.c Delay.cpp Timer23.cpp Spi.cpp Uart.cpp Ccp.cpp Sys.cpp Adc.cpp Osc.cpp Clc.cpp Comp.cpp Cp0.cpp Cvref.cpp Dma.cpp I2c.cpp Irq.cpp Nvm.cpp Pmd.cpp Resets.cpp Rtcc.cpp Timer1.cpp Vdetect.cpp Wdt.cpp Usb.cpp main.cpp

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/USB/UsbEP.o ${OBJECTDIR}/USB/UsbCdcAcm.o ${OBJECTDIR}/USB/UsbCentral.o ${OBJECTDIR}/USB/UsbISR.o ${OBJECTDIR}/Pins.o ${OBJECTDIR}/config_bits.o ${OBJECTDIR}/Delay.o ${OBJECTDIR}/Timer23.o ${OBJECTDIR}/Spi.o ${OBJECTDIR}/Uart.o ${OBJECTDIR}/Ccp.o ${OBJECTDIR}/Sys.o ${OBJECTDIR}/Adc.o ${OBJECTDIR}/Osc.o ${OBJECTDIR}/Clc.o ${OBJECTDIR}/Comp.o ${OBJECTDIR}/Cp0.o ${OBJECTDIR}/Cvref.o ${OBJECTDIR}/Dma.o ${OBJECTDIR}/I2c.o ${OBJECTDIR}/Irq.o ${OBJECTDIR}/Nvm.o ${OBJECTDIR}/Pmd.o ${OBJECTDIR}/Resets.o ${OBJECTDIR}/Rtcc.o ${OBJECTDIR}/Timer1.o ${OBJECTDIR}/Vdetect.o ${OBJECTDIR}/Wdt.o ${OBJECTDIR}/Usb.o ${OBJECTDIR}/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/USB/UsbEP.o.d ${OBJECTDIR}/USB/UsbCdcAcm.o.d ${OBJECTDIR}/USB/UsbCentral.o.d ${OBJECTDIR}/USB/UsbISR.o.d ${OBJECTDIR}/Pins.o.d ${OBJECTDIR}/config_bits.o.d ${OBJECTDIR}/Delay.o.d ${OBJECTDIR}/Timer23.o.d ${OBJECTDIR}/Spi.o.d ${OBJECTDIR}/Uart.o.d ${OBJECTDIR}/Ccp.o.d ${OBJECTDIR}/Sys.o.d ${OBJECTDIR}/Adc.o.d ${OBJECTDIR}/Osc.o.d ${OBJECTDIR}/Clc.o.d ${OBJECTDIR}/Comp.o.d ${OBJECTDIR}/Cp0.o.d ${OBJECTDIR}/Cvref.o.d ${OBJECTDIR}/Dma.o.d ${OBJECTDIR}/I2c.o.d ${OBJECTDIR}/Irq.o.d ${OBJECTDIR}/Nvm.o.d ${OBJECTDIR}/Pmd.o.d ${OBJECTDIR}/Resets.o.d ${OBJECTDIR}/Rtcc.o.d ${OBJECTDIR}/Timer1.o.d ${OBJECTDIR}/Vdetect.o.d ${OBJECTDIR}/Wdt.o.d ${OBJECTDIR}/Usb.o.d ${OBJECTDIR}/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/USB/UsbEP.o ${OBJECTDIR}/USB/UsbCdcAcm.o ${OBJECTDIR}/USB/UsbCentral.o ${OBJECTDIR}/USB/UsbISR.o ${OBJECTDIR}/Pins.o ${OBJECTDIR}/config_bits.o ${OBJECTDIR}/Delay.o ${OBJECTDIR}/Timer23.o ${OBJECTDIR}/Spi.o ${OBJECTDIR}/Uart.o ${OBJECTDIR}/Ccp.o ${OBJECTDIR}/Sys.o ${OBJECTDIR}/Adc.o ${OBJECTDIR}/Osc.o ${OBJECTDIR}/Clc.o ${OBJECTDIR}/Comp.o ${OBJECTDIR}/Cp0.o ${OBJECTDIR}/Cvref.o ${OBJECTDIR}/Dma.o ${OBJECTDIR}/I2c.o ${OBJECTDIR}/Irq.o ${OBJECTDIR}/Nvm.o ${OBJECTDIR}/Pmd.o ${OBJECTDIR}/Resets.o ${OBJECTDIR}/Rtcc.o ${OBJECTDIR}/Timer1.o ${OBJECTDIR}/Vdetect.o ${OBJECTDIR}/Wdt.o ${OBJECTDIR}/Usb.o ${OBJECTDIR}/main.o

# Source Files
SOURCEFILES=USB/UsbEP.cpp USB/UsbCdcAcm.cpp USB/UsbCentral.cpp USB/UsbISR.cpp Pins.cpp config_bits.c Delay.cpp Timer23.cpp Spi.cpp Uart.cpp Ccp.cpp Sys.cpp Adc.cpp Osc.cpp Clc.cpp Comp.cpp Cp0.cpp Cvref.cpp Dma.cpp I2c.cpp Irq.cpp Nvm.cpp Pmd.cpp Resets.cpp Rtcc.cpp Timer1.cpp Vdetect.cpp Wdt.cpp Usb.cpp main.cpp


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
Device=PIC32MM0256GPM064
ProjectDir=/home/owner/MPLABXProjects/test-xc32.X
ConfName=default
ImagePath=dist/default/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ImageDir=dist/default/${IMAGE_TYPE}
ImageName=test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: [${MP_CC_DIR}/xc32-objdump -m mips:micromips -d ${ImageDir}/${PROJECTNAME}.${IMAGE_TYPE}.elf > list.txt]"
	@${MP_CC_DIR}/xc32-objdump -m mips:micromips -d ${ImageDir}/${PROJECTNAME}.${IMAGE_TYPE}.elf > list.txt
	@echo "--------------------------------------"

MP_PROCESSOR_OPTION=32MM0256GPM064
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/config_bits.o: config_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/config_bits.o.d 
	@${RM} ${OBJECTDIR}/config_bits.o 
	@${FIXDEPS} "${OBJECTDIR}/config_bits.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -ffunction-sections -fdata-sections -Os -MMD -MF "${OBJECTDIR}/config_bits.o.d" -o ${OBJECTDIR}/config_bits.o config_bits.c   -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/config_bits.o: config_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/config_bits.o.d 
	@${RM} ${OBJECTDIR}/config_bits.o 
	@${FIXDEPS} "${OBJECTDIR}/config_bits.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -ffunction-sections -fdata-sections -Os -MMD -MF "${OBJECTDIR}/config_bits.o.d" -o ${OBJECTDIR}/config_bits.o config_bits.c   -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/USB/UsbEP.o: USB/UsbEP.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbEP.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbEP.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbEP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbEP.o.d" -o ${OBJECTDIR}/USB/UsbEP.o USB/UsbEP.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/USB/UsbCdcAcm.o: USB/UsbCdcAcm.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbCdcAcm.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbCdcAcm.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbCdcAcm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbCdcAcm.o.d" -o ${OBJECTDIR}/USB/UsbCdcAcm.o USB/UsbCdcAcm.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/USB/UsbCentral.o: USB/UsbCentral.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbCentral.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbCentral.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbCentral.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbCentral.o.d" -o ${OBJECTDIR}/USB/UsbCentral.o USB/UsbCentral.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/USB/UsbISR.o: USB/UsbISR.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbISR.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbISR.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbISR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbISR.o.d" -o ${OBJECTDIR}/USB/UsbISR.o USB/UsbISR.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Pins.o: Pins.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Pins.o.d 
	@${RM} ${OBJECTDIR}/Pins.o 
	@${FIXDEPS} "${OBJECTDIR}/Pins.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Pins.o.d" -o ${OBJECTDIR}/Pins.o Pins.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Delay.o: Delay.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Delay.o.d 
	@${RM} ${OBJECTDIR}/Delay.o 
	@${FIXDEPS} "${OBJECTDIR}/Delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Delay.o.d" -o ${OBJECTDIR}/Delay.o Delay.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Timer23.o: Timer23.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Timer23.o.d 
	@${RM} ${OBJECTDIR}/Timer23.o 
	@${FIXDEPS} "${OBJECTDIR}/Timer23.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Timer23.o.d" -o ${OBJECTDIR}/Timer23.o Timer23.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Spi.o: Spi.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Spi.o.d 
	@${RM} ${OBJECTDIR}/Spi.o 
	@${FIXDEPS} "${OBJECTDIR}/Spi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Spi.o.d" -o ${OBJECTDIR}/Spi.o Spi.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Uart.o: Uart.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Uart.o.d 
	@${RM} ${OBJECTDIR}/Uart.o 
	@${FIXDEPS} "${OBJECTDIR}/Uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Uart.o.d" -o ${OBJECTDIR}/Uart.o Uart.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Ccp.o: Ccp.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Ccp.o.d 
	@${RM} ${OBJECTDIR}/Ccp.o 
	@${FIXDEPS} "${OBJECTDIR}/Ccp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Ccp.o.d" -o ${OBJECTDIR}/Ccp.o Ccp.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Sys.o: Sys.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Sys.o.d 
	@${RM} ${OBJECTDIR}/Sys.o 
	@${FIXDEPS} "${OBJECTDIR}/Sys.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Sys.o.d" -o ${OBJECTDIR}/Sys.o Sys.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Adc.o: Adc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Adc.o.d 
	@${RM} ${OBJECTDIR}/Adc.o 
	@${FIXDEPS} "${OBJECTDIR}/Adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Adc.o.d" -o ${OBJECTDIR}/Adc.o Adc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Osc.o: Osc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Osc.o.d 
	@${RM} ${OBJECTDIR}/Osc.o 
	@${FIXDEPS} "${OBJECTDIR}/Osc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Osc.o.d" -o ${OBJECTDIR}/Osc.o Osc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Clc.o: Clc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Clc.o.d 
	@${RM} ${OBJECTDIR}/Clc.o 
	@${FIXDEPS} "${OBJECTDIR}/Clc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Clc.o.d" -o ${OBJECTDIR}/Clc.o Clc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Comp.o: Comp.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Comp.o.d 
	@${RM} ${OBJECTDIR}/Comp.o 
	@${FIXDEPS} "${OBJECTDIR}/Comp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Comp.o.d" -o ${OBJECTDIR}/Comp.o Comp.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Cp0.o: Cp0.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Cp0.o.d 
	@${RM} ${OBJECTDIR}/Cp0.o 
	@${FIXDEPS} "${OBJECTDIR}/Cp0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Cp0.o.d" -o ${OBJECTDIR}/Cp0.o Cp0.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Cvref.o: Cvref.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Cvref.o.d 
	@${RM} ${OBJECTDIR}/Cvref.o 
	@${FIXDEPS} "${OBJECTDIR}/Cvref.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Cvref.o.d" -o ${OBJECTDIR}/Cvref.o Cvref.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Dma.o: Dma.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Dma.o.d 
	@${RM} ${OBJECTDIR}/Dma.o 
	@${FIXDEPS} "${OBJECTDIR}/Dma.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Dma.o.d" -o ${OBJECTDIR}/Dma.o Dma.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/I2c.o: I2c.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/I2c.o.d 
	@${RM} ${OBJECTDIR}/I2c.o 
	@${FIXDEPS} "${OBJECTDIR}/I2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/I2c.o.d" -o ${OBJECTDIR}/I2c.o I2c.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Irq.o: Irq.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Irq.o.d 
	@${RM} ${OBJECTDIR}/Irq.o 
	@${FIXDEPS} "${OBJECTDIR}/Irq.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Irq.o.d" -o ${OBJECTDIR}/Irq.o Irq.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Nvm.o: Nvm.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Nvm.o.d 
	@${RM} ${OBJECTDIR}/Nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/Nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Nvm.o.d" -o ${OBJECTDIR}/Nvm.o Nvm.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Pmd.o: Pmd.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Pmd.o.d 
	@${RM} ${OBJECTDIR}/Pmd.o 
	@${FIXDEPS} "${OBJECTDIR}/Pmd.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Pmd.o.d" -o ${OBJECTDIR}/Pmd.o Pmd.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Resets.o: Resets.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Resets.o.d 
	@${RM} ${OBJECTDIR}/Resets.o 
	@${FIXDEPS} "${OBJECTDIR}/Resets.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Resets.o.d" -o ${OBJECTDIR}/Resets.o Resets.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Rtcc.o: Rtcc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Rtcc.o.d 
	@${RM} ${OBJECTDIR}/Rtcc.o 
	@${FIXDEPS} "${OBJECTDIR}/Rtcc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Rtcc.o.d" -o ${OBJECTDIR}/Rtcc.o Rtcc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Timer1.o: Timer1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Timer1.o.d 
	@${RM} ${OBJECTDIR}/Timer1.o 
	@${FIXDEPS} "${OBJECTDIR}/Timer1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Timer1.o.d" -o ${OBJECTDIR}/Timer1.o Timer1.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Vdetect.o: Vdetect.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Vdetect.o.d 
	@${RM} ${OBJECTDIR}/Vdetect.o 
	@${FIXDEPS} "${OBJECTDIR}/Vdetect.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Vdetect.o.d" -o ${OBJECTDIR}/Vdetect.o Vdetect.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Wdt.o: Wdt.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Wdt.o.d 
	@${RM} ${OBJECTDIR}/Wdt.o 
	@${FIXDEPS} "${OBJECTDIR}/Wdt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Wdt.o.d" -o ${OBJECTDIR}/Wdt.o Wdt.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Usb.o: Usb.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Usb.o.d 
	@${RM} ${OBJECTDIR}/Usb.o 
	@${FIXDEPS} "${OBJECTDIR}/Usb.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Usb.o.d" -o ${OBJECTDIR}/Usb.o Usb.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/USB/UsbEP.o: USB/UsbEP.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbEP.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbEP.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbEP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbEP.o.d" -o ${OBJECTDIR}/USB/UsbEP.o USB/UsbEP.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/USB/UsbCdcAcm.o: USB/UsbCdcAcm.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbCdcAcm.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbCdcAcm.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbCdcAcm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbCdcAcm.o.d" -o ${OBJECTDIR}/USB/UsbCdcAcm.o USB/UsbCdcAcm.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/USB/UsbCentral.o: USB/UsbCentral.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbCentral.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbCentral.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbCentral.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbCentral.o.d" -o ${OBJECTDIR}/USB/UsbCentral.o USB/UsbCentral.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/USB/UsbISR.o: USB/UsbISR.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/UsbISR.o.d 
	@${RM} ${OBJECTDIR}/USB/UsbISR.o 
	@${FIXDEPS} "${OBJECTDIR}/USB/UsbISR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/USB/UsbISR.o.d" -o ${OBJECTDIR}/USB/UsbISR.o USB/UsbISR.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Pins.o: Pins.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Pins.o.d 
	@${RM} ${OBJECTDIR}/Pins.o 
	@${FIXDEPS} "${OBJECTDIR}/Pins.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Pins.o.d" -o ${OBJECTDIR}/Pins.o Pins.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Delay.o: Delay.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Delay.o.d 
	@${RM} ${OBJECTDIR}/Delay.o 
	@${FIXDEPS} "${OBJECTDIR}/Delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Delay.o.d" -o ${OBJECTDIR}/Delay.o Delay.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Timer23.o: Timer23.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Timer23.o.d 
	@${RM} ${OBJECTDIR}/Timer23.o 
	@${FIXDEPS} "${OBJECTDIR}/Timer23.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Timer23.o.d" -o ${OBJECTDIR}/Timer23.o Timer23.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Spi.o: Spi.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Spi.o.d 
	@${RM} ${OBJECTDIR}/Spi.o 
	@${FIXDEPS} "${OBJECTDIR}/Spi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Spi.o.d" -o ${OBJECTDIR}/Spi.o Spi.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Uart.o: Uart.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Uart.o.d 
	@${RM} ${OBJECTDIR}/Uart.o 
	@${FIXDEPS} "${OBJECTDIR}/Uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Uart.o.d" -o ${OBJECTDIR}/Uart.o Uart.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Ccp.o: Ccp.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Ccp.o.d 
	@${RM} ${OBJECTDIR}/Ccp.o 
	@${FIXDEPS} "${OBJECTDIR}/Ccp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Ccp.o.d" -o ${OBJECTDIR}/Ccp.o Ccp.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Sys.o: Sys.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Sys.o.d 
	@${RM} ${OBJECTDIR}/Sys.o 
	@${FIXDEPS} "${OBJECTDIR}/Sys.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Sys.o.d" -o ${OBJECTDIR}/Sys.o Sys.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Adc.o: Adc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Adc.o.d 
	@${RM} ${OBJECTDIR}/Adc.o 
	@${FIXDEPS} "${OBJECTDIR}/Adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Adc.o.d" -o ${OBJECTDIR}/Adc.o Adc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Osc.o: Osc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Osc.o.d 
	@${RM} ${OBJECTDIR}/Osc.o 
	@${FIXDEPS} "${OBJECTDIR}/Osc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Osc.o.d" -o ${OBJECTDIR}/Osc.o Osc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Clc.o: Clc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Clc.o.d 
	@${RM} ${OBJECTDIR}/Clc.o 
	@${FIXDEPS} "${OBJECTDIR}/Clc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Clc.o.d" -o ${OBJECTDIR}/Clc.o Clc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Comp.o: Comp.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Comp.o.d 
	@${RM} ${OBJECTDIR}/Comp.o 
	@${FIXDEPS} "${OBJECTDIR}/Comp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Comp.o.d" -o ${OBJECTDIR}/Comp.o Comp.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Cp0.o: Cp0.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Cp0.o.d 
	@${RM} ${OBJECTDIR}/Cp0.o 
	@${FIXDEPS} "${OBJECTDIR}/Cp0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Cp0.o.d" -o ${OBJECTDIR}/Cp0.o Cp0.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Cvref.o: Cvref.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Cvref.o.d 
	@${RM} ${OBJECTDIR}/Cvref.o 
	@${FIXDEPS} "${OBJECTDIR}/Cvref.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Cvref.o.d" -o ${OBJECTDIR}/Cvref.o Cvref.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Dma.o: Dma.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Dma.o.d 
	@${RM} ${OBJECTDIR}/Dma.o 
	@${FIXDEPS} "${OBJECTDIR}/Dma.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Dma.o.d" -o ${OBJECTDIR}/Dma.o Dma.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/I2c.o: I2c.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/I2c.o.d 
	@${RM} ${OBJECTDIR}/I2c.o 
	@${FIXDEPS} "${OBJECTDIR}/I2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/I2c.o.d" -o ${OBJECTDIR}/I2c.o I2c.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Irq.o: Irq.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Irq.o.d 
	@${RM} ${OBJECTDIR}/Irq.o 
	@${FIXDEPS} "${OBJECTDIR}/Irq.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Irq.o.d" -o ${OBJECTDIR}/Irq.o Irq.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Nvm.o: Nvm.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Nvm.o.d 
	@${RM} ${OBJECTDIR}/Nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/Nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Nvm.o.d" -o ${OBJECTDIR}/Nvm.o Nvm.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Pmd.o: Pmd.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Pmd.o.d 
	@${RM} ${OBJECTDIR}/Pmd.o 
	@${FIXDEPS} "${OBJECTDIR}/Pmd.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Pmd.o.d" -o ${OBJECTDIR}/Pmd.o Pmd.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Resets.o: Resets.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Resets.o.d 
	@${RM} ${OBJECTDIR}/Resets.o 
	@${FIXDEPS} "${OBJECTDIR}/Resets.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Resets.o.d" -o ${OBJECTDIR}/Resets.o Resets.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Rtcc.o: Rtcc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Rtcc.o.d 
	@${RM} ${OBJECTDIR}/Rtcc.o 
	@${FIXDEPS} "${OBJECTDIR}/Rtcc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Rtcc.o.d" -o ${OBJECTDIR}/Rtcc.o Rtcc.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Timer1.o: Timer1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Timer1.o.d 
	@${RM} ${OBJECTDIR}/Timer1.o 
	@${FIXDEPS} "${OBJECTDIR}/Timer1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Timer1.o.d" -o ${OBJECTDIR}/Timer1.o Timer1.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Vdetect.o: Vdetect.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Vdetect.o.d 
	@${RM} ${OBJECTDIR}/Vdetect.o 
	@${FIXDEPS} "${OBJECTDIR}/Vdetect.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Vdetect.o.d" -o ${OBJECTDIR}/Vdetect.o Vdetect.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Wdt.o: Wdt.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Wdt.o.d 
	@${RM} ${OBJECTDIR}/Wdt.o 
	@${FIXDEPS} "${OBJECTDIR}/Wdt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Wdt.o.d" -o ${OBJECTDIR}/Wdt.o Wdt.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Usb.o: Usb.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Usb.o.d 
	@${RM} ${OBJECTDIR}/Usb.o 
	@${FIXDEPS} "${OBJECTDIR}/Usb.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/Usb.o.d" -o ${OBJECTDIR}/Usb.o Usb.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"/home/owner/MPLABXProjects/test-xc32.X" -I"USB" -frtti -fexceptions -fno-check-new -fenforce-eh-specs -ffunction-sections -fdata-sections -Os -Werror -Wall -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.cpp  -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION) -Os -o dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)   -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC00490:0x1FC016FF -mreserve=boot@0x1FC00490:0x1FC00BEF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=1024,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -Os -o dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -save-temps=obj -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=1024,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}/xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/test-xc32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
