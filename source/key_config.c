#include "key_config.h"
#include "tef6686.h"

static uint8 current_volume = 70;

void Key_FreqUp_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
        case KEY_EVENT_HOLD:
            RadioCtrl_FreqUp();
            break;
        default:
            break;
    }
}

void Key_FreqDown_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
        case KEY_EVENT_HOLD:
            RadioCtrl_FreqDown();
            break;
        default:
            break;
    }
}

void Key_SeekUp_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
            RadioCtrl_SeekUp();
            break;
        default:
            break;
    }
}

void Key_SeekDown_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
            RadioCtrl_SeekDown();
            break;
        default:
            break;
    }
}

void Key_Mute_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
            if (RadioParam.mute_status) {
                RadioCtrl_MuteOff();
            } else {
                RadioCtrl_MuteOn();
            }
            break;
        default:
            break;
    }
}

void Key_BandSwitch_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
            if (RadioCtrl_GetBand() == BAND_FM) {
                RadioCtrl_SetBand(BAND_AM);
            } else {
                RadioCtrl_SetBand(BAND_FM);
            }
            break;
        default:
            break;
    }
}

void Key_VolumeUp_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
        case KEY_EVENT_HOLD:
            if (current_volume < 250) {
                current_volume += 5;
                RadioCtrl_SetVolume(current_volume);
            }
            break;
        default:
            break;
    }
}

void Key_VolumeDown_Callback(uint8 key_num, KeyEvent_e event)
{
    key_num = key_num;
    switch (event) {
        case KEY_EVENT_SHORT:
        case KEY_EVENT_HOLD:
            if (current_volume > 5) {
                current_volume -= 5;
                RadioCtrl_SetVolume(current_volume);
            }
            break;
        default:
            break;
    }
}

void Key_Config_Init(void)
{
    Key_Init();

    Key_RegisterCallback(KEY_FREQ_UP,     Key_FreqUp_Callback);
    Key_RegisterCallback(KEY_FREQ_DOWN,   Key_FreqDown_Callback);
    Key_RegisterCallback(KEY_SEEK_UP,     Key_SeekUp_Callback);
    Key_RegisterCallback(KEY_SEEK_DOWN,   Key_SeekDown_Callback);
    Key_RegisterCallback(KEY_MUTE,        Key_Mute_Callback);
    Key_RegisterCallback(KEY_BAND_SWITCH, Key_BandSwitch_Callback);
    Key_RegisterCallback(KEY_VOLUME_UP,   Key_VolumeUp_Callback);
    Key_RegisterCallback(KEY_VOLUME_DOWN, Key_VolumeDown_Callback);
}

void Key_Config_Scan(void)
{
    Key_SetPinState(KEY_FREQ_UP,     P3_0);
    Key_SetPinState(KEY_FREQ_DOWN,   P3_1);
    Key_SetPinState(KEY_SEEK_UP,     P3_2);
    Key_SetPinState(KEY_SEEK_DOWN,   P3_3);
    Key_SetPinState(KEY_MUTE,        P3_4);
    Key_SetPinState(KEY_BAND_SWITCH, P3_5);
    Key_SetPinState(KEY_VOLUME_UP,   P3_6);
    Key_SetPinState(KEY_VOLUME_DOWN, P3_7);

    Key_Scan();
}