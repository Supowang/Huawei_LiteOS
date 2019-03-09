#include "sys_init.h"
#include "at_api_interface.h"
#include "at_hal_init.h"
#include "at_socket.h"
UINT32 g_TskHandle;
extern int mqtt_thread(void);

#define TCP_SERVER_IP	"39.108.190.129"
#define TCP_SERVER_PORT	"1989"
#define RECV_LEN  1024
unsigned char recv_data[RECV_LEN];
char *data_send = "Welcome to TCP Test!\r\n";

void tcp_test(void)
{
	int ret = -1;
	at_net_context *con = NULL;
	con = at_net_connect(TCP_SERVER_IP,TCP_SERVER_PORT,AT_PROTO_TCP);
	if(con == NULL)
	{
		printf("TCP connect failed\r\n");
	}
	else
	{
		printf("TCP connect success! con->fd is %d",con->fd);
	}
	at_net_send(con,(const unsigned char*)data_send,strlen(data_send));
	
	while(1)
	{
		at_net_send(con,(const unsigned char*)data_send,strlen(data_send));
		ret = at_net_recv_timeout(con,recv_data,RECV_LEN-1,1000);
		if(ret <0)
		{
			printf("recv error\n\r");
		}
		else 
		{
			recv_data[ret] = 0;
			printf("recv:ret=%d (%s)\n\r", ret, recv_data);
			memset(recv_data,0,strlen((const char *)recv_data));
		}
		HAL_Delay(1000);
	}
}

VOID task1(VOID)
{
	int count = 0;
	DHT11_Data_TypeDef DHT11_Data;
	DHT11_Msg_TypeDef DHT11_Msg;
	BH1750_Data_TypeDef BH1750_Data;
	SMOKE_Data_TypeDef SMOKE_Data;
	at_hal_init();
	printf("Welcome\r\n");
	at_api_register(&at_interface);
	mqtt_thread();
	//tcp_test();
	while(1)
	{
#if 0
		if(DHT11_Read_TempAndHumidity(&DHT11_Data)==SUCCESS)
		{
			printf("读取DHT11成功!-->湿度为%.1f ％RH ，温度为 %.1f℃ \n",DHT11_Data.humidity,DHT11_Data.temperature);
		}
		else
		{
			printf("读取DHT11信息失败\n");
			DHT11_Init();      
		}
		sprintf(DHT11_Msg.temp, "%3.1f", DHT11_Data.temperature);
		OLED_ShowString(0,4,(uint8_t *)"Temp:");
		OLED_ShowString(40,4,(uint8_t *)"     ");		
		OLED_ShowString(40,4,(uint8_t *)DHT11_Msg.temp);
		OLED_ShowString(80,4,(uint8_t *)"C");
		sprintf(DHT11_Msg.hum, "%3.1f", DHT11_Data.humidity);
		OLED_ShowString(0,6,(uint8_t *)"Humi:");
		OLED_ShowString(40,6,(uint8_t *)"     ");		
		OLED_ShowString(40,6,(uint8_t *)DHT11_Msg.hum);
		OLED_ShowString(80,6,(uint8_t *)"%RH");
		
#else
#if 0
		printf("获取光照强度成功!-->光强为%.1f lux\r\n",Convert_BH1750());
		sprintf(BH1750_Data.Lux,"%.1f",Convert_BH1750());
		OLED_ShowString(0,4,(uint8_t *)"Light:");
		OLED_ShowString(48,4,(uint8_t *)"     ");		
		OLED_ShowString(48,4,(uint8_t *)BH1750_Data.Lux);
		OLED_ShowString(104,4,(uint8_t *)"lux");
#endif		
		printf("获取烟感值成功!-->数值为%d\r\n",Convert_Smoke());
		sprintf(SMOKE_Data.Value, "%4d", Convert_Smoke());
		OLED_ShowString(0,4,(uint8_t *)"Smoke:");
		OLED_ShowString(48,4,(uint8_t *)"       ");		
		OLED_ShowString(48,4,(uint8_t *)SMOKE_Data.Value);
		OLED_ShowString(104,4,(uint8_t *)"   ");
#endif		
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
	int count = 0;
	while(1)
	{
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
