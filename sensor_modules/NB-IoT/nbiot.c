#include "nbiot.h"
#include "usart.h"
#include "hexstring.h"
#include <stdlib.h>
#include <string.h>
/***************************************************************
* �ļ�����: nbiot
* ��    ��: ���������ֲ�
* ��    ��: V1.0
* ��д����: 2018-12-01
* ��    ��: NB�����շ�
****************************************************************
* Ӳ��ƽ̨: �������������������ֲ�EVB_L1������ʹ��
* ��    ��: iot-club.taobao.com��https://shop449662000.taobao.com
* ��    ̳: www.iot-club.cn ��www.iotclub.net
****************************************************************/
static char cmdSend[512];

/***************************************************************
* ��������: NB_SendCmd
* ˵    ��: NB��������
* ��    ��: uint8_t *cmd����Ҫ���͵�����
*			uint8_t *result��������õĽ��
*			uint32_t timeOut���ȴ����������ʱ��
*			uint8_t isPrintf���Ƿ��ӡLog
* �� �� ֵ: ret������ֵ
***************************************************************/
int NB_SendCmd(uint8_t *cmd,uint8_t *result,uint32_t timeOut,uint8_t isPrintf)
{
	int ret=0;
	memset(USART2_RX_BUF,0,strlen((const char *)USART2_RX_BUF));//�������
	if(isPrintf)
		printf("MCU-->>NB: %s\r\n",cmd);
	HAL_UART_Transmit(&huart2, cmd, strlen((const char *)cmd), timeOut);
	HAL_Delay(500);
	while(1)
	{
		if(strstr((const char *)USART2_RX_BUF,(const char *)result))
		{
			ret = 0;
			break;
		}
		else
		{
 			HAL_UART_Transmit(&huart2, cmd, strlen((const char *)cmd), timeOut);
			USART2_RX_LEN = 0;
			HAL_Delay(timeOut);
		}
	}
	if(isPrintf)
		printf("NB-->>MCU: %s\r\n",USART2_RX_BUF);
	HAL_Delay(timeOut);
	USART2_RX_LEN = 0;
	return ret;
}

/***************************************************************
* ��������: NB_SendCmd_Mul
* ˵    ��: NB�������ݣ�����ֵ�ж��
* ��    ��: uint8_t *cmd����Ҫ���͵�����
*						uint8_t *result��������õĽ��1
*						uint8_t *result��������õĽ��2
*						uint32_t timeOut���ȴ����������ʱ��
*						uint8_t isPrintf���Ƿ��ӡLog
* �� �� ֵ: ret������ֵ
***************************************************************/
int NB_SendCmd_Mul(uint8_t *cmd,uint8_t *result1,uint8_t *result2,uint32_t timeOut,uint8_t isPrintf)
{
	int ret=0;
	memset(USART2_RX_BUF,0,strlen((const char *)USART2_RX_BUF));//�������
	if(isPrintf)
		printf("MCU-->>NB: %s\r\n",cmd);
	HAL_UART_Transmit(&huart2, cmd, strlen((const char *)cmd), timeOut);
	HAL_Delay(100);
	while(1)
	{
		if(strstr((const char *)USART2_RX_BUF,(const char *)result1) 
			||strstr((const char *)USART2_RX_BUF,(const char *)result2))
		{
			ret = 0;
			break;
		}
		else
		{
			HAL_UART_Transmit(&huart2, cmd, strlen((const char *)cmd), timeOut);
			HAL_Delay(timeOut);
		}
	}
	if(isPrintf)
		printf("NB-->>MCU: %s\r\n",USART2_RX_BUF);
	HAL_Delay(timeOut);
	USART2_RX_LEN = 0;
	return ret;
}

/***************************************************************
* ��������: NB_InitConnect
* ˵    ��: NB��ʼ����������
* ��    ��: ��
* �� �� ֵ: ��
***************************************************************/
void NB_InitConnect(void)
{
	NB_SendCmd((uint8_t *)"AT+CFUN?\r\n",(uint8_t *)"OK",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+CGSN=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+CIMI\r\n",(uint8_t *)"OK",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+CGATT=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+CGATT?\r\n",(uint8_t *)"+CGATT:1",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+CSQ\r\n",(uint8_t *)"OK",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+COPS?\r\n",(uint8_t *)"+COPS:0,2,\"46011\"",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+CEREG=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
	NB_SendCmd((uint8_t *)"AT+CEREG?\r\n",(uint8_t *)"+CEREG:1,1",DefaultTimeout,1);
}

/***************************************************************
* ��������: NB_SendMsgToServer
* ˵    ��: NB������Ϣ��������
* ��    ��: *msg��Ҫ���͵����ݣ�String��ʽ��
* �� �� ֵ: ��
***************************************************************/
void NB_SendMsgToServer(uint8_t *msg)
{
	//����AT+NMGS=len,msg
	memset(cmdSend,0,sizeof(cmdSend));
	uint8_t len = 0;
	char msgLen[4]={0};
	len = strlen((const char *)msg)/2;
	DecToString(len, msgLen);
	strcat(cmdSend,"AT+NMGS=");
	strcat(cmdSend,msgLen);
	strcat(cmdSend,",");
	strcat(cmdSend,(const char *)msg);
	strcat(cmdSend,"\r\n");
	NB_SendCmd((uint8_t *)cmdSend,(uint8_t *)"OK",DefaultTimeout,1);	
}
/***************************************************************
* ��������: NB_SetCDPServer
* ˵    ��: NB����CDP������
* ��    ��: *ncdpIP��Ҫ���͵ķ�������ƽ̨����IP��ַ
*			*port��Ҫ���͵ķ�������ƽ̨���Ķ˿ں�
* �� �� ֵ: ��
***************************************************************/
void NB_SetCDPServer(uint8_t *ncdpIP,uint8_t *port)
{
	//����AT+NCDP=ncdpIP,port
	memset(cmdSend,0,sizeof(cmdSend));
	strcat(cmdSend,"AT+NCDP=");	
	strcat(cmdSend,(const char *)ncdpIP);
	strcat(cmdSend,",");
	strcat(cmdSend,(const char *)port);
	strcat(cmdSend,"\r\n");
	NB_SendCmd((uint8_t *)cmdSend,(uint8_t *)"OK",DefaultTimeout,1);
}

/***************************************************************
* ��������: NB_ReceiveMsg
* ˵    ��: NB�ӷ�������������
* ��    ��: msgReceive,NB���յ�������,�ò�������ʱ���ȷ�����ڴ�ռ�
* �� �� ֵ: SUCCESS�����ճɹ�
*			ERROR������ʧ�ܻ��޽�������
***************************************************************/
uint8_t NB_ReceiveMsg(uint8_t *msgReceive)
{
	//����+NNMI:2,4F4E
	char *pos1;
	pos1 = strstr((char *)USART2_RX_BUF,"+NNMI:");
	if(pos1)
	{
		HAL_Delay(500);
		char *pos2;
		uint8_t msgReceiveLen;
		msgReceiveLen = atoi(pos1+6);
		printf("NET-->>NB: %s \r\n",USART2_RX_BUF);		
		pos2 = strstr((char*)USART2_RX_BUF,",");
		memcpy(msgReceive,pos2+1,msgReceiveLen*2);	//pos2ָ����ǡ�,��  pos2+1ָ���������
		memset(USART2_RX_BUF,0,USART2_RX_LEN);
		USART2_RX_LEN = 0;
		return SUCCESS;
	}
	return ERROR;
}
