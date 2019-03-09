#ifndef __SIM900A_H__
#define __SIM900A_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
/* Defines ------------------------------------------------------------------*/

#define SIM900A_Q_SIZE    3
#define SIM900A_ITEM_SIZE sizeof(queue_buf)

/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
typedef struct
{
    char * cmd;
    char * resp;
}AT_CMD;


typedef struct
{
    uint32_t len;
    uint8_t* addr;
}queue_buf;

/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SIM900A_H__ */
