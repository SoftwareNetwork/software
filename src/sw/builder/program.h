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

#include "node.h"

#include <sw/support/version.h>

#include <optional>

#define SW_DECLARE_PROGRAM_CLONE \
    std::unique_ptr<Program> clone() const override

#define SW_DEFINE_PROGRAM_CLONE(t)            \
    std::unique_ptr<Program> t::clone() const \
    {                                         \
        return std::make_unique<t>(*this);    \
    }

#define SW_DEFINE_PROGRAM_CLONE_INLINE(t)           \
    std::unique_ptr<Program> clone() const override \
    {                                               \
        return std::make_unique<t>(*this);          \
    }

namespace sw
{

struct SwBuilderContext;

struct SW_BUILDER_API Program : ICastable, detail::Executable
{
    path file;

    Program();
    Program(const Program &);
    Program &operator=(const Program &);
    virtual ~Program() = default;

    virtual std::unique_ptr<Program> clone() const = 0;
};

using ProgramPtr = std::shared_ptr<Program>;

// reconsider
struct SW_BUILDER_API PredefinedProgram
{
    void setProgram(const ProgramPtr &p) { program = p; }
    Program &getProgram();
    const Program &getProgram() const;

private:
    ProgramPtr program;
};

}
