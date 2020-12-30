// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2016-2019 Egor Pugin <egor.pugin@gmail.com>

#include "package_name.h"

//#include <primitives/log.h>
//DECLARE_STATIC_LOGGER(logger, "package_id");

namespace sw
{

std::pair<String, String> split_package_string(const String &s)
{
    /*
    different variants:
        org.sw.demo.package-1.0.0   - the main one currently, but it's hard to use '-' in ppath
        org.sw.demo.package 1.0.0   - very obvious and solid, but not very practical?
        org.sw.demo.package@1.0.0   - not that bad
        org.sw.demo.package/1.0.0   - not that bad, but probably bad rather than good?

    other cases (?):
        org.sw.demo.package-with-dashes--1.0.0   - double dash to indicate halfs (@ and ' ' also work)
    */

    size_t pos;

    // fancy case
    /*pos = s.find_first_of("@/"); // space (' ') can be met in version, so we'll fail in this case
    if (pos != s.npos)
        return { s.substr(0, pos), s.substr(pos + 1) };

    // double dashed case
    pos = s.find("--");
    if (pos != s.npos)
        return { s.substr(0, pos), s.substr(pos + 1) };*/

    // simple dash + space case
    pos = s.find_first_of("-"); // also space ' '?
    if (pos == s.npos)
        return { s, {} };
    return { s.substr(0, pos), s.substr(pos + 1) };
}

PackageName::PackageName(const String &target)
{
    auto [p, v] = split_package_string(target);
    ppath = p;
    if (v.empty())
        throw SW_RUNTIME_ERROR("Empty version when constructing package id '" + target + "', resolve first");
    version = v;
}

PackageName::PackageName(const PackagePath &p, const PackageVersion &v)
    : ppath(p), version(v)
{
}

String PackageName::getVariableName() const
{
    auto v = version.toString();
    auto vname = ppath.toString() + "_" + (v == "*" ? "" : ("_" + v));
    std::replace(vname.begin(), vname.end(), '.', '_');
    return vname;
}

String PackageName::toString(const String &delim) const
{
    return ppath.toString() + delim + version.toString();
}

std::string PackageName::toRangeString(const String &delim) const
{
    return ppath.toString() + delim + version.toRangeString();
}

PackageName extractPackageIdFromString(const String &target)
{
    auto [pp, v] = split_package_string(target);
    if (v.empty())
        throw SW_RUNTIME_ERROR("Bad target: " + target);
    return {pp, v};
}

}
