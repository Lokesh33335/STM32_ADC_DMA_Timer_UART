#include "stm32f401xe.h"

volatile uint16_t adc_value;
volatile uint8_t adc_ready = 0;

/* ================= UART ================= */
void uart_send_char(char c)
{
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

void uart_send_number(uint16_t num)
{
    char buf[6];
    int i = 0;

    if (num == 0)
    {
        uart_send_char('0');
        return;
    }

    while (num > 0)
    {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    for (int j = i - 1; j >= 0; j--)
        uart_send_char(buf[j]);
}

/* ================= DMA IRQ ================= */
void DMA2_Stream0_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF0)
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;  // Clear TC flag
        adc_ready = 1;                   // New ADC value ready
    }
}

/* ================= MAIN ================= */
int main(void)
{
    /* ================= CLOCK ================= */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    /* ================= GPIO ================= */
    /* PA5 - LED */
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));

    /* PA0 - ADC */
    GPIOA->MODER |= (3U << (0 * 2));

    /* PA2 - USART2 TX */
    GPIOA->MODER &= ~(3U << (2 * 2));
    GPIOA->MODER |=  (2U << (2 * 2));
    GPIOA->AFR[0] |=  (7U << (2 * 4));

    /* ================= TIM2 (100Hz TRGO) ================= */
    TIM2->PSC = 1600 - 1;   // 16 MHz / 1600 = 10 kHz
    TIM2->ARR = 100 - 1;    // 10 kHz / 100 = 100 Hz
       // 10kHz / 100 = 100Hz
    TIM2->EGR = TIM_EGR_UG;

    TIM2->CR2 &= ~TIM_CR2_MMS;
    TIM2->CR2 |= TIM_CR2_MMS_1;  // TRGO on update

    TIM2->CR1 |= TIM_CR1_CEN;

    /* ================= DMA (ADC1) ================= */
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream0->CR & DMA_SxCR_EN);

    DMA2_Stream0->PAR  = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)&adc_value;
    DMA2_Stream0->NDTR = 1;

    DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL;   // Channel 0
    DMA2_Stream0->CR |= DMA_SxCR_CIRC;
    DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;  // 16-bit
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;  // 16-bit
    DMA2_Stream0->CR |= DMA_SxCR_TCIE;     // Transfer complete interrupt

    DMA2_Stream0->CR |= DMA_SxCR_EN;
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    /* ================= ADC ================= */
    ADC1->SQR3 = 0;              // Channel 0 (PA0)
    ADC1->SMPR2 |= (7U << 0);    // Max sample time

    ADC1->CR2 &= ~ADC_CR2_EXTSEL;
    ADC1->CR2 |= (6U << 24);    // TIM2 TRGO
    ADC1->CR2 |= ADC_CR2_EXTEN_0;

    ADC1->CR2 |= ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_DDS;

    ADC1->CR2 |= ADC_CR2_ADON;   // Power ON
    ADC1->CR2 |= ADC_CR2_ADON;   // Start conversions

    /* ================= USART2 ================= */
    USART2->BRR = 16000000 / 115200;

    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_UE;

    /* ================= LOOP ================= */
    while (1)
    {
        if (adc_ready)
        {
            adc_ready = 0;

            uart_send_number(adc_value);
            uart_send_char('\r');
            uart_send_char('\n');

            GPIOA->ODR ^= (1U << 5);  // LED toggles every 10 ms
        }
    }
}

