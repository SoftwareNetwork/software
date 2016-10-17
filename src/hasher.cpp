/*
 * Copyright (c) 2016, Egor Pugin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of
 *        its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hasher.h"

#include "hash.h"

#include <boost/algorithm/string.hpp>

#include <algorithm>

#define DEFINE_OPERATOR(t)        \
    Hasher Hasher::operator|(t v) \
    {                             \
        auto tmp = *this;         \
        tmp |= v;                 \
        return tmp;               \
    }

DEFINE_OPERATOR(bool)
DEFINE_OPERATOR(const String &)
DEFINE_OPERATOR(const path &);

void Hasher::do_hash()
{
    hash = sha256(hash);
}

Hasher &Hasher::operator|=(bool b)
{
    hash += b ? "1" : "0";
    do_hash();
    return *this;
}

Hasher &Hasher::operator|=(const String &v)
{
    auto s = v;
    boost::trim(s);

    std::vector<String> out;
    boost::split(out, s, boost::is_any_of(" \t\r\n\v\f"));

    std::vector<String> out2;
    for (auto &o : out)
    {
        boost::trim(o);
        if (o.empty())
            continue;
        out2.push_back(o);
    }

    std::sort(out2.begin(), out2.end());

    for (auto &o : out2)
        hash += o;

    do_hash();
    return *this;
}

Hasher &Hasher::operator|=(const path &v)
{
    hash += normalize_path(v);
    do_hash();
    return *this;
}
