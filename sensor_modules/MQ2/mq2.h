#ifndef __MQ2_H
#define __MQ2_H

#include "stm32l4xx_hal.h"

////-------------------BH1750 IIC端口定义-------------------------//  
//使用PA7接口直接读取ADC的值
//使用PA6可以读取高低电平

/***************************************************************
* 函数名称: SMOKE_Data_TypeDef
* 说    明: MQ2烟感数据结构体
* 参    数: 无
* 返 回 值: 无
***************************************************************/
typedef struct
{
	char Value[4];
	char MaxValue[4];		
}SMOKE_Data_TypeDef;


/**************************************************************/
/******************     Smoke接口     ************************/
/**************************************************************/
uint16_t Convert_Smoke(void);

#endif





