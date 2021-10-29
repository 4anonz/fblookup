/* fblookup.h */
/**
 * The MIT License (MIT) THIS TOOL IS ONLY FOR EDUCATIONAL PURPOSES ONLY 
 * Copyright (C) 2021 Muhammad Zayyad

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.

*/

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
/* if compling on Windows include these header files */
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, ws2_32.lib)
#else

/* else unix-based header files (linux, macOs)*/
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif
/*
    As some of Berkerly sockets and winsock functions deffers,
    This code helps to maintain code flexibility  
*/
#if defined(_WIN32)
#define GetErrorNo() (WSAGetLastError())
#define CloseSocket(s) closesocket(s)
#else
#define GetErrorNo() (errno)
#define CloseSocket(s) close(s)
#define IsValidSocket(s) ((s) >= 0)
#define SOCKET int
#endif

/*Global headers*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const char *white = "\033[1;37m",
//*grey = "\033[0;37m",
//*purple = "\033[0;35m",
*red = "\033[1;31m",
*green = "\033[0;32m",
*yellow = "\033[1;33m",
*cyan = "\033[0;36m",
//*cafe = "\033[0;33m",
//*fiuscha = "\033[0;35m",
*blue = "\033[1;34m",
*reset = "\e[0m";

/*fblookup.h*/