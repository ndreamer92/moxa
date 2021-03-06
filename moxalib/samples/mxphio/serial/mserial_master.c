/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mserial_master.c

    Routines to simulate a simple serial master.

    2008-11-15	CF Lin
		new release
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mserial_port.h"

static int	
handle_packet (unsigned int fd, unsigned char *buffer, int len)
{
	static int count = 1;

	buffer[len] = 0;
	printf("RECV[%d]: %s\n", len, buffer);
#ifdef WIN32
	Sleep(1000);
#else
	sleep(1);
#endif
	sprintf(buffer, "I am a master %d", count++);
	len = strlen(buffer);
	/* send it to the peer */
	len = mxsp_write(fd, buffer, len, NULL);
	buffer[len] = 0;
	printf("SEND[%d]: %s\n", len, buffer);
	return len;
}

int
#if defined(_WIN32_WCE)
WINAPI 
WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main (int argc, char **argv)
#endif
{
	unsigned int fd;
	int port = 0;
	int len = 0, n, max;
	unsigned char buffer[256];

#if defined(_WIN32_WCE)
	int	argc;
	char cmdline[256], *argv[32];

	WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)lpCmdLine, 255, cmdline, 256, NULL, NULL);
	argc = split_line(argv+1, 32, cmdline)+1;
#endif

	if (argc < 1)
	{
		printf("usage: <progname> <port>\n");
		return 1;
	}
	port = atoi(argv[1]);
	fd = mxsp_open(port);
	if (fd ==0 )
	{
		printf("fail to open COM port %d\n", port);
		return -1;
	}
/*
	mxsp_set_baudrate(fd, 115200);
	mxsp_set_parity(fd, MSP_PARITY_ODD);
	mxsp_set_databits(fd, 7);
	mxsp_set_stopbits(fd, 2);
	mxsp_set_interface(fd, MSP_RS485_4WIRE_MODE);
	mxsp_set_flow_control(fd, MSP_FLOWCTRL_HW);
*/
	/* set read timeout */
	//mxsp_set_timeout_read(fd, 100);

	/* send a hello message to the slave */
	mxsp_write(fd, "I am a master 0", 15, NULL);
	while(1)
	{
		len = 0;
		max = sizeof(buffer);
		while(max)
		{
			n = mxsp_read(fd, buffer+len, max, NULL);
			if (n < 0)
				return -1;
			if (n==0)
				break;
			len += n;
			max -= n;
		}
		if (len > 0)
			len = handle_packet(fd, buffer, len);
		if (len < 0)
			break;
	}
	mxsp_close(fd);
	return 0;
}
