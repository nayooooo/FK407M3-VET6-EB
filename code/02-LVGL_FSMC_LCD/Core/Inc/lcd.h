/**
 ****************************************************************************************************
 * @file        lcd.h
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
#ifndef __LCD_H
#define __LCD_H		
#include "main.h"


/*****************************************需要修改的地方 1 ***************************/
#define LCD_WIDTH  320
#define LCD_HEIGHT 480   //更换屏幕对应修改分辨率 

/*****************************************需要修改的地方 2 ***************************/

//定义接口方式
#define I80_16BIT  0x0
#define I80_H8BIT  0x1
#define I80_L8BIT  0x2
#define D4WSPI     0x3

#define Interface  I80_16BIT    //I80_16BIT//I80_H8BIT//I80_L8BIT//D4WSPI  //对应显示屏的接口
//#define Interface  I80_H8BIT
//#define Interface  I80_L8BIT
//#define Interface  D4WSPI

/*****************************************需要修改的地方 3 ***************************/
//第三处修改在LCD.C中 LCD_INIT_CODE()  30行开始

/*****************************************需要修改的地方 4 ***************************/
//横竖屏切换
#define Landscape  1 //1:横显示  0：竖显示  PS:横竖按照FPC的出线方向来看  
	 
//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	

/*****************************************需要修改的地方 5 ***************************/
#define RGB 0x00
#define BGR 0x08
#define COLORORDER BGR  //RGB BGR切换，如果显示红色和蓝色反了，可以通过切换这个开关来调整

#define FAST     1  //快速刷图  1:快速刷图  0：慢刷   快刷比慢刷快24%

//LCD重要参数集
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。
	u8  scan_dir;		//扫描方向
	u8	wramcmd;		//开始写gram指令
	u8  setxcmd;		//设置x坐标指令
	u8  setycmd;		//设置y坐标指令	 
}_lcd_dev; 	  

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
//LCD的画笔颜色和背景色	   
extern u16  POINT_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色
//////////////////////////////////////////////////////////////////////////////////	 
//-----------------LCD端口定义---------------- 
#define LCD_USE_FSCM		(1)

#define LCD_BL_SET()				LCD_BL_GPIO_Port->BSRR = LCD_BL_Pin
#define LCD_BL_RESET()				LCD_BL_GPIO_Port->BSRR = LCD_BL_Pin << 16
#define LCD_RST_SET()				LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin
#define LCD_RST_RESET()				LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin << 16

#if !LCD_USE_FSCM
#define LCD_RS_SET()				GPIOD->BSRR = GPIO_PIN_11
#define LCD_RS_RESET()				GPIOD->BSRR = GPIO_PIN_11 << 16
#define LCD_CS_SET()				GPIOD->BSRR = GPIO_PIN_7
#define LCD_CS_RESET()				GPIOD->BSRR = GPIO_PIN_7 << 16
#define LCD_WR_SET()				GPIOD->BSRR = GPIO_PIN_5
#define LCD_WR_RESET()				GPIOD->BSRR = GPIO_PIN_5 << 16
#define LCD_RD_SET()				GPIOD->BSRR = GPIO_PIN_4
#define LCD_RD_RESET()				GPIOD->BSRR = GPIO_PIN_4 << 16
#else
//LCD地址结构体
typedef struct
{
	volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0001FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

#define LCD_FSMC_ADDR_CMD			((uint32_t)0x60020000)
#define LCD_FSMC_ADDR_DATA			((uint32_t)0x60020002)
#endif  // LCD_USE_FSCM 

extern u8 DFT_SCAN_DIR;

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY      0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
	    															  
void LCD_Clear(u16 Color);	 												//清屏
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				//填充单色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				//填充指定颜色

void LCD_SetCursor(u16 Xpos, u16 Ypos);										//设置光标
void LCD_DrawPoint(u16 x,u16 y, uint32_t color);							//画点

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);				//画线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);		   	//画矩形
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color);							//画圆

void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);						//显示一个字符
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 *p,u8 size,u16 color);		//显示一个字符串,12/16字体
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color);  					//显示一个数字
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 color);			//显示 数字

void LCD_Set_Background_Color(u16 color);
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);


u16 LCD_Get_Width(void);
u16 LCD_Get_Height(void);
u8 LCD_Get_Dir(void);
u8 LCD_Get_Scan_Dir(void);
//void LCD_WR_REG(u8);
//void LCD_WR_DATA(u16);
void LCD_WR_REG(volatile u8);
void LCD_WR_DATA(volatile u16);
void LCD_Scan_Dir(u8 dir);							//设置屏扫描方向
void LCD_Display_Dir(u8 dir);						//设置屏幕显示方向
void LCD_Init(void);								//初始化
void LCD_WriteRAM_Prepare(void);
//void LCD_WriteRAM(u16 RGB_Code);	
void LCD_WriteRAM(volatile u16 RGB_Code);	
#if !LCD_USE_FSCM
	#ifdef DATAOUT
		#undef DATAOUT
	#endif  // DATAOUT
	#define U16_2_GPIOx_BSRR(data, data_bit_mask, lor, left_shift_num, right_shift_num) \
				(lor? \
				 ((((uint32_t)(data&data_bit_mask))<<left_shift_num)|((uint32_t)((~(data&data_bit_mask))&data_bit_mask)<<(16+left_shift_num))): \
				 ((((uint32_t)(data&data_bit_mask))>>right_shift_num)|(((uint32_t)(((~(data&data_bit_mask))&data_bit_mask))<<16)>>right_shift_num)))
	#define U16_2_GPIOx_BSRR_2_SET_PIN_VAL(GPIOx, data, data_bit_mask, lor, left_shift_num, right_shift_num) \
				GPIOx->BSRR = U16_2_GPIOx_BSRR(data, data_bit_mask, lor, left_shift_num, right_shift_num)
	inline void DATAOUT(u16 data)
	{
		U16_2_GPIOx_BSRR_2_SET_PIN_VAL(GPIOD, data, 0x0003, 1, 14, 0);
		U16_2_GPIOx_BSRR_2_SET_PIN_VAL(GPIOD, data, 0x000C, 0, 0, 2);
		U16_2_GPIOx_BSRR_2_SET_PIN_VAL(GPIOE, data, 0x1FF0, 1, 3, 0);
		U16_2_GPIOx_BSRR_2_SET_PIN_VAL(GPIOD, data, 0xE000, 0, 0, 5);
	}
#endif
 					   																			 			  		 
#endif  
	 
	 



