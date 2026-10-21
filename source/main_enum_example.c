#include "stc8h.h"
#include "key_config.h"
#include "radio_ctrl.h"
#include "radio_display.h"

sbit LED_FM     = P1^2;
sbit LED_AM     = P1^3;
sbit LED_MUTE   = P1^4;

static uint16 timer_cnt = 0;

void Timer0_Init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = (65536 - 10000) / 256;
    TL0 = (65536 - 10000) % 256;
    ET0 = 1;
    TR0 = 1;
    EA = 1;
}

void Timer0_ISR(void) interrupt 1
{
    TH0 = (65536 - 10000) / 256;
    TL0 = (65536 - 10000) % 256;

    timer_cnt++;

    RadioDisplay_TimerTick();

    if (timer_cnt >= 2) {
        timer_cnt = 0;
        Key_Config_Scan();
    }
}

void HandleKeyEvent(uint8 key_num, KeyEvent_e event)
{
    switch (key_num) {
        case KEY_FREQ_UP:
            switch (event) {
                case KEY_EVENT_SHORT:
                    printf("KEY_FREQ_UP: Short press\n");
                    RadioCtrl_FreqUp();
                    break;
                case KEY_EVENT_LONG:
                    printf("KEY_FREQ_UP: Long press\n");
                    break;
                case KEY_EVENT_HOLD:
                    printf("KEY_FREQ_UP: Holding...\n");
                    RadioCtrl_FreqUp();
                    break;
                case KEY_EVENT_RELEASE:
                    printf("KEY_FREQ_UP: Released\n");
                    break;
                default:
                    break;
            }
            break;

        case KEY_FREQ_DOWN:
            switch (event) {
                case KEY_EVENT_SHORT:
                case KEY_EVENT_HOLD:
                    RadioCtrl_FreqDown();
                    break;
                default:
                    break;
            }
            break;

        case KEY_SEEK_UP:
            if (event == KEY_EVENT_SHORT) {
                if (!RadioDisplay_IsSeeking()) {
                    RadioDisplay_SeekStart(1);
                }
            }
            break;

        case KEY_SEEK_DOWN:
            if (event == KEY_EVENT_SHORT) {
                if (!RadioDisplay_IsSeeking()) {
                    RadioDisplay_SeekStart(0);
                }
            }
            break;

        case KEY_MUTE:
            if (event == KEY_EVENT_SHORT) {
                if (RadioParam.mute_status) {
                    RadioCtrl_MuteOff();
                    LED_MUTE = 1;
                } else {
                    RadioCtrl_MuteOn();
                    LED_MUTE = 0;
                }
            }
            break;

        case KEY_BAND_SWITCH:
            if (event == KEY_EVENT_SHORT) {
                if (RadioCtrl_GetBand() == BAND_FM) {
                    RadioCtrl_SetBand(BAND_AM);
                    LED_FM = 1;
                    LED_AM = 0;
                } else {
                    RadioCtrl_SetBand(BAND_FM);
                    LED_FM = 0;
                    LED_AM = 1;
                }
            }
            break;

        default:
            break;
    }
}

void PollKeys(void)
{
    uint8 i;

    for (i = 0; i < KEY_NUM; i++) {
        KeyEvent_e event = Key_GetEvent(i);

        if (event != KEY_EVENT_NONE) {
            HandleKeyEvent(i, event);
            Key_ClearEvent(i);
        }
    }
}

void main(void)
{
    LED_FM = 1;
    LED_AM = 1;
    LED_MUTE = 1;

    P3M0 = 0x00;
    P3M1 = 0xFF;

    Timer0_Init();
    Key_Config_Init();
    RadioCtrl_Init();
    RadioDisplay_Init();

    while (1) {
        PollKeys();

        RadioDisplay_Process();

        {
            static uint16 cnt = 0;

            cnt++;
            if (cnt >= 10000) {
                cnt = 0;
                RadioDisplay_PrintStatus();
            }
        }
    }
}