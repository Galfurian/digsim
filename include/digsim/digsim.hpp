/// @file digsim.hpp
/// @brief Main header file for the DigSim library.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

enum : unsigned char {
    DIGSIM_MAJOR_VERSION = 1, ///< Major version of the library.
    DIGSIM_MINOR_VERSION = 2, ///< Minor version of the library.
    DIGSIM_MICRO_VERSION = 0  ///< Micro version of the library.
};

// Base types and aliases
#include "digsim/common.hpp"

// Logging utilities
#include "digsim/logger.hpp"

// Core simulation classes
#include "digsim/dependency_graph.hpp"
#include "digsim/input.hpp"
#include "digsim/isignal.hpp"
#include "digsim/module.hpp"
#include "digsim/output.hpp"
#include "digsim/scheduler.hpp"
#include "digsim/signal.hpp"

// Simulation components
#include "digsim/clock.hpp"
#include "digsim/probe.hpp"
