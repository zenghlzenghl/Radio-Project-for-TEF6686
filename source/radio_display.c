#include "radio_display.h"
#include "tef6686.h"
#include <stdio.h>

sbit LED_STEREO = P1^5;

static uint16 timer_1s_cnt = 0;
static uint8 stereo_status = 0;
static uint8 rssi_value = 0;
static uint8 station_received = 0;
static uint8 seek_state = SEEK_IDLE;
static uint16 seek_timer_cnt = 0;

static void Display_Stereo(void);
static void ReadStatus_1s(void);
static void SeekProcess(void);

void RadioDisplay_Init(void)
{
    LED_STEREO = 1;
    timer_1s_cnt = 0;
    stereo_status = 0;
    rssi_value = 0;
    station_received = 0;
    seek_state = SEEK_IDLE;
    seek_timer_cnt = 0;
}

void RadioDisplay_TimerTick(void)
{
    timer_1s_cnt++;
}

static void Display_Stereo(void)
{
    if (station_received && stereo_status) {
        LED_STEREO = 0;
    } else {
        LED_STEREO = 1;
    }
}

static void ReadStatus_1s(void)
{
    if (timer_1s_cnt >= 100) {
        timer_1s_cnt = 0;

        rssi_value = RadioCtrl_GetLevel();

        if (rssi_value >= RadioParam.fm_loc_sstop) {
            station_received = 1;
        } else {
            station_received = 0;
        }

        if (station_received) {
            stereo_status = TEF6686_GetStereoStatus();
        }

        Display_Stereo();
    }
}

static void SeekProcess(void)
{
    uint8 status;

    if (seek_state != SEEK_IN_PROGRESS) {
        return;
    }

    seek_timer_cnt++;

    if (seek_timer_cnt >= 50) {
        seek_timer_cnt = 0;

        status = TEF6686_SeekStatus();

        if (status == 1) {
            seek_state = SEEK_IDLE;
            rssi_value = RadioCtrl_GetLevel();

            if (rssi_value >= RadioParam.fm_loc_sstop) {
                station_received = 1;
                stereo_status = TEF6686_GetStereoStatus();
            }

            Display_Stereo();

            printf(">>> Seek Complete! Freq: %d.%d MHz, RSSI: %d dBuV, Stereo: %s\n",
                   RadioCtrl_GetFreq() / 10,
                   RadioCtrl_GetFreq() % 10,
                   rssi_value,
                   station_received ? (stereo_status ? "Yes" : "No") : "No");
        } else if (status == 2) {
            seek_state = SEEK_IDLE;
            station_received = 0;
            stereo_status = 0;
            Display_Stereo();

            printf(">>> Seek Failed: No station found\n");
        }
    }
}

void RadioDisplay_Process(void)
{
    ReadStatus_1s();
    SeekProcess();
}

void RadioDisplay_SeekStart(uint8 direction)
{
    if (seek_state == SEEK_IDLE) {
        if (direction) {
            RadioCtrl_SeekUp();
        } else {
            RadioCtrl_SeekDown();
        }
        seek_state = SEEK_IN_PROGRESS;
        seek_timer_cnt = 0;
        station_received = 0;
        stereo_status = 0;
        printf("Seek %s...\n", direction ? "UP" : "DOWN");
    }
}

uint8 RadioDisplay_IsSeeking(void)
{
    return (seek_state == SEEK_IN_PROGRESS);
}

uint8 RadioDisplay_GetStereoStatus(void)
{
    return stereo_status;
}

uint8 RadioDisplay_GetRssiValue(void)
{
    return rssi_value;
}

uint8 RadioDisplay_IsStationReceived(void)
{
    return station_received;
}

void RadioDisplay_PrintStatus(void)
{
    uint16 freq;

    freq = RadioCtrl_GetFreq();

    printf("Freq: %d.%d MHz  RSSI: %d dBuV  Stereo: %s\n",
           freq / 10, freq % 10,
           rssi_value,
           station_received ?
               (stereo_status ? "Yes" : "No") : "No Station");
}