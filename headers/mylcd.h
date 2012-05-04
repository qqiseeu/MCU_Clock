#ifndef _LCDCTRL_H
#define _LCDCTRL_H

/*for using the 1602 lcd*/

extern void lcd_init(void);
extern void lcd_disp(UCHAR time[], UCHAR curpos, bit runp);
extern      changetime(UCHAR time[], UCHAR pos, UCHAR act);

#endif
