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

sbit key1 = P3^0;     //��������������LCDEN�����������޷�ʹ��
sbit key2 = P3^1;
sbit key3 = P3^2;
sbit key4 = P3^3;

static UCHAR getkey(void);
UCHAR keyscan(void)
{
    static UCHAR KeyState     = KEY_STATE_INIT;    //ע����Щ����static����
    static UINT KeyTimeCount = 0;
    static UCHAR LastKey      = RUNNING;
    static UCHAR KeyTemp      = RUNNING;

    KeyTemp = getkey();                       //KeyTemp���ڱ���˲ʱ�ļ�ֵ

    switch (KeyState)
        {
        case KEY_STATE_INIT:
            if (KeyTemp != RUNNING)
                KeyState = KEY_STATE_WOBBLE;    //������
            return RUNNING;

        case KEY_STATE_WOBBLE:
            KeyState     = KEY_STATE_PRESS;
            return RUNNING;

        case KEY_STATE_PRESS:
            if (KeyTemp != RUNNING)           //��Ϊ�Ƕ��������Ѱ���
                {
                    LastKey  = KeyTemp;
                    KeyState = KEY_STATE_LONG;
                    return RUNNING;
                }
            else
                KeyState = KEY_STATE_INIT;        //��Ϊ��������
            return RUNNING;

        case KEY_STATE_LONG:
            if (KeyTemp != RUNNING)
                {
                    if (++KeyTimeCount > KEY_LONG_PERIOD)      //�ж��Ƿ�����������
                        {
                            KeyState     = KEY_STATE_CONTINUE;
                            KeyTimeCount = 0;
                        }
                    return RUNNING;
                }
            else
                {
                    KeyTimeCount = 0;                     //��ֹ��ζ̰����KeyTimeCount�ۻ�
                    KeyState = KEY_STATE_RELEASE;         //�ǳ����������ͷ�
                    return RUNNING;
                }
            
        case KEY_STATE_CONTINUE:
            if (KeyTemp != RUNNING)             //��������ʱ�ж��Ƿ���key4������
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
            return LastKey;                //���ػ�õļ�ֵ

        default:
            return RUNNING;
        }
                    
}

/*ע��˴�δ���ǵ����ͬʱ���µ����*/
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
        
