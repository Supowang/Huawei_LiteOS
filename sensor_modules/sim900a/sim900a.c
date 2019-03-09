#if defined(WITH_AT_FRAMEWORK) && defined(USE_SIM900A)
/* Includes -----------------------------------------------------------------*/
#include "sim900a.h"
#include "at_api_interface.h"
#include "hal_init.h"
#include "atiny_adapter.h"

/* Defines ------------------------------------------------------------------*/
#define AT_DATAF_PREFIX         "\r\n+IPD"
#define AT_DATAF_PREFIX_MULTI   "\r\n+RECEIVE"

/* Typedefs -----------------------------------------------------------------*/

typedef enum 
{
    AT_CMD_AT = 0,
    AT_CMD_CPIN,
    AT_CMD_COPS,
    AT_CMD_CLOSE,
    AT_CMD_SHUT,
    AT_CMD_ECHO_OFF,
    AT_CMD_ECHO_ON,
    AT_CMD_MUX,
    AT_CMD_CLASS,
    AT_CMD_PDP_CONT,
    AT_CMD_PDP_ATT,
    AT_CMD_PDP_ACT,
    AT_CMD_CSTT,
    AT_CMD_CIICR,
    AT_CMD_CIFSR,
    AT_CMD_CIPHEAD,
    AT_CMD_CONN,
    AT_CMD_SEND,
    AT_CMD_MAX
}AT_CmdType;

/* Macros -------------------------------------------------------------------*/
const AT_CMD sim900a_cmd[] = {
        {"AT",          "OK"},
        {"AT+CPIN?",    "OK"}, //check sim card
        {"AT+COPS?",    "CHINA MOBILE"},
        {"AT+CIPCLOSE", "CLOSE OK"},
        {"AT+CIPSHUT",  "SHUT OK"},
        {"ATE0",        "OK\r\n"},
        {"ATE1",        "OK\r\n"},
        {"AT+CIPMUX",   "OK"},
        {"AT+CGCLASS",  "OK"},
        {"AT+CGDCONT",  "OK"},
        {"AT+CGATT",    "OK"},
        {"AT+CGACT",    "OK"},
        {"AT+CSTT",     "OK"},
        {"AT+CIICR",    "OK"},
        {"AT+CIFSR",    ""},
        {"AT+CIPHEAD",  "OK"},
        {"AT+CIPSTART", "CONNECT OK"},
        {"AT+CIPSEND",  "SEND OK"},
        {NULL, NULL},
};

/* Local variables ----------------------------------------------------------*/

QUEUE_DEFS(sim900a_q, SIM900A_Q_SIZE, SIM900A_ITEM_SIZE);

/* Extern variables ---------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
/* Public functions ---------------------------------------------------------*/

int32_t at_send_cmd(uint8_t * buf, uint32_t len, uint8_t * resp)
{
    char cmd[64] = {0};
    snprintf(cmd, 64, "%s\r\n", buf);
    return hal_uart_send((uint8_t*)cmd, strlen(cmd), resp);
}

int32_t at_send_data(uint8_t *buf, uint32_t len, uint8_t *resp)
{
    return hal_uart_send(buf, len, resp);
}

int32_t sim900a_send_cmd(AT_CmdType e)
{
    if (e >= AT_CMD_MAX)
        return AT_FAILED;
        
    return at_send_cmd((uint8_t *)sim900a_cmd[e].cmd, strlen((char*)sim900a_cmd[e].cmd), (uint8_t *)sim900a_cmd[e].resp);
}

int32_t sim900a_echo_off(void)
{
    return sim900a_send_cmd(AT_CMD_ECHO_OFF);
}
int32_t sim900a_echo_on(void)
{
    return sim900a_send_cmd(AT_CMD_ECHO_ON);
}
int32_t sim900a_reset(void)
{
    return sim900a_send_cmd(AT_CMD_SHUT);
}

int32_t sim900a_set_mux_mode(int32_t m)
{
    char cmd[64] = {0};
    snprintf(cmd, 64, "%s=%d", sim900a_cmd[AT_CMD_MUX].cmd, (int)m);
    return at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t*)sim900a_cmd[AT_CMD_MUX].resp);
}

int32_t sim900a_connect(const int8_t * host, const int8_t * port, int32_t proto)
{
    int32_t ret = AT_FAILED;
    char cmd[64] = {0};
    
    sim900a_reset();

    snprintf(cmd, 64, "%s=\"B\"", sim900a_cmd[AT_CMD_CLASS].cmd);
    at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t*)sim900a_cmd[AT_CMD_CLASS].resp);
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, 64, "%s=1,\"IP\",\"CMNET\"", sim900a_cmd[AT_CMD_PDP_CONT].cmd);
    at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t*)sim900a_cmd[AT_CMD_PDP_CONT].resp);
    
    memset(cmd, 0, sizeof(cmd)); 
    snprintf(cmd, 64, "%s=1", sim900a_cmd[AT_CMD_PDP_ATT].cmd);
    at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t*)sim900a_cmd[AT_CMD_PDP_ATT].resp);
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, 64, "%s=1", sim900a_cmd[AT_CMD_CIPHEAD].cmd);
    at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t*)sim900a_cmd[AT_CMD_CIPHEAD].resp);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, 64, "%s=\"%s\",\"%s\",\"%s\"", sim900a_cmd[AT_CMD_CONN].cmd, 
                        proto == 1 ? "UDP" : "TCP", host, port); 
    ret = at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t*)sim900a_cmd[AT_CMD_CONN].resp);

    return ret;
}

int32_t  sim900a_recv_timeout(int32_t id, int8_t * buf, uint32_t len, int32_t timeout)
{
    queue_buf qbuf = {0, NULL};
    int32_t rxlen = 0;
    timeout /= 5;
    
    
    do {
        if (hal_mb_get(&qbuf) == 0 && qbuf.len > 0)
        {
            rxlen = (len < qbuf.len) ? len : qbuf.len;

            memcpy(buf, qbuf.addr, rxlen);
            atiny_free(qbuf.addr);
            break;
        }
        if (timeout != AT_WAIT_FOREVER || timeout > 0)
        {
            drv_delay_ms(5);
            timeout --;
        }
    }while(timeout > 0);
    return rxlen;
}

int32_t  sim900a_recv(int32_t id, int8_t * buf, uint32_t len)
{
    return sim900a_recv_timeout(id, buf, len, AT_WAIT_FOREVER);
}

int32_t sim900a_send(int32_t id , const uint8_t* buf, uint32_t len)
{
    int32_t ret = AT_FAILED;
    char cmd[64] = {0};
    
    snprintf(cmd, 64, "%s=%ld", sim900a_cmd[AT_CMD_SEND].cmd, len);

    ret = at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t *)">");
    ret = at_send_data((uint8_t *)buf, len, (uint8_t*)sim900a_cmd[AT_CMD_SEND].resp);
    if (ret == 0)
        ret = len;
    return ret;
}

void sim900a_check(void)
{
    //check module response
    while(AT_FAILED == sim900a_send_cmd(AT_CMD_AT))
    {
        drv_delay_ms(500);
    }
    if(AT_OK == sim900a_send_cmd(AT_CMD_CPIN))
    {
    }
    if(AT_OK == sim900a_send_cmd(AT_CMD_COPS))
    {
    }
}

int32_t sim900a_recv_cb(int32_t id)
{
    return AT_FAILED;
}

int32_t sim900a_close(int32_t id)
{
    char cmd[64] = {0};
    snprintf(cmd, 64, "%s", sim900a_cmd[AT_CMD_CLOSE].cmd);
    return at_send_cmd((uint8_t*)cmd, strlen(cmd), (uint8_t*)sim900a_cmd[AT_CMD_CLOSE].resp);
}

int32_t sim900a_data_handler(uint8_t byte)
{
    static int state = 0;  //0: get id, 1: get len, 2:get data
    static int len = 0;
    static char buf[1024] = {0};
    static int buf_count = 0;
        
    switch (state)
    {
        case 0:  //byte = ','
           len = 0;
            memset(buf, 0, 1024);
            buf_count = 0;
            state ++;
            break;

        case 1: //byte = len:
            if (byte >= '0' && byte <= '9')
                len = len * 10 + byte - '0';
            else
                state ++;
            break;
        case 2: //byte = data
            if (buf_count < len)
                buf[buf_count ++] = byte;
           
            if (buf_count == len)
            {
                state ++;
                queue_buf b;
                
                if (len > 0) 
                {
                    b.len = len;
                    b.addr = atiny_malloc(len);
                    memcpy(b.addr, buf, len);
                    
                    hal_mb_put(&b);
                }
            }
            break; 
        default:
            break;
    }
    if (state > 2)
    {
        state = 0;
        return 1;
    }
    return 0;
}
int32_t sim900a_ini()
{
    net_event e = {
        .perfix = AT_DATAF_PREFIX,
        .suffix = NULL,
        .cb = sim900a_data_handler,
    };
    
    hal_mailbox_init(&sim900a_q);
    recv_event_register(&e);
    
    sim900a_echo_off();
    sim900a_check();
    sim900a_reset();
    sim900a_set_mux_mode(0);
    
    return AT_OK;
}

int32_t sim900a_deinit(void)
{
    return AT_OK;
}

/* Private functions --------------------------------------------------------*/

at_adaptor_api at_interface = {
    .init = sim900a_ini,
    .connect = sim900a_connect, /*TCP or UDP connect */
    .send = sim900a_send, /*send data, if no response, retrun error*/
    .recv_timeout = sim900a_recv_timeout,
    .recv = sim900a_recv,
    .close = sim900a_close,/*close connect*/
    .recv_cb = sim900a_recv_cb,/*receive event handle, no available by now */
    .deinit = sim900a_deinit,
};

#endif
