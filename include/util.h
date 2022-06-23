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

#include <functional>
#include <string>
#include <vector>

#include "json.h"
#include "utility/windows_support.h"

bool ip_matches(const std::string& ip, const std::string& netmask, const std::string& ip_prefix);

std::string exec(const std::string& cmd);

std::vector<std::string> split(const std::string& str, char delimiter);

bool atoi(const char* a, int& val);

float ping_time(const std::string& address);

bool can_ping(const std::string& ip, int timeout);

std::string str_tolower(const std::string& s);

void set_thread_name(const std::string& name);

/**
 * @brief Convert json object to its string representation
 * @param json Json object to convert
 * @param nice If true return nicely formatted json
 * @return string representation of json object
 */
std::string CM_API json_to_string(const Json::Value& json, bool nice = false);

/**
 * @brief Convert json to its string representation
 * @param str string to be converted to json object
 * @param json resulting json object
 * @return true if conversion was successful
 */
bool CM_API string_to_json(const std::string& str, Json::Value* json);

/**
 * @brief Bind arguments to the front of a function.
 *
 * C++17 implementation of the C++20 `std::bind_front(...)` function.
 *
 * @param f The function to bind front arguments to.
 * @param front_args The front arguments to bind.
 * @return auto
 */
template<typename F, typename... FRONT_ARGS>
auto bind_front(F&& f, FRONT_ARGS&&... front_args)
{
    // front_args are copied because multiple invocations of this closure are possible
    return [captured_f = std::forward<F>(f), front_args...](auto&&... back_args) {
        return std::invoke(captured_f, front_args..., std::forward<decltype(back_args)>(back_args)...);
    };
}
