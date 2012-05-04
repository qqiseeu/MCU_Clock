/*Synchronize time with PC using UART*/
#include <reg52.h>
#include "myclock.h"
#include <stdio.h>

extern UCHAR IsLegal(UCHAR time[]);
extern UCHAR Recv;
extern bit   SerBegin;

void UART_SyncTime(UCHAR time[])
{
    static UCHAR i = 0;

    if (SerBegin == 1)
        {
            if (Recv == '$')
                i       = 0;
            else
                {
                    time[i] = Recv;
                    i++;
                    if (i >= T_LENGTH)
                        SerBegin = 0;
                }
        }
    else if (i > 0)
        {
            i = 0;
            if (!IsLegal(time))
                {
                    ES = 0;
                    TI = 1;
                    puts("Wrong type");
                    while (!TI)
                        ;
                    TI = 0;
                    ES = 1;
                }
        }
}
       
    
