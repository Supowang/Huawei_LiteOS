#include <stdio.h>
#include <string.h>
#include "at_hal_init.h"

#define RSP_BUFFER_MAX 64
#define RSP_TIMEOUT 2000 //2s

/* Local variables ----------------------------------------------------------*/
uint16_t rx_rd_index  = 0;
uint16_t rx_wr_index = 0;
uint16_t rx_count      = 0;

uint8_t g_rx_buf[UART_RXBUF_MAXSIZE];

/* Extern variables ---------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/

/* 接收串口数据 */
void g_ring_buf(uint8_t in_data)
{
    g_rx_buf[rx_wr_index]=in_data;
    rx_wr_index++;
    rx_count++;

    if (rx_wr_index == UART_RXBUF_MAXSIZE)
    {
        rx_wr_index = 0;
    }

     /* Check for overflow */
    if (rx_count == UART_RXBUF_MAXSIZE)
    {
        rx_wr_index = 0;
        rx_count      = 0;
        rx_rd_index  = 0;
    }
}

/* 从ring buffer里面获取data */
static int get_rx_byte(void)
{
    int c = -1;

    __disable_irq();
    if (rx_count > 0)
    {
        c = g_rx_buf[rx_rd_index];

        rx_rd_index++;
        if (rx_rd_index == UART_RXBUF_MAXSIZE)
        {
            rx_rd_index = 0;
        }
        rx_count--;
    }
    __enable_irq();

    return c;
}


void hal_uart_send(char * buf, int len)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, len,0xFFFF);
}

int hal_uart_recv(uint8_t *buf, uint32_t len, uint32_t timeout)
{
      int c=0, i=0;

      do
      {
          c = get_rx_byte();
          if (c < 0)
          {
               timeout--;
               drv_delay_ms(1);
          }
          else
          {
               buf[i++] = (char)c;
          }
      }while(i<len && timeout>0);

      return i;
}



void hal_uart_init(void)
{
    MX_USART3_UART_Init();  //usart drv init 
}


