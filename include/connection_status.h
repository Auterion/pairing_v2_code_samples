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
 * @file connection_status.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include <string>

#include "utility/windows_support.h"

enum class ConnectionStatusEnum {
    IDLE = 0,
    CONFIGURE_FOR_PAIRING = 1,
    PAIRING_IDLE = 2,
    PAIRING = 3,
    CONFIGURE_FOR_CONNECTING = 4,
    CONNECTING = 5,
    CONNECTED = 6,
    DISCONNECTED = 7,
    RECONFIGURING = 8,
    RECONFIGURED = 9,

    DRIVER_NOT_CONNECTED = 100,
    DRIVER_CONNECTED = 101,
    DRIVER_WIRED_CONNECTED = 102,

    ERROR_STATUS = -1,
    ERROR_PAIRING = -2,
    ERROR_CONNECTING = -3,
    ERROR_RECONFIGURING = -4,

    ERROR_DRIVER_DETECTION = -100,
    ERROR_DRIVER_CONNECTION = -101,
    ERROR_DRIVER_LOGIN = -102,
    ERROR_DRIVER_CONFIGURATION = -103,
    ERROR_DRIVER_TIMEOUT = -104
};
struct CM_API ConnectionStatus {
    ConnectionStatusEnum code;
    std::string context;
};
