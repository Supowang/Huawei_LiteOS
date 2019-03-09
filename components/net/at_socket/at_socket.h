#ifndef _ATINY_SOCKET_H_
#define _ATINY_SOCKET_H_

#include <stdio.h>
#include <stdint.h>

#include "at_hal_init.h"
#include "at_api_interface.h"

#define AT_PROTO_TCP 0 /* < The TCP transport protocol */
#define AT_PROTO_UDP 1 /* < The UDP transport protocol */

typedef struct
{
    int fd;
} at_net_context;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@ingroup at_socket
 *@brief create socket and connect to the server
 *
 *@par Description:
 *This API is used to create socket and connect to the server. It will do name resolution with both IPv6 and IPv4, do internal memory allocation for socket structure and release it by the atiny_net_close interface.
 *@attention none.
 *
 *@param host           [IN] Host to connect to.
 *@param port           [IN] Port to connect to.
 *@param proto          [IN] Protocol: ATINY_PROTO_TCP or ATINY_PROTO_UDP.
 *
 *@retval #pointer      Point to the Socket you created.
 *@retval NULL          Create socket or connect to server failed.
 *@par Dependency: none.
 *@see at_net_recv | at_net_send | at_net_recv_timeout | ati_net_close
 */
void* at_net_connect(const char* host, const char* port, int proto);

/**
 *@ingroup at_socket
 *@brief read characters from peer.
 *
 *@par Description:
 *This API is used to read at most 'len' characters. If no error occurs, the actual amount read is returned.
 *@attention none.
 *
 *@param ctx            [IN] Pointer to the socket.
 *@param buf            [IN] The buffer to write to.
 *@param len            [IN] Maximum length of the buffer.
 *
 *@retval #int          the number of bytes received.
 *@retval 0             indicates recv() would block.
 *@retval -1            recv failed, you should try to connect again.
 *@par Dependency: none.
 *@see at_net_recv | at_net_send | at_net_recv_timeout | ati_net_close
 */
int at_net_recv(void* ctx, unsigned char* buf, size_t len);

/**
 *@ingroup at_socket
 *@brief write characters to peer.
 *
 *@par Description:
 *This API is used to write at most 'len' characters. If no error occurs, the actual amount send is returned.
 *@attention none.
 *
 *@param ctx            [IN] Pointer to the socket.
 *@param buf            [IN] The buffer to read from.
 *@param len            [IN] The length of the buffer.
 *
 *@retval #int          the number of bytes sent.
 *@retval 0             indicates send() would block.
 *@retval -1            send failed, you should try to connect again.
 *@par Dependency: none.
 *@see at_net_recv | at_net_send | at_net_recv_timeout | ati_net_close
 */
int at_net_send(void* ctx, const unsigned char* buf, size_t len);

/**
 *@ingroup at_socket
 *@brief read characters from peer with timeout option.
 *
 *@par Description:
 *This API is used to read at most 'len' characters, blocking for at most 'timeout' seconds. If no error occurs, the actual amount read is returned.
 *@attention
 *<ul>
 *<li>This function will block (until data becomes available or</li>
 *<li>timeout is reached) even if the socket is set to</li>
 *<li>non-blocking. Handling timeouts with non-blocking reads</li>
 *<li>requires a different strategy.</li>
 *</ul>
 *
 *@param ctx            [IN] Pointer to the socket.
 *@param buf            [IN] The buffer to write to.
 *@param len            [IN] Maximum length of the buffer.
 *@param timeout        [IN] Maximum number of milliseconds to wait for data, 0 means no timeout (wait forever).
 *
 *@retval #int          the number of bytes sent.
 *@retval 0             indicates recv() would block.
 *@retval -1            recv failed, you should try to connect again.
 *@retval -2            this operation is timed out.
 *@par Dependency: none.
 *@see at_net_recv | at_net_send | at_net_recv_timeout | ati_net_close
 */
int at_net_recv_timeout(void* ctx, unsigned char* buf, size_t len,
                           uint32_t timeout);

/**
 *@ingroup at_socket
 *@brief gracefully shutdown the connection.
 *
 *@par Description:
 *This API is used to gracefully shutdown the connection and free associated data.
 *@attention none.
 *
 *@param ctx            [IN] Pointer to the socket to free.
 *
 *@retval none.
 *@par Dependency: none.
 *@see at_net_recv | at_net_send | at_net_recv_timeout | ati_net_close
 */
void at_net_close(void* ctx);

#ifdef __cplusplus
}
#endif

#endif /* _ATINY_SOCKET_H_ */

