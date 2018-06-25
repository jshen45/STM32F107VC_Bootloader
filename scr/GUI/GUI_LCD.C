 #include "GUI_LCD.h" 

void Drow_touch_point(u8 x,u16 y)
{
    x=239-x; //‘≠µ„Œª÷√«–ªª
    LCD_DrawCircle(x,y,10);
	LCD_DrawLine(x-10,y,20,1);  
	LCD_DrawLine(x,y-10,20,0);
}


void LCD_ShowNum(uint8_t x,uint16_t y,uint16_t data)
{
LCD_DisplayChar(x,y,data/10000+48); 
LCD_DisplayChar(x,y+25,data%10000/1000+48);   // %10000
LCD_DisplayChar(x,y+50,data%1000/100+48); 
LCD_DisplayChar(x,y+75,data%100/10+48);	 
LCD_DisplayChar(x,y+100,data%10+48);
}
