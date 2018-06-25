#include "delay.h"


//在72M 系统时钟 下面准确
void delay_ms(unsigned int nms)
{
    unsigned int i;
	while(nms--)for(i=15000;i>0;i--);
}

void delay_us(unsigned int nus)
{
	unsigned int i;
	while(nus--)for(i=15;i>0;i--);
}	
