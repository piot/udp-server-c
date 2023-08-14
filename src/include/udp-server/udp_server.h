/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef UDP_SERVER_H
#define UDP_SERVER_H

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

/// Maximum size is 1200, since the general recommendations are somewhere between 1200 and 1400 octets.
/// (https://www.ietf.org/id/draft-ietf-dnsop-avoid-fragmentation-06.html#section-3.3).
/// Steam Networking has a 1200 octet packet size limit (https://partner.steamgames.com/doc/api/ISteamNetworking)
static const size_t UDP_SERVER_MAX_OCTET_SIZE = 1200;

typedef struct UdpServerSocket {
    UDP_SERVER_SOCKET_HANDLE handle;
    bool isBlocking;
} UdpServerSocket;

int udpServerStartup(void);

int udpServerInit(UdpServerSocket* self, uint16_t port, bool blocking);
int udpServerSend(UdpServerSocket* self, const uint8_t* data, size_t size, const struct sockaddr_in* peer_address);
ssize_t udpServerReceive(UdpServerSocket* self, uint8_t* data, size_t maxSize, struct sockaddr_in* peer_address);

#endif
