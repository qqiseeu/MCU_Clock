/*Using keyboard to adjust time*/
#include <reg52.h>
#include "myclock.h"

#define PAU_STA 0x0f
#define PLUS    0x0e
#define MINUS   0x0d
#define MOVE    0x0c
#define SERIAL  0x0b
#define RUNNING 0x0a

#define KEY_STATE_INIT   0
#define KEY_STATE_WOBBLE 1
#define KEY_STATE_PRESS  2
#define KEY_STATE_LONG   3
#define KEY_STATE_CONTINUE 4
#define KEY_STATE_RELEASE 5

#define KEY_LONG_PERIOD 0x6fff

sbit key1 = P3^0;     //独立键盘由于与LCDEN引脚相连，无法使用
sbit key2 = P3^1;
sbit key3 = P3^2;
sbit key4 = P3^3;

static UCHAR getkey(void);
UCHAR keyscan(void)
{
    static UCHAR KeyState     = KEY_STATE_INIT;    //注意这些都是static变量
    static UINT KeyTimeCount = 0;
    static UCHAR LastKey      = RUNNING;
    static UCHAR KeyTemp      = RUNNING;

    KeyTemp = getkey();                       //KeyTemp用于保存瞬时的键值

    switch (KeyState)
        {
        case KEY_STATE_INIT:
            if (KeyTemp != RUNNING)
                KeyState = KEY_STATE_WOBBLE;    //消抖用
            return RUNNING;

        case KEY_STATE_WOBBLE:
            KeyState     = KEY_STATE_PRESS;
            return RUNNING;

        case KEY_STATE_PRESS:
            if (KeyTemp != RUNNING)           //认为非抖动，键已按下
                {
                    LastKey  = KeyTemp;
                    KeyState = KEY_STATE_LONG;
                    return RUNNING;
                }
            else
                KeyState = KEY_STATE_INIT;        //认为发生抖动
            return RUNNING;

        case KEY_STATE_LONG:
            if (KeyTemp != RUNNING)
                {
                    if (++KeyTimeCount > KEY_LONG_PERIOD)      //判断是否发生长按动作
                        {
                            KeyState     = KEY_STATE_CONTINUE;
                            KeyTimeCount = 0;
                        }
                    return RUNNING;
                }
            else
                {
                    KeyTimeCount = 0;                     //防止多次短按造成KeyTimeCount累积
                    KeyState = KEY_STATE_RELEASE;         //非长按，键已释放
                    return RUNNING;
                }
            
        case KEY_STATE_CONTINUE:
            if (KeyTemp != RUNNING)             //发生长按时判断是否是key4被按下
                switch (LastKey)
                    {
                    case PAU_STA:
                    case PLUS:
                    case MINUS:
                        return RUNNING;
                    case MOVE:
                        LastKey = RUNNING;
                        return SERIAL;
                    default:
                        return RUNNING;
                    }
            else
                KeyState = KEY_STATE_RELEASE;
            
            return RUNNING;

        case KEY_STATE_RELEASE:
            KeyState = KEY_STATE_INIT;
            return LastKey;                //返回获得的键值

        default:
            return RUNNING;
        }
                    
}

/*注意此处未考虑到多键同时按下的情况*/
static UCHAR getkey(void)
{
    if (key1 == 0)
        return PAU_STA;
    if (key2 == 0)
        return PLUS;
    if (key3 == 0)
        return MINUS;
    if (key4 == 0)
        return MOVE;
    
    return RUNNING;
}
        
