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

#include <openssl/rsa.h>
#include <memory>
#include <string>

#include "utility/windows_support.h"

using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;

class CM_API OpenSSL_RSA {
public:
    OpenSSL_RSA();

    ~OpenSSL_RSA();

    bool generate();

    bool generate_public(std::string key);

    bool generate_private(std::string key);

    std::string get_public_key();

    std::string get_private_key();

    std::string encrypt(std::string plain_text);

    std::string decrypt(std::string cipher_text);

    std::string sign(std::string plain_text);

    bool verify(std::string cipher_text, std::string signature);

private:
    RSA_ptr _rsa_public;
    RSA_ptr _rsa_private;
};
