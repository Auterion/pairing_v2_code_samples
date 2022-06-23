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
 * @file driver_registry.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include <functional>
#include <string>

#include "connection_driver.h"
#include "utility/windows_support.h"

void register_driver(
    std::string driver_name,
    const std::string& driver_setting_template,
    const std::string& pairing_settings,
    std::function<std::unique_ptr<ConnectionDriver>()>);

std::unique_ptr<ConnectionDriver> create_driver(std::string driver_name);

void CM_API get_drivers_settings_templates(Json::Value& settings);

void CM_API get_driver_pairing_settings(const std::string& driver_name, Json::Value& settings);
