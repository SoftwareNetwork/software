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

#include "program.h"

#include "rule.h"

#include <primitives/exceptions.h>

namespace sw
{

Program::Program()
{
}

Program::Program(const Program &rhs)
    : file(rhs.file)
{
}

Program &Program::operator=(const Program &rhs)
{
    file = rhs.file;
    return *this;
}

PredefinedProgram::PredefinedProgram() = default;
PredefinedProgram::~PredefinedProgram() = default;

Program &PredefinedProgram::getProgram()
{
    if (!program)
        throw SW_RUNTIME_ERROR("Program was not set");
    return *program;
}

const Program &PredefinedProgram::getProgram() const
{
    if (!program)
        throw SW_RUNTIME_ERROR("Program was not set");
    return *program;
}

IRulePtr PredefinedProgram::getRule1(const String &name) const
{
    auto i = rules.find(name);
    if (i == rules.end())
        throw SW_RUNTIME_ERROR("No such rule: " + name);
    return i->second->clone();
}

void PredefinedProgram::setRule(const String &name, IRulePtr r)
{
    rules[name] = std::move(r);
}

}
