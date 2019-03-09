#ifndef __ESP8266_H__
#define __ESP8266_H__

#include <stdio.h>
#include "at_api_interface.h"
#include "at_uart.h"

#define WIFI_SSID              "Supowang2580"
#define WIFI_PASSWD            "25802580"


#define ESP8266_Q_SIZE    3
#define ESP8266_ITEM_SIZE sizeof(queue_buf)
    
typedef struct _cmd {
    char * cmd;
    char * resp;
}CMD;

typedef enum _net_mode {
    STA = 1,
    AP, 
    STA_AP,
}enum_net_mode;

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;

typedef struct _queue_buf{
    uint32_t len;
    uint8_t * addr;
}queue_buf;

typedef enum {
    CMD_AT = 0,
    CMD_RST,
    CMD_ECHO_OFF,
    CMD_CWMODE,
    CMD_JOINAP,
    CMD_CIPMUX,
    CMD_CONN,
    CMD_SEND,
    CMD_CLOSE,
    CMD_MAX
}E_CMD;


#endif /* __ESP8266_H__ */

