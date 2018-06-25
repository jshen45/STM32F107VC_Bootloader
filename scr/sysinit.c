
#include "sysinit.h"  
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


void Gpio_int(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); 
  
  //Green,Blue,and Red LEDs definition                              
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
  GPIO_Init(GPIOD,&GPIO_InitStructure);	     
}
