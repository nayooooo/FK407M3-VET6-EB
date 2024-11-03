/***
	************************************************************************************************************************************************************************************************
	*	@version V1.0
	*  @date    2023-5-18
	*	@author  反客科技
	*	@brief   SPI驱动显示屏，屏幕控制器 ST7789
   **********************************************************************************************************************************************************************************************
   *  @description
	*
	*	实验平台：反客STM32F407VET6核心板 （型号：FK407M3-VET6）+ 1.69寸液晶模块（型号：SPI169M1-240*280）
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*
>>>>> 重要说明：
	*
	*  1.屏幕配置为16位RGB565格式
	*  2.SPI通信速度为 21M  
   *
>>>>> 其他说明：
	*
	*	1. 中文字库使用的是小字库，即用到了对应的汉字再去取模，用户可以根据需求自行增添或删减
	*	2. 各个函数的功能和使用可以参考函数的说明
	*
	*********************************************************************************************************************************************************************************************FANKE*****
***/

#include "lcd_spi_169.h"

//SPI_HandleTypeDef hspi3;			// SPI_HandleTypeDef 结构体变量
#include "spi.h"

#define  LCD_SPI hspi3           // SPI局部宏，方便修改和移植


struct	//LCD相关参数结构体
{
	uint32_t Color;  				//	LCD当前画笔颜色
	uint32_t BackColor;			//	背景色
   uint8_t  ShowNum_Mode;		// 数字显示模式
	uint8_t  Direction;			//	显示方向
   uint16_t Width;            // 屏幕像素长度
   uint16_t Height;           // 屏幕像素宽度	
   uint8_t  X_Offset;         // X坐标偏移，用于设置屏幕控制器的显存写入方式
   uint8_t  Y_Offset;         // Y坐标偏移，用于设置屏幕控制器的显存写入方式
}LCD;

/*****************************************************************************************
*	函 数 名: LCD_WriteCMD
*	入口参数: CMD - 需要写入的控制指令
*	返 回 值: 无
*	函数功能: 用于写入控制字
*	说    明: 无
******************************************************************************************/

void  LCD_WriteCommand(uint8_t lcd_command)
{

	while((LCD_SPI.Instance->SR & 0x0080) != RESET); 	// 先判断SPI是否空闲，等待通信完成
	
	LCD_DC_Command;	//	DC引脚输出低，代表写指令	

	(&LCD_SPI)->Instance->DR = lcd_command;		// 发送数据
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// 等待发送缓冲区清空
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	等待通信完成

	LCD_DC_Data;	//	DC引脚输出高，代表写数据		
}

/****************************************************************************************************************************************
*	函 数 名: LCD_WriteData_8bit
*
*	入口参数: lcd_data - 需要写入的数据，8位
*
*	函数功能: 写入8位数据
*	
****************************************************************************************************************************************/

void  LCD_WriteData_8bit(uint8_t lcd_data)
{
	LCD_SPI.Instance->DR = lcd_data;									// 发送数据
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// 等待发送缓冲区清空
}

/****************************************************************************************************************************************
*	函 数 名: LCD_WriteData_16bit
*
*	入口参数: lcd_data - 需要写入的数据，16位
*
*	函数功能: 写入16位数据
*	
****************************************************************************************************************************************/

void  LCD_WriteData_16bit(uint16_t lcd_data)
{
	LCD_SPI.Instance->DR = lcd_data>>8;								// 发送数据，高8位
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// 等待发送缓冲区清空
	LCD_SPI.Instance->DR = lcd_data;									// 发送数据，低8位
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// 等待发送缓冲区清空
}

/****************************************************************************************************************************************
*	函 数 名: LCD_WriteBuff
*
*	入口参数: DataBuff - 数据区，DataSize - 数据长度
*
*	函数功能: 批量写入数据到屏幕
*	
****************************************************************************************************************************************/

void  LCD_WriteBuff(uint16_t *DataBuff, uint16_t DataSize)
{
	uint32_t i;

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// 关闭SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // 切换成16位数据格式
   LCD_SPI.Instance->CR1 |= 0x0040;					// 使能SPI
	
	LCD_CS_L;	// 片选拉低，使能IC
	
	for(i=0;i<DataSize;i++)				
	{

		LCD_SPI.Instance->DR = DataBuff[i];
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// 等待发送缓冲区清空

	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	等待通信完成
	LCD_CS_H;	// 片选拉高	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// 关闭SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// 切换成8位数据格式
   LCD_SPI.Instance->CR1 |= 0x0040;	// 使能SPI	
}

/****************************************************************************************************************************************
*	函 数 名: SPI_LCD_Init
*
*	函数功能: 初始化SPI以及屏幕控制器的各种参数
*	
****************************************************************************************************************************************/

void SPI_LCD_Init(void)
{
//   MX_SPI3_Init();               // 初始化SPI和控制引脚
	
	{
		
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		GPIO_LCD_CS_CLK;					// 使能 CS           引脚时钟	
	  GPIO_LCD_Backlight_CLK;   		// 使能 背光         引脚时钟
	  GPIO_LCD_DC_CLK;          		// 使能 数据指令选择 引脚时钟
		

	// 初始化片选引脚，使用硬件 SPI 片选

		GPIO_InitStruct.Pin 		= LCD_CS_PIN;						// 背光 引脚
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// 推挽输出模式
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;					   // 无上下拉
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_HIGH;			// 速度等级高
		HAL_GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);			// 初始化  

	// 初始化 背光 引脚  
		GPIO_InitStruct.Pin 		= LCD_Backlight_PIN;				// 背光 引脚
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// 推挽输出模式
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;					   // 无上下拉
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;			// 速度等级低
		HAL_GPIO_Init(LCD_Backlight_PORT, &GPIO_InitStruct);	// 初始化  

	// 初始化 数据指令选择 引脚  
		GPIO_InitStruct.Pin 		= LCD_DC_PIN;				      // 数据指令选择 引脚
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// 推挽输出模式
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;						// 无上下拉
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_HIGH;			// 速度等级高
		HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStruct);	      // 初始化  
		
		LCD_DC_Data;			// DC引脚拉高，默认处于写数据状态
		LCD_CS_H;				// 拉高片选，禁止通信
		LCD_Backlight_OFF;  	// 先关闭背光，初始化完成之后再打开	
	}

   HAL_Delay(10);               // 屏幕刚完成复位时（包括上电复位），需要等待5ms才能发送指令

	LCD_CS_L;	// 片选拉低，使能IC，开始通信

 	LCD_WriteCommand(0x36);       // 显存访问控制 指令，用于设置访问显存的方式
	LCD_WriteData_8bit(0x00);     // 配置成 从上到下、从左到右，RGB像素格式

	LCD_WriteCommand(0x3A);			// 接口像素格式 指令，用于设置使用 12位、16位还是18位色
	LCD_WriteData_8bit(0x05);     // 此处配置成 16位 像素格式

// 接下来很多都是电压设置指令，直接使用厂家给设定值
 	LCD_WriteCommand(0xB2);			
	LCD_WriteData_8bit(0x0C);
	LCD_WriteData_8bit(0x0C); 
	LCD_WriteData_8bit(0x00); 
	LCD_WriteData_8bit(0x33); 
	LCD_WriteData_8bit(0x33); 			

	LCD_WriteCommand(0xB7);		   // 栅极电压设置指令	
	LCD_WriteData_8bit(0x35);     // VGH = 13.26V，VGL = -10.43V

	LCD_WriteCommand(0xBB);			// 公共电压设置指令
	LCD_WriteData_8bit(0x19);     // VCOM = 1.35V

	LCD_WriteCommand(0xC0);
	LCD_WriteData_8bit(0x2C);

	LCD_WriteCommand(0xC2);       // VDV 和 VRH 来源设置
	LCD_WriteData_8bit(0x01);     // VDV 和 VRH 由用户自由配置

	LCD_WriteCommand(0xC3);			// VRH电压 设置指令  
	LCD_WriteData_8bit(0x12);     // VRH电压 = 4.6+( vcom+vcom offset+vdv)
				
	LCD_WriteCommand(0xC4);		   // VDV电压 设置指令	
	LCD_WriteData_8bit(0x20);     // VDV电压 = 0v

	LCD_WriteCommand(0xC6); 		// 正常模式的帧率控制指令
	LCD_WriteData_8bit(0x0F);   	// 设置屏幕控制器的刷新帧率为60帧    

	LCD_WriteCommand(0xD0);			// 电源控制指令
	LCD_WriteData_8bit(0xA4);     // 无效数据，固定写入0xA4
	LCD_WriteData_8bit(0xA1);     // AVDD = 6.8V ，AVDD = -4.8V ，VDS = 2.3V

	LCD_WriteCommand(0xE0);       // 正极电压伽马值设定
	LCD_WriteData_8bit(0xD0);
	LCD_WriteData_8bit(0x04);
	LCD_WriteData_8bit(0x0D);
	LCD_WriteData_8bit(0x11);
	LCD_WriteData_8bit(0x13);
	LCD_WriteData_8bit(0x2B);
	LCD_WriteData_8bit(0x3F);
	LCD_WriteData_8bit(0x54);
	LCD_WriteData_8bit(0x4C);
	LCD_WriteData_8bit(0x18);
	LCD_WriteData_8bit(0x0D);
	LCD_WriteData_8bit(0x0B);
	LCD_WriteData_8bit(0x1F);
	LCD_WriteData_8bit(0x23);

	LCD_WriteCommand(0xE1);      // 负极电压伽马值设定
	LCD_WriteData_8bit(0xD0);
	LCD_WriteData_8bit(0x04);
	LCD_WriteData_8bit(0x0C);
	LCD_WriteData_8bit(0x11);
	LCD_WriteData_8bit(0x13);
	LCD_WriteData_8bit(0x2C);
	LCD_WriteData_8bit(0x3F);
	LCD_WriteData_8bit(0x44);
	LCD_WriteData_8bit(0x51);
	LCD_WriteData_8bit(0x2F);
	LCD_WriteData_8bit(0x1F);
	LCD_WriteData_8bit(0x1F);
	LCD_WriteData_8bit(0x20);
	LCD_WriteData_8bit(0x23);

	LCD_WriteCommand(0x21);       // 打开反显，因为面板是常黑型，操作需要反过来

 // 退出休眠指令，LCD控制器在刚上电、复位时，会自动进入休眠模式 ，因此操作屏幕之前，需要退出休眠  
	LCD_WriteCommand(0x11);       // 退出休眠 指令
   HAL_Delay(120);               // 需要等待120ms，让电源电压和时钟电路稳定下来

 // 打开显示指令，LCD控制器在刚上电、复位时，会自动关闭显示 
	LCD_WriteCommand(0x29);       // 打开显示   	

	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	等待通信完成
	LCD_CS_H;	// 片选拉高		

// 以下进行一些驱动的默认设置
   LCD_SetDirection(Direction_H_Flip);  	      //	设置显示方向
	LCD_SetBackColor(LCD_BLACK);           // 设置背景色
   LCD_SetColor(LCD_WHITE);               // 设置画笔色  
	LCD_Clear();                           // 清屏

// 全部设置完毕之后，打开背光	
   LCD_Backlight_ON;  // 引脚输出高电平点亮背光
	
}

/****************************************************************************************************************************************
*	函 数 名:	 LCD_SetAddress
*
*	入口参数:	 x1 - 起始水平坐标   y1 - 起始垂直坐标  
*              x2 - 终点水平坐标   y2 - 终点垂直坐标	   
*	
*	函数功能:   设置需要显示的坐标区域		 			 
*****************************************************************************************************************************************/

void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)		
{
	LCD_CS_L;	// 片选拉低，使能IC
	
	LCD_WriteCommand(0x2a);			//	列地址设置，即X坐标
	LCD_WriteData_16bit(x1+LCD.X_Offset);
	LCD_WriteData_16bit(x2+LCD.X_Offset);

	LCD_WriteCommand(0x2b);			//	行地址设置，即Y坐标
	LCD_WriteData_16bit(y1+LCD.Y_Offset);
	LCD_WriteData_16bit(y2+LCD.Y_Offset);

	LCD_WriteCommand(0x2c);			//	开始写入显存，即要显示的颜色数据
	
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	等待通信完成
	LCD_CS_H;	// 片选拉高		
}

/****************************************************************************************************************************************
*	函 数 名:	LCD_SetColor
*
*	入口参数:	Color - 要显示的颜色，示例：0x0000FF 表示蓝色
*
*	函数功能:	此函数用于设置画笔的颜色，例如显示字符、画点画线、绘图的颜色
*
*	说    明:	1. 为了方便用户使用自定义颜色，入口参数 Color 使用24位 RGB888的颜色格式，用户无需关心颜色格式的转换
*					2. 24位的颜色中，从高位到低位分别对应 R、G、B  3个颜色通道
*
*****************************************************************************************************************************************/

void LCD_SetColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //各个颜色通道的值

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // 转换成 16位 的RGB565颜色
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	LCD.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value);  // 将颜色写入全局LCD参数		
}

void LCD_SetColor_RGB565(uint16_t Color)
{
	LCD.Color = Color;
}

/****************************************************************************************************************************************
*	函 数 名:	LCD_SetBackColor
*
*	入口参数:	Color - 要显示的颜色，示例：0x0000FF 表示蓝色
*
*	函数功能:	设置背景色,此函数用于清屏以及显示字符的背景色
*
*	说    明:	1. 为了方便用户使用自定义颜色，入口参数 Color 使用24位 RGB888的颜色格式，用户无需关心颜色格式的转换
*					2. 24位的颜色中，从高位到低位分别对应 R、G、B  3个颜色通道
*
*****************************************************************************************************************************************/

void LCD_SetBackColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //各个颜色通道的值

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // 转换成 16位 的RGB565颜色
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	LCD.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value);	// 将颜色写入全局LCD参数			   	
}

/****************************************************************************************************************************************
*	函 数 名:	LCD_SetDirection
*
*	入口参数:	direction - 要显示的方向
*
*	函数功能:	设置要显示的方向
*
*	说    明:   1. 可输入参数 Direction_H 、Direction_V 、Direction_H_Flip 、Direction_V_Flip        
*              2. 使用示例 LCD_DisplayDirection(Direction_H) ，即设置屏幕横屏显示
*
*****************************************************************************************************************************************/

void LCD_SetDirection(uint8_t direction)
{
	LCD.Direction = direction;    // 写入全局LCD参数

	LCD_CS_L;	// 片选拉低，使能IC
			
  if( direction == Direction_H )   // 横屏显示
   {
      LCD_WriteCommand(0x36);    		// 显存访问控制 指令，用于设置访问显存的方式
      LCD_WriteData_8bit(0x70);        // 横屏显示
      LCD.X_Offset   = 20;             // 设置控制器坐标偏移量
      LCD.Y_Offset   = 0;   
      LCD.Width      = LCD_Height;		// 重新赋值长、宽
      LCD.Height     = LCD_Width;		
   }
   else if( direction == Direction_V )
   {
      LCD_WriteCommand(0x36);    		// 显存访问控制 指令，用于设置访问显存的方式
      LCD_WriteData_8bit(0x00);        // 垂直显示
      LCD.X_Offset   = 0;              // 设置控制器坐标偏移量
      LCD.Y_Offset   = 20;     
      LCD.Width      = LCD_Width;		// 重新赋值长、宽
      LCD.Height     = LCD_Height;						
   }
   else if( direction == Direction_H_Flip )
   {
      LCD_WriteCommand(0x36);   			 // 显存访问控制 指令，用于设置访问显存的方式
      LCD_WriteData_8bit(0xA0);         // 横屏显示，并上下翻转，RGB像素格式
      LCD.X_Offset   = 20;              // 设置控制器坐标偏移量
      LCD.Y_Offset   = 0;      
      LCD.Width      = LCD_Height;		 // 重新赋值长、宽
      LCD.Height     = LCD_Width;				
   }
   else if( direction == Direction_V_Flip )
   {
      LCD_WriteCommand(0x36);    		// 显存访问控制 指令，用于设置访问显存的方式
      LCD_WriteData_8bit(0xC0);        // 垂直显示 ，并上下翻转，RGB像素格式
      LCD.X_Offset   = 0;              // 设置控制器坐标偏移量
      LCD.Y_Offset   = 20;     
      LCD.Width      = LCD_Width;		// 重新赋值长、宽
      LCD.Height     = LCD_Height;				
   }      
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	等待通信完成
	LCD_CS_H;	// 片选拉高			
}


/****************************************************************************************************************************************
*	函 数 名:	LCD_Clear
*
*	函数功能:	清屏函数，将LCD清除为 LCD.BackColor 的颜色
*
*	说    明:	先用 LCD_SetBackColor() 设置要清除的背景色，再调用该函数清屏即可
*
*****************************************************************************************************************************************/

void LCD_Clear(void)
{
	uint32_t i;

	LCD_SetAddress(0,0,LCD.Width-1,LCD.Height-1);			//设置坐标	

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// 关闭SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // 切换成16位数据格式
   LCD_SPI.Instance->CR1 |= 0x0040;					// 使能SPI
	
	LCD_CS_L;	// 片选拉低，使能IC
	
	for(i=0;i<LCD.Width*LCD.Height;i++)				
	{

		LCD_SPI.Instance->DR = LCD.BackColor;
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// 等待发送缓冲区清空
	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	等待通信完成
	LCD_CS_H;	// 片选拉高	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// 关闭SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// 切换成8位数据格式
   LCD_SPI.Instance->CR1 |= 0x0040;	// 使能SPI
}


/****************************************************************************************************************************************
*	函 数 名:	LCD_DrawPoint
*
*	入口参数:	x - 起始水平坐标
*					y - 起始垂直坐标
*					color  - 要绘制的颜色，使用 24位 RGB888 的颜色格式，用户无需关心颜色格式的转换
*
*	函数功能:	在指定坐标绘制指定颜色的点
*
*	说    明:	使用示例 LCD_DrawPoint( 10, 10, 0x0000FF) ，在坐标(10,10)绘制蓝色的点
*
*****************************************************************************************************************************************/

void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{
	LCD_SetAddress(x,y,x,y);	//	设置坐标 
	
	LCD_CS_L;	// 片选拉低，使能IC

	LCD_WriteData_16bit(LCD.Color)	;
	
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	等待通信完成
	LCD_CS_H;	// 片选拉高		
} 

void LCD_DrawPoint_LVGL(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_SetAddress(x,y,x,y);
	
	LCD_CS_L;
	
	LCD_WriteData_16bit(color);
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);
	
	LCD_CS_H;
}




/**************************************************************************************************************************************************************************************************************************************反客 STM32F4核心板******************************FANKE***/
// 实验平台：反客 STM32F4核心板
//


