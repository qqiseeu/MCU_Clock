#ifndef _KEYBD_H
#define _KEYBD_H

/*for using the independent keyboard*/


#define PAU_STA 0x0f
#define PLUS    0x0e
#define MINUS   0x0d
#define MOVE    0x0c
#define SERIAL  0x0b
#define RUNNING 0x0a

extern UCHAR keyscan(void);

#endif
