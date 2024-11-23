/**
 ****************************************************************************************************
 * @file        lcd.c
 * @author      EYA-DISPLAY
 * @version     V2.0
 * @date        2022-04-28
 * @brief       Һ��������Demo
 * @license     Copyright (c) 2022-2032, �����տƼ�����(�㶫)
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:EYA-ETmcu������
 * ��˾��ַ:www.eya-display.com
 *
 ****************************************************************************************************
 **/
#include "lcd.h"
#include "stdlib.h"
#include "font.h"

#define delay_ms(nms) HAL_Delay(nms)


//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(u8 i)
{
	while(i--);
}

//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 


u8 DFT_SCAN_DIR; //ɨ�跽�����
//////////////////////////////////////////////////////////////////////////////////	 
extern const u8 gImage_logo[];   //ͼƬ����(������Ϣͷ),�洢��image1.c����.				 
//LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;


#if(Interface==D4WSPI)
	//д�Ĵ�������  SPI�ӿ���Ҫ�õ��ĺ���
	//regval:�Ĵ���ֵ
	void LCD_WR_REG(u8 regval)
	{ 
		LCD_RS=0;//д��ַ 
		LCD_CS=0; 
		SPI2_ReadWriteByte(regval);
		LCD_CS=1;	
	}
	//дLCD����
	//data:Ҫд���ֵ
	void LCD_WR_DATA(u16 data)
	{										    	   
		LCD_RS=1;//д����
		LCD_CS=0; 
		SPI2_ReadWriteByte(data);
		LCD_CS=1;	
	}
	//LCDдGRAM
	//RGB_Code:��ɫֵ
	void LCD_WriteRAM(u16 RGB_Code)
	{							    
		LCD_RS=1;
		LCD_CS=0;
		SPI2_ReadWriteByte(RGB_Code>>8);
		SPI2_ReadWriteByte(RGB_Code);
		LCD_CS=1;
	}
#elif(Interface==I80_16BIT)	
	#if LCD_USE_FSCM
	#include "fsmc.h"
	#endif  // LCD_USE_FSCM
	//д�Ĵ�������  //�����õ��ĺ���
	//regval:�Ĵ���ֵ  
//	void LCD_WR_REG(u8 regval)
//	{ 
//	#if !LCD_USE_FSCM
//		LCD_RS_RESET();
//		LCD_CS_RESET();
//		DATAOUT(regval);
//		LCD_WR_RESET();
//		LCD_WR_SET();
//		LCD_CS_SET();
//	#else
//		regval = regval;
//		*(uint16_t*)(LCD_FSMC_ADDR_CMD) = regval;
////		HAL_SRAM_Write_16b(&hsram1, (uint32_t*)LCD_FSMC_ADDR_CMD, (uint16_t*)&regval, 1);
//	#endif  // LCD_USE_FSCM
//	}
	#if LCD_USE_FSCM
	void LCD_WR_REG(volatile u8 regval)
	{
		regval = regval;
		LCD->LCD_REG = regval;
	}
	#endif
	//дLCD����
	//data:Ҫд���ֵ
//	void LCD_WR_DATA(u16 data)
//	{
//	#if !LCD_USE_FSCM
//		LCD_RS_SET();
//		LCD_CS_RESET();
//		DATAOUT(data);
//		LCD_WR_RESET();
//		LCD_WR_SET();
//		LCD_CS_SET();
//	#else
//		data = data;
//		*(uint16_t*)(LCD_FSMC_ADDR_DATA) = data;
////		HAL_SRAM_Write_16b(&hsram1, (uint32_t*)LCD_FSMC_ADDR_DATA, &data, 1);
//	#endif  // LCD_USE_FSCM
//	}	
	#if LCD_USE_FSCM
	void LCD_WR_DATA(volatile u16 data)
	{
		data = data;
		LCD->LCD_RAM = data;
	}
	#endif
	//LCDдGRAM
	//RGB_Code:��ɫֵ
//	void LCD_WriteRAM(u16 RGB_Code)
//	{
//	#if !LCD_USE_FSCM
//		LCD_RS_SET();
//		LCD_CS_RESET();
//		DATAOUT(RGB_Code);
//		LCD_WR_RESET();
//		LCD_WR_SET();
//		LCD_CS_SET();
//	#else
//		RGB_Code = RGB_Code;
//		*(uint16_t*)(LCD_FSMC_ADDR_DATA) = RGB_Code;
////		HAL_SRAM_Write_16b(&hsram1, (uint32_t*)LCD_FSMC_ADDR_DATA, &RGB_Code, 1);
//	#endif  // LCD_USE_FSCM
//	}
	#if LCD_USE_FSCM
	void LCD_WriteRAM(volatile u16 RGB_Code)
	{
		RGB_Code = RGB_Code;
		LCD->LCD_RAM = RGB_Code;
	}
	#endif
#else
	//д�Ĵ�������
	//regval:�Ĵ���ֵ
	void LCD_WR_REG(u8 regval)
	{ 
		LCD_RS=0;//д��ַ 
		LCD_CS=0; 
		DATAOUT(regval); 
		LCD_WR=0; 
		LCD_WR=1; 
		LCD_CS=1;	
	}
	//дLCD����
	//data:Ҫд���ֵ
	void LCD_WR_DATA(u16 data)
	{										    	   
		LCD_RS=1;
		LCD_CS=0;
		DATAOUT(data);
		LCD_WR=0;
		LCD_WR=1;
		LCD_CS=1;
	}
	//LCDдGRAM
	//RGB_Code:��ɫֵ
	void LCD_WriteRAM(u16 RGB_Code)
	{							    
		LCD_RS=1;
		LCD_CS=0;
		DATAOUT(RGB_Code>>8);
		LCD_WR=0;
		LCD_WR=1;
		DATAOUT(RGB_Code);
		LCD_WR=0;
		LCD_WR=1;
		LCD_CS=1;
	}
#endif

//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD_WR_REG(lcddev.wramcmd);
}	 
  
//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(Xpos>>8); 
	LCD_WR_DATA(Xpos&0XFF);
	LCD_WR_DATA(Xpos>>8); 
	LCD_WR_DATA(Xpos&0XFF);
  	
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(Ypos>>8); 
	LCD_WR_DATA(Ypos&0XFF);
	LCD_WR_DATA(Ypos>>8); 
	LCD_WR_DATA(Ypos&0XFF);
} 		 
//����LCD���Զ�ɨ�跽��
//dir:0~7,����8������(���嶨���lcd.h)   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u8 dirreg=0;
//	u16 temp;  
	lcddev.scan_dir = dir;
	switch(dir)//����ת��
	{
		case 0:dir=6;break;
		case 1:dir=7;break;
		case 2:dir=4;break;
		case 3:dir=5;break;
		case 4:dir=1;break;
		case 5:dir=0;break;
		case 6:dir=3;break;
		case 7:dir=2;break;	     
	}
	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36; 
  regval|=COLORORDER;	//0x08 0x00  ������ɫ����ͨ�������޸�
	LCD_WR_REG(dirreg);
	LCD_WR_DATA(regval);
			
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
		
  	
}      

//����LCD��ʾ����6804��֧�ֺ�����ʾ��
//dir:0,������1,����
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			 
	{
		lcddev.dir=0;	//����
		lcddev.width=LCD_WIDTH;
		lcddev.height=LCD_HEIGHT;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=D2U_R2L;	    //����-�趨��ʾ����	

	}else 				  //����
	{	  				
		lcddev.dir=1;	 
		lcddev.width=LCD_HEIGHT;
		lcddev.height=LCD_WIDTH;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=L2R_D2U;     //����-�趨��ʾ����	
		
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}
//��������
//color:Ҫ���������ɫ
void LCD_Clear(u16 color)
{
  u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 	//�õ��ܵ���
  LCD_Set_Window(0,0,lcddev.width,lcddev.height);
	LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
  
	for(index=0; index<totalpoint; index++)
	{
		LCD_WriteRAM(color);
	}
	LCD_Set_Background_Color(color);

}  
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	xlen=ex-sx+1;	   
	for(i=sy;i<=ey;i++)
	{
	 	LCD_SetCursor(sx,i);      				//���ù��λ�� 
		LCD_WriteRAM_Prepare();     			//��ʼд��GRAM	  
		for(j=0;j<xlen;j++)LCD_WriteRAM(color);	//���ù��λ�� 	    
	}
}  
//��ָ�����������ָ����ɫ��			 
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 		//�õ����Ŀ��
	height=ey-sy+1;		//�߶�
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM
		for(j=0;j<width;j++)LCD_WriteRAM(color[i*height+j]);//д������ 
	}	  
}  
//����
//x,y:����
//color: �����ɫ(32λ��ɫ,�������LTDC)
void LCD_DrawPoint(u16 x,u16 y, uint32_t color)
{
	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD_WriteRAM(color); 
}
//����
//x1,y1:�������
//x2,y2:�յ�����  
//color:��ɫ
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b,color);             //5
 		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-a,y0+b,color);             //1       
 		LCD_DrawPoint(x0-b,y0+a,color);             
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
  		LCD_DrawPoint(x0-b,y0-a,color);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//chr:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
//color : �ַ�����ɫ;
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{  							  
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';    /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* ����1206���� */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* ����1608���� */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* ��ȡ�ַ��ĵ������� */

        for (t1 = 0; t1 < 8; t1++)   /* һ���ֽ�8���� */
        {
            if (temp & 0x80)        /* ��Ч��,��Ҫ��ʾ */
            {
                LCD_DrawPoint(x, y, color);        /* �������,Ҫ��ʾ����� */
            }
            else if (mode == 0)     /* ��Ч��,����ʾ */
            {
                LCD_DrawPoint(x, y, BACK_COLOR); /* ������ɫ,�൱������㲻��ʾ(ע�ⱳ��ɫ��ȫ�ֱ�������) */
            }

            temp <<= 1; /* ��λ, �Ա��ȡ��һ��λ��״̬ */
            y++;

            if (y >= lcddev.height)return;  /* �������� */

            if ((y - y0) == size)   /* ��ʾ��һ����? */
            {
                y = y0; /* y���긴λ */
                x++;    /* x������� */

                if (x >= lcddev.width)return;   /* x���곬������ */

                break;
            }
        }
    }
}   
//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 *p,u8 size,u16 color)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,0,color);
        x+=size/2;
        p++;
    }  
}
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0,color);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0,color); 
	}
} 
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 color)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01,color);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01,color);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01,color); 
	}
} 

void LCD_Set_Background_Color(u16 color)
{
	BACK_COLOR = color;
}

u16 LCD_Get_Width(void)
{
	return lcddev.width;
}

u16 LCD_Get_Height(void)
{
	return lcddev.height;
}

u8 LCD_Get_Dir(void)
{
	return lcddev.dir;
}

u8 LCD_Get_Scan_Dir(void)
{
	return lcddev.scan_dir;
}

//���ô���
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{   
	width=sx+width-1;
	height=sy+height-1;

	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(sx>>8);  
	LCD_WR_DATA(sx&0XFF);	  
	LCD_WR_DATA(width>>8);   
	LCD_WR_DATA(width&0XFF);   
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(sy>>8);   
	LCD_WR_DATA(sy&0XFF);  
	LCD_WR_DATA(height>>8);   
	LCD_WR_DATA(height&0XFF);  
}

//LCD��ʼ���������������
void LCD_INIT_CODE(void)
{
	//*************LCD Driver Initial **********//
	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x40);
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0x4B);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x16);
	LCD_WR_DATA(0x0F);
	
	LCD_WR_REG(0xE1);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x1A);
	LCD_WR_DATA(0x1B);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x30);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x32);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x0F);
	
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA(0xA0);
	
	LCD_WR_REG(0xB4);
	LCD_WR_DATA(0x02);
	
	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x15);
	
	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0x41);
	
	LCD_WR_REG(0xC5);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x30);
	LCD_WR_DATA(0x80);
	
	LCD_WR_REG(0xB6);
	LCD_WR_DATA(0x02);
	
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x48);   
	
	LCD_WR_REG(0x3a);    
	LCD_WR_DATA(0x55);
	
	LCD_WR_REG(0xE9);
	LCD_WR_DATA(0x00);  
	
	LCD_WR_REG(0XF7);
	LCD_WR_DATA(0xA9);
	LCD_WR_DATA(0x51);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x82);
	
	LCD_WR_REG(0x11);
	delay_ms(120);
	LCD_WR_REG(0x29);
}

//��ʼ��lcd
void LCD_Init(void)
{
#if !LCD_USE_FSCM
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
	
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif  // LCD_USE_FSCM
	
	//LCD��λ 
	LCD_RST_SET();
	delay_ms(10);
	LCD_RST_RESET();
	delay_ms(20);
	LCD_RST_SET();
	delay_ms(20);
#if !LCD_USE_FSCM
	LCD_RD_SET();
#endif  // LCD_USE_FSCM
  
	LCD_INIT_CODE();

	LCD_Display_Dir(Landscape);		 	//1:������0:����   �������������л�
	LCD_Clear(WHITE);//ˢ��
	
	LCD_BL_SET();
	
}  




















