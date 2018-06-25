#include "touch_7846.h"
#include <math.h>
#include <stdlib.h> //abs����
#include "gui_lcd.h"
#include "delay.h"

Pen_Holder Pen_Point;//�����ʵ��
//������ɫ
u16  POINT_COLOR=Red;//Ĭ�Ϻ�ɫ    
u16  BACK_COLOR=White;//������ɫ.Ĭ��Ϊ��ɫ


//��������ʼ��
//ʹ���ⲿ8M����,PLL��72MƵ��	

#define EXTI9_5_IRQChannel           ((u8)0x17)  /* External Line[9:5] Interrupts */	    


//SPIд����
//��7846д��1byte����   
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

//��ȡһ��X,Yֵ
//������X,Y����ֵ���붼����100
//�ɹ�����1,���ɹ�����0
//����������100~3800֮��.			   
u8 read_once(void)
{
   Pen_Point.X=TPReadX();	  //  4096-
   Pen_Point.Y=TPReadY(); //4096-
   return 1;
}	 

//�ж����ȼ�����/����	   
void NVIC_TOUCHConfiguration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	//�洢��ӳ��,������    
#ifdef  VECT_TAB_RAM  									   
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else   							 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//���ȼ��ֵ���2�� �ܹ�5��		 
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;  //ʹ���ⲿ�ж�10~15
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//�׼�2,���Ա�1����.
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 //�ײ�0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 	 									 
}
	    
//�ⲿ�жϳ�ʼ������
void touch_init(void)
{				
    GPIO_InitTypeDef GPIO_InitStructure;   
    EXTI_InitTypeDef	 EXTI_InitStructure;

  //ʹ��SPI��ʽʱ		
  SPI_InitTypeDef   SPI_InitStructure; 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);  
  GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

 //Configure SPI3 pins: SCK, MISO and MOSI 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
  GPIO_Init(GPIOC,&GPIO_InitStructure);   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //��������
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
  GPIO_Init(GPIOC,&GPIO_InitStructure);	   
  T_DCS(); 				 
					
	//PE7����			
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
	
	Read_Ads7846();        //��һ�ζ�ȡ��ʼ��
	
	//LCD_Clear(Black);//����  	  
	//if(get_adjdata())return;//�Ѿ�У׼
//	else			   //δУ׼?
	//{ 										    
	//	
	  //  touch_adjust();  //��ĻУ׼,���Զ�����	  
	//}
	//get_adjdata();
}


//��ȡADS7846	  
//������ȡ10������,��ʮ����������,Ȼ���
//�м�����ȡƽ��ֵ,�õ����յ�X,Yֵ  	   
u8 Read_Ads7846(void)
{
	u8 t,t1,count=0;
	u16 databuffer[2][10];//������
	u16 temp=0;	 
			  //NPEN
	 if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)==0)	 read_once();  //����û����
	 else return 0;
	
	while(count<10)   					  //ѭ������10��
	{	
	   //if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)==0)	    		   
	   {	if(read_once())//�����ɹ�
		   {	  
			databuffer[0][count]=Pen_Point.X;
			databuffer[1][count]=Pen_Point.Y;
			count++;  
		    }
		}
	}

	if(count==10)//һ��Ҫ����10������,������
	{  
	    do//������X��������
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[0][t]>databuffer[0][t+1])//��������
				{
					temp=databuffer[0][t+1];
					databuffer[0][t+1]=databuffer[0][t];
					databuffer[0][t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	  
		do//������Y��������
		{	
			t1=0;		 
			for(t=0;t<count-1;t++)
			{
				if(databuffer[1][t]>databuffer[1][t+1])//��������
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
	return 0;//����ʧ��
}	   
//�ж�,��⵽PEN�ŵ�һ���½���.
//��λPen_Point.Key_StaΪ����״̬
//�ж���0���ϵ��жϼ��
void EXTI9_5_IRQHandler(void)
{ 							     
	Pen_Point.Key_Sta=Key_Down;//��������   
	EXTI->PR=1<<7;  //���LINE7�ϵ��жϱ�־λ 
	//Read_Ads7846();		
} 	  
//�����������
//type:������Ӧ����
//0,����,����,����չ.һ��Ҫ�����ɿ��ŷ���
//1,����,����,����չ.����������/���Ӳ���
//2,��չ����֧��:
//��:MOVE_LEFT,MOVE_RIGHT,MOVE_UP,MOVE_DOWNʹ��
u8 Touch_Key_Pro(u8 type)
{
	u16 tempx,tempy;//��ʱ����X,Y���� 	 	 
	u8 ml=0,mr=0,mu=0,md=0;//�ĸ��������ƶ�����  
	u8 first=1;	  		 	    					   		 
	//�������ǰ��µ�
	//�ֶ�����������ʱ��,������Ҫ15ms�����˳����ѭ��	   	
	do
	{	    
		Pen_Point.Key_Sta=Key_Up;//����״̬��Ϊ�ɿ�    
		if(Read_Ads7846())//�ɹ�����
		{	 
			if(first)
			{
				Pen_Point.X0=Pen_Point.X;Pen_Point.Y0=Pen_Point.Y;//�õ���ʼ���µ�����
				tempx=Pen_Point.X;tempy=Pen_Point.Y;  
				//printf("X0:%d Y0:%d\n",Pen_Point.X,Pen_Point.Y);
				first=0;  //������
			}else if(type==2) //��չ����														  
			{	  
				if(tempx>Pen_Point.X)ml++;
				else mr++;	   	
				if(tempy>Pen_Point.Y)mu++;
				else md++;
				//�趨һ������ֵ,������һ���ƶ��������ֵ,���һ�δ������ֵ
				//��Ϊ������������.400��ʱ��,��ӳ�Ƚ���
				if(abs(tempx-Pen_Point.X)>700||abs(tempy-Pen_Point.Y)>700)//�ж���
				{
					ml=mr=mu=md=0;//ȫ�����
					tempx=Pen_Point.X=Pen_Point.X0;//���긴λ
					tempy=Pen_Point.Y=Pen_Point.Y0;
					break;//�˳����ݲɼ�,���Ϊ��ɼ�
				}   
				tempx=Pen_Point.X;tempy=Pen_Point.Y;//ת����ʱ���� 
			}else if(type==1)break; 
			//printf("X:%d Y:%d\n",Pen_Point.X,Pen_Point.Y); 
		}
		delay_ms(10);//10ms���� 						   		     								 	
	}while(NPEN||Pen_Point.Key_Sta==Key_Down);//PEN=0���߰���״̬Ϊ����״̬;
	delay_ms(50);
	Pen_Point.Key_Sta=Key_Up;//����״̬��Ϊ�ɿ�	 
 	//����/����չ ��ֵ����  
	if(abs(tempx-Pen_Point.X0)<=200&&abs(tempy-Pen_Point.Y0)<=200||type<2)//���ΰ���/����չ��������
	{	 
		if(Pen_Point.xfac!=0)//�Ѿ�У׼����
		{
			Pen_Point.X0=Pen_Point.xfac*Pen_Point.X0+Pen_Point.xoff;
			Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y0+Pen_Point.yoff;
		}
		Pen_Point.X=Pen_Point.X0;Pen_Point.Y=Pen_Point.Y0;
		return CLICK_POINT;//û���ƶ�  
	}	 
	//��չ��ֵ����
	if(abs(tempx-Pen_Point.X0)<=500&&abs(tempy-Pen_Point.Y0)<=500)return 0;//������������Ҫ����500
	else if(Pen_Point.xfac!=0)//�Ѿ�У׼����
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X0+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y0+Pen_Point.yoff;

		tempx=Pen_Point.xfac*tempx+Pen_Point.xoff;
		tempy=Pen_Point.yfac*tempy+Pen_Point.yoff;
	}
		
	if(abs(ml-mr)>abs(mu-md))//���� ����
	{
		if(abs(tempx-Pen_Point.X0)>abs(tempy-Pen_Point.Y0))//��������
		{
			if(tempx>Pen_Point.X0)return MOVE_RIGHT;
			else return MOVE_LEFT; 
		}else						//����������
		{
			if(tempy>Pen_Point.Y0)return MOVE_DOWN;
			else return MOVE_UP;
		}
	}else
	{
		if(abs(tempy-Pen_Point.Y0)>abs(tempx-Pen_Point.X0))//��������
		{	    
			if(tempy>Pen_Point.Y0)return MOVE_DOWN;
			else return MOVE_UP;			 
		}else						//����������
		{	  
			if(tempx>Pen_Point.X0)return MOVE_RIGHT;
			else return MOVE_LEFT;
		}
	}   	  
}
 
//�жϴ����ǲ�����ָ������֮��
//(x1,y1):��ʼ����
//(x2,y2):��������
//����ֵ :1,�ڸ�������.0,���ڸ�������.
u8 Is_In_Area(u8 x1,u16 y1,u8 x2,u16 y2)
{
	if(Pen_Point.X<=x2&&Pen_Point.X>=x1&&Pen_Point.Y<=y2&&Pen_Point.Y>=y1)return 1;
	else return 0;
}  
//�Ѵ������õ�������ת��Ϊ����(��ťֵ)
//M_Type:��ťģʽ;
//0,ϵͳ�˵�	
//1,�ļ����ģʽ 	
//2,ʱ��ģʽ��,�����Ļ�ȡ,����֧��! 
//3,���ֲ���ģʽ��
//4,����ѡ��ģʽ��
//5,������ģʽ��
//6,�����жϳ��򰴼�

//7,TXT���/JPEG��� ʱ��������			  
//����ֵ:��Ӧ�İ���ֵ(1~n)
//����0,���ʾ�˴ΰ�����Ч  
u8 Touch_To_Num(u8 M_Type)
{
	u8 xtemp;
	u16 t,ytemp;
	switch(M_Type)
	{
		case 0://ϵͳ�˵���
			xtemp=Touch_Key_Pro(1);//��ʹ����չ��ť
			if(!(xtemp&CLICK_POINT))return 0;//���ǵ㴥ֱ���˳�
			ytemp=10;
			for(t=0;t<9;t++)
			{
				xtemp=80*(t%3)+10;
				if(t>5)ytemp=216;
				else if(t>2)ytemp=113;
				if(Is_In_Area(xtemp,ytemp,xtemp+59,ytemp+90))return t+1;//���ذ���ֵ 
			}
			break;
		case 1://1,�ļ����ģʽ
			xtemp=Touch_Key_Pro(2);//֧����չ	 
			if(xtemp&CLICK_POINT)  //�㴥��	 �õ�1~8��ֵ
			{
				for(t=0;t<8;t++)
				{	  
					if(Is_In_Area(0,42+t*32,239,42+(t+1)*32))return t+1;
				}
				if(Is_In_Area(0,300,40,319))return 9;//ѡ��ť������
				if(Is_In_Area(200,300,239,319))return 10;//���ذ�ť������
				return 0;//������Ч
			}
			if(xtemp)return xtemp&0xf0;//����������!! 
			break; 
		case 2://2,ʱ��ģʽ��,�����Ļ�ȡ,����֧��!
			xtemp=Touch_Key_Pro(1);//����չ
			if(xtemp&CLICK_POINT)  //�㴥��	 �õ�1~8��ֵ
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
															   
				else if(Is_In_Area(0,300,40,319))return 13;//ѡ��ť������
				else if(Is_In_Area(200,300,239,319))return 14;//���ذ�ť������
				return 0;//������Ч
			}
			break;
		case 3://MP3��������İ�ťֵ
			xtemp=Touch_Key_Pro(1);//����չ
			if(xtemp&CLICK_POINT)  //�㴥��	 �õ�1~8��ֵ
			{
				if(Is_In_Area(214,72,240,96))return 1;//ѭ��ģʽ����	    
				else if(Is_In_Area(87,103,153,119))return 2; //supper bass
				else if(Is_In_Area(8,223,56,247))return 3;   //��Ч��/ȡ
				else if(Is_In_Area(64,223,112,247))return 4; //����
				else if(Is_In_Area(120,223,168,247))return 5;//����
				else if(Is_In_Area(178,224,204,276))return 6;//�س�	 
				else if(Is_In_Area(214,224,238,248))return 7;//����	    
				else if(Is_In_Area(214,252,238,276))return 8; //����
					
				else if(Is_In_Area(28,288,52,312))return 9;	  //��һȡ
				else if(Is_In_Area(108,288,132,312))return 10;//����
				else if(Is_In_Area(188,288,212,312))return 11;//��һ��

				else if(Is_In_Area(30,83,210,98))return 12;	  //����������,y����������һ�㷶Χ
															   
				//else if(Is_In_Area(0,300,40,319))return 13;//ѡ��ť������
				//else if(Is_In_Area(200,300,239,319))return 14;//���ذ�ť������
				return 0;//������Ч
			}
		case 4://4,����ѡ��ģʽ��
			xtemp=Touch_Key_Pro(1);//��֧����չ	  
			if(xtemp&CLICK_POINT)  //�㴥��	 �õ�1~8��ֵ
			{
				for(t=0;t<8;t++)
				{	  
					if(Is_In_Area(0,42+t*32,239,42+(t+1)*32))return t+1;
				}
				if(Is_In_Area(0,300,40,319))return 9;//ѡ��ť������
				if(Is_In_Area(200,300,239,319))return 10;//���ذ�ť������
				return 0;//������Ч
			}										    
			break;
		case 5://5,������ģʽ��
			xtemp=Touch_Key_Pro(1);//��֧����չ	
			if(xtemp&CLICK_POINT)  //�㴥��	 �õ�1~8��ֵ
			{
				if(Is_In_Area(37,215,61,239))return 1;//ѭ��ģʽ����	    
				else if(Is_In_Area(93,215,117,239))return 2;//ѭ��ģʽ����
				else if(Is_In_Area(152,218,203,236))return 3;   //��Ч��/ȡ		 
					
				else if(Is_In_Area(28,261,52,285))return 4;	 //��һȡ
				else if(Is_In_Area(108,261,132,285))return 5;//����
				else if(Is_In_Area(188,261,212,285))return 6;//��һ��
																		   
				else if(Is_In_Area(0,300,40,319))return 7;//ѡ�ť������
				else if(Is_In_Area(200,300,239,319))return 8;//���ذ�ť������
				return 0;//������Ч
			}  
			break; 
		case 6://6,�����жϳ��򰴼�
			xtemp=Touch_Key_Pro(1);//��֧����չ	
			if(xtemp&CLICK_POINT)  //�㴥��	 �õ�1~8��ֵ
			{
				if(Is_In_Area(111,151,153,169))return 1;//ѭ��ģʽ����	       
				return 0;//������Ч
			}  
			break;
		case 7://JPEG/BMP/TXT�������ģʽ
			xtemp=Touch_Key_Pro(1);//��֧����չ	
			if(xtemp&CLICK_POINT)  //�㴥��	 �õ�1~8��ֵ
			{
				if(Is_In_Area(0,0,239,60))return KEY_PREV;        //����
				else if(Is_In_Area(0,60,239,260))return KEY_FUNC; //����		    
				else if(Is_In_Area(0,260,239,319))return KEY_NEXT;//���·� 	   
				return 0;//������Ч
			}  			
			break; 	  	    		 
	}
	return 0;
}
//�õ����̷���ֵ
//�������޸�ʱ���ʱ���õ�
//���أ�0~11
//����11��������Ч
//����10���˸񰴼�
u8 Get_KeyBoard_Val(void)
{
	u8 t;
	for(t=0;t<10;t++) //�������
	{				 
		if(Is_In_Area(14+18*t,269,30+18*t,285))return t;//16*16����  
	}  
	if(Is_In_Area(194,269,226,285))return 10;//32*16���� 
	return 11;
}
//����У׼����
//��������FM24C16�� 40~52��ε�ַ���䣬�ܹ�12���ֽ�
void save_adjdata(void)
{
	s32 temp;			 
	//����У�����!		   							  
	temp=Pen_Point.xfac*100000000;//����xУ������      
   // FM24C16_WriteLenByte(40,temp,4);

	temp=Pen_Point.yfac*100000000;//����yУ������    
   // FM24C16_WriteLenByte(44,temp,4);
	//����xƫ����
   // FM24C16_WriteLenByte(48,Pen_Point.xoff,2);		    
	//����yƫ����
//	FM24C16_WriteLenByte(50,Pen_Point.yoff,2);	
			     							 
//	temp=FM24C16_ReadOneByte(52);
	temp&=0XF0;
	temp|=0X0A;//���У׼����
	//FM24C16_WriteOneByte(52,temp);			 
}
//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
u8 get_adjdata(void)
{					  
//	s32 tempfac;
	//tempfac=FM24C16_ReadOneByte(52);//����ʮ���ֽڵĵ���λ��������Ƿ�У׼���� 		 
/*	if((tempfac&0X0F)==0X0A)//�������Ѿ�У׼����			   
	{    												 
		tempfac=FM24C16_ReadLenByte(40,4);		   
		Pen_Point.xfac=(float)tempfac/100000000;//�õ�xУ׼����
		tempfac=FM24C16_ReadLenByte(44,4);			          
		Pen_Point.yfac=(float)tempfac/100000000;//�õ�yУ׼����
	    //�õ�xƫ����
		tempfac=FM24C16_ReadLenByte(48,2);			   	  
		Pen_Point.xoff=tempfac;					 
	    //�õ�yƫ����
		tempfac=FM24C16_ReadLenByte(50,2);				 	  
		Pen_Point.yoff=tempfac;					 
		return 1;	 
	}		  */
	return 0;  
}

//������У׼����
//�õ��ĸ�У׼����
void touch_adjust(void)
{	
    						 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	   
	cnt=0;		
			
	//POINT_COLOR=Blue;
	BACK_COLOR =White;
	LCD_SetBackColor(White);
	LCD_Clear(White);//����  	 
	POINT_COLOR=Red;//��ɫ 
	LCD_SetTextColor(POINT_COLOR); 
	//LCD_Clear(White);//���� 	
	 
	//Drow_touch_point(20,20);//����1 	��/////////////////
	Pen_Point.Key_Sta=Key_Up;//���������ź� 
	Pen_Point.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//����������
		{
			if(Touch_Key_Pro(0)&CLICK_POINT)//�õ����ΰ���ֵ
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(White);//���� 
				//	Drow_touch_point(220,20);//����2
					break;
				case 2:
					LCD_Clear(White);//���� 
				//	Drow_touch_point(20,300);//����3
					break;
				case 3:
					LCD_Clear(White);//���� 
				//	Drow_touch_point(220,300);//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
						//Drow_touch_point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
					//	Drow_touch_point(20,20);
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
					//	Drow_touch_point(20,20);
						continue;
					}//��ȷ��
					//������
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					POINT_COLOR=Blue;
					LCD_Clear(White);//����
					//TFT_ShowString(35,110,"Touch Screen Adjust OK!");//У�����
					delay_ms(1000);
					LCD_Clear(White);//����   
					save_adjdata();
					return;//У�����				 
			}
		}
	} 
}
