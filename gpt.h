/*
 Final Project
 Team  11
 */

#ifndef GPT_H
#define GPT_H
#include "Std_Types.h"


#define MAX_VALUE 0xffffffff
#define Edge_aligned_mode_pos1 5
#define Edge_aligned_mode_pos2 6
#define Counter_dir_pos 4
#define UIF_POS 0
#define timer_pos 0
#define Auto_reload_pos 7
#define UG_pos 0

 void GPT_Init(void);
 void GPT_StartTimer(uint32 OverFlowTicks);
 uint8 GPT_CheckTimeIsElapsed(void);
 uint32 GPT_GetElapsedTime(void);
 uint32 GPT_GetRemainingTime(void);


#endif
