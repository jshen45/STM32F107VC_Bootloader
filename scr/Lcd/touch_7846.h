
#ifndef __TOUCH_7846_H
#define __TOUCH_7846_H

#ifdef __cplusplus
 extern "C" {
#endif
 
#include "stm32f10x.h"	 
#include "stm3210x_9325_lcd.h"


//笔杆结构体
typedef struct 
{
	u16 X0;//原始坐标
	u16 Y0;
	u16 X; //最终/暂存坐标
	u16 Y;						   	    
	u8  Key_Sta;//笔的状态
	u8  Key_LSta;//上一次笔的状态
//触摸屏校准参数
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;

/*-------------------------------
  管脚对应请查看原理图
--------------------------------*/


 
#define TDIN  (1<<12) // PC12 MOSI
#define TCLK  (1<<10) // PC10 CLK
#define TCS   (1<<4) // PC4 CS    			    
#define TDIN_SET(x) GPIOC->ODR=(GPIOC->ODR&~TDIN)|(x ? TDIN:0)
#define TCLK_SET(x) GPIOC->ODR=(GPIOC->ODR&~TCLK)|(x ? TCLK:0)													    
#define TCS_SET(x)  GPIOC->ODR=(GPIOC->ODR&~TCS)|(x ? TCS:0)  
#define DOUT GPIOC->IDR&1<<11 //PC11数据输入

#define T_CS()   TCS_SET(0)
#define T_DCS()   TCS_SET(1)

//改变读数顺序
#define CMD_RDY 0X90  //0B10010000即用差分方式读X坐标
#define CMD_RDX	0XD0  //0B11010000即用差分方式读Y坐标   											 
//extern u16 POINT_COLOR;		//画笔颜色
//extern u16 BACK_COLOR;		//画笔颜色	   


//按键状态	 
#define Key_Down 0x01
#define Key_Up   0x00 
//触摸屏按键处理
#define CLICK_POINT 0X01   
#define MOVE_LEFT   0X10
#define MOVE_RIGHT  0X20
#define MOVE_UP     0X40
#define MOVE_DOWN   0X80

#define	KEY_NEXT 0X01
#define KEY_PREV 0X02
#define KEY_FUNC 0X03
		
//外部笔
#define PEN  GPIOE->IDR&(1<<7) //Pe7 
#define NPEN !(0x0080&PEN)      //!PEN

unsigned char SPI_WriteByte(u8 num);
void SpiDelay(unsigned int DelayCnt);
u16 TPReadX(void);
u16 TPReadY(void);	   
u8 read_once(void);	
u8 Read_Ads7846(void); 

void EXTI9_5_IRQHandler(void);
u8 Touch_Key_Pro(u8 type);
u8 Is_In_Area(u8 x1,u16 y1,u8 x2,u16 y2);
u8 Touch_To_Num(u8 M_Type);
u8 Get_KeyBoard_Val(void);
void save_adjdata(void);
u8 get_adjdata(void);
void touch_adjust(void);
void NVIC_TOUCHConfiguration(void);
void touch_init(void);


#ifdef __cplusplus
}
#endif

#endif 


















