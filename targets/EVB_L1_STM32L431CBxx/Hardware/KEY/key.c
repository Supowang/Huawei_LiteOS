#include "key.h"
/***************************************************************
* 文件名称: KEY
* 作    者: 物联网俱乐部
* 版    本: V1.0
* 编写日期: 2018-12-01
* 功    能: 按键键值识别
****************************************************************
* 硬件平台: 本例程配套物联网俱乐部EVB_L1开发板使用
* 淘    宝: iot-club.taobao.com、https://shop449662000.taobao.com
* 论    坛: www.iot-club.cn 、www.iotclub.net
****************************************************************/
uint8_t keyValue;
/***************************************************************
* 函数名称: HAL_GPIO_EXTI_Callback
* 说    明: 重写HAL_GPIO_EXTI_Callback函数，按键中断的回调函数，读取按键值。使用中断方式时，该函数不用声明
* 参    数: GPIO_Pin，触发中断的引脚
* 返 回 值: 无
***************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case KEY1_Pin:	
			keyValue = KEY1_VALUE; 
			printf("you press key1\r\n");
			break;
		case KEY2_Pin:	
			printf("you press key2\r\n");
			keyValue = KEY2_VALUE;
			break;
		default:		
			keyValue = KEY_VALUE_NONE;
			break;
	}
}

/***************************************************************
* 函数名称: KeyScan
* 说    明: 按键扫描函数，判断按键按下
* 参    数: 无
* 返 回 值: 键值
***************************************************************/
uint8_t KeyScan(void)
{
	if(KEY1==0||KEY2==0)
	{
		HAL_Delay(10);						//延时消抖
		if		 (KEY1==0)
			return KEY1_VALUE;
		else if(KEY2==0)
			return KEY2_VALUE;
	}
	return KEY_VALUE_NONE;
}

/**********************END OF FILE**********************/
