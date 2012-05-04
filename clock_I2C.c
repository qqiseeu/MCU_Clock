/*Using the AT24C02 EEPROM to provide a power-fail protection*/
#include <reg52.h>
#include "myclock.h"

sbit SDA = P2^0;
sbit SCL = P2^1;

extern void  delayms(UINT xms);
static void  delay(void);
static void  start(void);
static void  stop(void);
static UCHAR  respons(void);
static void  write_byte(UCHAR date);
static UCHAR read_byte(void);
static void  write_add(UCHAR address, UCHAR date);
static UCHAR read_add(UCHAR address);

UCHAR code TIMEINIT[] = "201006196000000";

void I2C_init(void)
{
    SDA = 1;
    delay();
    SCL = 1;
    delay();
}

void I2C_readtime(UCHAR time[])    //将EEPROM里的数据读入time数组
{
    UCHAR written, i;
    if ((written=read_add(1)) != 1)
        {
            write_add(1, 1);
            for (i=0; i<T_LENGTH; i++)
                {
                    time[i] = TIMEINIT[i];
                    write_add(i+2, time[i]);
                    delayms(5);
                }
        }
    else
        for (i=0; i<T_LENGTH; i++)
            {
                time[i] = read_add(i+2);
            }
}

void I2C_writetime(UCHAR time[])   //将time数组中的数据写入EEPROM
{
    UCHAR i;

    for (i=0; i<T_LENGTH; i++)
        {
            write_add(i+2, time[i]);
            delayms(5);
            /*每完成一次数据输入后AT24C02会进入一个“内部写周期”，*/
            /*此时其会拒绝一切输入，故需要一定延时来跳过该周期*/
        }
}

static void delay(void)
{
    ;;
}

static void start(void)
{
    SDA = 1;
    delay();
    SCL = 1;
    delay();
    SDA = 0;
    delay();
}

static void stop(void)
{
    SDA = 0;
    delay();
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}

static UCHAR respons(void)
{
    UCHAR i = 0;
    SCL     = 1;
    delay();
    while (SDA==1 && i<250)
        i++;
    if (i == 250)
        {
            stop();
            return 0;
        }
    SCL = 0;
    delay();
    return 1;
}

static void write_byte(UCHAR date)
{
    UCHAR i, temp;
    temp = date;
    for (i=0; i<8; i++)
        {
            temp = temp << 1;
            SCL  = 0;
            delay();
            SDA  = CY;
            delay();
            SCL  = 1;
            delay();
        }

    SCL = 0;   //与最后一次循环中的“SCL=1”语句合起来形成一次正脉冲，完成最后一位的传输
    delay();
    SDA = 1;   //释放SDA线
    delay();
}
        
UCHAR read_byte(void)
{
    UCHAR i, k;
    SCL = 0;                    //传输数据时SCL线为低SDA方可变化
    delay();
    SDA = 1;                    //使SDA线为空闲状态
    delay();
    for (i=0; i<8; i++)
        {
            SCL = 1;
            delay();
            k   = (k<<1) | SDA;
            SCL = 0;
            delay();
        }

    return k;
}

void write_add(UCHAR address, UCHAR date)
{
    start();
    write_byte(0xa0);
    if (!respons())
        return;
    write_byte(address);
    if (!respons())
            return;
    write_byte(date);
    if (!respons())
            return;
    stop();
}

UCHAR read_add(UCHAR address)
{
    UCHAR date;
    start();
    write_byte(0xa0);
    if (!respons())
            return;
    write_byte(address);
    if (!respons())
        return;
    start();                    //注意此处要再发送一次启动信号
    write_byte(0xa1);
    if (!respons())
        return;
    date = read_byte();
    stop();
    return date;
}
