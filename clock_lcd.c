/*Using the lcd to display time*/
#include <reg52.h>
#include "myclock.h"
#include "mykeyboard.h"         //����Ҫ�õ�PLUS��MINUS������

sbit        LCDEN = P3^4;
sbit        LCDRS = P3^5;
UCHAR code *WEEK[] = 
    {
        "Mon", "Tue", "Wed", "Thu",
        "Fri", "Sat", "Sun"
    };
UCHAR code LCDPOS[] = 
    {
        0x81, 0x82, 0x83, 0x84, 0x86, 0x87, 0x89, 0x8a,
        0x8c, 0xc4, 0xc5, 0xc7, 0xc8, 0xca, 0xcb
    };

extern void  delayms(UINT xms);
static void  write_com(UCHAR com);
static void  write_data(UCHAR date);
static void  write_str(UCHAR *str);
static UCHAR timerange(UCHAR time, UCHAR act, UCHAR l_bound, UCHAR u_bound);

void lcd_init(void)   //��ʼ����clock.c��init������ִ��
{
    DULA  = 0;         //�ر�WELA��DULA���Ƶ�����������������ܻ���P0��ƽ�仯��˸
    WELA  = 0;
    LCDEN = 0;         //Һ��ʹ��
    write_com(0x38);    //����16x2��ʾ,5x7����,8λ���ݽӿ�
    write_com(0x0c);    //���ÿ���ʾ���������
    write_com(0x06);    //дһ���ַ����ַָ���1
    write_com(0x01);    //��ʾ��0������ָ����0
}

void lcd_disp(UCHAR time[], UCHAR curpos, bit runp)
{
    UCHAR i;

    write_com(0x0c);
    write_com(0x80+0x01);        
    for (i=0; i<9; i++)          //��һ��д����,λ�ô�0x01~0x0e
        {
            if (i==4 || i==6)
                    write_data('-');
            if (i==8)
                {
                    write_com(0x14);   //�������һ����ʾ����
                    write_str(WEEK[time[i]-49]);    //ע��time[i]Ҫ��ȥ'1'��ascii��
                    continue;
                }
            write_data(time[i]);
            delayms(1);
        }

    write_com(0x80+0x44);
    for (; i<T_LENGTH; i++)     //�ڶ���дʱ�䣬λ�ô�0x44~0x4b
        {
            if (i==11 || i==13 )
                write_data(':');
            write_data(time[i]);
            delayms(1);
        }
    if (runp == 0)       //��������ͣ״̬����ʾ���
        {
            write_com(LCDPOS[curpos]);
            write_com(0x0f);
        }
}              

void changetime(UCHAR time[], UCHAR pos, UCHAR act)  //�ƶ���겢ִ����Ӧ����
{
    switch (act)
        {
        case PLUS:
        case MINUS:
            switch (pos)
                {
                    /*��*/
                case 0:
                case 1:
                case 2:
                case 3:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;
                    
                    /*����*/
                case 8:
                    time[pos]     = timerange(time[pos], act, 1, 7);
                    break;

                    /*��*/
                case 4:
                    time[pos] = timerange(time[pos], act, 0, 1);
                    break;
                case 5:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;

                    /*��*/
                case 6:
                    time[pos] = timerange(time[pos], act, 0, 3);
                    break;
                case 7:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;

                    /*Сʱ*/
                case 9:
                    time[pos] = timerange(time[pos], act, 0, 2);
                    break;
                case 10:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;

                    /*�֡���*/
                case 11:
                case 13:
                    time[pos] = timerange(time[pos], act, 0, 5);
                    break;
                case 12:
                case 14:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;
                default:
                    time[0]   = 'E';
                    time[1]   = 'r';
                    time[2]   = 'r';
                    break;
                }
            break;
            
        default:
            break;
        }
}

static void write_com(UCHAR com)  //дָ��
{
    LCDRS = 0;
    P0    = com;
    delayms(3);
    LCDEN = 1;
    delayms(3);
    LCDEN = 0;
}

static void write_data(UCHAR date)   //д����
{
    LCDRS = 1;
    P0    = date;
    delayms(3);
    LCDEN = 1;
    delayms(3);
    LCDEN = 0;
}

static void write_str(UCHAR *str)    //д�볤�Ȳ�����16���ַ���
{
    for (; *str!=0; str++)
        {
            write_data(*str);
            delayms(3);
        }
}

static UCHAR timerange(UCHAR time, UCHAR act, UCHAR l_bound, UCHAR u_bound) //��l_bound~u_bound�ķ�Χ�ڸı�ʱ��
{
    if (act == MINUS)
        {
            if (time-'0' <= l_bound)
                return ('0'+u_bound);
            else 
                return (time-1);
        }
    else if (act == PLUS)
        {
            if (time-'0' >= u_bound)
                return ('0'+l_bound);
            else
                return (time+1);
        }
    else
        return 'E';
}
