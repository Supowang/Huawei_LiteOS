#ifndef __BH1750_H
#define __BH1750_H

#include "stm32l4xx_hal.h"
#include "main.h"
//---------------------BH750 �궨��----------------------------// 
#define I2C_OWN_ADDRESS				0x0A

#define I2C_WR	        			0		/* д����bit */
#define I2C_RD	        			1		/* ������bit */

#define BH1750_Addr 				0x46
#define BH1750_ON   				0x01
#define BH1750_CON  				0x10
#define BH1750_ONE  				0x20
#define BH1750_RSET 				0x07


////-------------------BH1750 IIC�˿ڶ���-------------------------//  
#define BH1750_SCLK_Clr() 			HAL_GPIO_WritePin(BH1750_SCK_GPIO_Port,BH1750_SCK_Pin, GPIO_PIN_RESET);//SCL IIC�ӿڵ�ʱ���ź�
#define BH1750_SCLK_Set() 			HAL_GPIO_WritePin(BH1750_SCK_GPIO_Port,BH1750_SCK_Pin, GPIO_PIN_SET);

#define BH1750_SDIN_Clr() 			HAL_GPIO_WritePin(BH1750_SDA_GPIO_Port,BH1750_SDA_Pin, GPIO_PIN_RESET);//SDA IIC�ӿڵ������ź�
#define BH1750_SDIN_Set() 			HAL_GPIO_WritePin(BH1750_SDA_GPIO_Port,BH1750_SDA_Pin, GPIO_PIN_SET);

#define BH1750_SDA_READ() 			HAL_GPIO_ReadPin(BH1750_SDA_GPIO_Port,BH1750_SDA_Pin)

#define SDA_IN()  					{GPIOB->MODER&=~(3<<(10*2));GPIOB->MODER|=0<<10*2;}       				//����Ϊ�������� 
#define SDA_OUT() 					{GPIOB->MODER&=~(3<<(10*2));GPIOB->MODER|=1<<10*2;}   					//����Ϊ�������

/***************************************************************
* ��������: BH1750_Data_TypeDef
* ˵    ��: BH1750�ṹ��
* ��    ��: ��
* �� �� ֵ: ��
***************************************************************/
typedef struct
{
  char Lux[5];			//��ǿ
}BH1750_Data_TypeDef;

/**************************************************************/
/******************     BH1750�ӿ�     ************************/
/**************************************************************/
void Init_BH1750(void);				//BH1750��ʼ��
float Convert_BH1750(void);			//ת���ɹ�ǿֵ

#endif





