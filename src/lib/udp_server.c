/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#if TORNADO_OS_WINDOWS
#include <WinSock2.h>
#include <Windows.h>
typedef USHORT in_port_t;
typedef SSIZE_T ssize_t;
typedef int socklen_t;
#else
#include <arpa/inet.h>
#endif
#include <clog/clog.h>
#include <stdio.h>
#include <string.h>
#include <udp-server/udp_server.h>


int udpServerStartup(void)
{
    #if TORNADO_OS_WINDOWS
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        if (wsaData.wHighVersion != 2 || wsaData.wVersion != 2) {
            return -3;
        }
        return -1;
    }
    return 0;
    #else
    return 0;
    #endif
}

static int udpServerCreate(void)
{
    int handle = socket(PF_INET, SOCK_DGRAM, 0);
    return handle;
}

static int udpServerBind(int handle, in_port_t port)
{
    struct sockaddr_in servaddr;

    tc_mem_clear_type(&servaddr);

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    int result;
    if ((result = bind(handle, (const struct sockaddr*) &servaddr, sizeof(servaddr))) < 0) {
        CLOG_WARN("could not bind to port %d", port);
        return result;
    }

    return 0;
}

int udpServerInit(UdpServerSocket* self, uint16_t port)
{
    self->handle = udpServerCreate();

    int result;
    if ((result = udpServerBind(self->handle, port)) < 0) {
        return result;
    }

    CLOG_INFO("listening to port %d", port);

    return 0;
}

int udpServerSend(UdpServerSocket* self, const uint8_t* data, size_t size, const struct sockaddr_in* peer_address)
{
    ssize_t number_of_octets_sent = sendto(self->handle, (const char*)data, size, 0, (struct sockaddr*) peer_address,
                                           sizeof(struct sockaddr_in));
    if (number_of_octets_sent < 0) {
        CLOG_WARN("Error send! %zd\n", number_of_octets_sent)
        return (int) number_of_octets_sent;
    }

    return ((size_t) number_of_octets_sent == size);
}

int udpServerReceive(UdpServerSocket* self, uint8_t* data, size_t* size, struct sockaddr_in* peer_address)
{
    socklen_t addr_size = sizeof(struct sockaddr_in);
    ssize_t number_of_octets = recvfrom(self->handle, (char*)data, *size, 0, (struct sockaddr*) peer_address, &addr_size);
    if (number_of_octets < 0) {
        CLOG_WARN("Error receive! %zd\n", number_of_octets)
        *size = 0;
        return (int) number_of_octets;
    }

    *size = number_of_octets;

    return (int) number_of_octets;
}
