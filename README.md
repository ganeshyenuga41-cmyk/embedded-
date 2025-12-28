# embedded-
### STM32-UART-DMA-ADC
### Description
This project demonstrates ADC data acquisition using DMA on STM32 Black Pill.
ADC conversion is triggered by a 100Hz timer and results are transmitted via UART.

### Features
100Hz Timer Trigger
ADC with DMA
UART Transmission
Serial output on PuTTY

### Hardware Used
STM32 Black Pill (STM32F411)
USB to UART Converter

### Tools
STM32CubeIDE
HAL Drivers
PuTTY

### Author
Ganesh


#include "stm32f401cc.h"

#define ADC_BUF_LEN  1

volatile uint16_t adc_buf[ADC_BUF_LEN];

/* Simple delay */
void delay(volatile uint32_t d)
{
    while (d--);
}

/* UART2 send char */
void uart2_tx(char c)
{
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

/* UART2 send string */
void uart2_print(char *s)
{
    while (*s)
        uart2_tx(*s++);
}

/* UART2 send number */
void uart2_print_num(uint16_t n)
{
    char buf[6];
    int i = 0;

    if (n == 0)
    {
        uart2_tx('0');
        return;
    }

    while (n > 0)
    {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }

    while (i--)
        uart2_tx(buf[i]);
}

/* TIM2 IRQ → trigger ADC */
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
    }
}

/* DMA IRQ → UART print */
void DMA2_Stream0_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF0)
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;

        uart2_print("ADC: ");
        uart2_print_num(adc_buf[0]);
        uart2_print("\r\n");
    }
}

int main(void)
{
    /* Enable clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_DMA2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* PA0 → ADC */
    GPIOA->MODER |= GPIO_MODER_MODE0;

    /* PA2 TX, PA3 RX */
    GPIOA->MODER |= (2 << 4) | (2 << 6);
    GPIOA->AFR[0] |= (7 << 8) | (7 << 12);

    /* USART2 115200 baud */
    USART2->BRR = 0x2D9;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;

    /* TIM2 → 100 Hz */
    TIM2->PSC = 8399;
    TIM2->ARR = 99;
    TIM2->CR2 |= TIM_CR2_MMS_1;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM2_IRQn);

    /* ADC config */
    ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS;
    ADC1->CR2 |= ADC_CR2_EXTEN_0;
    ADC1->CR2 |= (0b0110 << 24); // TIM2 TRGO
    ADC1->SQR3 = 0;
    ADC1->CR2 |= ADC_CR2_ADON;

    /* DMA2 Stream0 ADC */
    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)adc_buf;
    DMA2_Stream0->NDTR = ADC_BUF_LEN;
    DMA2_Stream0->CR =
        (0 << 25) |   // Channel 0
        DMA_SxCR_MINC |
        DMA_SxCR_TCIE |
        DMA_SxCR_CIRC |
        DMA_SxCR_PSIZE_0 |
        DMA_SxCR_MSIZE_0;

    DMA2_Stream0->CR |= DMA_SxCR_EN;

    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    /* Start ADC */
    ADC1->CR2 |= ADC_CR2_SWSTART;

    uart2_print("System Ready\r\n");

    while (1)
    {
        __WFI(); // low power wait
    }
}
