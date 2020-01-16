/*
 * SW - Build System and Package Manager
 * Copyright (C) 2017-2019 Egor Pugin
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

#include "commands.h"

#include <sw/manager/storage.h>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <Objbase.h>
#include <Shlobj.h>
#endif

#include <primitives/log.h>
DECLARE_STATIC_LOGGER(logger, "command.open");

DEFINE_SUBCOMMAND(open, "Open package directory.");

static ::cl::opt<String> open_arg(::cl::Positional, ::cl::desc("package to open"), ::cl::sub(subcommand_open));

static void open_nix(const path &p)
{
#ifdef _WIN32
    SW_UNREACHABLE;
#endif
    String s;
#ifdef __linux__
    s += "xdg-";
#endif
    s += "open \"" + normalize_path(p) + "\"";
    if (system(s.c_str()) != 0)
    {
#if !(defined(__linux__) || defined(__APPLE__))
        SW_UNIMPLEMENTED;
#endif
    }
}

void open_directory(const path &p)
{
#ifdef _WIN32
    auto pidl = ILCreateFromPath(p.wstring().c_str());
    if (pidl)
    {
        CoInitialize(0);
        // ShellExecute does not work here for some scenarios
        auto r = SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
        if (FAILED(r))
        {
            LOG_INFO(logger, "Error in SHOpenFolderAndSelectItems");
        }
        ILFree(pidl);
    }
    else
    {
        LOG_INFO(logger, "Error in ILCreateFromPath");
    }
#else
    open_nix(p);
#endif
}

void open_file(const path &p)
{
#ifdef _WIN32
    CoInitialize(0);
    auto r = ShellExecute(0, L"open", p.wstring().c_str(), 0, 0, 0);
    if (r <= (HINSTANCE)HINSTANCE_ERROR)
    {
        throw SW_RUNTIME_ERROR("Error in ShellExecute");
    }
#else
    open_nix(p);
#endif
}

SUBCOMMAND_DECL(open)
{
    auto swctx = createSwContext();
    auto &sdb = swctx->getLocalStorage();
    auto pkgs = swctx->resolve(sw::UnresolvedPackages{ open_arg });
    auto &p2 = pkgs.find(open_arg)->second;

    if (!sdb.isPackageInstalled(*p2))
    {
        LOG_INFO(logger, "Package '" + p2->toString() + "' not installed");
        return;
    }

    auto p = swctx->resolve(open_arg);

    LOG_INFO(logger, "package: " + p.toString());
    LOG_INFO(logger, "package dir: " + p.getDir().u8string());

    open_directory(p.getDirSrc() / ""); // on win we must add last slash
}
