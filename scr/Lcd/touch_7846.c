#include "touch_7846.h"
#include <math.h>
#include <stdlib.h> //abs函数
#include "gui_lcd.h"
#include "delay.h"

Pen_Holder Pen_Point;//定义笔实体
//画笔颜色
u16  POINT_COLOR=Red;//默认红色    
u16  BACK_COLOR=White;//背景颜色.默认为白色


//触摸屏初始化
//使用外部8M晶振,PLL到72M频率	

#define EXTI9_5_IRQChannel           ((u8)0x17)  /* External Line[9:5] Interrupts */	    


//SPI写数据
//向7846写入1byte数据   
unsigned char SPI_WriteByte(u8 num)    
{  
  unsigned char Data = 0;
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==RESET); 
  SPI_I2S_SendData(SPI3,num);   
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RXNE)==RESET); 
  Data = SPI_I2S_ReceiveData(SPI3);

  return Data; 	 				   
} 	

void SpiDelay(unsigned int DelayCnt)
{
 unsigned int i;
 for(i=0;i<DelayCnt;i++);
}

u16 TPReadX(void)
{ 
   u16 x=0;
   T_CS();
   SpiDelay(10);
   SPI_WriteByte(0x90);
   SpiDelay(10);  
   x=SPI_WriteByte(0xFF);
   x<<=8;
   x+=SPI_WriteByte(0x0);	
   T_DCS(); 
   SpiDelay(10);    
   x=x>>4;
   return x;
}


u16 TPReadY(void)
{
  u16 y=0;
  T_CS();
  SpiDelay(10);
  SPI_WriteByte(0xd0);
  SpiDelay(10);	 
  y=SPI_WriteByte(0x0);
  y<<=8;
  y+=SPI_WriteByte(0x0); 
  T_DCS(); 
  y = y>>4; 
  return (y);
}

//读取一次X,Y值
//读到的X,Y坐标值必须都大于100
//成功返回1,不成功返回0
//读数限制在100~3800之间.			   
u8 read_once(void)
{
   Pen_Point.X=TPReadX();	  //  4096-
   Pen_Point.Y=TPReadY(); //4096-
   return 1;
}	 

//中断优先级管理/开启	   
void NVIC_TOUCHConfiguration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	//存储器映射,不用理    
#ifdef  VECT_TAB_RAM  									   
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else   							 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//优先级分到第2组 总共5组		 
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;  //使用外部中断10~15
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//阶级2,可以被1抢断.
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 //阶层0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 	 									 
}
	    
//外部中断初始化函数
void touch_init(void)
{				
    GPIO_InitTypeDef GPIO_InitStructure;   
    EXTI_InitTypeDef	 EXTI_InitStructure;

  //使用SPI方式时		
  SPI_InitTypeDef   SPI_InitStructure; 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);  
  GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

 //Configure SPI3 pins: SCK, MISO and MOSI 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
  GPIO_Init(GPIOC,&GPIO_InitStructure);   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入
  GPIO_Init(GPIOC,&GPIO_InitStructure);	

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//;Low//; 
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; 
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStructure.SPI_CRCPolynomial = 7; 
  SPI_Init(SPI3,&SPI_InitStructure); 
  SPI_Cmd(SPI3,ENABLE); 
  
  //CS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  GPIO_Init(GPIOC,&GPIO_InitStructure);	   
  T_DCS(); 				 
					
	//PE7触摸			
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource7);
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);							  						   										  
	NVIC_TOUCHConfiguration();    		 
	
	Read_Ads7846();        //第一次读取初始化
	
	//LCD_Clear(Black);//清屏  	  
	//if(get_adjdata())return;//已经校准
//	else			   //未校准?
	//{ 										    
	//	
	  //  touch_adjust();  //屏幕校准,带自动保存	  
	//}
	//get_adjdata();
}


//读取ADS7846	  
//连续读取10次数据,对十次数据排序,然后对
//中间三次取平均值,得到最终的X,Y值  	   
u8 Read_Ads7846(void)
{
	u8 t,t1,count=0;
	u16 databuffer[2][10];//数据组
	u16 temp=0;	 
			  //NPEN
	 if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)==0)	 read_once();  //触摸没按下
	 else return 0;
	
	while(count<10)   					  //循环读数10次
	{	
	   //if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)==0)	    		   
	   {	if(read_once())//读数成功
		   {	  
			databuffer[0][count]=Pen_Point.X;
			databuffer[1][count]=Pen_Point.Y;
			count++;  
		    }
		}
	}

	if(count==10)//一定要读到10次数据,否则丢弃
	{  
	    do//将数据X升序排列
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[0][t]>databuffer[0][t+1])//升序排列
				{
					temp=databuffer[0][t+1];
					databuffer[0][t+1]=databuffer[0][t];
					databuffer[0][t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	  
		do//将数据Y升序排列
		{	
			t1=0;		 
			for(t=0;t<count-1;t++)
			{
				if(databuffer[1][t]>databuffer[1][t+1])//升序排列
				{
					temp=databuffer[1][t+1];
					databuffer[1][t+1]=databuffer[1][t];
					databuffer[1][t]=temp;
					t1=1;	 
				}  
			}
		}while(t1);		   
		 	 		  
		Pen_Point.X=(databuffer[0][3]+databuffer[0][4]+databuffer[0][5])/3;
		Pen_Point.Y=(databuffer[1][3]+databuffer[1][4]+databuffer[1][5])/3;	 
		 
		LCD_ShowNum(200,5,Pen_Point.X); // Pen_Point.X
	    LCD_ShowNum(200,150,Pen_Point.Y); // Pen_Point.Y
		return 1;									   
	}
	return 0;//读数失败
}	   
//中断,检测到PEN脚的一个下降沿.
//置位Pen_Point.Key_Sta为按下状态
//中断线0线上的中断检测
void EXTI9_5_IRQHandler(void)
{ 							     
	Pen_Point.Key_Sta=Key_Down;//按键按下   
	EXTI->PR=1<<7;  //清除LINE7上的中断标志位 
	//Read_Ads7846();		
} 	  
//按键处理程序
//type:按键响应类型
//0,单点,定点,不扩展.一定要按键松开才返回
//1,单点,滑动,不扩展.滚动条操作/连加操作
//2,扩展按键支持:
//即:MOVE_LEFT,MOVE_RIGHT,MOVE_UP,MOVE_DOWN使能
u8 Touch_Key_Pro(u8 type)
{
	u16 tempx,tempy;//暂时保存X,Y坐标 	 	 
	u8 ml=0,mr=0,mu=0,md=0;//四个方向上移动次数  
	u8 first=1;	  		 	    					   		 
	//按键还是按下的
	//手动按触摸屏的时候,至少需要15ms才能退出这个循环	   	
	do
	{	    
		Pen_Point.Key_Sta=Key_Up;//按键状态变为松开    
		if(Read_Ads7846())//成功读数
		{	 
			if(first)
			{
				Pen_Point.X0=Pen_Point.X;Pen_Point.Y0=Pen_Point.Y;//得到初始按下的坐标
				tempx=Pen_Point.X;tempy=Pen_Point.Y;  
				//printf("X0:%d Y0:%d\n",Pen_Point.X,Pen_Point.Y);
				first=0;  //标记清空
			}else if(type==2) //扩展按键														  
			{	  
				if(tempx>Pen_Point.X)ml++;
				else mr++;	   	
				if(tempy>Pen_Point.Y)mu++;
				else md++;
				//设定一个门限值,不能让一次移动大于这个值,如果一次大于这个值
				//认为触摸屏误动作了.400的时候,反映比较慢
				if(abs(tempx-Pen_Point.X)>700||abs(tempy-Pen_Point.Y)>700)//有抖动
				{
					ml=mr=mu=md=0;//全部清掉
					tempx=Pen_Point.X=Pen_Point.X0;//坐标复位
					tempy=Pen_Point.Y=Pen_Point.Y0;
					break;//退出数据采集,结果为点采集
				}   
				tempx=Pen_Point.X;tempy=Pen_Point.Y;//转移临时坐标 
			}else if(type==1)break; 
			//printf("X:%d Y:%d\n",Pen_Point.X,Pen_Point.Y); 
		}
		delay_ms(10);//10ms消抖 						   		     								 	
	}while(NPEN||Pen_Point.Key_Sta==Key_Down);//PEN=0或者按键状态为按下状态;
	delay_ms(50);
	Pen_Point.Key_Sta=Key_Up;//按键状态变为松开	 
 	//单次/不扩展 键值处理  
	if(abs(tempx-Pen_Point.X0)<=200&&abs(tempy-Pen_Point.Y0)<=200||type<2)//单次按键/不扩展按键功能
	{	 
		if(Pen_Point.xfac!=0)//已经校准过了
		{
			Pen_Point.X0=Pen_Point.xfac*Pen_Point.X0+Pen_Point.xoff;
			Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y0+Pen_Point.yoff;
		}
		Pen_Point.X=Pen_Point.X0;Pen_Point.Y=Pen_Point.Y0;
		return CLICK_POINT;//没有移动  
	}	 
	//扩展键值处理
	if(abs(tempx-Pen_Point.X0)<=500&&abs(tempy-Pen_Point.Y0)<=500)return 0;//滑动距离最少要大于500
	else if(Pen_Point.xfac!=0)//已经校准过了
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X0+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y0+Pen_Point.yoff;

		tempx=Pen_Point.xfac*tempx+Pen_Point.xoff;
		tempy=Pen_Point.yfac*tempy+Pen_Point.yoff;
	}
		
	if(abs(ml-mr)>abs(mu-md))//数量 满足
	{
		if(abs(tempx-Pen_Point.X0)>abs(tempy-Pen_Point.Y0))//质量满足
		{
			if(tempx>Pen_Point.X0)return MOVE_RIGHT;
			else return MOVE_LEFT; 
		}else						//质量不满足
		{
			if(tempy>Pen_Point.Y0)return MOVE_DOWN;
			else return MOVE_UP;
		}
	}else
	{
		if(abs(tempy-Pen_Point.Y0)>abs(tempx-Pen_Point.X0))//质量满足
		{	    
			if(tempy>Pen_Point.Y0)return MOVE_DOWN;
			else return MOVE_UP;			 
		}else						//质量不满足
		{	  
			if(tempx>Pen_Point.X0)return MOVE_RIGHT;
			else return MOVE_LEFT;
		}
	}   	  
}
 
//判断触点是不是在指定区域之内
//(x1,y1):起始坐标
//(x2,y2):结束坐标
//返回值 :1,在该区域内.0,不在该区域内.
u8 Is_In_Area(u8 x1,u16 y1,u8 x2,u16 y2)
{
	if(Pen_Point.X<=x2&&Pen_Point.X>=x1&&Pen_Point.Y<=y2&&Pen_Point.Y>=y1)return 1;
	else return 0;
}  
//把触摸屏得到的数据转换为数字(按钮值)
//M_Type:按钮模式;
//0,系统菜单	
//1,文件浏览模式 	
//2,时间模式下,按键的获取,连按支持! 
//3,音乐播放模式下
//4,功能选择模式下
//5,收音机模式下
//6,闹钟中断程序按键

//7,TXT浏览/JPEG浏览 时按键处理			  
//返回值:对应的按键值(1~n)
//返回0,则表示此次按键无效  
u8 Touch_To_Num(u8 M_Type)
{
	u8 xtemp;
	u16 t,ytemp;
	switch(M_Type)
	{
		case 0://系统菜单下
			xtemp=Touch_Key_Pro(1);//不使用扩展按钮
			if(!(xtemp&CLICK_POINT))return 0;//不是点触直接退出
			ytemp=10;
			for(t=0;t<9;t++)
			{
				xtemp=80*(t%3)+10;
				if(t>5)ytemp=216;
				else if(t>2)ytemp=113;
				if(Is_In_Area(xtemp,ytemp,xtemp+59,ytemp+90))return t+1;//返回按键值 
			}
			break;
		case 1://1,文件浏览模式
			xtemp=Touch_Key_Pro(2);//支持扩展	 
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				for(t=0;t<8;t++)
				{	  
					if(Is_In_Area(0,42+t*32,239,42+(t+1)*32))return t+1;
				}
				if(Is_In_Area(0,300,40,319))return 9;//选择按钮被按下
				if(Is_In_Area(200,300,239,319))return 10;//返回按钮被按下
				return 0;//按键无效
			}
			if(xtemp)return xtemp&0xf0;//滑动触摸了!! 
			break; 
		case 2://2,时间模式下,按键的获取,连按支持!
			xtemp=Touch_Key_Pro(1);//不扩展
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(4,142,28,166))return 1;	    
				else if(Is_In_Area(60,142,84,166))return 2;
				else if(Is_In_Area(96,142,120,166))return 3;
				else if(Is_In_Area(136,142,160,166))return 4;
				else if(Is_In_Area(172,142,196,166))return 5;
				else if(Is_In_Area(212,142,236,166))return 6;

				else if(Is_In_Area(4,206,28,230))return 7;	    
				else if(Is_In_Area(60,206,84,230))return 8;
				else if(Is_In_Area(96,206,120,230))return 9;
				else if(Is_In_Area(136,206,160,230))return 10;
				else if(Is_In_Area(172,206,196,230))return 11;
				else if(Is_In_Area(212,206,236,230))return 12;
															   
				else if(Is_In_Area(0,300,40,319))return 13;//选择按钮被按下
				else if(Is_In_Area(200,300,239,319))return 14;//返回按钮被按下
				return 0;//按键无效
			}
			break;
		case 3://MP3界面下面的按钮值
			xtemp=Touch_Key_Pro(1);//不扩展
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(214,72,240,96))return 1;//循环模式更改	    
				else if(Is_In_Area(87,103,153,119))return 2; //supper bass
				else if(Is_In_Area(8,223,56,247))return 3;   //音效存/取
				else if(Is_In_Area(64,223,112,247))return 4; //音量
				else if(Is_In_Area(120,223,168,247))return 5;//其他
				else if(Is_In_Area(178,224,204,276))return 6;//回车	 
				else if(Is_In_Area(214,224,238,248))return 7;//增加	    
				else if(Is_In_Area(214,252,238,276))return 8; //减少
					
				else if(Is_In_Area(28,288,52,312))return 9;	  //后一取
				else if(Is_In_Area(108,288,132,312))return 10;//播放
				else if(Is_In_Area(188,288,212,312))return 11;//上一曲

				else if(Is_In_Area(30,83,210,98))return 12;	  //进度条按下,y方向扩大了一点范围
															   
				//else if(Is_In_Area(0,300,40,319))return 13;//选择按钮被按下
				//else if(Is_In_Area(200,300,239,319))return 14;//返回按钮被按下
				return 0;//按键无效
			}
		case 4://4,功能选择模式下
			xtemp=Touch_Key_Pro(1);//不支持扩展	  
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				for(t=0;t<8;t++)
				{	  
					if(Is_In_Area(0,42+t*32,239,42+(t+1)*32))return t+1;
				}
				if(Is_In_Area(0,300,40,319))return 9;//选择按钮被按下
				if(Is_In_Area(200,300,239,319))return 10;//返回按钮被按下
				return 0;//按键无效
			}										    
			break;
		case 5://5,收音机模式下
			xtemp=Touch_Key_Pro(1);//不支持扩展	
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(37,215,61,239))return 1;//循环模式更改	    
				else if(Is_In_Area(93,215,117,239))return 2;//循环模式更改
				else if(Is_In_Area(152,218,203,236))return 3;   //音效存/取		 
					
				else if(Is_In_Area(28,261,52,285))return 4;	 //后一取
				else if(Is_In_Area(108,261,132,285))return 5;//播放
				else if(Is_In_Area(188,261,212,285))return 6;//上一曲
																		   
				else if(Is_In_Area(0,300,40,319))return 7;//选项按钮被按下
				else if(Is_In_Area(200,300,239,319))return 8;//返回按钮被按下
				return 0;//按键无效
			}  
			break; 
		case 6://6,闹钟中断程序按键
			xtemp=Touch_Key_Pro(1);//不支持扩展	
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(111,151,153,169))return 1;//循环模式更改	       
				return 0;//按键无效
			}  
			break;
		case 7://JPEG/BMP/TXT浏览按键模式
			xtemp=Touch_Key_Pro(1);//不支持扩展	
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(0,0,239,60))return KEY_PREV;        //向上
				else if(Is_In_Area(0,60,239,260))return KEY_FUNC; //不定		    
				else if(Is_In_Area(0,260,239,319))return KEY_NEXT;//向下翻 	   
				return 0;//按键无效
			}  			
			break; 	  	    		 
	}
	return 0;
}
//得到键盘返回值
//在闹钟修改时间的时候用到
//返回：0~11
//返回11：按键无效
//返回10：退格按键
u8 Get_KeyBoard_Val(void)
{
	u8 t;
	for(t=0;t<10;t++) //加载外框
	{				 
		if(Is_In_Area(14+18*t,269,30+18*t,285))return t;//16*16方框  
	}  
	if(Is_In_Area(194,269,226,285))return 10;//32*16方框 
	return 11;
}
//保存校准参数
//保存区域：FM24C16的 40~52这段地址区间，总共12个字节
void save_adjdata(void)
{
	s32 temp;			 
	//保存校正结果!		   							  
	temp=Pen_Point.xfac*100000000;//保存x校正因素      
   // FM24C16_WriteLenByte(40,temp,4);

	temp=Pen_Point.yfac*100000000;//保存y校正因素    
   // FM24C16_WriteLenByte(44,temp,4);
	//保存x偏移量
   // FM24C16_WriteLenByte(48,Pen_Point.xoff,2);		    
	//保存y偏移量
//	FM24C16_WriteLenByte(50,Pen_Point.yoff,2);	
			     							 
//	temp=FM24C16_ReadOneByte(52);
	temp&=0XF0;
	temp|=0X0A;//标记校准过了
	//FM24C16_WriteOneByte(52,temp);			 
}
//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
u8 get_adjdata(void)
{					  
//	s32 tempfac;
	//tempfac=FM24C16_ReadOneByte(52);//第五十二字节的第四位用来标记是否校准过！ 		 
/*	if((tempfac&0X0F)==0X0A)//触摸屏已经校准过了			   
	{    												 
		tempfac=FM24C16_ReadLenByte(40,4);		   
		Pen_Point.xfac=(float)tempfac/100000000;//得到x校准参数
		tempfac=FM24C16_ReadLenByte(44,4);			          
		Pen_Point.yfac=(float)tempfac/100000000;//得到y校准参数
	    //得到x偏移量
		tempfac=FM24C16_ReadLenByte(48,2);			   	  
		Pen_Point.xoff=tempfac;					 
	    //得到y偏移量
		tempfac=FM24C16_ReadLenByte(50,2);				 	  
		Pen_Point.yoff=tempfac;					 
		return 1;	 
	}		  */
	return 0;  
}

//触摸屏校准代码
//得到四个校准参数
void touch_adjust(void)
{	
    						 
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	   
	cnt=0;		
			
	//POINT_COLOR=Blue;
	BACK_COLOR =White;
	LCD_SetBackColor(White);
	LCD_Clear(White);//清屏  	 
	POINT_COLOR=Red;//红色 
	LCD_SetTextColor(POINT_COLOR); 
	//LCD_Clear(White);//清屏 	
	 
	//Drow_touch_point(20,20);//画点1 	、/////////////////
	Pen_Point.Key_Sta=Key_Up;//消除触发信号 
	Pen_Point.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//按键按下了
		{
			if(Touch_Key_Pro(0)&CLICK_POINT)//得到单次按键值
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(White);//清屏 
				//	Drow_touch_point(220,20);//画点2
					break;
				case 2:
					LCD_Clear(White);//清屏 
				//	Drow_touch_point(20,300);//画点3
					break;
				case 3:
					LCD_Clear(White);//清屏 
				//	Drow_touch_point(220,300);//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
						LCD_Clear(White);//清屏 
						//Drow_touch_point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
						LCD_Clear(White);//清屏 
					//	Drow_touch_point(20,20);
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
						LCD_Clear(White);//清屏 
					//	Drow_touch_point(20,20);
						continue;
					}//正确了
					//计算结果
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					POINT_COLOR=Blue;
					LCD_Clear(White);//清屏
					//TFT_ShowString(35,110,"Touch Screen Adjust OK!");//校正完成
					delay_ms(1000);
					LCD_Clear(White);//清屏   
					save_adjdata();
					return;//校正完成				 
			}
		}
	} 
}
