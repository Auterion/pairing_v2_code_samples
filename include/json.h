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
 * @file json.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include "json/json.h"
#include <string>

const std::string json_default_configuration_file = "pairing-cm.json";
const std::string json_machine_name = "machine_name";
const std::string json_request = "request";
const std::string json_response = "response";
const std::string json_broadcast = "broadcast";
const std::string json_pair = "pair";
const std::string json_connect = "connect";
const std::string json_disconnect = "disconnect";
const std::string json_reconfigure = "reconfigure";
const std::string json_status = "status";
const std::string json_remote_ip = "remote_ip";
const std::string json_port = "port";
const std::string json_coalesce_bytes = "coalesce_bytes";
const std::string json_coalesce_ms = "coalesce_ms";
const std::string json_coalesce_nodelay = "coalesce_nodelay";
const std::string json_auto_connect = "auto_connect";
const std::string json_last_connect = "last_connected";
const std::string json_public_key = "public_key";
const std::string json_private_key = "private_key";
const std::string json_ethernet_device = "ethernet_device";
const std::string json_encryption_key = "encryption_key";
const std::string json_configuration_file = "configuration_file";
const std::string json_aes_encryption = "aes_encryption";
const std::string json_rsa_encryption = "rsa_encryption";
const std::string json_rsa_encrypted = "rsa_encrypted";
const std::string json_link_layer = "link_layer";
const std::string json_link_layer_udp = "udp";
const std::string json_section_pairing = "pairing";
const std::string json_section_connection = "connection";
const std::string json_section_local = "local";
const std::string json_sequence = "seq";
const std::string json_timestamp = "timestamp";
const std::string json_multicast_ip = "multicast_ip";

const std::string json_setting_name = "name";
const std::string json_setting_description = "description";
const std::string json_setting_advanced = "advanced";
const std::string json_setting_value = "value";
const std::string json_setting_validator = "validator";
const std::string json_setting_hidden = "hidden";
const std::string json_setting_index = "index";
const std::string json_setting_values = "values";
const std::string json_setting_labels = "labels";

const std::string json_driver = "driver";
const std::string json_drivers = "drivers";
const std::string json_driver_name = "name";
const std::string json_driver_instance = "instance";
const std::string json_driver_max_instances = "max_instances";
const std::string json_driver_ip = "ip";
const std::string json_driver_ip_status = "ip_status";
const std::string json_driver_vlan = "vlan";
const std::string json_driver_mavlink = "mavlink";
const std::string json_driver_mavlink_port = "mavlink_port";
const std::string json_driver_simplified = "simplified";
const std::string json_driver_autopair = "autopair";
const std::string json_driver_download_bandwidth = "download_bandwidth";
const std::string json_driver_streaming_priority = "streaming_priority";
const std::string json_driver_section = "section";
const std::string json_driver_password = "password";
const std::string json_driver_mode = "mode";
const std::string json_driver_network_id = "network_id";
const std::string json_driver_reconfigure_network_id = "reconfigure_network_id";
const std::string json_driver_encryption_key = "encryption_key";
const std::string json_driver_randomize_encryption_key = "randomize_encryption_key";
const std::string json_driver_encryption_type = "encryption_type";
const std::string json_driver_channel = "channel";
const std::string json_driver_bandwidth = "bandwidth";
const std::string json_driver_tx_power = "tx_power";
const std::string json_driver_tx_power_max = "max";
const std::string json_driver_tx_rate = "tx_rate";
const std::string json_driver_em_inf = "em_inf";
const std::string json_driver_distance = "distance";
const std::string json_driver_telemetry_rssi = "RSSI";
const std::string json_driver_telemetry_snr = "SNR";
const std::string json_driver_telemetry_soc = "battery_soc";
