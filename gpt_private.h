/*
 Final Project
 Team  11
 */

#ifndef GPT_PRIVATE_H
#define GPT_PRIVATE_H
#include "Utils.h"

#define TIM2 ((GptType *)0x40000000)

typedef struct {
	uint32 CR1;
	uint32 CR2;
	uint32 SMCR;
	uint32 DIER;
    uint32 SR;
    uint32 EGR;
    uint32 CCMR1;
    uint32 CCMR2;
    uint32 CCER;
    uint32 CNT;
    uint32 PSC;
    uint32 ARR;
} GptType;

#endif 
