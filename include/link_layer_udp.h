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
 * @file link_layer_udp.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include "link_layer.h"
#include "json.h"
#include "sockets.h"
#include "utility/windows_support.h"

class CM_API LinkLayerUDP : public LinkLayer {
public:
    /**
     * @brief Constructor
     * @param port UDP listening port
     * @param multicast_ip Multicast IP, if empty then use broadcast
     */
    LinkLayerUDP(uint16_t port = 0, const std::string& multicast_ip = "");

    /**
     * @brief Destructor
     */
    virtual ~LinkLayerUDP();

    /**
     * @brief Stop link layer
     */
    void stop() override;

    /**
     * @brief Initialize
     * @return Result of the initialization
     */
    bool init() override;

    /**
     * @brief Send message
     * @param ip address where to send to
     * @param port port where to send to
     * @return true if sending succeeded
     */
    bool send(const std::string& msg, const std::string& ip, uint16_t port);

    /**
     * @brief Send multicast message
     * @param local_ip address to send from
     * @param multicast_ip multicast address where to send to
     * @param port port where to send to
     * @return true if sending succeeded
     */
    bool send_multicast(const std::string& msg, const std::string& local_ip, const std::string& multicast_ip, uint16_t port);

    /**
     * @brief Send message
     * @param message to be sent
     * @param params json object containing destination address
     * @return true if sending succeeded
     */
    bool send(const std::string& message, const Json::Value& params) override;

    /**
     * @brief Get local port on which UDP socket is bound
     * @return local port
     */
    uint16_t get_local_port();

    /**
     * @brief Add interface to multicast group
     * @return interface_ip ip of the local interface
     */
    void add_multicast_membership(const std::string& interface_ip);

private:
    std::atomic<bool> _should_exit{false};
    std::thread _worker_thread;
    SOCKET _sock;
    uint16_t _port;
    std::string _multicast_ip;
    std::set<std::string> _local_interfaces;

    /**
     * @brief Thread worker
     */
    void worker();
};
