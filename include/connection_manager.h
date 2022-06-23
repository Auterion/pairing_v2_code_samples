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
 * @file connection_manager.h
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

#include "connection_driver.h"
#include "connection_status.h"
#include "json.h"
#include "link_layer.h"
#include "openssl_aes.h"
#include "openssl_rsa.h"
#include "utility/windows_support.h"

const uint16_t default_master_port = 29350;
const uint16_t default_slave_port = 29360;

/**
 * @brief Base class with common methods used by both master & slave connection managers
 */
class CM_API ConnectionManager {
public:
    /**
     * @brief Constructor
     */
    ConnectionManager();

    /**
     * @brief Destructor
     */
    virtual ~ConnectionManager();

    /**
     * @brief Initialize connection manager with json string
     * @return Result of the initialization
     */
    virtual bool init(const std::string& configuration);

    /**
     * @brief Iterate to the next state of the state machine
     */
    virtual void iterate(){};

    /**
     * @brief Register a callback that will be called when the status changes
     * @param status_callback A function that will be called on status change
     */
    void register_status_callback(std::function<void(ConnectionStatus)> status_callback);

    /**
     * @brief Register a callback that will be called when the paired list changes
     * @param paired_list_changed A function that will be called on change
     */
    void register_paired_list_changed_callback(std::function<void()> paired_list_changed);

    /**
     * @brief Obtain a list of paired remotes
     * @return A vector containing remote names
     */
    std::list<std::string> get_paired_list();

    /**
     * @brief Set remote that was last to connect
     * @param last_connected Last connected remote
     */
    void set_last_connected(const std::string& last_connected);

    /**
     * @brief Get remote that was last to connect
     * @return Last connected remote
     */
    std::string get_last_connected();

    /**
     * @brief Query autoconnect property for specified paired remote
     * @param name remote name
     * @return autoconnect state
     */
    bool get_paired_autoconnect(const std::string& name);

    /**
     * @brief Register a callback that will be called when the driver telemetry data changes
     * @param telemetry_callback A function that will be called on change
     */
    void register_telemetry_callback(std::function<void(const std::string&, const Json::Value&)> telemetry_callback);

    /**
     * @brief Get driver instance specific connection settings
     * @param instance driver instance to get settings from
     * @param settings returned values
     * @return false if there are no connection settings or status
     */
    bool get_driver_instance_connection_settings(const std::string& instance, Json::Value& settings);

    /**
     * @brief Get driver instance specific pairing settings
     * @param instance driver instance to get settings from
     * @param settings returned values
     * @return true if driver instance exists
     */
    bool get_driver_instance_pairing_settings(const std::string& instance, Json::Value& settings);

    /**
     * @brief Should we report wired status for specified driver (blinking pattern on Skynode)
     * @param driver_instance driver instance
     * @return true if wired status reporting is enabled
     */
    bool report_wired_status(const std::string& driver_instance);

    /**
     * @brief Get a list of potential radio candidates
     * @param candidate_list set on return
     */
    static void get_radio_candidate_list(std::set<std::string>& candidate_list);

protected:
    const int _mavlink_router_period = 10000; // mavlink router refresh period in milliseconds
    const int _broadcast_period = 3000; // Send broadcast period in milliseconds
    const int _status_period = 2000; // Send status period in milliseconds
    const int _status_timeout = 6000; // Timeout for receiving status in milliseconds
    const int _reconfiguration_timeout = 20000; // Timeout for reconfiguration in milliseconds

    bool _use_aes_encryption = false;
    bool _use_rsa_encryption = true;
    std::list<std::shared_ptr<ConnectionDriver>> _connection_drivers;
    Json::Value _configuration;
    std::string _machine_name;
    std::shared_ptr<LinkLayer> _link_layer;
    std::string _ethernet_device = "eth0";

    /**
     * @brief Advance to the next state of the state machine
     */
    void next_state();

    /**
     * @brief Configure all communication drivers with specified settings
     * @param settings to configure the drivers
     * @param section use this section of the configuration
     * @param driver_set limited set of drivers to configure. If empty configure all.
     * @return true if configuration was successful
     */
    bool configure_drivers(const Json::Value& settings, const std::string& section = "", const std::set<std::string>& driver_set = {});

    /**
     * @brief Parse received message
     * @param msg message to parse
     * @param from origin of the message
     * @param parsed parsed resulting json object
     * @return true if parsing was successful
     */
    bool parse_received_message(const std::string& msg, const std::string& from, Json::Value& parsed);

    /**
     * @brief Get connection manager RSA public key
     * @return public key string
     */
    std::string get_pk();

    /**
     * @brief Add remote info to the internal list of paired
     * @param val remote info needed for connection
     */
    void add_paired(const Json::Value& val);

    /**
     * @brief Update specified remote with new configuration
     * @param name remote name
     * @param val new connection configuration
     */
    void update_paired(const std::string& name, const Json::Value& val);

    /**
     * @brief Remove specified remote from the list of paired devices
     * @param name remote name
     */
    void remove_paired(const std::string& name);

    /**
     * @brief Set autoconnect property for specified paired remote
     * @param name remote name
     * @param val autoconnect state
     * @return true if autoconnect changed
     */
    bool set_paired_autoconnect(const std::string& name, bool val);

    /**
     * @brief Get paired remote connection information
     * @param name remote name
     * @param val returned remote info
     * @return true if successful
     */
    bool get_paired(const std::string& name, Json::Value& val);

    /**
     * @brief Set paired instance remote ip
     * @param name remote name
     * @param instance driver instance
     * @param ip new ip for that remote driver instance
     */
    void set_paired_instance_remote_ip(const std::string& name, const std::string& instance, const std::string& ip);

    /**
     * @brief Encrypt string using AES algorithm
     * @param msg string to encrypt
     * @return Encrypted string
     */
    std::string aes_encrypt(const std::string& msg);

    /**
     * @brief Decrypt string using AES algorithm
     * @param msg string to decrypt
     * @return Decrypted string (empty if decryption failed)
     */
    std::string aes_decrypt(const std::string& msg);

    /**
     * @brief Sign and encrypt string using RSA algorithm
     * @param msg string to sign & encrypt
     * @return Signed & encrypted string
     */
    std::string rsa_sign_and_encrypt(const std::string& remote, const std::string& msg);

    /**
     * @brief Decrypt and verify signature of the string using RSA algorithm
     * @param msg string to decrypt & verify
     * @return Decrypted string (empty if decryption or verification failed)
     */
    std::string rsa_decrypt_and_verify(const std::string& remote, const std::string& msg);

    /**
     * @brief Report current status of connection manager
     * @param status status to report
     */
    void report_status(ConnectionStatusEnum status, const std::string& context = "");

    /**
     * @brief Filter received message. Can be used in testing.
     * @param val message parsed into a json structure
     * @return true if message should be discarded
     */
    virtual bool message_received_filter(const Json::Value&) { return false; }

    /**
     * @brief Merge configuration b into a
     * @param a base configuration
     * @param b added configuration
     * @param add_missing_drivers if driver is defined in b but not in a then add it
     */
    void merge_configurations(Json::Value& a, const Json::Value& b, bool add_missing_drivers = true);

    /**
     * @brief Stop threads and cleanup
     */
    virtual void stop();

    /**
     * @brief Method called when a driver updates its status
     * @param code Connection status code
     * @param context Driver name
     */
    virtual void driver_status_callback(const std::string& context, const ConnectionStatusEnum& code);

private:
    OpenSSL_AES _aes;
    OpenSSL_RSA _rsa;
    std::mutex _remote_mutex;
    std::map<std::string, OpenSSL_RSA> _remote_rsa_map;
    std::function<void()> _paired_list_changed;
    std::mutex _paired_map_mutex;
    std::map<std::string, Json::Value> _paired_map;
    uint32_t driver_configure_timeout = 30000;
    std::atomic<bool> _should_exit{false};
    std::thread _state_machine_thread;
    std::mutex _state_machine_mutex;
    std::condition_variable _cv_state_machine;
    bool _cv_state_machine_ready = true;
    std::mutex _status_callback_mutex;
    std::function<void(ConnectionStatus)> _status_callback;
    std::function<void(const std::string&, const Json::Value&)> _telemetry_callback;
    std::string _configuration_file;

    /**
     * @brief State machine thread worker
     */
    void state_machine_worker();

    /**
     * @brief Store pairing info in a persistent storage file
     */
    void save_pairing_info();

    /**
     * @brief Load pairing info from a persistent storage file
     * @return true if encryption keys were read from the configuration file
     */
    bool load_pairing_info();

    /**
     * @brief Add and associate public key with the name
     * @param name remote name
     * @param val remote info containing public_key
     */
    void add_public_key(const std::string& name, const Json::Value& val);

    /**
     * @brief Add and associate public key with the name
     * @param name remote name
     * @param public_key remote public key
     */
    void add_public_key(const std::string& name, const std::string& public_key);
};
