#include "sys_init.h"

UINT32 g_TskHandle;
UINT32  g_demoTskHandle;

DHT11_Data_TypeDef  DHT11_Data;

VOID task1(VOID)
{
	int count = 1;
	char str[10];
	while(1)
	{
		sprintf(str,"%d",count);
		Gui_DrawFont_GBK16(60,60,RED,GRAY0,(uint8_t*)str);
		printf("********This is LiteOS Task 1, count is %d \r\n",count++);
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		LOS_TaskDelay(1000);
	}
}
UINT32 creat_task1()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "task1";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task1;
    task_init_param.uwStackSize = 0x200;

    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

VOID task2(VOID)
{
	int count = 1;
	char str[10];
	while(1)
	{
		sprintf(str,"%d",count);
		Gui_DrawFont_GBK16(60,80,BLUE,GRAY0,(uint8_t*)str);
		printf("++++++++This is LiteOS Task 2, count is %d \r\n",count++);
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		LOS_TaskDelay(2000);
	}
}
UINT32 creat_task2()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "task2";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task2;
    task_init_param.uwStackSize = 0x200;

    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

void taskDHT11(void)
{
	
	char temp[10],humi[10];
	
	Gui_DrawFont_GBK16(2,100,BLACK,GRAY0,(uint8_t *)"humi:");
	Gui_DrawFont_GBK16(2,120,BLACK,GRAY0,(uint8_t *)"temp:");
	
	while (1)
  {
		if(DHT11_Read_TempAndHumidity(&DHT11_Data)==SUCCESS)
		{
				printf("读取DHT11成功!-->湿度为%.1f RH ,温度为%.1f C\n",DHT11_Data.humidity,DHT11_Data.temperature);
				sprintf(humi,"%.1f",DHT11_Data.humidity);
				sprintf(temp,"%.1f",DHT11_Data.temperature);
				Gui_DrawFont_GBK16(50,100,RED,GRAY0,(uint8_t*)humi);
				Gui_DrawFont_GBK16(50,120,RED,GRAY0,(uint8_t*)temp);
				HAL_GPIO_TogglePin(GPIOE, LED1_Pin);
		}
		else
		{
				printf("读取DHT11信息失败\n");
				DHT11_Init();
		}
		
		LOS_TaskDelay(3000);
  }
}
UINT32 creat_taskDHT11()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "taskDHT11";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)taskDHT11;
    task_init_param.uwStackSize = 0x200;

    uwRet = LOS_TaskCreate(&g_demoTskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

int main(void)
{
	UINT32 uwRet = 0;
	hardware_init();
	uwRet = LOS_KernelInit();
  if (uwRet != LOS_OK)
  {
      return LOS_NOK;
  }
	uwRet = creat_task1();
  if (uwRet != LOS_OK)
  {
      return LOS_NOK;
  }
  uwRet = creat_task2();
  if (uwRet != LOS_OK)
  {
      return LOS_NOK;
  }
	uwRet = creat_taskDHT11();
  if (uwRet != LOS_OK)
  {
      return LOS_NOK;
  }
	(void)LOS_Start();
}
