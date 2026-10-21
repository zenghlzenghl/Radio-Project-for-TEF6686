#ifndef RADIO_CTRL_H
#define RADIO_CTRL_H

#include "tef6686.h"

enum {
    BAND_FM = 0,
    BAND_AM = 1
};

enum {
    AREA_EUROPE = 0,
    AREA_USA    = 1,
    AREA_JAPAN  = 2,
    AREA_CHINA  = 3
};

#define FM_FREQ_MIN   8750
#define FM_FREQ_MAX   10800
#define FM_STEP       10

#define AM_FREQ_MIN   531
#define AM_FREQ_MAX   1620
#define AM_STEP       9

typedef struct {
    uint16 current_freq;
    uint8  current_band;
    uint8  area_type;
    uint8  mute_status;
    uint8  signal_meter;
    uint8  station_is_good;
    uint8  fm_dis_sstop;
    uint8  fm_loc_sstop;
    uint8  am_dis_sstop;
    uint8  am_loc_sstop;
} RADIO_PARAM;

extern RADIO_PARAM RadioParam;

void RadioCtrl_Init(void);
void RadioCtrl_SetFreq(uint16 freq);
uint16 RadioCtrl_GetFreq(void);
void RadioCtrl_SetBand(uint8 band);
uint8 RadioCtrl_GetBand(void);
void RadioCtrl_FreqUp(void);
void RadioCtrl_FreqDown(void);
void RadioCtrl_SeekUp(void);
void RadioCtrl_SeekDown(void);
void RadioCtrl_MuteOn(void);
void RadioCtrl_MuteOff(void);
uint8 RadioCtrl_GetLevel(void);
void RadioCtrl_SetVolume(uint8 vol);

#endif