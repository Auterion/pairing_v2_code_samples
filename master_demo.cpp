/****************************************************************************
 *
 *   Copyright (c) 2022 Auterion, Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file main.cpp
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <thread>

#include "connection_manager_master.h"
#include "json.h"
#include "utility/logging/logging_internal.h"
#include "util.h"

const std::string config_begin = ""
                                 R"({                                           )"
                                 R"("machine_name" : "DemoGCS",                 )"
                                 R"("encryption_key" : "1234567890",            )"
                                 R"("link_layer" : "udp",                       )"
                                 R"("configuration_file" : "pairing-cm.json",   )"
                                 R"("aes_encryption" : false,                   )"
                                 R"("rsa_encryption" : true,                    )"
                                 R"("drivers" : [                               )";

const std::string config_microhard = ""
                                     R"(  {                                     )"
                                     R"(    "name" : "Microhard",               )"
                                     R"(    "password" : "auterionfct",         )"
                                     R"(    "local" : {                         )"
                                     R"(        "mode" : "0",                   )"
                                     R"(        "tx_rate" : "8"                 )"
                                     R"(    },                                  )"
                                     R"(    "pairing" : {                       )"
                                     R"(        "encryption_key" : "1234567890",)"
                                     R"(        "network_id" : "AUTERION",      )"
                                     R"(        "channel" : "36",               )"
                                     R"(        "bandwidth" : "0",              )"
                                     R"(        "tx_power" : "20"               )"
                                     R"(    },                                  )"
                                     R"(    "connection" : {                    )"
                                     R"(        "channel" : "56",               )"
                                     R"(        "bandwidth" : "0",              )"
                                     R"(        "tx_power" : "20"               )"
                                     R"(    }                                   )"
                                     R"(  }                                     )";

const std::string config_silvus = ""
                                  R"(  {                                     )"
                                  R"(    "name" : "Silvus",                  )"
                                  R"(    "ip" : "172.20.145.251",            )"
                                  R"(    "simplified" : true,                )"
                                  R"(    "encryption_key" : "1234567890",    )"
                                  R"(    "network_id" : "AUTERION",          )"
                                  R"(    "pairing" : {                       )"
                                  R"(        "channel" : "2490",             )"
                                  R"(        "bandwidth" : "10",             )"
                                  R"(        "tx_power" : "30"               )"
                                  R"(    },                                  )"
                                  R"(    "connection" : {                    )"
                                  R"(        "channel" : "2490",             )"
                                  R"(        "bandwidth" : "10",             )"
                                  R"(        "tx_power" : "30"               )"
                                  R"(    }                                   )"
                                  R"(  }                                     )";

const std::string config_zerotier = ""
                                    R"(  {                                     )"
                                    R"(    "name" : "ZeroTier",                )"
                                    R"(    "ip" : "10.147.17",                 )"
                                    R"(    "ip_status" : true,                 )"
                                    R"(    "mavlink" : true                    )"
                                    R"(  }                                     )";

const std::string config_wifi = ""
                                R"(  {                                     )"
                                R"(    "name" : "WIFI",                    )"
                                R"(    "ip" : "10.41.0",                   )"
                                R"(    "ip_status" : true,                 )"
                                R"(    "simplified" : true,                )"
                                R"(    "autopair" : true,                  )"
                                R"(    "mavlink" : false                   )"
                                R"(  }                                     )";

const std::string config_usb_c = ""
                                 R"(  {                                     )"
                                 R"(    "name" : "USB-C",                   )"
                                 R"(    "ip" : "10.41.1",                   )"
                                 R"(    "ip_status" : true,                 )"
                                 R"(    "simplified" : true,                )"
                                 R"(    "autopair" : true,                  )"
                                 R"(    "mavlink" : false                   )"
                                 R"(  }                                     )";

const std::string config_doodle_labs = ""
                                       R"(  {                                     )"
                                       R"(    "name" : "DoodleLabs",              )"
                                       R"(    "ip" : "10.223.8.26",               )"
                                       R"(    "autopair" : true,                  )"
                                       R"(    "pairing" : {                       )"
                                       R"(        "encryption_key" : "1234567890",)"
                                       R"(        "network_id" : "AUTERION",      )"
                                       R"(        "channel" : "7",                )"
                                       R"(        "bandwidth" : "15"              )"
                                       R"(    },                                  )"
                                       R"(    "connection" : {                    )"
                                       R"(        "channel" : "7",                )"
                                       R"(        "bandwidth" : "15",             )"
                                       R"(        "network_id" : "AUTERION"       )"
                                       R"(    }                                   )"
                                       R"(  }                                     )";

const std::string config_end = ""
                               R"(]                                       )"
                               R"(}                                       )";

static std::set<std::string> vehicles;

static void display_lists(ConnectionManagerMaster& connection_manager)
{
    int i;

    std::string separator = "";
    for (i = 0; i < 1 + 20 + 4 * 12; i++) {
        separator += "*";
    }
    std::cout << separator << std::endl;
    std::cout << std::right;
    std::cout << std::setw(1) << "N" << std::setw(20) << "Vehicle";
    std::cout << std::setw(12) << "Pairing" << std::setw(12) << "Paired";
    std::cout << std::setw(12) << "Connecting" << std::setw(12) << "Connected";
    std::cout << std::endl;

    auto pairing_list = connection_manager.get_pairing_list();
    auto paired_list = connection_manager.get_paired_list();
    auto connected_list = connection_manager.get_connected_list();

    vehicles.clear();
    for (const auto& v : pairing_list) {
        vehicles.insert(v);
    }
    for (const auto& v : paired_list) {
        vehicles.insert(v);
    }

    i = 0;
    for (const auto& v : vehicles) {
        std::cout << std::setw(1) << i << std::setw(20) << v;
        if (std::find(pairing_list.begin(), pairing_list.end(), v) != pairing_list.end()) {
            std::cout << std::setw(12) << "YES";
        } else {
            std::cout << std::setw(12) << "NO";
        }
        if (std::find(paired_list.begin(), paired_list.end(), v) != paired_list.end()) {
            std::cout << std::setw(12) << "YES";
        } else {
            std::cout << std::setw(12) << "NO";
        }
        bool connected = std::find(connected_list.begin(), connected_list.end(), v) != connected_list.end();
        if (!connected && connection_manager.get_paired_autoconnect(v)) {
            std::cout << std::setw(12) << "YES";
        } else {
            std::cout << std::setw(12) << "NO";
        }
        if (connected) {
            std::cout << std::setw(12) << "YES";
        } else {
            std::cout << std::setw(12) << "NO";
        }
        if (connected) {
            std::cout << " (";
            bool first = true;
            for (const auto& i : connection_manager.get_connected_drivers(v)) {
                if (!first) {
                    std::cout << ", ";
                }
                std::cout << i->instance();
                first = false;
            }
            std::cout << ")";
        }
        std::cout << std::endl;
        i++;
    }

    std::cout << std::left;
    std::cout << separator << std::endl;
}

std::string get_vehicle_name(int n)
{
    int i = 0;
    for (const auto& v : vehicles) {
        if ((n - '0') == i) {
            return v;
        }
        i++;
    }

    return "";
}

static void display_help()
{
    std::cout << "l  - display lists" << std::endl;
    std::cout << "p  - enter pairing mode" << std::endl;
    std::cout << "sp - stop pairing" << std::endl;
    std::cout << "sc - stop connecting" << std::endl;
    std::cout << "pN - pair to N" << std::endl;
    std::cout << "cN - connect to N" << std::endl;
    std::cout << "dN - disconnect from N" << std::endl;
    std::cout << "uN - unpair from N" << std::endl;
    std::cout << "x  - exit" << std::endl;
}

static bool find_in_argv(const std::string& s, int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (s == arg || "all" == arg) {
            return true;
        }
    }

    return false;
}

int main(int argc, char* argv[])
{
    spdlog::cfg::load_env_levels();

    ConnectionManagerMaster connection_manager;

    SPDLOG_INFO("Starting connection manager master");

    bool driver_microhard = find_in_argv("microhard", argc, argv);
    bool driver_silvus = find_in_argv("silvus", argc, argv);
    bool driver_wifi = find_in_argv("wifi", argc, argv);
    bool driver_usb_c = find_in_argv("usbc", argc, argv);
    bool driver_doodle_labs = find_in_argv("doodlelabs", argc, argv);
    bool driver_zerotier = find_in_argv("zerotier", argc, argv);
    bool first = true;
    std::string config = config_begin;
    if (driver_microhard) {
        if (!first) {
            config += ",";
        }
        config += config_microhard;
        first = false;
    }
    if (driver_silvus) {
        if (!first) {
            config += ",";
        }
        config += config_silvus;
        first = false;
    }
    if (driver_wifi) {
        if (!first) {
            config += ",";
        }
        config += config_wifi;
        first = false;
    }
    if (driver_usb_c) {
        if (!first) {
            config += ",";
        }
        config += config_usb_c;
        first = false;
    }
    if (driver_doodle_labs) {
        if (!first) {
            config += ",";
        }
        config += config_doodle_labs;
        first = false;
    }
    if (driver_zerotier) {
        if (!first) {
            config += ",";
        }
        config += config_zerotier;
        first = false;
    }
    config += config_end;

    if (first) {
        SPDLOG_ERROR("No drivers were specified. Specify at least one or more of [all, microhard, silvus, doodlelabs, wifi, usbc]");
        return -1;
    }

    connection_manager.register_pairing_list_changed_callback([&]() {
        std::cout << "***** pairing list changed" << std::endl;
        display_lists(connection_manager);
    });

    connection_manager.register_paired_list_changed_callback([&]() {
        std::cout << "***** paired list changed" << std::endl;
        display_lists(connection_manager);
    });

    connection_manager.register_connected_list_changed_callback([&]() {
        std::cout << "***** connected list changed" << std::endl;
        display_lists(connection_manager);
    });

    connection_manager.register_telemetry_callback([](const std::string& instance, const Json::Value& data) {
        const std::string output = json_to_string(data);
        std::cout << "***** " << instance << " Telemetry data: " << std::endl << output << std::endl;
    });

    connection_manager.register_status_callback([](ConnectionStatus status) {
        std::cout << "***** Status = " << static_cast<int>(status.code) << " " << status.context << std::endl;
    });

    if (!connection_manager.init(config)) {
        SPDLOG_ERROR("Could not initialize connection manager");
        return -1;
    }

    display_help();

    bool run = true;
    while (run) {
        std::string in;
        std::cin >> in;
        if (in.length() == 0) {
            continue;
        }
        switch (in[0]) {
            case 'x':
                run = false;
                break;
            case 'l':
                display_lists(connection_manager);
                break;
            case 'p':
                connection_manager.enter_pairing_mode();
                if (in.length() > 1) {
                    connection_manager.pair_to(get_vehicle_name(in[1]));
                }
                break;
            case 's':
                if (in.length() == 2) {
                    if (in[1] == 'p') {
                        connection_manager.stop_pairing();
                    } else if (in[1] == 'c') {
                        connection_manager.stop_connecting();
                    }
                }
                break;
            case 'c':
                if (in.length() == 2) {
                    connection_manager.connect_to(get_vehicle_name(in[1]));
                }
                break;
            case 'd':
                if (in.length() == 2) {
                    connection_manager.disconnect_from(get_vehicle_name(in[1]));
                }
                break;
            case 'u':
                if (in.length() == 2) {
                    connection_manager.unpair_from(get_vehicle_name(in[1]));
                }
                break;
            case '?':
            case 'h':
                display_help();
                break;
        }
    }

    return 0;
}
