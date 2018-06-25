
#ifndef __GUI_LCD_H
#define __GUI_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif					   

#include "stm3210x_9325_lcd.h"

void Drow_touch_point(u8 x,u16 y);
void LCD_ShowNum(uint8_t x,uint16_t y,uint16_t data);


#ifdef __cplusplus
}
#endif

#endif 

