#include "stc8h.h"
#include "key_config.h"
#include "radio_ctrl.h"

sbit LED_FM     = P1^2;
sbit LED_AM     = P1^3;
sbit LED_MUTE   = P1^4;
sbit LED_STEREO = P1^5;

static uint16 timer_10ms_cnt = 0;
static uint16 timer_100ms_cnt = 0;
static uint16 timer_1s_cnt = 0;

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
    
    timer_10ms_cnt++;
    timer_100ms_cnt++;
    timer_1s_cnt++;
    
    if (timer_10ms_cnt >= 2) {
        timer_10ms_cnt = 0;
        Key_Config_Scan();
    }
}

void Display_Band(void)
{
    if (RadioCtrl_GetBand() == BAND_FM) {
        LED_FM = 0;
        LED_AM = 1;
    } else {
        LED_FM = 1;
        LED_AM = 0;
    }
}

void Display_Mute(void)
{
    if (RadioParam.mute_status) {
        LED_MUTE = 0;
    } else {
        LED_MUTE = 1;
    }
}

void Task_10ms(void)
{
}

void Task_100ms(void)
{
    if (timer_100ms_cnt >= 10) {
        timer_100ms_cnt = 0;
        
        RadioCtrl_GetLevel();
        
        Display_Band();
        Display_Mute();
    }
}

void Task_1s(void)
{
    static uint8 blink_cnt = 0;
    
    if (timer_1s_cnt >= 100) {
        timer_1s_cnt = 0;
        
        blink_cnt++;
        if (blink_cnt >= 2) {
            blink_cnt = 0;
        }
    }
}

void GPIO_Init(void)
{
    LED_FM = 1;
    LED_AM = 1;
    LED_MUTE = 1;
    LED_STEREO = 1;
    
    P3M0 = 0x00;
    P3M1 = 0xFF;
}

void main(void)
{
    GPIO_Init();
    
    Timer0_Init();
    
    Key_Config_Init();
    
    RadioCtrl_Init();
    
    while (1) {
        Task_10ms();
        Task_100ms();
        Task_1s();
    }
}