/*The driver program*/
#include <reg52.h>
#include "myclock.h"
#include "mykeyboard.h"
#include "mysercom.h"
#include "myI2C.h"
#include "mylcd.h"
#include "myincrease.h"

UCHAR time[T_LENGTH];
UCHAR Recv;
bit SerBegin = 0;    //判断是否开始接收数据
bit tickn = 1;      //tickn=1表示经过1秒时间
bit runflag = 1;      //runflag=1表示时钟正在计时
bit serflag = 0;      //serflag=1表示串口打开
UINT num_sec = 0;         //定时器0的时标，当num_sec=3686时表示1s时间到
UINT num_min = 0;        //定时器0的时标，当num_min=60时表示1min时间到
UCHAR pos = 0;

void init(void);
void delayms(UINT xms);


int main(void)
{
    init();
    while (1)
        {
            if (num_min >= 30)          //每过一分钟向EEPROM里写一次数据
                {
                    num_min = 0;
                    I2C_writetime(time);
                }
            
            switch (keyscan())
                {
                case RUNNING:
                    if (tickn == 1) //每过1秒钟更新一次LCD上显示的时间
                        {
                            tickn = 0;
                            num_min++;
                            increase(time);           //增加时间,runflag==0时该句最多被执行一次

                            lcd_disp(time, pos, runflag);
                        }
                    if (serflag == 1)
                        UART_SyncTime(time);
                    break;
                case PAU_STA:   //用于暂停或继续      
                    if (serflag == 0)
                        {
                            if (runflag == 0)
                                if (!IsLegal(time)) //每次暂停后时间可能被修改，需检查合法性
                                    break;
                            runflag = ~runflag;
                            TR0     = ~TR0;
                            lcd_disp(time, pos, runflag);
                        }
                    break;
                case PLUS:
                    if (runflag == 0 && serflag == 0)
                        {
                            changetime(time, pos, PLUS);    //光标所指的时间加1个单位
                            lcd_disp(time, pos, runflag);
                        }
                    break;
                case MINUS:
                    if (runflag == 0 && serflag == 0)
                        {
                            changetime(time, pos, MINUS);    //光标所指的时间减一个单位
                            lcd_disp(time, pos, runflag);
                        }
                    break;
                case MOVE:
                    if (runflag == 0 && serflag == 0)
                        {
                            pos++;
                            if (pos >= T_LENGTH)
                                pos = 0;
                            lcd_disp(time, pos, runflag);
                        }
                    break;
                case SERIAL:    //长按键四来打开/关闭串口模式
                    TR0     = serflag; 
                    runflag = serflag;
                    ES      = ~ES;    
                    TR1     = ~TR1; 
                    serflag = ~serflag;
                    DIOLA   = 1;
                    if (serflag == 1)
                        P1 = 0xaa;
                    else
                        P1  = 0xff;
                    DIOLA = 0;
                    break;
                default:
                    break;
                }
        }
}

void init(void)
{
    /*lcd显示初始化*/
    I2C_init();
    I2C_readtime(time);
    
    lcd_init();

    RD = 0;       //矩阵键盘S4、S8、S12、S16初始化
    
    TMOD = 0x22;   //定时器0用于计时，定时器1用于串口通信

    TH0 = 6;
    TL0 = 6;
    
    TH1 = 0xfd;
    TL1 = 0xfd;

    EA = 1;
    ET0 = 1;
    TR0 = 1;
    SCON = 0x50;
}

void delayms(UINT xms)
{
    UINT i, j;
    for (i=0; i<xms; i++)
        for (j=0; j<110; j++)
            ;
}

void T0_time(void) interrupt 1
{
    num_sec++;
    if (num_sec == 3686)
        {
            tickn = 1;
            num_sec   = 0;
        }
}

void ser(void) interrupt 4
{
    RI           = 0;
    Recv         = SBUF;
    if (Recv == '$')
        SerBegin = ~SerBegin;
}
