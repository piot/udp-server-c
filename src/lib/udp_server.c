/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifdef TORNADO_OS_WINDOWS
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

#include <fcntl.h>
#include <stdbool.h>

#if defined TORNADO_OS_WINDOWS
// #define UDP_SERVER_SOCKET_CLOSE closesocket
// #define UDP_SERVER_ERROR_INPROGRESS WSAEINPROGRESS
#define UDP_SERVER_ERROR_WOULDBLOCK WSAEWOULDBLOCK
#define UDP_SERVER_ERROR_AGAIN WSAEINPROGRESS
// #define UDP_SERVER_ERROR_NOT_CONNECTED WSAENOTCONN
#define UDP_SERVER_GET_ERROR WSAGetLastError()
#define UDP_SERVER_SIZE_CAST(a) (int) a
#else
// #define UDP_SERVER_SHUTDOWN_READ_WRITE SHUT_RDWR
// #define UDP_SERVER_ERROR_INPROGRESS EINPROGRESS
#define UDP_SERVER_ERROR_WOULDBLOCK EINPROGRESS
#define UDP_SERVER_ERROR_AGAIN EAGAIN
// #define UDP_SERVER_INVALID_SOCKET_HANDLE (-1)
#include <errno.h>
#include <unistd.h>
// #define UDP_SERVER_SOCKET_CLOSE close
#define UDP_SERVER_GET_ERROR errno
#define UDP_SERVER_SIZE_CAST(a) a
#endif

static int setSocketNonBlocking(UDP_SERVER_SOCKET_HANDLE handle, bool nonBlocking)
{
#if defined TORNADO_OS_WINDOWS
    u_long mode = nonBlocking ? 1U : 0U;
    int result = ioctlsocket(handle, FIONBIO, &mode);
    if (result != NO_ERROR) {
        return result; // udp_client_error(result, "ioctlsocket failed with error");
    }
    return 0;
#else
    int flags = fcntl(handle, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    if (nonBlocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    return fcntl(handle, F_SETFL, flags);
#endif
}

int udpServerStartup(void)
{
#ifdef TORNADO_OS_WINDOWS
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

static UDP_SERVER_SOCKET_HANDLE udpServerCreate(bool blocking)
{
    UDP_SERVER_SOCKET_HANDLE handle = socket(PF_INET, SOCK_DGRAM, 0);

    if (!blocking) {
        setSocketNonBlocking(handle, true);
    }
    return handle;
}

static int udpServerBind(UDP_SERVER_SOCKET_HANDLE handle, in_port_t port)
{
    struct sockaddr_in servaddr;

    tc_mem_clear_type(&servaddr);

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    int result;
    if ((result = bind(handle, (const struct sockaddr*) &servaddr, sizeof(servaddr))) < 0) {
        CLOG_WARN("udpServerBind: could not bind to port %d", port)
        return result;
    }

    return 0;
}

int udpServerInit(UdpServerSocket* self, uint16_t port, bool blocking)
{
    self->handle = udpServerCreate(blocking);
    self->isBlocking = blocking;

    int result;
    if ((result = udpServerBind(self->handle, port)) < 0) {
        return result;
    }

    CLOG_INFO("udpServerInit: listening to port %d", port)

    return 0;
}

int udpServerSend(UdpServerSocket* self, const uint8_t* data, size_t size, const struct sockaddr_in* peer_address)
{
    if (size > UDP_SERVER_MAX_OCTET_SIZE) {
        CLOG_SOFT_ERROR("udpServerSend. Maximum UDP datagram size is %zu, but encountered %zu",
                        UDP_SERVER_MAX_OCTET_SIZE, size)
        return -4;
    }

    if (size == 0) {
        CLOG_SOFT_ERROR("udpServerSend. Can not send zero size datagrams")
        return -5;
    }

    ssize_t number_of_octets_sent = sendto(self->handle, (const char*) data, UDP_SERVER_SIZE_CAST(size), 0,
                                           (const struct sockaddr*) peer_address, sizeof(struct sockaddr_in));
    if (number_of_octets_sent < 0) {
        CLOG_WARN("udpServerSend: error send %zd", number_of_octets_sent)
        return (int) number_of_octets_sent;
    }

    return ((size_t) number_of_octets_sent == size);
}

ssize_t udpServerReceive(UdpServerSocket* self, uint8_t* data, size_t dataMaxSize, struct sockaddr_in* peer_address)
{
    if (dataMaxSize != UDP_SERVER_MAX_OCTET_SIZE) {
        CLOG_SOFT_ERROR("udpServerReceive. Use recommended UDP max buffer size of %zu, instead of %zu",
                        UDP_SERVER_MAX_OCTET_SIZE, dataMaxSize)
    }

    socklen_t addr_size = sizeof(struct sockaddr_in);
    ssize_t number_of_octets = recvfrom(self->handle, (char*) data, UDP_SERVER_SIZE_CAST(dataMaxSize), 0,
                                        (struct sockaddr*) peer_address, &addr_size);
    if (number_of_octets < 0) {
        if (number_of_octets == -1 && !self->isBlocking) {
            int last_err = UDP_SERVER_GET_ERROR;
            if (last_err == UDP_SERVER_ERROR_AGAIN || last_err == UDP_SERVER_ERROR_WOULDBLOCK) {
            }
            return 0;
        }
        CLOG_WARN("udpServerReceive: error receive! %zd", number_of_octets)
        return (int) number_of_octets;
    }

    return (int) number_of_octets;
}
