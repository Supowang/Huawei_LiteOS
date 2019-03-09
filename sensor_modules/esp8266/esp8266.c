#include <stdio.h>
#include "esp8266.h"
#include "usart.h"
#include "at_hal_init.h"
#include <stdbool.h>
#if defined(WITH_AT_SOCKET) && defined(USE_ESP8266)
extern uint16_t rx_rd_index;
extern uint16_t rx_wr_index;
extern uint16_t rx_count;
extern uint8_t g_rx_buf[UART_RXBUF_MAXSIZE];

bool esp8266_send_cmd(char * cmd, uint32_t len,char * reply1, char * reply2, uint32_t waittime)
{
		char buf[256] = {0};
		rx_rd_index = 0;
		rx_wr_index = 0;
		rx_count = 0;
		hal_uart_send((char*)cmd,len);
		if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
		
		drv_delay_ms( waittime );                 //延时 
    hal_uart_recv((uint8_t *)buf, 120, 20);
		printf("%s",buf);
		if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
			return ( ( bool ) strstr ( buf, reply1 ) || 
						 ( bool ) strstr ( buf, reply2 ) ); 
 	
		else if ( reply1 != 0 )
			return ( ( bool ) strstr ( buf, reply1 ) );
	
		else
			return ( ( bool ) strstr ( buf, reply2 ) );
}


/**
  * 函数功能: 重启ESP8266模块
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：被ESP8266_AT_Test调用
  */
void ESP8266_Rst ( void )
{
#if 1
	 esp8266_send_cmd ( "AT+RST\r\n", strlen("AT+RST\r\n"),"OK", "ready", 2500 );   	
#else
#endif
}

/**
  * 函数功能: 对ESP8266模块进行AT测试启动
  * 输入参数: 无
  * 返 回 值: 1，选择成功
  *           0，选择失败
  * 说    明：无
  */
bool ESP8266_AT_Test ( void )
{
	char count=0;
	
	HAL_Delay(1000 );
	while(count<1)
	{
		if(esp8266_send_cmd("AT\r\n",strlen("AT\r\n"),"OK",NULL,1000)) return 1;
		ESP8266_Rst();
    HAL_Delay(1000);
		++count;
	}
	return 0;
}


/**
  * 函数功能: 选择ESP8266模块的工作模式
  * 输入参数: enumMode，工作模式
  * 返 回 值: 1，选择成功
  *           0，选择失败
  * 说    明：无
  */
bool ESP8266_Net_Mode_Choose ( enum_net_mode enumMode )
{
	bool result=0;
	char count=0;
	while(count<10)
	{
		switch ( enumMode )
		{
			case STA:
				result=esp8266_send_cmd ( "AT+CWMODE=1\r\n", strlen("AT+CWMODE=1\r\n"),"OK", "no change", 2500 ); 
			break;
			case AP:
				result=esp8266_send_cmd ( "AT+CWMODE=2\r\n", strlen("AT+CWMODE=2\r\n"),"OK", "no change", 2500 ); 
			break;
			case STA_AP:
				result=esp8266_send_cmd ( "AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"),"OK", "no change", 2500 ); 
			break;
			default:
				result=false;
			break;
		}
		if(result) return result;
		++count;
	}
	return 0;
}

/**
  * 函数功能: ESP8266模块连接外部WiFi
  * 输入参数: pSSID，WiFi名称字符串
  *           pPassWord，WiFi密码字符串
  * 返 回 值: 1，连接成功
  *           0，连接失败
  * 说    明：无
  */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];
	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", pSSID, pPassWord );
	if(esp8266_send_cmd(cCmd,strlen(cCmd),"OK",NULL,1000))
		return 1;
	else
		return 0;	
}


/**
  * 函数功能: ESP8266模块启动多连接
  * 输入参数: enumEnUnvarnishTx，配置是否多连接
  * 返 回 值: 1，配置成功
  *           0，配置失败
  * 说    明：无
  */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	char count=0;
	sprintf ( cStr, "AT+CIPMUX=%d\r\n", ( enumEnUnvarnishTx ? 1 : 0 ) );
	while(count<10)
	{
		if(esp8266_send_cmd(cStr,strlen(cStr),"OK",0,500))return 1;
		++count;
	}
	return 0;		
}

/**
  * 函数功能: 配置ESP8266模块进入透传发送
  * 输入参数: 无
  * 返 回 值: 1，配置成功
  *           0，配置失败
  * 说    明：无
  */
bool ESP8266_UnvarnishSend ( void )
{
	
	if ( ! esp8266_send_cmd ( "AT+CIPMODE=1\r\n", strlen("AT+CIPMODE=1\r\n"),"OK", 0, 1000 ) )
		return false;
	return  esp8266_send_cmd ( "AT+CIPSEND\r\n", strlen("AT+CIPSEND\r\n"),"OK", ">", 1000 );
	
}

/**
  * 函数功能: 配置ESP8266模块退出透传模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void ESP8266_ExitUnvarnishSend ( void )
{
	HAL_Delay(1000);	
	hal_uart_send("+++",strlen("+++"));	
	HAL_Delay(500); 
	
}

int32_t esp8266_init(void)
{
  drv_delay_ms(1000);
	printf("正在配置ESP8266 ......\n" );
	ESP8266_Rst();
	if(ESP8266_AT_Test())
  {
    printf("AT test OK\n");
  }
	printf("\n< 1 >\n");
	if(ESP8266_Net_Mode_Choose(STA))
  {
    printf("ESP8266_Net_Mode_Choose OK\n");
  }  
  printf("\n< 2 >\n");
	
  while(!ESP8266_JoinAP(WIFI_SSID,WIFI_PASSWD))
	{
		 printf("connecting wifi....");
		 HAL_Delay(1000);
	}	
	printf("\n< 3 >\n");
  ESP8266_Enable_MultipleId(DISABLE);
	printf("配置 ESP8266 完毕\n");
    return 0;
}

int32_t esp8266_connect(const int8_t* host, const int8_t* port, int32_t proto)
{
    char cmd[64] = {0};

    snprintf(cmd, 64, "%s=\"%s\",\"%s\",%s\r\n", "AT+CIPSTART", proto == PROTO_UDP? "UDP" : "TCP", host, port);
		if(esp8266_send_cmd((char *)cmd,strlen(cmd), "OK", "ALREAY CONNECT",4000))
		{
			while(!ESP8266_UnvarnishSend());//开启透传模式
			return 0;
		}
			
    else
			return -1;
}

int32_t esp8266_recv_timeout(int32_t id, int8_t * buf, uint32_t len,
                           int32_t timeout)
{
		return hal_uart_recv((uint8_t *)buf, len, timeout);
}

int32_t esp8266_recv(int32_t id, int8_t * buf, uint32_t len)
{
    return esp8266_recv_timeout(id, buf, len, 5000);
}

int32_t esp8266_send(int32_t id , const uint8_t  *buf, uint32_t len)
{
	#if 0
    char cmd[64] = {0};
    int ret = -1;
    snprintf(cmd, 64, "%s=%d\r\n", "AT+CIPSEND", len);
         
    esp8266_send_cmd((char *)cmd, strlen(cmd),(char *)">",0,1000);
         
    ret = esp8266_send_cmd((char *)buf, len, (char *)"SEND OK",0,1000);
    if (ret >0)
        ret = len;
    return ret;
	#endif
	hal_uart_send((char*)buf,len);
	return 0;
}

int32_t esp8266_close(int32_t id)
{
        char cmd[64] = {0};
				ESP8266_ExitUnvarnishSend();  //退出透传模式
        snprintf(cmd, 64, "%s\r\n", "AT+CIPCLOSE\r\n");
        return esp8266_send_cmd((char *)cmd, strlen(cmd), (char *)"OK",0,1000);
}

at_adaptor_api at_interface = {
    .init = esp8266_init,
    .connect = esp8266_connect,
    .send = esp8266_send,
    .recv_timeout = esp8266_recv_timeout,

    .recv = esp8266_recv,    
    .close = esp8266_close,    
};
#endif
