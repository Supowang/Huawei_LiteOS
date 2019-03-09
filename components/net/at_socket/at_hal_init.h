#ifndef __AT_HAL_INIT_H_
#define __AT_HAL_INIT_H_

/* Includes ------------------------------------------------------------------*/


#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "sys_init.h"
#include "at_uart.h"
//#include "malloc.h"

#ifdef __cplusplus
 extern "C" {
#endif
void at_hal_init(void);
void at_usleep(unsigned long usec);
void drv_delay_ms(unsigned int ms);
uint64_t at_gettime_ms(void);
void* at_malloc(size_t size);
void* at_calloc(size_t n, size_t size);
void at_free(void* ptr);
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __SYS_H_ */

