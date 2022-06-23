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
 * @file connection_manager_master.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include <map>

#include "connection_manager.h"
#include "link_layer_udp.h"
#include "usm.h"
#include "utility/windows_support.h"

/**
 * @brief Connection Manager master state machine states
 */
enum MasterTransactionState {
    M_IDLE, /**< @brief Initial boot state, wait for start pairing, connect, disconnect or reconfigure */
    M_CONFIG_PAIRING, /**< @brief Configure connection drivers for pairing */
    M_PAIR, /**< @brief Pair to vehicle and wait for response */
    M_CONFIG_CONNECT, /**< @brief Configure connection drivers for connecting */
    M_RECONFIGURE, /**< @brief Configure with new connection parameters, send reconfigure to remotes and wait for their status */
    M_RECONFIGURING /**< @brief Waiting for confirmation of new connection parameters */
};

/**
 * @brief Implementation of master connection manager typically used on GCS side
 */
class CM_API ConnectionManagerMaster : public ConnectionManager, public usm::StateMachine<MasterTransactionState> {
public:
    /**
     * @brief Constructor
     */
    ConnectionManagerMaster();

    /**
     * @brief Destructor
     */
    ~ConnectionManagerMaster();

    /**
     * @brief Initialize connection manager with json string
     * @param configuration json string with connection manager configuration
     * @return Result of the initialization
     */
    virtual bool init(const std::string& configuration) override;

    /**
     * @brief Iterate to the next state of the state machine
     */
    virtual void iterate() override;

    /**
     * @brief Register a callback that will be called when the pairing list changes
     * @param pairing_list_changed A function that will be called on change
     */
    void register_pairing_list_changed_callback(std::function<void()> pairing_list_changed);

    /**
     * @brief Obtain a list of remotes available for pairing
     * @return A vector containing remote names
     */
    std::list<std::string> get_pairing_list();

    /**
     * @brief Register a callback that will be called when the connected list changes
     * @param connected_list_changed A function that will be called on change
     */
    void register_connected_list_changed_callback(std::function<void()> connected_list_changed);

    /**
     * @brief Register a callback that will be called when the vehicle is connected
     * @param connected_callback A function that will be called when a vehicle is connected
     */
    void register_connected_callback(std::function<void(const std::string&)> connected_callback);

    /**
     * @brief Obtain a list of connected remotes
     * @return A vector containing remote names
     */
    std::list<std::string> get_connected_list();

    /**
     * @brief Obtain a list of connected drivers for a connected remote name
     * @return A vector containing connected drivers
     */
    std::list<std::shared_ptr<ConnectionDriver>> get_connected_drivers(const std::string& connected_name);

    /**
     * @brief External command to enter pairing mode
     */
    void enter_pairing_mode();

    /**
     * @brief External command to stop pairing and exit pairing mode
     */
    void stop_pairing();

    /**
     * @brief External command to stop connecting
     */
    void stop_connecting();

    /**
     * @brief External command to pair to specific remote
     * @param name Remote name to pair to
     * @param skip_config if true skip configuration and go directly to pairing
     */
    void pair_to(const std::string& name, bool skip_config = false);

    /**
     * @brief External command to connect to specific remote
     * @param name Remote name to connect to
     */
    void connect_to(const std::string& name);

    /**
     * @brief External command to disconnect from specific remote
     * @param name Remote name to disconnect from
     */
    void disconnect_from(const std::string& name);

    /**
     * @brief Reconfigure all connected remotes with new connection driver configuration
     * @param new_configuration Json string containing new configuration
     */
    void reconfigure(const std::string& new_configuration);

    /**
     * @brief External command to unpair from specific remote
     * @param name Remote name to unpair
     */
    void unpair_from(const std::string& name);

    /**
     * @brief Get a list of active mavlink router endpoints ports
     * @param name Remote name
     * @return list of active mavlink router UDP endpoints
     */
    std::list<uint16_t> get_active_mavlink_ports(const std::string& name);

    /**
     * @brief Get vehicle IP that has highest bandwidth for downloading.
     * @param name Remote name
     * @param instance Driver instance to which returned IP belongs
     * @param best_bandwidth Bandwidth of the returned IP
     * @return IP
     */
    std::string get_best_ip_for_download(const std::string& name, std::string& instance, int& best_bandwidth);

    /**
     * @brief Get vehicle IP that is most reliable for streaming
     * @param name Remote name
     * @param instance Driver instance to which returned IP belongs
     * @return IP
     */
    std::string get_best_ip_for_streaming(const std::string& name, std::string& instance);

    /**
     * @brief Stop threads and cleanup
     */
    void stop() override;

    /**
     * @brief Advertise master directly to specified remote ip
     * @param ip remote ip
     */
    void advertise(const std::string& ip);

protected:
    /**
     * @brief Method called when a driver updates its status
     * @param code Connection status code
     * @param context Driver name
     */
    void driver_status_callback(const std::string& context, const ConnectionStatusEnum& code) override;

private:
    const int request_timeout = 500;
    const int request_retries = 10;

    /**
     * @brief Structure containing pairing information
     */
    struct PairingInfo {
        std::string info; // @brief Json string with pairing information
        bool expired = false; // @brief expired flag - set when broadcast message times out
        std::chrono::steady_clock::time_point time_stamp; // @brief Last time we received broadcasted pairing info
    };

    /**
     * @brief Structure containing connection information for individual remotes
     */
    struct DriverConnectionInfo {
        Json::Value connect_response_json; // @brief received JSON connect response
        std::map<std::string, std::chrono::steady_clock::time_point>
            time_stamps; // @brief Last time we received status message on each remote driver
    };

    std::atomic<bool> _should_exit{true};
    std::mutex _exit_thread_mutex;
    std::condition_variable _cv_exit_thread;
    std::thread _worker_thread;
    std::function<void()> _pairing_list_changed;
    std::mutex _pairing_map_mutex;
    std::map<std::string, PairingInfo> _pairing_map;
    std::function<void()> _connected_list_changed;
    std::function<void(const std::string&)> _connected_callback;
    std::mutex _connected_map_mutex;
    std::map<std::string, DriverConnectionInfo> _connected_map;
    std::shared_ptr<LinkLayerUDP> _udp_link_layer;
    std::mutex _mutex;
    std::string _auto_pair_to;
    std::set<std::string> _removed_pairings;
    bool _stop_pairing = false;
    bool _skip_pair_config = false;
    Json::Value _reconfigure_params;
    std::atomic<bool> _pairing_active{false};
    std::atomic<bool> _reconfigure_drivers{false};
    std::atomic<bool> _reconfiguring{false};
    std::atomic<size_t> _reconfigure_num;
    std::chrono::steady_clock::time_point _reconfigure_time;
    std::string _reconnect_after_reconfiguration_machine_name;

    std::mutex _wait_pair_response_mutex;
    std::condition_variable _wait_pair_response_cv;
    std::atomic<bool> _got_pair_response{false};
    std::atomic<int> _pairing_retries = request_retries;
    std::string _last_advertised;

    /**
     * @brief Worker thread. Checking for expiration of connected remotes
     */
    void worker();

    /**
     * @brief Process pairing protocol received messages
     * @param msg received message
     * @param from message origin
     */
    void message_received(const std::string& msg, const std::string& from);

    /**
     * @brief Send pairing request to specified remote
     * @param name remote name
     */
    void send_pairing_request(const std::string& name);

    /**
     * @brief Send connection request to specified remote
     * @param name remote name
     * @return true if successful
     */
    bool send_connection_request(const std::string& name);

    /**
     * @brief Send reconfigure requests to connected remotes
     * @param new_params new connection driver configuration
     */
    std::list<std::string> send_reconfigure_requests(const Json::Value& new_params);

    /**
     * @brief Execute code specific to each state
     * @param current_state current state machine state
     * @return Transition to the next state
     */
    usm::Transition run_current_state(MasterTransactionState current_state) override;

    /**
     * @brief Choose next USM state and reset state variables
     * @param current_state current state machine state
     * @param transition transition to the next state
     * @return new state
     */
    MasterTransactionState choose_next_state(MasterTransactionState current_state, usm::Transition transition) override;

    /**
     * @brief Choose next USM state
     * @param current_state current state machine state
     * @param transition transition to the next state
     * @return new state
     */
    MasterTransactionState choose_next_usm_state(MasterTransactionState current_state, usm::Transition transition);

    /**
     * @brief Print state machine transition, used for logging
     * @param current_state current state
     * @param new_state new state
     * @param t transition between states
     */
    void print_transition(MasterTransactionState current_state, MasterTransactionState new_state, usm::Transition t) const override;

    /**
     * @brief Convert state to its string representation, used for logging
     * @param state state to convert
     * @return String representation of the state
     */
    std::string transaction_state_to_string(MasterTransactionState state) const;

    /**
     * @brief Execute M_IDLE state code
     * @return Transition enum for the next state
     */
    usm::Transition run_idle();

    /**
     * @brief Execute M_CONFIG_PAIRING state code
     * @return Transition enum for the next state
     */
    usm::Transition run_config_pairing();

    /**
     * @brief Execute M_PAIR state code
     * @return Transition enum for the next state
     */
    usm::Transition run_pair();

    /**
     * @brief Execute M_CONFIG_CONNECT state code
     * @return Transition enum for the next state
     */
    usm::Transition run_config_connect();

    /**
     * @brief Execute M_RECONFIGURE state code
     * @return Transition enum for the next state
     */
    usm::Transition run_reconfigure();

    /**
     * @brief Execute M_RECONFIGURING state code
     * @return Transition enum for the next state
     */
    usm::Transition run_reconfiguring();

    /**
     * @brief Process broadcast message from remote
     * @param broadcasted_val json containing remote information
     * @param from origin of the message
     */
    void process_broadcast_request(const Json::Value& broadcasted_val, const std::string& from);

    /**
     * @brief Process pairing response from remote
     * @param val json response
     * @param from origin of the message
     */
    void process_pairing_response(const Json::Value& val, const std::string& from);

    /**
     * @brief Process connect response from remote
     * @param val json response
     * @param from origin of the message
     */
    void process_connect_response(const Json::Value& val, const std::string& from);

    /**
     * @brief Process status request from remote
     * @param val json response
     * @param from origin of the message
     */
    void process_status_request(const Json::Value& val, const std::string& from);

    /**
     * @brief Get a list of remotes that should be connected
     * @param include_connected include already connected vehicles to the list
     * @return list of remotes in autoconnect mode
     */
    std::list<std::string> autoconnecting_remotes(bool include_connected = false);

    /**
     * @brief Get remote IP for specified driver instance
     * @param name remote name
     * @param instance name of the driver instance
     * @return remote IP
     */
    std::string get_remote_ip_for_instance(const std::string& name, const std::string& instance);

    /**
     * @brief Check if the source of the message is in our database
     * @param from origin of the message
     * @return true if source is valid
     */
    bool source_valid(const std::string& from);

    /**
     * @brief Get mavlink port for specified connected remote & driver instance
     * @param connected_name name of connected remote
     * @param instance driver instance for which we need the port
     * @return used port or 0 if not found
     */
    uint16_t get_connected_driver_mavlink_port(const std::string& connected_name, const std::string& instance);
};
