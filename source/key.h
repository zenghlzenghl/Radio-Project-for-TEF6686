#ifndef KEY_H
#define KEY_H

#include "stc8h.h"

#define KEY_NUM          8

#define KEY_DEBOUNCE_TIME    20   // 消抖时间 20ms
#define KEY_LONG_TIME        1000 // 长按判定时间 1000ms
#define KEY_REPEAT_TIME      200  // 连续按时间间隔 200ms
#define KEY_HOLD_TIME        500  // 按住事件触发时间 500ms

typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT,
    KEY_EVENT_LONG,
    KEY_EVENT_HOLD,
    KEY_EVENT_RELEASE
} KeyEvent_e;

typedef void (*key_callback_t)(uint8 key_num, KeyEvent_e event);

typedef struct {
    uint8  pin_state;
    uint8  last_state;
    uint8  stable_state;
    KeyEvent_e event;

    uint16 debounce_cnt;
    uint16 press_time;
    uint16 hold_cnt;
    uint16 repeat_cnt;

    uint8  pressed_flag;
    uint8  long_pressed_flag;
    uint8  hold_event_flag;

    key_callback_t callback;
} key_t;

void Key_Init(void);
void Key_RegisterCallback(uint8 key_num, key_callback_t callback);
void Key_Scan(void);
void Key_SetPinState(uint8 key_num, uint8 state);
KeyEvent_e Key_GetEvent(uint8 key_num);
uint8 Key_IsPressed(uint8 key_num);
void Key_ClearEvent(uint8 key_num);

#endif