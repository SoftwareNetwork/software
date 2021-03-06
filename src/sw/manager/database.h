// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2016-2019 Egor Pugin <egor.pugin@gmail.com>

#pragma once

#include <sw/support/filesystem.h>

#include <chrono>
#include <memory>
#include <optional>
#include <vector>

namespace sqlpp::sqlite3 { class connection; }

namespace sw
{

struct SW_MANAGER_API Database
{
    std::unique_ptr<sqlpp::sqlite3::connection> db;
    path fn;

    Database(const path &db_name, const String &schema);
    ~Database();

    void open(bool read_only = false, bool in_memory = false);

    int getIntValue(const String &key);
    void setIntValue(const String &key, int v);

protected:
    //
    template <typename T>
    std::optional<T> getValue(const String &key) const;

    template <typename T>
    T getValue(const String &key, const T &default_) const;

    template <typename T>
    void setValue(const String &key, const T &v) const;
};

}
