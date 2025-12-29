# STM32_ADC_DMA_Timer_UART
In this project, ADC values are read every 10 ms using a 100Hz timer. Each ADC reading is transferred to memory using DMA, and after the DMA transfer completes, the value is sent to the PC through UART. The ADC values are displayed on the PuTTY serial terminal.



# STM32_ADC_DMA_Timer_UART

## Project Description
In this project, ADC values are sampled every 10 ms using a 100 Hz timer.  
Since frequency f = 1 / T, a 100 Hz timer corresponds to a time period of 10 ms.

The timer overflow is used to trigger ADC conversion. Each ADC result is transferred to memory using DMA, and after the DMA transfer completes, the value is transmitted to the PC through UART. The ADC values are displayed on the PuTTY serial terminal.

## Implementation Approach
- This project follows a bare-metal approach
- HAL libraries and CubeMX generated drivers are not used
- All peripherals are configured directly using STM32 registers

## Hardware Used
- STM32F401RE
- ST-LINK programmer
- Buck Converter or Potentiometer (ADC input)
- PC with PuTTY serial terminal

## Software Used
- STM32CubeIDE (used only for building and flashing)
- PuTTY


## Working Flow

1. A hardware timer is configured to generate an event every 10 ms (100 Hz).
2. On every timer overflow, an ADC conversion is triggered automatically.
3. The ADC conversion result is moved to memory using DMA, avoiding CPU polling.
4. When the DMA transfer completes, a DMA interrupt is generated.
5. Inside the DMA interrupt handler, the ADC value is sent to the PC using UART.
6. The transmitted ADC values are observed on the PuTTY serial terminal.



## How to Run
1. Open the project in STM32CubeIDE
2. Build and flash the code to the STM32 board
3. Open PuTTY and connect to the correct COM port (check Device Manager)
4. Set connection type to Serial and baud rate to 115200
5. Change the buck convertter output voltage value and observe ADC values updating every 10 ms in putty


## Connections
1. Connect buck converter (+)pin to PA0 of stm 32 
2. Buck converter (-)pin to GND of stm32 


## Notes
- Register-level configuration is used for Timer, ADC, DMA, and UART
- This implementation avoids abstraction layers to provide better control over hardware



## Supported Boards

This project can be implemented on any STM32 microcontroller board that supports:
- ADC with external trigger capability
- DMA controller
- General-purpose timer
- UART peripheral

I have used STM32-F401RE (available board with me)
Examples of suitable boards include:
- STM32- NUCLEO-F401RE 
- STM32F411CE Black Pill
- STM32F103C8 (Blue Pill)
- STM32F446RE Nucleo
- Other STM32 Cortex-M based boards with ADC, DMA, and UART support

Only minor register-level changes may be required depending on the STM32 series used.

