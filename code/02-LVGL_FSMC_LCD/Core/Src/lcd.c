/**
 ****************************************************************************************************
 * @file        lcd.c
 * @author      EYA-DISPLAY
 * @version     V2.0
 * @date        2022-04-28
 * @brief       液晶屏驱动Demo
 * @license     Copyright (c) 2022-2032, 亦亞徽科技集团(广东)
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:EYA-ETmcu开发板
 * 公司网址:www.eya-display.com
 *
 ****************************************************************************************************
 **/
#include "lcd.h"
#include "stdlib.h"
#include "font.h"

#define delay_ms(nms) HAL_Delay(nms)


//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
	while(i--);
}

//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 


u8 DFT_SCAN_DIR; //扫描方向变量
//////////////////////////////////////////////////////////////////////////////////	 
extern const u8 gImage_logo[];   //图片数据(包含信息头),存储在image1.c里面.				 
//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;


#if(Interface==D4WSPI)
	//写寄存器函数  SPI接口需要用到的函数
	//regval:寄存器值
	void LCD_WR_REG(u8 regval)
	{ 
		LCD_RS=0;//写地址 
		LCD_CS=0; 
		SPI2_ReadWriteByte(regval);
		LCD_CS=1;	
	}
	//写LCD数据
	//data:要写入的值
	void LCD_WR_DATA(u16 data)
	{										    	   
		LCD_RS=1;//写参数
		LCD_CS=0; 
		SPI2_ReadWriteByte(data);
		LCD_CS=1;	
	}
	//LCD写GRAM
	//RGB_Code:颜色值
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
	//写寄存器函数  //并口用到的函数
	//regval:寄存器值  
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
	//写LCD数据
	//data:要写入的值
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
	//LCD写GRAM
	//RGB_Code:颜色值
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
	//写寄存器函数
	//regval:寄存器值
	void LCD_WR_REG(u8 regval)
	{ 
		LCD_RS=0;//写地址 
		LCD_CS=0; 
		DATAOUT(regval); 
		LCD_WR=0; 
		LCD_WR=1; 
		LCD_CS=1;	
	}
	//写LCD数据
	//data:要写入的值
	void LCD_WR_DATA(u16 data)
	{										    	   
		LCD_RS=1;
		LCD_CS=0;
		DATAOUT(data);
		LCD_WR=0;
		LCD_WR=1;
		LCD_CS=1;
	}
	//LCD写GRAM
	//RGB_Code:颜色值
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

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD_WR_REG(lcddev.wramcmd);
}	 
  
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
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
//设置LCD的自动扫描方向
//dir:0~7,代表8个方向(具体定义见lcd.h)   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u8 dirreg=0;
//	u16 temp;  
	lcddev.scan_dir = dir;
	switch(dir)//方向转换
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
		case L2R_U2D://从左到右,从上到下
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://从下到上,从右到左
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36; 
  regval|=COLORORDER;	//0x08 0x00  红蓝反色可以通过这里修改
	LCD_WR_REG(dirreg);
	LCD_WR_DATA(regval);
			
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
		
  	
}      

//设置LCD显示方向（6804不支持横屏显示）
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			 
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=LCD_WIDTH;
		lcddev.height=LCD_HEIGHT;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=D2U_R2L;	    //竖显-设定显示方向	

	}else 				  //横屏
	{	  				
		lcddev.dir=1;	 
		lcddev.width=LCD_HEIGHT;
		lcddev.height=LCD_WIDTH;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=L2R_D2U;     //横显-设定显示方向	
		
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}
//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
  u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 	//得到总点数
  LCD_Set_Window(0,0,lcddev.width,lcddev.height);
	LCD_WriteRAM_Prepare();     //开始写入GRAM	
  
	for(index=0; index<totalpoint; index++)
	{
		LCD_WriteRAM(color);
	}
	LCD_Set_Background_Color(color);

}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	xlen=ex-sx+1;	   
	for(i=sy;i<=ey;i++)
	{
	 	LCD_SetCursor(sx,i);      				//设置光标位置 
		LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
		for(j=0;j<xlen;j++)LCD_WriteRAM(color);	//设置光标位置 	    
	}
}  
//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 		//得到填充的宽度
	height=ey-sy+1;		//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD_WriteRAM(color[i*height+j]);//写入数据 
	}	  
}  
//画点
//x,y:坐标
//color: 点的颜色(32位颜色,方便兼容LTDC)
void LCD_DrawPoint(u16 x,u16 y, uint32_t color)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD_WriteRAM(color); 
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
//color:颜色
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol,color);//画点 
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
//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
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
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//在指定位置显示一个字符
//x,y:起始坐标
//chr:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
//color : 字符的颜色;
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{  							  
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';    /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* 调用1206字体 */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* 调用1608字体 */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* 获取字符的点阵数据 */

        for (t1 = 0; t1 < 8; t1++)   /* 一个字节8个点 */
        {
            if (temp & 0x80)        /* 有效点,需要显示 */
            {
                LCD_DrawPoint(x, y, color);        /* 画点出来,要显示这个点 */
            }
            else if (mode == 0)     /* 无效点,不显示 */
            {
                LCD_DrawPoint(x, y, BACK_COLOR); /* 画背景色,相当于这个点不显示(注意背景色由全局变量控制) */
            }

            temp <<= 1; /* 移位, 以便获取下一个位的状态 */
            y++;

            if (y >= lcddev.height)return;  /* 超区域了 */

            if ((y - y0) == size)   /* 显示完一列了? */
            {
                y = y0; /* y坐标复位 */
                x++;    /* x坐标递增 */

                if (x >= lcddev.width)return;   /* x坐标超区域了 */

                break;
            }
        }
    }
}   
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 *p,u8 size,u16 color)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0,color);
        x+=size/2;
        p++;
    }  
}
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
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
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
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

//设置窗口
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

//LCD初始化放在这个函数中
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

//初始化lcd
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
	
	//LCD复位 
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

	LCD_Display_Dir(Landscape);		 	//1:竖屏；0:横屏   横竖屏从这里切换
	LCD_Clear(WHITE);//刷白
	
	LCD_BL_SET();
	
}  




















