// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2016-2019 Egor Pugin <egor.pugin@gmail.com>

#pragma once

#include "enums.h"
#include "package_unresolved.h"

namespace sw
{

struct SW_SUPPORT_API PackageId
{
    // try to extract from string
    PackageId(const String &);
    PackageId(const PackagePath &, const Version &);

    const PackagePath &getPath() const { return ppath; }
    const Version &getVersion() const { return version; }

    bool operator<(const PackageId &rhs) const { return std::tie(ppath, version) < std::tie(rhs.ppath, rhs.version); }
    bool operator==(const PackageId &rhs) const { return std::tie(ppath, version) == std::tie(rhs.ppath, rhs.version); }
    bool operator!=(const PackageId &rhs) const { return !operator==(rhs); }

    String getVariableName() const;

    String toString() const;
    String toString(const String &delim) const;
    String toString(Version::Level, const String &delim = "-") const;

private:
    PackagePath ppath;
    Version version;
};

using PackageIdSet = std::unordered_set<PackageId>;

SW_SUPPORT_API
PackageId extractPackageIdFromString(const String &target);

}

namespace std
{

template<> struct hash<::sw::PackageId>
{
    size_t operator()(const ::sw::PackageId &p) const
    {
        auto h = std::hash<::sw::PackagePath>()(p.getPath());
        return hash_combine(h, std::hash<::sw::Version>()(p.getVersion()));
    }
};

}
