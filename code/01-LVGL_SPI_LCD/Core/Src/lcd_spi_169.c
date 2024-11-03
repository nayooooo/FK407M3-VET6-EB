/***
	************************************************************************************************************************************************************************************************
	*	@version V1.0
	*  @date    2023-5-18
	*	@author  ���ͿƼ�
	*	@brief   SPI������ʾ������Ļ������ ST7789
   **********************************************************************************************************************************************************************************************
   *  @description
	*
	*	ʵ��ƽ̨������STM32F407VET6���İ� ���ͺţ�FK407M3-VET6��+ 1.69��Һ��ģ�飨�ͺţ�SPI169M1-240*280��
	*	�Ա���ַ��https://shop212360197.taobao.com
	*	QQ����Ⱥ��536665479
	*
>>>>> ��Ҫ˵����
	*
	*  1.��Ļ����Ϊ16λRGB565��ʽ
	*  2.SPIͨ���ٶ�Ϊ 21M  
   *
>>>>> ����˵����
	*
	*	1. �����ֿ�ʹ�õ���С�ֿ⣬���õ��˶�Ӧ�ĺ�����ȥȡģ���û����Ը����������������ɾ��
	*	2. ���������Ĺ��ܺ�ʹ�ÿ��Բο�������˵��
	*
	*********************************************************************************************************************************************************************************************FANKE*****
***/

#include "lcd_spi_169.h"

//SPI_HandleTypeDef hspi3;			// SPI_HandleTypeDef �ṹ�����
#include "spi.h"

#define  LCD_SPI hspi3           // SPI�ֲ��꣬�����޸ĺ���ֲ


struct	//LCD��ز����ṹ��
{
	uint32_t Color;  				//	LCD��ǰ������ɫ
	uint32_t BackColor;			//	����ɫ
   uint8_t  ShowNum_Mode;		// ������ʾģʽ
	uint8_t  Direction;			//	��ʾ����
   uint16_t Width;            // ��Ļ���س���
   uint16_t Height;           // ��Ļ���ؿ��	
   uint8_t  X_Offset;         // X����ƫ�ƣ�����������Ļ���������Դ�д�뷽ʽ
   uint8_t  Y_Offset;         // Y����ƫ�ƣ�����������Ļ���������Դ�д�뷽ʽ
}LCD;

/*****************************************************************************************
*	�� �� ��: LCD_WriteCMD
*	��ڲ���: CMD - ��Ҫд��Ŀ���ָ��
*	�� �� ֵ: ��
*	��������: ����д�������
*	˵    ��: ��
******************************************************************************************/

void  LCD_WriteCommand(uint8_t lcd_command)
{

	while((LCD_SPI.Instance->SR & 0x0080) != RESET); 	// ���ж�SPI�Ƿ���У��ȴ�ͨ�����
	
	LCD_DC_Command;	//	DC��������ͣ�����дָ��	

	(&LCD_SPI)->Instance->DR = lcd_command;		// ��������
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����

	LCD_DC_Data;	//	DC��������ߣ�����д����		
}

/****************************************************************************************************************************************
*	�� �� ��: LCD_WriteData_8bit
*
*	��ڲ���: lcd_data - ��Ҫд������ݣ�8λ
*
*	��������: д��8λ����
*	
****************************************************************************************************************************************/

void  LCD_WriteData_8bit(uint8_t lcd_data)
{
	LCD_SPI.Instance->DR = lcd_data;									// ��������
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
}

/****************************************************************************************************************************************
*	�� �� ��: LCD_WriteData_16bit
*
*	��ڲ���: lcd_data - ��Ҫд������ݣ�16λ
*
*	��������: д��16λ����
*	
****************************************************************************************************************************************/

void  LCD_WriteData_16bit(uint16_t lcd_data)
{
	LCD_SPI.Instance->DR = lcd_data>>8;								// �������ݣ���8λ
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	LCD_SPI.Instance->DR = lcd_data;									// �������ݣ���8λ
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
}

/****************************************************************************************************************************************
*	�� �� ��: LCD_WriteBuff
*
*	��ڲ���: DataBuff - ��������DataSize - ���ݳ���
*
*	��������: ����д�����ݵ���Ļ
*	
****************************************************************************************************************************************/

void  LCD_WriteBuff(uint16_t *DataBuff, uint16_t DataSize)
{
	uint32_t i;

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// �ر�SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // �л���16λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;					// ʹ��SPI
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	for(i=0;i<DataSize;i++)				
	{

		LCD_SPI.Instance->DR = DataBuff[i];
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������

	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// �ر�SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// �л���8λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;	// ʹ��SPI	
}

/****************************************************************************************************************************************
*	�� �� ��: SPI_LCD_Init
*
*	��������: ��ʼ��SPI�Լ���Ļ�������ĸ��ֲ���
*	
****************************************************************************************************************************************/

void SPI_LCD_Init(void)
{
//   MX_SPI3_Init();               // ��ʼ��SPI�Ϳ�������
	
	{
		
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		GPIO_LCD_CS_CLK;					// ʹ�� CS           ����ʱ��	
	  GPIO_LCD_Backlight_CLK;   		// ʹ�� ����         ����ʱ��
	  GPIO_LCD_DC_CLK;          		// ʹ�� ����ָ��ѡ�� ����ʱ��
		

	// ��ʼ��Ƭѡ���ţ�ʹ��Ӳ�� SPI Ƭѡ

		GPIO_InitStruct.Pin 		= LCD_CS_PIN;						// ���� ����
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;					   // ��������
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_HIGH;			// �ٶȵȼ���
		HAL_GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);			// ��ʼ��  

	// ��ʼ�� ���� ����  
		GPIO_InitStruct.Pin 		= LCD_Backlight_PIN;				// ���� ����
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;					   // ��������
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;			// �ٶȵȼ���
		HAL_GPIO_Init(LCD_Backlight_PORT, &GPIO_InitStruct);	// ��ʼ��  

	// ��ʼ�� ����ָ��ѡ�� ����  
		GPIO_InitStruct.Pin 		= LCD_DC_PIN;				      // ����ָ��ѡ�� ����
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;						// ��������
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_HIGH;			// �ٶȵȼ���
		HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStruct);	      // ��ʼ��  
		
		LCD_DC_Data;			// DC�������ߣ�Ĭ�ϴ���д����״̬
		LCD_CS_H;				// ����Ƭѡ����ֹͨ��
		LCD_Backlight_OFF;  	// �ȹرձ��⣬��ʼ�����֮���ٴ�	
	}

   HAL_Delay(10);               // ��Ļ����ɸ�λʱ�������ϵ縴λ������Ҫ�ȴ�5ms���ܷ���ָ��

	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC����ʼͨ��

 	LCD_WriteCommand(0x36);       // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
	LCD_WriteData_8bit(0x00);     // ���ó� ���ϵ��¡������ң�RGB���ظ�ʽ

	LCD_WriteCommand(0x3A);			// �ӿ����ظ�ʽ ָ���������ʹ�� 12λ��16λ����18λɫ
	LCD_WriteData_8bit(0x05);     // �˴����ó� 16λ ���ظ�ʽ

// �������ܶ඼�ǵ�ѹ����ָ�ֱ��ʹ�ó��Ҹ��趨ֵ
 	LCD_WriteCommand(0xB2);			
	LCD_WriteData_8bit(0x0C);
	LCD_WriteData_8bit(0x0C); 
	LCD_WriteData_8bit(0x00); 
	LCD_WriteData_8bit(0x33); 
	LCD_WriteData_8bit(0x33); 			

	LCD_WriteCommand(0xB7);		   // դ����ѹ����ָ��	
	LCD_WriteData_8bit(0x35);     // VGH = 13.26V��VGL = -10.43V

	LCD_WriteCommand(0xBB);			// ������ѹ����ָ��
	LCD_WriteData_8bit(0x19);     // VCOM = 1.35V

	LCD_WriteCommand(0xC0);
	LCD_WriteData_8bit(0x2C);

	LCD_WriteCommand(0xC2);       // VDV �� VRH ��Դ����
	LCD_WriteData_8bit(0x01);     // VDV �� VRH ���û���������

	LCD_WriteCommand(0xC3);			// VRH��ѹ ����ָ��  
	LCD_WriteData_8bit(0x12);     // VRH��ѹ = 4.6+( vcom+vcom offset+vdv)
				
	LCD_WriteCommand(0xC4);		   // VDV��ѹ ����ָ��	
	LCD_WriteData_8bit(0x20);     // VDV��ѹ = 0v

	LCD_WriteCommand(0xC6); 		// ����ģʽ��֡�ʿ���ָ��
	LCD_WriteData_8bit(0x0F);   	// ������Ļ��������ˢ��֡��Ϊ60֡    

	LCD_WriteCommand(0xD0);			// ��Դ����ָ��
	LCD_WriteData_8bit(0xA4);     // ��Ч���ݣ��̶�д��0xA4
	LCD_WriteData_8bit(0xA1);     // AVDD = 6.8V ��AVDD = -4.8V ��VDS = 2.3V

	LCD_WriteCommand(0xE0);       // ������ѹ٤��ֵ�趨
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

	LCD_WriteCommand(0xE1);      // ������ѹ٤��ֵ�趨
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

	LCD_WriteCommand(0x21);       // �򿪷��ԣ���Ϊ����ǳ����ͣ�������Ҫ������

 // �˳�����ָ�LCD�������ڸ��ϵ硢��λʱ�����Զ���������ģʽ ����˲�����Ļ֮ǰ����Ҫ�˳�����  
	LCD_WriteCommand(0x11);       // �˳����� ָ��
   HAL_Delay(120);               // ��Ҫ�ȴ�120ms���õ�Դ��ѹ��ʱ�ӵ�·�ȶ�����

 // ����ʾָ�LCD�������ڸ��ϵ硢��λʱ�����Զ��ر���ʾ 
	LCD_WriteCommand(0x29);       // ����ʾ   	

	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����		

// ���½���һЩ������Ĭ������
   LCD_SetDirection(Direction_H_Flip);  	      //	������ʾ����
	LCD_SetBackColor(LCD_BLACK);           // ���ñ���ɫ
   LCD_SetColor(LCD_WHITE);               // ���û���ɫ  
	LCD_Clear();                           // ����

// ȫ���������֮�󣬴򿪱���	
   LCD_Backlight_ON;  // ��������ߵ�ƽ��������
	
}

/****************************************************************************************************************************************
*	�� �� ��:	 LCD_SetAddress
*
*	��ڲ���:	 x1 - ��ʼˮƽ����   y1 - ��ʼ��ֱ����  
*              x2 - �յ�ˮƽ����   y2 - �յ㴹ֱ����	   
*	
*	��������:   ������Ҫ��ʾ����������		 			 
*****************************************************************************************************************************************/

void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)		
{
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	LCD_WriteCommand(0x2a);			//	�е�ַ���ã���X����
	LCD_WriteData_16bit(x1+LCD.X_Offset);
	LCD_WriteData_16bit(x2+LCD.X_Offset);

	LCD_WriteCommand(0x2b);			//	�е�ַ���ã���Y����
	LCD_WriteData_16bit(y1+LCD.Y_Offset);
	LCD_WriteData_16bit(y2+LCD.Y_Offset);

	LCD_WriteCommand(0x2c);			//	��ʼд���Դ棬��Ҫ��ʾ����ɫ����
	
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����		
}

/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetColor
*
*	��ڲ���:	Color - Ҫ��ʾ����ɫ��ʾ����0x0000FF ��ʾ��ɫ
*
*	��������:	�˺����������û��ʵ���ɫ��������ʾ�ַ������㻭�ߡ���ͼ����ɫ
*
*	˵    ��:	1. Ϊ�˷����û�ʹ���Զ�����ɫ����ڲ��� Color ʹ��24λ RGB888����ɫ��ʽ���û����������ɫ��ʽ��ת��
*					2. 24λ����ɫ�У��Ӹ�λ����λ�ֱ��Ӧ R��G��B  3����ɫͨ��
*
*****************************************************************************************************************************************/

void LCD_SetColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //������ɫͨ����ֵ

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // ת���� 16λ ��RGB565��ɫ
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	LCD.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value);  // ����ɫд��ȫ��LCD����		
}

void LCD_SetColor_RGB565(uint16_t Color)
{
	LCD.Color = Color;
}

/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetBackColor
*
*	��ڲ���:	Color - Ҫ��ʾ����ɫ��ʾ����0x0000FF ��ʾ��ɫ
*
*	��������:	���ñ���ɫ,�˺������������Լ���ʾ�ַ��ı���ɫ
*
*	˵    ��:	1. Ϊ�˷����û�ʹ���Զ�����ɫ����ڲ��� Color ʹ��24λ RGB888����ɫ��ʽ���û����������ɫ��ʽ��ת��
*					2. 24λ����ɫ�У��Ӹ�λ����λ�ֱ��Ӧ R��G��B  3����ɫͨ��
*
*****************************************************************************************************************************************/

void LCD_SetBackColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //������ɫͨ����ֵ

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // ת���� 16λ ��RGB565��ɫ
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	LCD.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value);	// ����ɫд��ȫ��LCD����			   	
}

/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetDirection
*
*	��ڲ���:	direction - Ҫ��ʾ�ķ���
*
*	��������:	����Ҫ��ʾ�ķ���
*
*	˵    ��:   1. ��������� Direction_H ��Direction_V ��Direction_H_Flip ��Direction_V_Flip        
*              2. ʹ��ʾ�� LCD_DisplayDirection(Direction_H) ����������Ļ������ʾ
*
*****************************************************************************************************************************************/

void LCD_SetDirection(uint8_t direction)
{
	LCD.Direction = direction;    // д��ȫ��LCD����

	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
			
  if( direction == Direction_H )   // ������ʾ
   {
      LCD_WriteCommand(0x36);    		// �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0x70);        // ������ʾ
      LCD.X_Offset   = 20;             // ���ÿ���������ƫ����
      LCD.Y_Offset   = 0;   
      LCD.Width      = LCD_Height;		// ���¸�ֵ������
      LCD.Height     = LCD_Width;		
   }
   else if( direction == Direction_V )
   {
      LCD_WriteCommand(0x36);    		// �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0x00);        // ��ֱ��ʾ
      LCD.X_Offset   = 0;              // ���ÿ���������ƫ����
      LCD.Y_Offset   = 20;     
      LCD.Width      = LCD_Width;		// ���¸�ֵ������
      LCD.Height     = LCD_Height;						
   }
   else if( direction == Direction_H_Flip )
   {
      LCD_WriteCommand(0x36);   			 // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0xA0);         // ������ʾ�������·�ת��RGB���ظ�ʽ
      LCD.X_Offset   = 20;              // ���ÿ���������ƫ����
      LCD.Y_Offset   = 0;      
      LCD.Width      = LCD_Height;		 // ���¸�ֵ������
      LCD.Height     = LCD_Width;				
   }
   else if( direction == Direction_V_Flip )
   {
      LCD_WriteCommand(0x36);    		// �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0xC0);        // ��ֱ��ʾ �������·�ת��RGB���ظ�ʽ
      LCD.X_Offset   = 0;              // ���ÿ���������ƫ����
      LCD.Y_Offset   = 20;     
      LCD.Width      = LCD_Width;		// ���¸�ֵ������
      LCD.Height     = LCD_Height;				
   }      
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����			
}


/****************************************************************************************************************************************
*	�� �� ��:	LCD_Clear
*
*	��������:	������������LCD���Ϊ LCD.BackColor ����ɫ
*
*	˵    ��:	���� LCD_SetBackColor() ����Ҫ����ı���ɫ���ٵ��øú�����������
*
*****************************************************************************************************************************************/

void LCD_Clear(void)
{
	uint32_t i;

	LCD_SetAddress(0,0,LCD.Width-1,LCD.Height-1);			//��������	

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// �ر�SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // �л���16λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;					// ʹ��SPI
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	for(i=0;i<LCD.Width*LCD.Height;i++)				
	{

		LCD_SPI.Instance->DR = LCD.BackColor;
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// �ر�SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// �л���8λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;	// ʹ��SPI
}


/****************************************************************************************************************************************
*	�� �� ��:	LCD_DrawPoint
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					color  - Ҫ���Ƶ���ɫ��ʹ�� 24λ RGB888 ����ɫ��ʽ���û����������ɫ��ʽ��ת��
*
*	��������:	��ָ���������ָ����ɫ�ĵ�
*
*	˵    ��:	ʹ��ʾ�� LCD_DrawPoint( 10, 10, 0x0000FF) ��������(10,10)������ɫ�ĵ�
*
*****************************************************************************************************************************************/

void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{
	LCD_SetAddress(x,y,x,y);	//	�������� 
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC

	LCD_WriteData_16bit(LCD.Color)	;
	
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����		
} 

void LCD_DrawPoint_LVGL(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_SetAddress(x,y,x,y);
	
	LCD_CS_L;
	
	LCD_WriteData_16bit(color);
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);
	
	LCD_CS_H;
}




/**************************************************************************************************************************************************************************************************************************************���� STM32F4���İ�******************************FANKE***/
// ʵ��ƽ̨������ STM32F4���İ�
//


