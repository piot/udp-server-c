/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef udp_server_h
#define udp_server_h

#include <stdint.h>
#include <stdlib.h>

#if TORNADO_OS_WINDOWS
#include <WinSock2.h>
#include <Windows.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif

typedef struct UdpServerSocket {
    int handle;
} UdpServerSocket;

int udpServerStartup(void);

int udpServerInit(UdpServerSocket* self, uint16_t port);
int udpServerSend(UdpServerSocket* self, const uint8_t* data, size_t size, const struct sockaddr_in* peer_address);
int udpServerReceive(UdpServerSocket* self, uint8_t* data, size_t* size, struct sockaddr_in* peer_address);

#endif
