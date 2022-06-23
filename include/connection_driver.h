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
 * @file connection_driver.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <limits.h>
#include <mutex>
#include <string>
#include <thread>

#include "connection_status.h"
#include "json.h"
#include "utility/windows_support.h"

/**
 * @brief Base class for all communication drivers.
 */
class CM_API ConnectionDriver {
public:
    virtual ~ConnectionDriver() = default;

    /**
     * @brief Stop the driver
     */
    virtual void stop() = 0;

    /**
     * @brief Connection information to be broadcasted to remotes
     * @param info resulting broadcast information
     * @return false if driver is not ready
     */
    virtual bool get_broadcast_info(Json::Value& info) = 0;

    /**
     * @brief Get local computer IP in the network assigned to this driver
     * @return local ip or empty
     */
    virtual std::string get_local_ip() = 0;

    /**
     * @brief Should we report wired status for this driver (blinking pattern on Skynode)
     * @return true if wired status reporting is enabled
     */
    virtual bool report_wired_status() = 0;

    /**
     * @brief Initialite driver with specified configuration
     * @param configuration json object containing driver configuration
     */
    virtual bool init(const Json::Value& configuration);

    /**
     * @brief Configure the driver with specified configuration
     * @param configuration json object containing driver configuration
     */
    virtual bool configure(const Json::Value& configuration);

    /**
     * @brief Enter pairing mode
     *
     * @todo Find a nicer solution for this, since it's only used by the microhard in preconfigured mode.
     */
    virtual void enter_pairing_mode(){};

    /**
     * @brief Register function for receiving telemetry data from the driver
     * @param telemetry_callback telemetry callback function
     */
    void register_telemetry_callback(std::function<void(const std::string& context, const Json::Value&)> telemetry_callback);

    /**
     * @brief Register function for receiving status updates from the driver
     * @param status_callback status callback function
     */
    void register_status_callback(std::function<void(const std::string& context, const ConnectionStatusEnum&)> status_callback);

    /**
     * @brief Are we using simplified pairing?
     * @return simplified status
     */
    bool simplified() const;

    /**
     * @brief Should we autopair as soon as we receive broadcast?
     * @return true if autopairing enabled
     */
    bool autopair() const;

    /**
     * @brief Return UDP port for mavlink protocol
     * @return UDP port or 0 if driver will not be used for mavlink
     */
    uint16_t mavlink_port();

    /**
     * @brief Get driver name
     * @return name
     *
     * @todo Make this function abstract, so that each driver has to report it's own name when loaded.
     */
    std::string name();

    /**
     * @brief Get driver instance
     * @return instance
     */
    std::string instance();

    /**
     * @brief Set driver instance
     * @instance name of this driver instance
     */
    void set_instance(const std::string& instance);

    /**
     * @brief Get driver download bandwidth
     * @return download bandwidth in bytes/s.
     */
    int download_bandwidth() const;

    /**
     * @brief Get driver streaming priority
     * @return streaming priority. Lower value means higher priorty. -1 means driver shouldn't be used for streaming.
     */
    int streaming_priority() const;

    /**
     * @brief Get driver settings
     * @param returned settings
     */
    virtual void get_pairing_settings(Json::Value& settings) = 0;

    /**
     * @brief Get driver instance connection settings. Connection settings are the settings that can be changed while GCS and vehicle are
     * connected
     * @param returned settings
     * @return false if there are no connection settings and status
     */
    virtual bool get_connection_settings(Json::Value& settings) = 0;

    /**
     * @brief Get radio IP from configuration
     * @return IP
     */
    virtual std::string get_ip();

    /**
     * @brief Set the radio IP.
     *
     * @param ip Radio IP.
     */
    virtual void set_ip(const std::string& ip);

    /**
     * @brief Get connection driver VLAN specification
     * @return VLAN specification or empty string if not available
     */
    virtual std::string get_vlan();

protected:
    std::mutex _configuration_mutex;
    Json::Value _configuration;
    bool _simplified = false;
    bool _autopair = false;
    bool _mavlink = true;
    int _download_bandwidth = INT_MAX;
    int _streaming_priority = INT_MAX;
    uint16_t _mavlink_port = 0;

    /**
     * @brief Report driver status to connection manager
     * @param status status to report
     */
    void report_status(const ConnectionStatusEnum& status);

    /**
     * @brief Report driver telemetry.
     * @param telemetry The telemetry information to report.
     */
    void report_telemetry(const Json::Value& telemetry);

    /**
     * @brief Whether the driver should report telemetry.
     */
    bool should_report_telemetry() const { return static_cast<bool>(_telemetry_callback); }

private:
    std::function<void(const std::string& context, const Json::Value&)> _telemetry_callback;
    std::function<void(const std::string& context, const ConnectionStatusEnum&)> _status_callback;
    ConnectionStatusEnum _last_reported_status = ConnectionStatusEnum::IDLE;
};
