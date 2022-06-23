/****************************************************************************
 *
 *      Copyright (c) 2022, Auterion Ltd. All rights reserved.
 *
 * All information contained herein is, and remains the property of
 * Auterion Ltd. and its suppliers, if any. The intellectual and technical
 * concepts contained herein are proprietary to Auterion Ltd. and its
 * suppliers and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Reproduction or distribution, in whole or in part, of this information
 * or reproduction of this material is strictly forbidden unless prior
 * written permission is obtained from Auterion Ltd.
 *
 ****************************************************************************/

/**
 * @file sockets.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define closesocket close
#define SOCKET int
#define INVALID_SOCKET -1
#elif defined __MINGW32__
#include <ws2tcpip.h>
#else
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <io.h>
#endif
