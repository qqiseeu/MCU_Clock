#include <reg52.h>
#include "myclock.h"

#define _SECOND 0x00
#define _MINUTE 0x01
#define _HOUR   0x02
#define _WEEK   0x03
#define _DAY    0x04
#define _MONTH  0x05
#define _YEAR   0x06

static UINT StrToNum(UCHAR kind, UCHAR time[]);
static bit NumToStr(UCHAR kind, UINT quantity, UCHAR time[]);
static UCHAR IsLeapYear(UINT year);   //判断是否为闰年

UCHAR IsLegal(UCHAR time[])
{
    UINT daynum;
    if (StrToNum(_HOUR, time) >= 24)
        return 0;

    daynum = StrToNum(_DAY, time);
    switch (StrToNum(_MONTH, time))
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if (daynum > 31)
                return 0;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if (daynum > 30)
                return 0;
            break;
        case 2:
            if (IsLeapYear(StrToNum(_YEAR, time)))
                if (daynum > 29)
                    return 0;
            if (daynum > 28)
                return 0;
            break;
        default:
            return 0;
        }
    return 1;
}
void increase(UCHAR time[])
{
    UCHAR i;
    UINT  quantity;
    bit Carry = 0;            //若发生进位，则Carry为1，否则为0
    for (i=0; i<7; i++)   //i=0~6分别代表秒、分、时、星期、日、月、年
        {
            quantity = StrToNum(i, time);
            if (quantity > 9999)
                {
                    time[0] = 'E';
                    time[1] = 'r';
                    time[2] = 'r';
                    break;
                }
            quantity++;
            Carry    = NumToStr(i, quantity, time);
            if (i == 3)      //若是星期数增加了，则日也要增加
                continue;
            if (Carry == 0)
                break;
        }
}

static UINT StrToNum(UCHAR kind, UCHAR time[])
{
    switch (kind)
        {
        case _SECOND:
            return (UINT)((time[13]-'0')*10 + time[14]-'0');
        case _MINUTE:
            return (UINT)((time[11]-'0')*10 + time[12]-'0');
        case _HOUR:
            return (UINT)((time[9]-'0')*10 + time[10]-'0');
        case _WEEK:
            return (UINT)(time[8]-'0');
        case _DAY:
            return (UINT)((time[6]-'0')*10 + time[7]-'0');
        case _MONTH:
            return (UINT)((time[4]-'0')*10 + time[5]-'0');
        case _YEAR:
            return (UINT)((time[0]-'0')*1000 + (time[1]-'0')*100 +
                          (time[2]-'0')*10 + time[3]-'0');
        default:
            return 0xffff;  //Error message
        }
}

static bit NumToStr(UCHAR kind, UINT quantity, UCHAR time[])
{
    switch (kind)
        {
        case _SECOND:
            if (quantity == 60)
                {
                    time[14] = '0';
                    time[13] = '0';
                    return 1;
                }
            else
                {
                    time[14] = ((UCHAR)(quantity%10) + '0');
                    time[13] = ((UCHAR)(quantity/10) + '0');
                    return 0;
                }
        case _MINUTE:
            if (quantity == 60)
                {
                    time[12] = '0';
                    time[11] = '0';
                    return 1;
                }
            else
                {
                    time[12] = ((UCHAR)(quantity%10) + '0');
                    time[11] = ((UCHAR)(quantity/10) + '0');
                    return 0;
                }
        case _HOUR:
            if (quantity == 24)
                {
                    time[10] = '0';
                    time[9]  = '0';
                    return 1;
                }
            else
                {
                    time[10] = ((UCHAR)(quantity%10) + '0');
                    time[19] = ((UCHAR)(quantity/10) + '0');
                    return 0;
                }
        case _WEEK:
            if (quantity == 8)
                {
                    time[8] = '1';
                    return 1;
                }
            else
                {
                    time[8] = ((UCHAR)(quantity) + '0');
                    return 0;
                }
        case _DAY:
            switch (StrToNum(_MONTH, time))
                {
                case 1:
                case 3:
                case 5:
                case 7:
                case 8:
                case 10:
                case 12:
                    if (quantity == 32)
                        {
                            time[7] = '0';
                            time[6] = '0';
                            return 1;
                        }
                    else
                        {
                            time[7] = ((UCHAR)(quantity%10) + '0');
                            time[6] = ((UCHAR)(quantity/10) + '0');
                            return 0;
                        }
                case 4:
                case 6:
                case 9:
                case 11:
                    if (quantity == 31)
                        {
                            time[7] = '0';
                            time[6] = '0';
                            return 1;
                        }
                    else
                        {
                            time[7] = ((UCHAR)(quantity%10) + '0');
                            time[6] = ((UCHAR)(quantity/10) + '0');
                            return 0;
                        }
                case 2:        //2月
                    if (IsLeapYear(StrToNum(_YEAR, time)))
                        if (quantity == 30)
                        {
                            time[7] = '0';
                            time[6] = '0';
                            return 1;
                        }
                        else
                        {
                            time[7] = ((UCHAR)(quantity%10) + '0');
                            time[6] = ((UCHAR)(quantity/10) + '0');
                            return 0;
                        }
                    else
                        if (quantity == 29)
                        {
                            time[7] = '0';
                            time[6] = '0';
                            return 1;
                        }
                    else
                        {
                            time[7] = ((UCHAR)(quantity%10) + '0');
                            time[6] = ((UCHAR)(quantity/10) + '0');
                            return 0;
                        }
                default:
                    time[0] = 'E';
                    time[1] = 'r';
                    time[2] = 'r';
                    break;
                }
        case _MONTH:
            if (quantity == 13)
                {
                    time[5] = '1';
                    time[4] = '0';
                    return 1;
                }
            else
                {
                    time[5] = ((UCHAR)(quantity%10) + '0');
                    time[4] = ((UCHAR)(quantity/10) + '0');
                    return 0;
                }
        case _YEAR:
            if (quantity == 10000)
                {
                    time[0] = '0';
                    time[1] = '0';
                    time[2] = '0';
                    time[3] = '0';
                    return 1;
                }
            else
                {
                    time[0] = ((UCHAR)(quantity/1000) + '0');
                    time[1] = ((UCHAR)((quantity/100)%10) + '0');
                    time[2] = ((UCHAR)((quantity/10)%10) + '0');
                    time[3] = ((UCHAR)(quantity%10) + '0');
                    return 0;
                }
        default:
            time[0] = 'E';
            time[1] = 'r';
            time[2] = 'r';
            return 0;
        }
}

static UCHAR IsLeapYear(UINT year)
{
    if (year%400==0 || (year%100!=0 && year%4==0))
        return 1;
    else
        return 0;
}
