/**
 ****************************************************************************************************
 * @file        lcd.h
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
#ifndef __LCD_H
#define __LCD_H		
#include "main.h"


/*****************************************��Ҫ�޸ĵĵط� 1 ***************************/
#define LCD_WIDTH  320
#define LCD_HEIGHT 480   //������Ļ��Ӧ�޸ķֱ��� 

/*****************************************��Ҫ�޸ĵĵط� 2 ***************************/

//����ӿڷ�ʽ
#define I80_16BIT  0x0
#define I80_H8BIT  0x1
#define I80_L8BIT  0x2
#define D4WSPI     0x3

#define Interface  I80_16BIT    //I80_16BIT//I80_H8BIT//I80_L8BIT//D4WSPI  //��Ӧ��ʾ���Ľӿ�
//#define Interface  I80_H8BIT
//#define Interface  I80_L8BIT
//#define Interface  D4WSPI

/*****************************************��Ҫ�޸ĵĵط� 3 ***************************/
//�������޸���LCD.C�� LCD_INIT_CODE()  30�п�ʼ

/*****************************************��Ҫ�޸ĵĵط� 4 ***************************/
//�������л�
#define Landscape  1 //1:����ʾ  0������ʾ  PS:��������FPC�ĳ��߷�������  
	 
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���	

/*****************************************��Ҫ�޸ĵĵط� 5 ***************************/
#define RGB 0x00
#define BGR 0x08
#define COLORORDER BGR  //RGB BGR�л��������ʾ��ɫ����ɫ���ˣ�����ͨ���л��������������

#define FAST     1  //����ˢͼ  1:����ˢͼ  0����ˢ   ��ˢ����ˢ��24%

//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������
	u8  scan_dir;		//ɨ�跽��
	u8	wramcmd;		//��ʼдgramָ��
	u8  setxcmd;		//����x����ָ��
	u8  setycmd;		//����y����ָ��	 
}_lcd_dev; 	  

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ
//////////////////////////////////////////////////////////////////////////////////	 
//-----------------LCD�˿ڶ���---------------- 
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
//LCD��ַ�ṹ��
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

//������ɫ
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
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
//#define LIGHTGRAY      0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	    															  
void LCD_Clear(u16 Color);	 												//����
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				//��䵥ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				//���ָ����ɫ

void LCD_SetCursor(u16 Xpos, u16 Ypos);										//���ù��
void LCD_DrawPoint(u16 x,u16 y, uint32_t color);							//����

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);				//����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);		   	//������
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color);							//��Բ

void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);						//��ʾһ���ַ�
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 *p,u8 size,u16 color);		//��ʾһ���ַ���,12/16����
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color);  					//��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 color);			//��ʾ ����

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
void LCD_Scan_Dir(u8 dir);							//������ɨ�跽��
void LCD_Display_Dir(u8 dir);						//������Ļ��ʾ����
void LCD_Init(void);								//��ʼ��
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
	 
	 



