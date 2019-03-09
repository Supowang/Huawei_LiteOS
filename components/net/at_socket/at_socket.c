#include "at_socket.h"

#define SOCKET_DEBUG

#if defined(SOCKET_DEBUG)
#define SOCKET_LOG(fmt, ...) \
    do \
    { \
        (void)printf("[SOCKET][%s:%d] " fmt "\r\n", \
        __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while(0)
#else
#define SOCKET_LOG(fmt, ...) ((void)0)
#endif


void* at_net_connect(const char* host, const char* port, int proto)
{
    at_net_context* ctx = NULL;

    ctx = at_malloc(sizeof(at_net_context));
    if (NULL == ctx)
    {
        SOCKET_LOG("malloc failed for socket context");
        return NULL;
    }    

    ctx->fd = at_api_connect(host, port, proto);
    if (ctx->fd < 0)
    {
        SOCKET_LOG("unkown host(%s) or port(%s)", host, port);
        at_free(ctx);
        ctx = NULL;
    }
    return ctx;
}

int at_net_recv(void* ctx, unsigned char* buf, size_t len)
{
    int ret = -1;
    int fd = ((at_net_context*)ctx)->fd;

    ret = at_api_recv(fd,buf,len);
	
    return ret;
}

int at_net_recv_timeout(void* ctx, unsigned char* buf, size_t len,
                           uint32_t timeout)
{
    int ret = -1;

    int fd = ((at_net_context*)ctx)->fd;

    ret = at_api_recv_timeout(fd, buf, len, timeout);
    return ret;
}

int at_net_send(void* ctx, const unsigned char* buf, size_t len)
{
    int ret = -1;
    int fd = ((at_net_context*)ctx)->fd;

    if (fd < 0)
    {
        SOCKET_LOG("ilegal socket(%d)", fd);
        return -1;
    }

    ret = at_api_send(fd, buf, len);

    return ret;
}

void at_net_close(void* ctx)
{
    int fd = ((at_net_context*)ctx)->fd;

    if (fd >= 0)
    {

        at_api_close(fd);
    }

    at_free(ctx);
}

