/*Synchronize time with PC using UART*/
#include <reg52.h>
#include "myclock.h"
#include <stdio.h>

extern UCHAR IsLegal(UCHAR time[]);
extern UCHAR Recv;
extern bit   SerBegin;
extern bit   NewRecv;

void UART_SyncTime(UCHAR time[], UCHAR tmp[])
{
    static UCHAR i = 0;

    if (SerBegin == 1 && NewRecv == 1)
        {
            if (Recv == '$')
                i       = 0;
            else
                {
                    tmp[i] = Recv;
                    i++;
                    NewRecv = 0;
                    if (i >= T_LENGTH)
                        SerBegin = 0;
                }
        }
    else if (SerBegin == 0 && i > 0)
        {
            i = 0;
            if (!IsLegal(tmp))
                {
                    ES = 0;
                    TI = 1;
                    puts("Wrong type");
                    while (!TI)
                        ;
                    TI = 0;
                    ES = 1;
                }
            else
                {
                    while (i < T_LENGTH)
                        {
                            time[i] = tmp[i];
                            i++;
                        }
                    i = 0;
                }
        }
}
       
    
