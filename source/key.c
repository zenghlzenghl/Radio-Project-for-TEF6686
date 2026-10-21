#include "key.h"

static key_t key_list[KEY_NUM];

void Key_Init(void)
{
    uint8 i;
    
    for (i = 0; i < KEY_NUM; i++) {
        key_list[i].pin_state = 1;
        key_list[i].last_state = 1;
        key_list[i].stable_state = 1;
        key_list[i].event = KEY_EVENT_NONE;
        
        key_list[i].debounce_cnt = 0;
        key_list[i].press_time = 0;
        key_list[i].hold_cnt = 0;
        key_list[i].repeat_cnt = 0;
        
        key_list[i].pressed_flag = 0;
        key_list[i].long_pressed_flag = 0;
        key_list[i].hold_event_flag = 0;
        
        key_list[i].callback = 0;
    }
}

void Key_RegisterCallback(uint8 key_num, key_callback_t callback)
{
    if (key_num < KEY_NUM) {
        key_list[key_num].callback = callback;
    }
}

void Key_SetPinState(uint8 key_num, uint8 state)
{
    if (key_num < KEY_NUM) {
        key_list[key_num].pin_state = state;
    }
}

KeyEvent_e Key_GetEvent(uint8 key_num)
{
    if (key_num < KEY_NUM) {
        return key_list[key_num].event;
    }
    return KEY_EVENT_NONE;
}

uint8 Key_IsPressed(uint8 key_num)
{
    if (key_num < KEY_NUM) {
        return key_list[key_num].pressed_flag;
    }
    return 0;
}

void Key_ClearEvent(uint8 key_num)
{
    if (key_num < KEY_NUM) {
        key_list[key_num].event = KEY_EVENT_NONE;
    }
}

void Key_Scan(void)
{
    uint8 i;
    
    for (i = 0; i < KEY_NUM; i++) {
        key_t *k = &key_list[i];
        
        k->event = KEY_EVENT_NONE;
        
        if (k->pin_state != k->last_state) {
            k->debounce_cnt = 0;
            k->last_state = k->pin_state;
        } else {
            if (k->debounce_cnt < KEY_DEBOUNCE_TIME) {
                k->debounce_cnt++;
            } else {
                if (k->pin_state != k->stable_state) {
                    k->stable_state = k->pin_state;
                    
                    if (k->stable_state == 0) {
                        k->pressed_flag = 1;
                        k->long_pressed_flag = 0;
                        k->hold_event_flag = 0;
                        k->press_time = 0;
                        k->hold_cnt = 0;
                        k->repeat_cnt = 0;
                    } else {
                        if (k->pressed_flag && !k->long_pressed_flag) {
                            k->event = KEY_EVENT_SHORT;
                            if (k->callback) {
                                k->callback(i, KEY_EVENT_SHORT);
                            }
                        } else {
                            k->event = KEY_EVENT_RELEASE;
                            if (k->callback) {
                                k->callback(i, KEY_EVENT_RELEASE);
                            }
                        }
                        k->pressed_flag = 0;
                    }
                }
            }
        }
        
        if (k->stable_state == 0 && k->pressed_flag) {
            k->press_time++;
            
            if (!k->long_pressed_flag && k->press_time >= KEY_LONG_TIME) {
                k->long_pressed_flag = 1;
                k->event = KEY_EVENT_LONG;
                if (k->callback) {
                    k->callback(i, KEY_EVENT_LONG);
                }
            }
            
            if (k->long_pressed_flag) {
                k->hold_cnt++;
                
                if (k->hold_cnt >= KEY_HOLD_TIME) {
                    k->hold_cnt = 0;
                    k->hold_event_flag = 1;
                    k->event = KEY_EVENT_HOLD;
                    if (k->callback) {
                        k->callback(i, KEY_EVENT_HOLD);
                    }
                }
                
                k->repeat_cnt++;
                if (k->repeat_cnt >= KEY_REPEAT_TIME) {
                    k->repeat_cnt = 0;
                    k->event = KEY_EVENT_SHORT;
                    if (k->callback) {
                        k->callback(i, KEY_EVENT_SHORT);
                    }
                }
            }
        }
    }
}