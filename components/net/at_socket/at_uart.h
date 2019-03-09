#ifndef __AT_UART_H__
#define __AT_UART_H__
#include "stdint.h"
#include "usart.h"
#define  UART_RXBUF_MAXSIZE           512
void hal_uart_init(void);
void hal_uart_send(char * buf, int len);
int hal_uart_recv(uint8_t *buf, uint32_t len, uint32_t timeout);
#endif /* __HW_UART_H__ */
