/*
 Final Project
 Team  11
 */

#include "Rcc.h"
#include "Bit_Operations.h"
#include "gpt.h"
#include "gpt_private.h"

//..................................................................................................................................................................
void GPT_Init(void){
    Rcc_Enable(RCC_TIM2);
    // Set the prescaler value to divide the clock by 256
    TIM2->PSC= 16000 - 1;

    // set Edge-aligned mode
    CLEAR_BIT(TIM2->CR1,Edge_aligned_mode_pos1);
    CLEAR_BIT(TIM2->CR1,Edge_aligned_mode_pos2);
    // up counter 
    CLEAR_BIT(TIM2->CR1,Counter_dir_pos);

    CLEAR_BIT(TIM2->SR,UIF_POS); //update interrupt flag

    TIM2->CNT =0;

    // Auto-reload preload enable
    SET_BIT(TIM2->CR1,Auto_reload_pos);

    SET_BIT(TIM2->EGR,UG_pos);

}

//..................................................................................................................................................................
void GPT_StartTimer(uint32 OverFlowTicks){
    CLEAR_BIT(TIM2->CR1,timer_pos);
    TIM2->CNT =0;
   // Wait for the timer to reach the reload value (i.e., overflow)
    TIM2->ARR = OverFlowTicks - 1;
    CLEAR_BIT(TIM2->SR,UIF_POS); //update interrupt flag
    // enable timer 
    SET_BIT(TIM2->CR1,timer_pos);

    }

//..................................................................................................................................................................
uint8 GPT_CheckTimeIsElapsed(void){
    
    if (READ_BIT(TIM2->SR,UIF_POS)) {
        // overflowed since the last call to GPT_StartTimer
        return 1;
    }
    else{
    return 0;
    }

}

//..................................................................................................................................................................
uint32 GPT_GetElapsedTime(void){
    uint32 currentTicks = TIM2->CNT;

   if(GPT_CheckTimeIsElapsed()) {
        return MAX_VALUE;
    }
   else{

        return currentTicks;
    }
    
}

//..................................................................................................................................................................
uint32 GPT_GetRemainingTime(void){

    // Check if GPT_StartTimer has been called
    if ((!READ_BIT(TIM2->CR1,timer_pos)) && GPT_CheckTimeIsElapsed()) {
        return MAX_VALUE;
    }

    // Get the current counter value
    unsigned long int currentTicks = TIM2->CNT;

    // Get the ARR value
    unsigned long int overflowTicks = TIM2->ARR + 1;

    // Check if an overflow has occurred since the last call to GPT_StartTimer
    if (GPT_CheckTimeIsElapsed()) {
        return 0;
    }

    // Calculate the remaining ticks until the overflow
    unsigned long int remainingTicks = overflowTicks - currentTicks;

    // Return the remaining ticks
    return remainingTicks;

}
