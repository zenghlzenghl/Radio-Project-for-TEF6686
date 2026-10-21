#include "stc8h.h"
#include "radio_ctrl.h"

sbit KEY_UP    = P3^0;
sbit KEY_DOWN  = P3^1;
sbit KEY_SEEK_UP   = P3^2;
sbit KEY_SEEK_DOWN = P3^3;
sbit KEY_MUTE  = P3^4;
sbit KEY_BAND  = P3^5;

sbit LED_FM = P1_2;
sbit LED_AM = P1_3;
sbit LED_STEREO = P1_4;

void delay_ms(uint16 ms)
{
    uint16 i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 500; j++);
}

uint8 key_debounce(sbit key)
{
    if (key == 0) {
        delay_ms(10);
        if (key == 0) {
            while (key == 0);
            return 1;
        }
    }
    return 0;
}

void display_band(void)
{
    if (RadioCtrl_GetBand() == BAND_FM) {
        LED_FM = 0;
        LED_AM = 1;
    } else {
        LED_FM = 1;
        LED_AM = 0;
    }
}

void main(void)
{
    RadioCtrl_Init();
    display_band();
    
    while (1) {
        if (key_debounce(KEY_UP)) {
            RadioCtrl_FreqUp();
        }
        
        if (key_debounce(KEY_DOWN)) {
            RadioCtrl_FreqDown();
        }
        
        if (key_debounce(KEY_SEEK_UP)) {
            RadioCtrl_SeekUp();
        }
        
        if (key_debounce(KEY_SEEK_DOWN)) {
            RadioCtrl_SeekDown();
        }
        
        if (key_debounce(KEY_MUTE)) {
            if (RadioParam.mute_status) {
                RadioCtrl_MuteOff();
            } else {
                RadioCtrl_MuteOn();
            }
        }
        
        if (key_debounce(KEY_BAND)) {
            if (RadioCtrl_GetBand() == BAND_FM) {
                RadioCtrl_SetBand(BAND_AM);
            } else {
                RadioCtrl_SetBand(BAND_FM);
            }
            display_band();
        }
        
        delay_ms(50);
    }
}