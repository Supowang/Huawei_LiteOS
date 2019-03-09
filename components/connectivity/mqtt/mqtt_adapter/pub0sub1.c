#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTPacket.h"
#include "transport.h"
#include "sys_init.h"

#define MQTT_SERVER_IP		"47.99.244.52"
#define MQTT_SERVER_PORT	"61613"
#define MQTT_PRODUCT_ID          "RUAP1R610V"
#define MQTT_DEV_NAME            "supowang"
#define MQTT_CLIENT_ID           "RUAP1R610Vsupowang"
#define MQTT_USR_NAME            "admin"
#define MQTT_PASSWORD            "password"

#define MQTT_SUBSCRIBE_TOPIC 	"home/garden/fountain"
#define MQTT_PUBLISH_TOPIC 		"event"

int time =0;

int mqtt_thread(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
	int i = 99999;
	int mysock = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	char * payload = "MQTTDATA!";
	int payloadlen = strlen(payload);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	int len = 0;
	char *host = MQTT_SERVER_IP;
	char* port = MQTT_SERVER_PORT;
	
	mysock = transport_open(host, port);
	if(mysock < 0)
		return mysock;

	printf("Sending to hostname %s port %s\n", host, port);

	data.clientID.cstring = MQTT_CLIENT_ID;
	data.keepAliveInterval = 2000;
	data.cleansession = 1;
	data.username.cstring = MQTT_USR_NAME;
	data.password.cstring = MQTT_PASSWORD;
	//data.username.cstring = NULL;
	//data.password.cstring = NULL;
	
	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, buf, len);
	HAL_Delay(100);
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
		
		//goto exit;

	/* subscribe */
	topicString.cstring = MQTT_SUBSCRIBE_TOPIC;
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	rc = transport_sendPacketBuffer(mysock, buf, len);
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
		
		//goto exit;

	/* loop getting msgs on subscribed topic */
	topicString.cstring = MQTT_PUBLISH_TOPIC;
	while (1)
	{
		time++;
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)	//这里的publish标志，表示为对方发送的publish消息
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char* payload_in;
			//int rc;
			MQTTString receivedTopic;

			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			if(strstr(payload_in,"ON"))
			{
				HAL_GPIO_WritePin(GPIOE, LED1_Pin,0);
				HAL_GPIO_WritePin(GPIOE, LED2_Pin,0);
			}
			else if(strstr(payload_in,"OFF"))
			{
				HAL_GPIO_WritePin(GPIOE, LED1_Pin,1);
				HAL_GPIO_WritePin(GPIOE, LED2_Pin,1);
			}
			printf("message arrived %.*s\n", payloadlen_in, payload_in);
			memset(buf,0,strlen(buf));
			memset(payload_in,0,strlen(payload_in));
		}
		if(time == 20)
		{
			len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, strlen(payload));
			rc = transport_sendPacketBuffer(mysock, buf, len);	
			time = 0;
		}
		HAL_Delay(100);
		rc = rc+1;
	}

	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(mysock, buf, len);

exit:
	transport_close(mysock);

	return 0;
}
