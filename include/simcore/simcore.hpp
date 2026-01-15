/// @file simcore.hpp
/// @brief Main header file for the SimCore library.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

enum : unsigned char {
    SIMCORE_MAJOR_VERSION = 1, ///< Major version of the library.
    SIMCORE_MINOR_VERSION = 2, ///< Minor version of the library.
    SIMCORE_MICRO_VERSION = 0  ///< Micro version of the library.
};

// Base types and aliases
#include "simcore/common.hpp"

// Logging utilities
#include "simcore/logger.hpp"

// Core simulation classes
#include "simcore/dependency_graph.hpp"
#include "simcore/input.hpp"
#include "simcore/isignal.hpp"
#include "simcore/module.hpp"
#include "simcore/output.hpp"
#include "simcore/scheduler.hpp"
#include "simcore/signal.hpp"