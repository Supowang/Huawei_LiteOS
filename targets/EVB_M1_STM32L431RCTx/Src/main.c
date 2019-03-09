#include "sys_init.h"

UINT32 g_TskHandle;

VOID task1(VOID)
{
	int count = 0;
	while(1)
	{
		printf("********This is LiteOS Task 1, count is %d \r\n",count++);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
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
	int count = 0;
	DHT11_Data_TypeDef  DHT11_Data;
	while(1)
	{
		if(DHT11_Read_TempAndHumidity(&DHT11_Data)==SUCCESS)
    {
			printf("读取DHT11成功!-->湿度为%.1f ％RH ，温度为 %.1f℃ \n",DHT11_Data.humidity,DHT11_Data.temperature);
    }
    else
    {
      printf("读取DHT11信息失败\n");
			DHT11_Init();      
    }
		printf("++++++++This is LiteOS Task 2, count is %d \r\n",count++);
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
	
	(void)LOS_Start();
}
