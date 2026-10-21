#include "radio_ctrl.h"

RADIO_PARAM RadioParam;

void RadioCtrl_Init(void)
{
    RadioParam.current_freq = FM_FREQ_MIN;
    RadioParam.current_band = BAND_FM;
    RadioParam.area_type = AREA_CHINA;
    RadioParam.mute_status = 0;
    RadioParam.signal_meter = 0;
    RadioParam.station_is_good = 0;
    RadioParam.fm_dis_sstop = 22;
    RadioParam.fm_loc_sstop = 45;
    RadioParam.am_dis_sstop = 22;
    RadioParam.am_loc_sstop = 45;
    
    TEF6686_Init();
}

void RadioCtrl_SetFreq(uint16 freq)
{
    if (RadioParam.current_band == BAND_FM) {
        if (freq < FM_FREQ_MIN) freq = FM_FREQ_MIN;
        if (freq > FM_FREQ_MAX) freq = FM_FREQ_MAX;
    } else {
        if (freq < AM_FREQ_MIN) freq = AM_FREQ_MIN;
        if (freq > AM_FREQ_MAX) freq = AM_FREQ_MAX;
    }
    
    RadioParam.current_freq = freq;
    TEF6686_TuneToFreq(freq);
}

uint16 RadioCtrl_GetFreq(void)
{
    return RadioParam.current_freq;
}

void RadioCtrl_SetBand(uint8 band)
{
    uint16 freq;
    
    if (band >= 2) return;
    
    RadioParam.current_band = band;
    
    if (band == BAND_FM) {
        freq = FM_FREQ_MIN;
    } else {
        freq = AM_FREQ_MIN;
    }
    
    RadioCtrl_SetFreq(freq);
}

uint8 RadioCtrl_GetBand(void)
{
    return RadioParam.current_band;
}

void RadioCtrl_FreqUp(void)
{
    uint16 freq = RadioParam.current_freq;
    
    if (RadioParam.current_band == BAND_FM) {
        freq += FM_STEP;
        if (freq > FM_FREQ_MAX) freq = FM_FREQ_MIN;
    } else {
        freq += AM_STEP;
        if (freq > AM_FREQ_MAX) freq = AM_FREQ_MIN;
    }
    
    RadioCtrl_SetFreq(freq);
}

void RadioCtrl_FreqDown(void)
{
    uint16 freq = RadioParam.current_freq;
    
    if (RadioParam.current_band == BAND_FM) {
        if (freq <= FM_FREQ_MIN) freq = FM_FREQ_MAX;
        else freq -= FM_STEP;
    } else {
        if (freq <= AM_FREQ_MIN) freq = AM_FREQ_MAX;
        else freq -= AM_STEP;
    }
    
    RadioCtrl_SetFreq(freq);
}

void RadioCtrl_SeekUp(void)
{
    TEF6686_SeekStart(1);
}

void RadioCtrl_SeekDown(void)
{
    TEF6686_SeekStart(0);
}

void RadioCtrl_MuteOn(void)
{
    RadioParam.mute_status = 1;
    TEF6686_Mute(1);
}

void RadioCtrl_MuteOff(void)
{
    RadioParam.mute_status = 0;
    TEF6686_Mute(0);
}

uint8 RadioCtrl_GetLevel(void)
{
    RadioParam.signal_meter = TEF6686_GetLevel();
    return RadioParam.signal_meter;
}

void RadioCtrl_SetVolume(uint8 vol)
{
    TEF6686_SetVolume(vol);
}