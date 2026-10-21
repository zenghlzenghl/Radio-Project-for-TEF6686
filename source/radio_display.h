#ifndef RADIO_DISPLAY_H
#define RADIO_DISPLAY_H

#include "stc8h.h"
#include "radio_ctrl.h"

#define SEEK_IDLE      0
#define SEEK_IN_PROGRESS 1

void RadioDisplay_Init(void);
void RadioDisplay_Process(void);
void RadioDisplay_TimerTick(void);
void RadioDisplay_PrintStatus(void);
void RadioDisplay_SeekStart(uint8 direction);
uint8 RadioDisplay_IsSeeking(void);
uint8 RadioDisplay_GetStereoStatus(void);
uint8 RadioDisplay_GetRssiValue(void);
uint8 RadioDisplay_IsStationReceived(void);

#endif