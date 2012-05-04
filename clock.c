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
bit SerBegin = 0;    //�ж��Ƿ�ʼ��������
bit tickn = 1;      //tickn=1��ʾ����1��ʱ��
bit runflag = 1;      //runflag=1��ʾʱ�����ڼ�ʱ
bit serflag = 0;      //serflag=1��ʾ���ڴ�
UINT num_sec = 0;         //��ʱ��0��ʱ�꣬��num_sec=3686ʱ��ʾ1sʱ�䵽
UINT num_min = 0;        //��ʱ��0��ʱ�꣬��num_min=60ʱ��ʾ1minʱ�䵽
UCHAR pos = 0;

void init(void);
void delayms(UINT xms);


int main(void)
{
    init();
    while (1)
        {
            if (num_min >= 30)          //ÿ��һ������EEPROM��дһ������
                {
                    num_min = 0;
                    I2C_writetime(time);
                }
            
            switch (keyscan())
                {
                case RUNNING:
                    if (tickn == 1) //ÿ��1���Ӹ���һ��LCD����ʾ��ʱ��
                        {
                            tickn = 0;
                            num_min++;
                            increase(time);           //����ʱ��,runflag==0ʱ�þ���౻ִ��һ��

                            lcd_disp(time, pos, runflag);
                        }
                    if (serflag == 1)
                        UART_SyncTime(time);
                    break;
                case PAU_STA:   //������ͣ�����      
                    if (serflag == 0)
                        {
                            if (runflag == 0)
                                if (!IsLegal(time)) //ÿ����ͣ��ʱ����ܱ��޸ģ�����Ϸ���
                                    break;
                            runflag = ~runflag;
                            TR0     = ~TR0;
                            lcd_disp(time, pos, runflag);
                        }
                    break;
                case PLUS:
                    if (runflag == 0 && serflag == 0)
                        {
                            changetime(time, pos, PLUS);    //�����ָ��ʱ���1����λ
                            lcd_disp(time, pos, runflag);
                        }
                    break;
                case MINUS:
                    if (runflag == 0 && serflag == 0)
                        {
                            changetime(time, pos, MINUS);    //�����ָ��ʱ���һ����λ
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
                case SERIAL:    //������������/�رմ���ģʽ
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
    /*lcd��ʾ��ʼ��*/
    I2C_init();
    I2C_readtime(time);
    
    lcd_init();

    RD = 0;       //�������S4��S8��S12��S16��ʼ��
    
    TMOD = 0x22;   //��ʱ��0���ڼ�ʱ����ʱ��1���ڴ���ͨ��

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
