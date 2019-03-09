/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/


#if !defined(SOCKET_ERROR)
	#define SOCKET_ERROR -1
#endif

#include "transport.h"

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
#define INVALID_SOCKET	-1
static int msock = INVALID_SOCKET;


int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	rc = at_api_send(sock, buf, buflen);
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
	int rc = at_api_recv(msock, buf, count);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = at_api_recv(sock, buf, count);	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, const char* port)
{
	msock= at_api_connect(addr, port, 0);
	printf("get msocke is %d",msock);
	if(msock < 0)
	{
		printf("socket create failed,mosock is %d\r\n",msock);
	}
	return msock;
}

#define SHUT_WR	1

int transport_close(int sock)
{
	int rc;
	rc = at_api_close(sock);

	return rc;
}
