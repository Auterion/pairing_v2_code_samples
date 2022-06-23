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
 * @file logging.h
 *
 * @author Matej Frančeškin (Matej@auterion.com)
 */

#pragma once

#include <functional>

#include "utility/windows_support.h"

namespace connection_manager {
namespace utility {
namespace logging {

enum class LogLevel { trace, debug, info, warning, error, critical, off };

/**
 * @brief Set the desired log level
 */
void CM_API set_log_level(const LogLevel& log_level);

/**
 * @brief Register a new logging function. All logging output will be redirected there.
 *
 * @param logging_func The function to use for logging.
 * @param flush_func The function to use for flushing the logger.
 */
void CM_API register_logger(const std::function<void(const std::string&)>& logging_func, const std::function<void()>& flush_func);

} // namespace logging
} // namespace utility
} // namespace connection_manager
