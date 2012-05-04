/*Using the lcd to display time*/
#include <reg52.h>
#include "myclock.h"
#include "mykeyboard.h"         //由于要用到PLUS及MINUS两个宏

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

void lcd_init(void)   //初始化在clock.c的init函数中执行
{
    DULA  = 0;         //关闭WELA及DULA控制的锁存器，否则数码管会因P0电平变化闪烁
    WELA  = 0;
    LCDEN = 0;         //液晶使能
    write_com(0x38);    //设置16x2显示,5x7点阵,8位数据接口
    write_com(0x0c);    //设置开显示，不开光标
    write_com(0x06);    //写一个字符后地址指针加1
    write_com(0x01);    //显示清0，数据指针清0
}

void lcd_disp(UCHAR time[], UCHAR curpos, bit runp)
{
    UCHAR i;

    write_com(0x0c);
    write_com(0x80+0x01);        
    for (i=0; i<9; i++)          //第一行写日期,位置从0x01~0x0e
        {
            if (i==4 || i==6)
                    write_data('-');
            if (i==8)
                {
                    write_com(0x14);   //光标右移一格，显示星期
                    write_str(WEEK[time[i]-49]);    //注意time[i]要减去'1'的ascii码
                    continue;
                }
            write_data(time[i]);
            delayms(1);
        }

    write_com(0x80+0x44);
    for (; i<T_LENGTH; i++)     //第二行写时间，位置从0x44~0x4b
        {
            if (i==11 || i==13 )
                write_data(':');
            write_data(time[i]);
            delayms(1);
        }
    if (runp == 0)       //若处于暂停状态则显示光标
        {
            write_com(LCDPOS[curpos]);
            write_com(0x0f);
        }
}              

void changetime(UCHAR time[], UCHAR pos, UCHAR act)  //移动光标并执行相应动作
{
    switch (act)
        {
        case PLUS:
        case MINUS:
            switch (pos)
                {
                    /*年*/
                case 0:
                case 1:
                case 2:
                case 3:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;
                    
                    /*星期*/
                case 8:
                    time[pos]     = timerange(time[pos], act, 1, 7);
                    break;

                    /*月*/
                case 4:
                    time[pos] = timerange(time[pos], act, 0, 1);
                    break;
                case 5:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;

                    /*日*/
                case 6:
                    time[pos] = timerange(time[pos], act, 0, 3);
                    break;
                case 7:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;

                    /*小时*/
                case 9:
                    time[pos] = timerange(time[pos], act, 0, 2);
                    break;
                case 10:
                    time[pos] = timerange(time[pos], act, 0, 9);
                    break;

                    /*分、秒*/
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

static void write_com(UCHAR com)  //写指令
{
    LCDRS = 0;
    P0    = com;
    delayms(3);
    LCDEN = 1;
    delayms(3);
    LCDEN = 0;
}

static void write_data(UCHAR date)   //写数据
{
    LCDRS = 1;
    P0    = date;
    delayms(3);
    LCDEN = 1;
    delayms(3);
    LCDEN = 0;
}

static void write_str(UCHAR *str)    //写入长度不超过16的字符串
{
    for (; *str!=0; str++)
        {
            write_data(*str);
            delayms(3);
        }
}

static UCHAR timerange(UCHAR time, UCHAR act, UCHAR l_bound, UCHAR u_bound) //在l_bound~u_bound的范围内改变时间
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
