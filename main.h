#ifndef STM32F401CC_H
#define STM32F401CC_H

#include <stdint.h>

/* ================== BASE ADDRESSES ================== */

#define PERIPH_BASE        0x40000000UL
#define AHB1PERIPH_BASE    (PERIPH_BASE + 0x00020000UL)
#define APB1PERIPH_BASE    (PERIPH_BASE + 0x00000000UL)
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x00010000UL)

/* ================== RCC ================== */

#define RCC_BASE           (AHB1PERIPH_BASE + 0x3800UL)

typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t RESERVED0[2];
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t RESERVE_
