#include "at_hal_init.h"
#include "los_memory.h"
void at_hal_init(void)
{
    hal_uart_init();
}
/**
 * at_adapter user interface
 */
void at_usleep(unsigned long usec)
{
    int i = 10;
    i *= usec;
    while(i--);
}

uint64_t at_gettime_ms(void)
{
    return HAL_GetTick();
}

int at_random(void* output, size_t len)
{
    return 0;//hal_rng_generate_buffer(output, len);
}

void at_reboot(void)
{
    
}

void* at_malloc(size_t size)
{
		return LOS_MemAlloc(m_aucSysMem0,size);
}

void* at_calloc(size_t n, size_t size)
{
    void *p = at_malloc(n * size);
    if(p)
    {
        memset(p, 0, n * size);
    }

    return p;
}
void at_free(void* ptr)
{ 
    (void)LOS_MemFree(m_aucSysMem0,ptr);
}

void drv_delay_ms(unsigned int ms)
{
		HAL_Delay(ms);
}
