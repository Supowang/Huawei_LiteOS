#include "key.h"
/***************************************************************
* �ļ�����: KEY
* ��    ��: ���������ֲ�
* ��    ��: V1.0
* ��д����: 2018-12-01
* ��    ��: ������ֵʶ��
****************************************************************
* Ӳ��ƽ̨: �������������������ֲ�EVB_L1������ʹ��
* ��    ��: iot-club.taobao.com��https://shop449662000.taobao.com
* ��    ̳: www.iot-club.cn ��www.iotclub.net
****************************************************************/
uint8_t keyValue;
/***************************************************************
* ��������: HAL_GPIO_EXTI_Callback
* ˵    ��: ��дHAL_GPIO_EXTI_Callback�����������жϵĻص���������ȡ����ֵ��ʹ���жϷ�ʽʱ���ú�����������
* ��    ��: GPIO_Pin�������жϵ�����
* �� �� ֵ: ��
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
* ��������: KeyScan
* ˵    ��: ����ɨ�躯�����жϰ�������
* ��    ��: ��
* �� �� ֵ: ��ֵ
***************************************************************/
uint8_t KeyScan(void)
{
	if(KEY1==0||KEY2==0)
	{
		HAL_Delay(10);						//��ʱ����
		if		 (KEY1==0)
			return KEY1_VALUE;
		else if(KEY2==0)
			return KEY2_VALUE;
	}
	return KEY_VALUE_NONE;
}

/**********************END OF FILE**********************/
