################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.c 

OBJS += \
./Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.o 

C_DEPS += \
./Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.o: ../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../X-CUBE-MEMS1/Target -I../Drivers/BSP/B-L4S5I-IOT01A -I../Drivers/BSP/Components/lsm6dsl -I"C:/Users/khk02/workspace/shh2021_LiveinSIOR/wearable_for_bus/Drivers/BSP/B-L4S5I-IOT01A" -I../Middlewares/ST/BlueNRG-MS/utils -I../Middlewares/ST/BlueNRG-MS/includes -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../BlueNRG_MS/App -I../BlueNRG_MS/Target -I../X-CUBE-AI/App -I../Middlewares/ST/AI/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

