// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2017-2020 Egor Pugin <egor.pugin@gmail.com>

#pragma once

#include <sw/support/settings.h>

namespace sw
{

using TargetSettings = PackageSettings;

// move to builder probably
SW_CORE_API
TargetSettings toTargetSettings(const struct OS &);

}
