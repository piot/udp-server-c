/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef udp_server_h
#define udp_server_h

#include <stdint.h>
#include <stdlib.h>

#ifdef TORNADO_OS_WINDOWS
#include <WinSock2.h>
#include <Windows.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <sys/types.h>
#include <stdbool.h>

#if defined TORNADO_OS_WINDOWS
#define UDP_SERVER_SOCKET_HANDLE SOCKET
#else
#define UDP_SERVER_SOCKET_HANDLE int
#endif

typedef struct UdpServerSocket {
    UDP_SERVER_SOCKET_HANDLE handle;
    bool isBlocking;
} UdpServerSocket;

int udpServerStartup(void);

int udpServerInit(UdpServerSocket* self, uint16_t port, bool blocking);
int udpServerSend(UdpServerSocket* self, const uint8_t* data, size_t size, const struct sockaddr_in* peer_address);
ssize_t udpServerReceive(UdpServerSocket* self, uint8_t* data, size_t maxSize, struct sockaddr_in* peer_address);

#endif
