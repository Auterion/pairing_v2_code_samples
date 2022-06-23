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

#pragma once

#include <openssl/evp.h>
#include <string>
#include <vector>

class OpenSSL_Base64 {
public:
    static std::string encode(const std::vector<unsigned char>& binary);

    static std::vector<unsigned char> decode(std::string encoded);
};
