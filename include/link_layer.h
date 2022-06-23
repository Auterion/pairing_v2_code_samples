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
 * @file link_layer.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include <functional>
#include <mutex>
#include <string>

#include "json.h"
#include "utility/windows_support.h"

/**
 * @brief Abstract base class for pairing protocol link layer
 */
class CM_API LinkLayer {
public:
    /**
     * @brief Constructor
     */
    LinkLayer();

    /**
     * @brief Destructor
     */
    virtual ~LinkLayer(){};

    /**
     * @brief Stop link layer
     */
    virtual void stop() = 0;

    /**
     * @brief Initialize
     * @return Result of the initialization
     */
    virtual bool init() = 0;

    /**
     * @brief Send message
     * @param message to be sent
     * @param params json object containing destination address
     * @return true if sending succeeded
     */
    virtual bool send(const std::string& message, const Json::Value& params) = 0;

    /**
     * @brief Register callback function to be called when message is received
     * @param message_received callback function
     */
    void register_message_callback(std::function<void(const std::string&, const std::string&)> message_received);

protected:
    std::mutex _message_received_mutex;
    std::function<void(const std::string&, const std::string&)> _message_received;
};
