/********************************************************************
* Copyright (C) 2003-2008 Texas Instruments Incorporated.
* All Rights Reserved
*********************************************************************
* file: lcd_osd.h
*
* Brief: This file contains the Register Description for dma
*
*********************************************************************/
#ifndef _LCD_OSD_H_
#define _LCD_OSD_H_

#include <cslr.h>

#include <tistdtypes.h>
#include <csl_general.h>

extern PSP_Handle hi2c;

Int16 OSD9616_send( Uint16 regAddr, Uint16 regData);
Int16 OSD9616_multiSend( Uint16 *regData, Uint16 length, PSP_Handle hi2c );
Int16 printchar(unsigned char a);
Int16 printstring(char *a);
Int16 clear_lcd();
Int16 print_ti();
Int16 print_dsp();
Int16 print_playaudio();
Int16 print_two(char *s1, char *s2);
Int16 print_charROM();
Int16 oled_init();
int oled_display_bargraph (int * message);



#endif
