# STM32_ADC_DMA_Timer_UART
In this project, ADC values are read every 10 ms using a 100Hz timer. Each ADC reading is transferred to memory using DMA, and after the DMA transfer completes, the value is sent to the PC through UART. The ADC values are displayed on the PuTTY serial terminal.
