/*
 * SW - Build System and Package Manager
 * Copyright (C) 2017-2020 Egor Pugin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <type_traits>

namespace sw
{

/// provides castable interface (as() methods)
struct SW_BUILDER_API ICastable
{
    virtual ~ICastable() = 0;

    template <class T, typename = std::enable_if_t<std::is_pointer_v<T>>>
    std::decay_t<std::remove_pointer_t<T>> *as()
    {
        return dynamic_cast<std::decay_t<std::remove_pointer_t<T>> *>(this);
    }

    template <class T, typename = std::enable_if_t<std::is_pointer_v<T>>>
    const std::decay_t<std::remove_pointer_t<T>> *as() const
    {
        return dynamic_cast<const std::decay_t<std::remove_pointer_t<T>> *>(this);
    }

    template <class T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
    std::decay_t<T> &as()
    {
        return dynamic_cast<std::decay_t<T> &>(*this);
    }

    template <class T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
    const std::decay_t<T> &as() const
    {
        return dynamic_cast<const std::decay_t<T> &>(*this);
    }
};

}
