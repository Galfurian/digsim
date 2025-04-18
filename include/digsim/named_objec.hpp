/// @file named_objec.hpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include <string>

namespace digsim
{

/// @brief A class that represents an object with a name.
class named_object_t
{
private:
    /// @brief The name of the object
    std::string name;

public:
    /// @brief Constructor for named_object_t.
    /// @param _name the name of the object.
    named_object_t(std::string _name)
        : name(std::move(_name))
    {
        // Nothing to do.
    }

    /// @brief Default constructor for named_object_t.
    virtual ~named_object_t() = default;

    /// @brief Get the name of the object.
    /// @return The name of the object.
    const std::string &get_name() const { return name; }
};

} // namespace digsim
