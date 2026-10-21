#ifndef KEY_CONFIG_H
#define KEY_CONFIG_H

#include "key.h"
#include "radio_ctrl.h"

#define KEY_FREQ_UP       0
#define KEY_FREQ_DOWN     1
#define KEY_SEEK_UP       2
#define KEY_SEEK_DOWN     3
#define KEY_MUTE          4
#define KEY_BAND_SWITCH   5
#define KEY_VOLUME_UP     6
#define KEY_VOLUME_DOWN   7

void Key_Config_Init(void);
void Key_Config_Scan(void);

#endif