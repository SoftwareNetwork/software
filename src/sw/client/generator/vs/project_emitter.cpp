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

#include "project_emitter.h"

#include "../generator.h"
#include "vs.h"

#include <sw/builder/os.h>
#include <sw/driver/build_settings.h>
#include <sw/driver/types.h>

#include <primitives/sw/cl.h>

static ::cl::opt<String> toolset("toolset", ::cl::desc("Set VS generator toolset"));

using namespace sw;

static const std::map<ArchType, String> platforms
{
    {
        ArchType::x86,
        "Win32",
    },
    {
        ArchType::x86_64,
        "x64",
    },
    {
        ArchType::arm,
        "ARM",
    },
    {
        ArchType::aarch64,
        "ARM64",
    },
};

 // vsgen
namespace generator
{

static String toString(ConfigurationType t)
{
    switch (t)
    {
    case ConfigurationType::Debug:
        return "Debug";
    case ConfigurationType::Release:
        return "Release";
    case ConfigurationType::MinimalSizeRelease:
        return "MinSizeRel";
    case ConfigurationType::ReleaseWithDebugInformation:
        return "RelWithDebInfo";
    default:
        throw SW_RUNTIME_ERROR("no such config");
    }
}

static String toString(ArchType t)
{
    auto i = platforms.find(t);
    if (i == platforms.end())
        return generator::toString(ArchType::x86); // return dummy default
                                                   //throw SW_RUNTIME_ERROR("no such platform");
    return i->second;
}

static String toString(LibraryType t)
{
    switch (t)
    {
    case LibraryType::Static:
        return "Static";
    case LibraryType::Shared:
        return "Dll";
    default:
        throw SW_RUNTIME_ERROR("no such lib type");
    }
}

} // namespace generator

std::string getVsToolset(const Version &v)
{
    switch (v.getMajor())
    {
    case 16:
        return "v142";
    case 15:
        return "v141";
    case 14:
        return "v140";
    case 12:
        return "v12";
    case 11:
        return "v11";
    case 10:
        return "v10";
    case 9:
        return "v9";
    case 8:
        return "v8";
        // _xp?
        // v71?
    }
    throw SW_RUNTIME_ERROR("Unknown VS version");
}

String get_configuration(const BuildSettings &s)
{
    String c = generator::toString(s.Native.ConfigurationType) + generator::toString(s.Native.LibrariesType);
    if (s.Native.MT)
        c += "Mt";
    return c;
}

static std::pair<String, String> get_project_configuration_pair(const BuildSettings &s)
{
    return {"Condition", "'$(Configuration)|$(Platform)'=='" + get_project_configuration(s) + "'"};
}

String get_project_configuration(const BuildSettings &s)
{
    String c;
    c += get_configuration(s);
    if (platforms.find(s.TargetOS.Arch) == platforms.end())
        c += " - " + toString(s.TargetOS.Arch);
    c += "|" + generator::toString(s.TargetOS.Arch);
    return c;
}

XmlEmitter::XmlEmitter(bool print_version)
    : Emitter("  ")
{
    if (print_version)
        addLine(R"(<?xml version="1.0" encoding="utf-8"?>)");
}

void XmlEmitter::beginBlock(const String &n, const std::map<String, String> &params, bool empty)
{
    beginBlock1(n, params, empty);
    increaseIndent();
}

void XmlEmitter::beginBlockWithConfiguration(const String &n, const BuildSettings &s, std::map<String, String> params, bool empty)
{
    params.insert(get_project_configuration_pair(s));
    beginBlock(n, params, empty);
}

void XmlEmitter::endBlock(bool text)
{
    decreaseIndent();
    endBlock1(text);
}

void XmlEmitter::addBlock(const String &n, const String &v, const std::map<String, String> &params)
{
    beginBlock1(n, params, v.empty());
    if (v.empty())
        return;
    addText(v);
    endBlock1(true);
}

void XmlEmitter::beginBlock1(const String &n, const std::map<String, String> &params, bool empty)
{
    blocks.push(n);
    addLine("<" + blocks.top());
    for (auto &[k, v] : params)
        addText(" " + k + "=\"" + v + "\"");
    if (empty)
        addText(" /");
    addText(">");
    if (empty)
        blocks.pop();
}

void XmlEmitter::endBlock1(bool text)
{
    if (text)
        addText("</" + blocks.top() + ">");
    else
        addLine("</" + blocks.top() + ">");
    blocks.pop();
}

void FiltersEmitter::beginProject()
{
    beginBlock("Project", {{"ToolsVersion", "4.0"},
        {"xmlns", "http://schemas.microsoft.com/developer/msbuild/2003"}});
}

void FiltersEmitter::endProject()
{
    endBlock();
}

void ProjectEmitter::beginProject(const sw::Version &version)
{
    beginBlock("Project", {{"DefaultTargets", "Build"},
        {"ToolsVersion", std::to_string(version.getMajor()) + ".0"},
        {"xmlns", "http://schemas.microsoft.com/developer/msbuild/2003"}});
}

void ProjectEmitter::endProject()
{
    endBlock();
}

void ProjectEmitter::addProjectConfigurations(const Project &p)
{
    beginBlock("ItemGroup", {{"Label", "ProjectConfigurations"}});
    for (auto &s : p.getSettings())
    {
        beginBlock("ProjectConfiguration", {{"Include", get_project_configuration(s)}});
        addBlock("Configuration", get_configuration(s));
        addBlock("Platform", generator::toString(BuildSettings(s).TargetOS.Arch));
        endBlock();
    }
    endBlock();
}

void ProjectEmitter::addPropertyGroupConfigurationTypes(const Project &p)
{
    for (auto &s : p.getSettings())
    {
        auto &d = p.getData(s);
        beginBlockWithConfiguration("PropertyGroup", s, {{"Label", "Configuration"}});
        addConfigurationType((int)d.type);
        //addBlock("UseDebugLibraries", generator::toString(s.Settings.Native.ConfigurationType));
        if (toolset.empty())
        {
            addBlock("PlatformToolset", getVsToolset(p.g->version));
        }
        else
            addBlock("PlatformToolset", toolset);

        endBlock();
    }
}

void ProjectEmitter::addConfigurationType(int t)
{
    switch ((VSProjectType)t)
    {
    case VSProjectType::Makefile:
        addBlock("ConfigurationType", "Makefile");
        break;
    case VSProjectType::Utility:
        addBlock("ConfigurationType", "Utility");
        break;
    case VSProjectType::Application:
        addBlock("ConfigurationType", "Application");
        break;
    case VSProjectType::DynamicLibrary:
        addBlock("ConfigurationType", "DynamicLibrary");
        break;
    case VSProjectType::StaticLibrary:
        addBlock("ConfigurationType", "StaticLibrary");
        break;
    default:
        break;
    }
}

void ProjectEmitter::addPropertySheets(const Project &p)
{
    for (auto &s : p.getSettings())
    {
        beginBlock("ImportGroup", {{"Condition", "'$(Configuration)|$(Platform)'=='" +
            get_project_configuration(s) + "'"},
            {"Label", "PropertySheets"}});
        addBlock("Import", "", {
            {"Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props"},
            {"Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')"},
            {"Label", "LocalAppDataPlatform"},
            });
        endBlock();
    }
}
