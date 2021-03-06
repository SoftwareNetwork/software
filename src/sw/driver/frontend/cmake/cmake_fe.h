// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2020 Egor Pugin <egor.pugin@gmail.com>

#pragma once

#include <sw/driver/entry_point.h>

class cmake;
class cmTarget;
class cmMakefile;

namespace sw
{
struct CheckSet;
struct NativeCompiledTarget;
}

namespace sw::driver::cpp
{

struct CmakeTargetEntryPoint : NativeTargetEntryPoint
{
    using Base = NativeTargetEntryPoint;

    mutable std::unique_ptr<cmake> cm;
    mutable SwBuild *b = nullptr;
    mutable PackageSettings ts;
    mutable NativeCompiledTarget *t = nullptr;
    mutable CheckSet *cs = nullptr;

    CmakeTargetEntryPoint(const path &fn);
    ~CmakeTargetEntryPoint();

    /*[[nodiscard]]
    std::vector<ITargetPtr> loadPackages(SwBuild &, const PackageSettings &) const override;
    [[nodiscard]]
    ITargetPtr loadPackage(SwBuild &, const Package &) const override;*/

private:
    path rootfn;

    void init() const;
    void loadPackages1(Build &) const override;

    static NativeCompiledTarget *addTarget(Build &, cmTarget &);
    void setupTarget(cmMakefile &, cmTarget &, NativeCompiledTarget &, const StringSet &list_of_targets) const;
};

}
