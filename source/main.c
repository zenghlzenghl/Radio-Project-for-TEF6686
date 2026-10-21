#include "stc8h.h"
#include "tef6686.h"

uint8 key_scan(void)
{
    if (P3_0 == 0) return 1;
    if (P3_1 == 0) return 2;
    if (P3_2 == 0) return 3;
    if (P3_3 == 0) return 4;
    if (P3_4 == 0) return 5;
    if (P3_5 == 0) return 6;
    if (P3_6 == 0) return 7;
    if (P3_7 == 0) return 8;
    return 0;
}

void delay_ms(uint16 ms)
{
    uint16 i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 500; j++) {
        }
    }
}

void main(void)
{
    uint16 freq = 8750;
    uint8 key, level;
    
    TEF6686_Init();
    TEF6686_SetVolume(70);
    
    while (1) {
        key = key_scan();
        
        switch (key) {
            case 1:
                if (freq >= 10800) freq = 8750;
                else freq += 10;
                TEF6686_TuneToFreq(freq);
                break;
                
            case 2:
                if (freq <= 8750) freq = 10800;
                else freq -= 10;
                TEF6686_TuneToFreq(freq);
                break;
                
            case 3:
                TEF6686_SeekStart(1);
                break;
                
            case 4:
                TEF6686_SeekStart(0);
                break;
                
            case 5:
                TEF6686_Mute(1);
                break;
                
            case 6:
                TEF6686_Mute(0);
                break;
                
            case 7:
                level = TEF6686_GetLevel();
                break;
                
            case 8:
                freq = TEF6686_GetFrequency();
                break;
        }
        
        delay_ms(50);
    }
}