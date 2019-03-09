#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTPacket.h"
#include "transport.h"
#include "sys_init.h"

//#define MQTT_SERVER_IP	"111.230.189.156"
#define MQTT_SERVER_IP	"150.109.60.107"
//#define MQTT_SERVER_IP   "39.108.190.129"
#define MQTT_SERVER_PORT	"1883"
#define MQTT_PRODUCT_ID          "RUAP1R610V"
#define MQTT_DEV_NAME            "supowang"
#define MQTT_CLIENT_ID           "RUAP1R610Vsupowang"
#define MQTT_USR_NAME            "RUAP1R610Vsupowang;21010406;12365;2147483648"
#define MQTT_PASSWORD            "4037099451d9b44772a9a53a4d2ccc15a715ab15;hmacsha1"

//#define MQTT_SUBSCRIBE_TOPIC "RUAP1R610V/supowang/control"
#define MQTT_SUBSCRIBE_TOPIC "control"
//#define MQTT_PUBLISH_TOPIC "RUAP1R610V/supowang/event"
#define MQTT_PUBLISH_TOPIC "mqtt"
#define OTA_PUBLISH_TOPIC "ota/report/RUAP1R610V/supowang"
#define OTA_SUBSCRIBE_TOPIC "ota/update/RUAP1R610V/supowang"

/**
 * @brief 发布或接收已订阅消息的结构体定义
 */
typedef struct {
    int         						qos;          // MQTT 服务质量等级
    unsigned char     			retained;     // RETAIN 标识位
    unsigned char     			dup;          // DUP 标识位
    unsigned short   				id;           // MQTT 消息标识符

    MQTTString 							ptopic;      // MQTT topic

    unsigned char     		*payload;     // MQTT 消息负载
    int      							payload_len;  // MQTT 消息负载长度
} MQTTPub_Message;

typedef MQTTPub_Message PublishParams;

/**
 * @brief 订阅消息的结构体定义
 */
typedef struct {
    int         						req_qos;          // MQTT 服务质量等级
    int     								count;     			// number of members in the topicFilters and reqQos arrays
    unsigned char     			dup;          // DUP 标识位
    unsigned short   				id;           // MQTT 消息标识符
    MQTTString 							ptopic;      // MQTT topic
} MQTTSub_Message;

typedef MQTTSub_Message SubscribeParams;



int iot_mqtt_connect(char* host, const char* port)
{
	int rc = 0;
	int mysock = 0;
	int len = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	mysock = transport_open(host, port);
	if(mysock < 0)
		return -1;
	printf("Sending to hostname %s port %s\n", host, port);
	
	data.clientID.cstring = MQTT_CLIENT_ID;
	data.keepAliveInterval = 2000;
	data.cleansession = 1;
	data.username.cstring = NULL;
	data.password.cstring = NULL;
	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, buf, len);
		/* wait for connack */
	if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			printf("Unable to connect, return code %d\n", connack_rc);
			//goto exit;
		}
	}
	else
	{
		printf("exit1");
	}
	rc = rc+1;
	return mysock;
}

int iot_mqtt_publish(int socket, PublishParams *pParams) 
{
	int len = 0;
	int rc = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	len = MQTTSerialize_publish(buf, buflen, pParams->dup, pParams->qos, pParams->retained, pParams->id, pParams->ptopic, pParams->payload, pParams->payload_len);
	rc = transport_sendPacketBuffer(socket, buf, len);
	return rc;
}

int iot_mqtt_subscribe(int socket, SubscribeParams *pParams) 
{
	/* subscribe */
	int len = 0;
	int rc = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	len = MQTTSerialize_subscribe(buf, buflen, pParams->dup, pParams->id, pParams->count, &pParams->ptopic, &pParams->req_qos);
	rc = transport_sendPacketBuffer(socket, buf, len);
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0)
		{
			printf("granted qos != 0, %d\n", granted_qos);
			//goto exit;
		}
	}
	else
	{
		printf("exit2");
	}
	return rc;
}

int iot_mqtt_read(unsigned char* payload_in,int payloadlen_in)
{
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int rc = 0;
	if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)	//这里的publish标志，表示为对方发送的publish消息
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			MQTTString receivedTopic;
			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,&payload_in, &payloadlen_in, buf, buflen);
			printf("topic is %s,message arrived %d %s %s %d\n", receivedTopic.cstring,payloadlen_in, payload_in,buf,buflen);
		}
		else
		{
			printf("no data recv\r\n");
		}
	return rc;
}

void mqtt_thread(void)
{
	int sock_id = 0;
	
	unsigned char * read_data;
	int read_len;
	char *data = "ota will be ok!";
	//发布主题参数初始化
	PublishParams pub;
	pub.dup = 0;
	pub.qos = 0;
	pub.retained = 0;
	pub.id = 0;
	pub.ptopic.cstring = MQTT_PUBLISH_TOPIC;
	pub.ptopic.lenstring.len = 0;
	pub.ptopic.lenstring.data = NULL;
	pub.payload = (unsigned char *)data;
	pub.payload_len = strlen(data);
	
	//订阅主题参数初始化
	SubscribeParams sub;
	sub.count = 1;
	sub.dup = 0;
	sub.id = 1;
	sub.req_qos = 0;
	sub.ptopic.cstring = MQTT_SUBSCRIBE_TOPIC;
	sub.ptopic.lenstring.len = 0;
	sub.ptopic.lenstring.data = NULL;
	
	//连接MQTT服务器
	sock_id = iot_mqtt_connect(MQTT_SERVER_IP,MQTT_SERVER_PORT);
	
	printf("socke id id %d \r\n",sock_id);
	
	//订阅MQTT topic
	iot_mqtt_subscribe(sock_id,&sub);
	
	
	while (1)
	{
		printf("read buf\r\n");
		iot_mqtt_read(read_data,read_len);
		printf("publish topic\r\n");
		pub.ptopic.cstring = MQTT_PUBLISH_TOPIC;
		pub.payload = (unsigned char *)data;
		pub.payload_len = strlen(data);
		iot_mqtt_publish(sock_id,&pub);
		HAL_Delay(2000);
		
	}

}
