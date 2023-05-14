/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <clog/console.h>
#include <stdio.h>
#include <udp-server/udp_server.h>

#if !TORNADO_OS_WINDOWS
#include <unistd.h>
#endif

clog_config g_clog;


int main(int argc, char* argv[])
{
	(void) argc;
	(void) argv;

	g_clog.log = clog_console;
	CLOG_VERBOSE("udp server example start")
	UdpServerSocket socket;

	udpServerStartup();

	int errorCode = udpServerInit(&socket, 27000, true);
	if (errorCode < 0) {
		CLOG_WARN("could not init %d", errorCode);
		return errorCode;
	}
	CLOG_VERBOSE("initialized")

	uint8_t buf[1280];
	size_t size;
	struct sockaddr_in addr;
	while (1) {
		CLOG_INFO("receiving...")
		size = 1280;
		errorCode = udpServerReceive(&socket, buf, &size, &addr);
		if (errorCode > 0) {
			printf("received %zu octets\n", size);
		}
	}
}
