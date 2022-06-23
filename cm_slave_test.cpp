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
 * @file cm_slave_test.cpp
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#include <chrono>
#include <gtest/gtest.h>
#include <thread>

#include "connection_driver_microhard.h"
#include "connection_manager_slave.h"
#include "utility/logging/logging_internal.h"

const std::string config = ""
                           R"({                                        )"
                           R"("machine_name" : "TestVehicle",          )"
                           R"("encryption_key" : "1234567890",         )"
                           R"("link_layer" : "udp",                    )"
                           R"("configuration_file" : "pairing-cm.json",)"
                           R"("aes_encryption" : false,                )"
                           R"("rsa_encryption" : true,                 )"
                           R"("ethernet_device" : "eth0",              )"
                           R"("drivers" : [                            )"
                           R"(  {                                      )"
                           R"(    "name" : "Microhard",                )"
                           R"(    "instance" : "Microhard",            )"
                           R"(    "password" : "auterionfct",          )"
                           R"(    "local" : {                          )"
                           R"(        "mode" : "1",                    )"
                           R"(        "tx_rate" : "4"                  )"
                           R"(    },                                   )"
                           R"(    "pairing" : {                        )"
                           R"(        "encryption_key" : "1234567890", )"
                           R"(        "network_id" : "AUTERION",       )"
                           R"(        "channel" : "36",                )"
                           R"(        "bandwidth" : "1",               )"
                           R"(        "tx_power" : "7"                 )"
                           R"(    }                                    )"
                           R"(  }                                      )"
                           R"(]                                        )"
                           R"(}                                        )";

void test_pair_connect_reconfigure()
{
    ConnectionManagerSlave connection_manager_slave;
    std::mutex status_mutex;
    std::condition_variable status_cv;
    std::list<ConnectionStatus> status_list;

    connection_manager_slave.register_status_callback([&](ConnectionStatus status) {
        {
            std::lock_guard<std::mutex> lck(status_mutex);
            status_list.push_back(status);
        }
        status_cv.notify_one();
    });

    bool res = connection_manager_slave.init(config);
    EXPECT_TRUE(res);
    std::cerr << "Detecting & connecting to modem" << std::endl;

    bool modem_found = false;
    while (!modem_found) {
        std::unique_lock<std::mutex> lock(status_mutex);
        status_cv.wait_for(lock, std::chrono::seconds(60), [&] {
            for (auto i : status_list) {
                if (i.code == ConnectionStatusEnum::DRIVER_CONNECTED) {
                    modem_found = true;
                    return true;
                }
            }
            return false;
        });
    }
    EXPECT_TRUE(modem_found);
    if (!modem_found) {
        return;
    }

    connection_manager_slave.enter_pairing_mode();

    std::cerr << "Waiting for pair & connect" << std::endl;
    bool connected = false;
    while (!connected) {
        std::unique_lock<std::mutex> lock(status_mutex);
        status_cv.wait_for(lock, std::chrono::seconds(120), [&] {
            for (auto i : status_list) {
                if (i.code == ConnectionStatusEnum::CONNECTED) {
                    connected = true;
                    return true;
                }
            }
            return false;
        });
    }
    EXPECT_TRUE(connected);

    if (!connected) {
        return;
    }

    std::cerr << "Waiting reconfiguration" << std::endl;
    bool reconfigured = false;
    while (!reconfigured) {
        std::unique_lock<std::mutex> lock(status_mutex);
        status_cv.wait_for(lock, std::chrono::seconds(30), [&] {
            for (auto i : status_list) {
                if (i.code == ConnectionStatusEnum::RECONFIGURED) {
                    reconfigured = true;
                    return true;
                }
            }
            return false;
        });
    }
    EXPECT_TRUE(reconfigured);
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void test_autoconnect()
{
    ConnectionManagerSlave connection_manager_slave;
    std::mutex status_mutex;
    std::condition_variable status_cv;
    std::list<ConnectionStatus> status_list;

    connection_manager_slave.register_status_callback([&](ConnectionStatus status) {
        {
            std::lock_guard<std::mutex> lck(status_mutex);
            status_list.push_back(status);
        }
        status_cv.notify_one();
    });

    bool res = connection_manager_slave.init(config);
    EXPECT_TRUE(res);
    std::cerr << "Detecting & connecting to modem" << std::endl;

    bool modem_found = false;
    while (!modem_found) {
        std::unique_lock<std::mutex> lock(status_mutex);
        status_cv.wait_for(lock, std::chrono::seconds(60), [&] {
            for (auto i : status_list) {
                if (i.code == ConnectionStatusEnum::DRIVER_CONNECTED) {
                    modem_found = true;
                    return true;
                }
            }
            return false;
        });
    }
    EXPECT_TRUE(modem_found);
    if (!modem_found) {
        return;
    }

    std::cerr << "Waiting for autoconnect" << std::endl;

    bool connected = false;
    while (!connected) {
        std::unique_lock<std::mutex> lock(status_mutex);
        status_cv.wait_for(lock, std::chrono::seconds(120), [&] {
            for (auto i : status_list) {
                if (i.code == ConnectionStatusEnum::CONNECTED) {
                    connected = true;
                    return true;
                }
            }
            return false;
        });
    }
    EXPECT_TRUE(connected);
}

TEST(ConnectionManagerSlaveTests, connection_driver_microhard)
{
    std::remove("pairing-cm.json");
    test_pair_connect_reconfigure();
    test_autoconnect();
}

int main(int argc, char** argv)
{
    spdlog::cfg::load_env_levels();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
