
#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif


//在72M 系统时钟 下面准确

void delay_ms(unsigned int nms);
void delay_us(unsigned int nus);

#ifdef __cplusplus
}
#endif

#endif 
