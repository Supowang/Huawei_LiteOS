#include "stm32l4xx.h"
#include "main.h"

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111

/*
#define LCD_DC_Pin GPIO_PIN_10
#define LCD_DC_GPIO_Port GPIOD
#define LCD_RES_Pin GPIO_PIN_11
#define LCD_RES_GPIO_Port GPIOD
#define LCD_SCL_Pin GPIO_PIN_12
#define LCD_SCL_GPIO_Port GPIOD
#define LCD_SDA_Pin GPIO_PIN_13
#define LCD_SDA_GPIO_Port GPIOD
#define LCD_BLK_Pin GPIO_PIN_14
#define LCD_BLK_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_15
#define LCD_CS_GPIO_Port GPIOD
*/

//液晶控制口置1操作语句宏定义
#define	LCD_SCL_SET  	HAL_GPIO_WritePin(GPIOD, LCD_SCL_Pin, GPIO_PIN_SET);//LCD_CTRLB->BSRR=LCD_SCL    
#define	LCD_SDA_SET  	HAL_GPIO_WritePin(GPIOD, LCD_SDA_Pin, GPIO_PIN_SET);//LCD_CTRLB->BSRR=LCD_SDA   
#define	LCD_CS_SET  	HAL_GPIO_WritePin(GPIOD, LCD_CS_Pin, GPIO_PIN_SET);//LCD_CTRLB->BSRR=LCD_CS  

    
#define	LCD_LED_SET  	HAL_GPIO_WritePin(GPIOD, LCD_BLK_Pin, GPIO_PIN_SET);//LCD_CTRLB->BSRR=LCD_LED   
#define	LCD_RS_SET  	HAL_GPIO_WritePin(GPIOD, LCD_DC_Pin, GPIO_PIN_SET);//LCD_CTRLB->BSRR=LCD_RS 
#define	LCD_RST_SET  	HAL_GPIO_WritePin(GPIOD, LCD_RES_Pin, GPIO_PIN_SET);//LCD_CTRLB->BSRR=LCD_RST 
//液晶控制口置0操作语句宏定义
#define	LCD_SCL_CLR  	HAL_GPIO_WritePin(GPIOD, LCD_SCL_Pin, GPIO_PIN_RESET);//LCD_CTRLB->BRR=LCD_SCL  
#define	LCD_SDA_CLR  	HAL_GPIO_WritePin(GPIOD, LCD_SDA_Pin, GPIO_PIN_RESET);//LCD_CTRLB->BRR=LCD_SDA 
#define	LCD_CS_CLR  	HAL_GPIO_WritePin(GPIOD, LCD_CS_Pin, GPIO_PIN_RESET);//LCD_CTRLB->BRR=LCD_CS 
    
#define	LCD_LED_CLR  	HAL_GPIO_WritePin(GPIOD, LCD_BLK_Pin, GPIO_PIN_RESET);//LCD_CTRLB->BRR=LCD_LED 
#define	LCD_RST_CLR  	HAL_GPIO_WritePin(GPIOD, LCD_RES_Pin, GPIO_PIN_RESET);//LCD_CTRLB->BRR=LCD_RST
#define	LCD_RS_CLR  	HAL_GPIO_WritePin(GPIOD, LCD_DC_Pin, GPIO_PIN_RESET);//LCD_CTRLB->BRR=LCD_RS 

void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);
void LCD_WriteData_16Bit(uint16_t Data);

