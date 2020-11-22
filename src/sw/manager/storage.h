// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2016-2019 Egor Pugin <egor.pugin@gmail.com>

#pragma once

#include "package.h"

#include <sw/support/settings.h>
#include <sw/support/storage.h>

namespace sw
{

struct LocalPackage;
struct PackageId;
struct PackageData;
struct Remote;

namespace vfs
{

/*struct SW_MANAGER_API VirtualFileSystem
{
    virtual ~VirtualFileSystem() = default;

    virtual void writeFile(const PackageId &pkg, const path &local_file, const path &vfs_file) const = 0;
};

// default fs
struct SW_MANAGER_API LocalFileSystem : VirtualFileSystem
{
};

// more than one destination
struct SW_MANAGER_API VirtualFileSystemMultiplexer : VirtualFileSystem
{
    std::vector<std::shared_ptr<VirtualFileSystem>> filesystems;

    void writeFile(const PackageId &pkg, const path &local_file, const path &vfs_file) const override
    {
        for (auto &fs : filesystems)
            fs->writeFile(pkg, local_file, vfs_file);
    }
};*/

struct SW_MANAGER_API File
{
    virtual ~File() = default;

    virtual bool copy(const path &to) const = 0;
};

struct SW_MANAGER_API FileWithHashVerification : vfs::File
{
    // available after successful copy()
    virtual String getHash() const = 0;
};

} // namespace vfs

SW_MANAGER_API
String toUserString(StorageFileType);

struct PackagesDatabase;
struct ServiceDatabase;

struct SW_MANAGER_API IResolvableStorageWithName : IStorage
{
    virtual ~IResolvableStorageWithName() = default;

    virtual String getName() const = 0;
};

struct SW_MANAGER_API IStorage2 : IResolvableStorageWithName
{
    virtual ~IStorage2() = default;

    //

    /// get file from this storage
    virtual std::unique_ptr<vfs::File> getFile(const PackageId &id, StorageFileType) const = 0;

    // ?

    //virtual LocalPackage download(const PackageId &) const = 0;
    //virtual LocalPackage install(const Package &) const = 0;

    // data exchange

    // get predefined file
    //virtual void get(const IStorage &source, const PackageId &id, StorageFileType) = 0;

    /// get specific file from storage from package directory
    //virtual void get(const IStorage &source, const PackageId &id, const path &from_rel_path, const path &to_file) = 0;

};

struct SW_MANAGER_API Storage : IStorage2
{
    Storage(const String &name);

    String getName() const override { return name; }

private:
    String name;
};

struct SW_MANAGER_API StorageWithPackagesDatabase : Storage
{
    StorageWithPackagesDatabase(const String &name, const path &db_dir);
    virtual ~StorageWithPackagesDatabase();

    PackageDataPtr loadData(const PackageId &) const override;
    //void get(const IStorage &source, const PackageId &id, StorageFileType) override;
    //ResolveResult resolve(const UnresolvedPackages &pkgs, UnresolvedPackages &unresolved_pkgs) const override;
    bool resolve(ResolveRequest &) const override;

//protected:?
    PackagesDatabase &getPackagesDatabase() const;

private:
    std::unique_ptr<PackagesDatabase> pkgdb;
    mutable std::mutex m;
    mutable std::unordered_map<PackageId, PackageData> data;
};

struct SW_MANAGER_API LocalStorageBase : StorageWithPackagesDatabase
{
    LocalStorageBase(const String &name, const path &db_dir);
    virtual ~LocalStorageBase();

    const StorageSchema &getSchema() const override { return schema; }

    virtual LocalPackage install(const Package &) const = 0;
    std::unique_ptr<vfs::File> getFile(const PackageId &id, StorageFileType) const override;

    void deletePackage(const PackageId &id) const;

private:
    StorageSchema schema;
};

struct SW_MANAGER_API OverriddenPackagesStorage : LocalStorageBase
{
    const LocalStorage &ls;

    OverriddenPackagesStorage(const LocalStorage &ls, const path &db_dir);
    virtual ~OverriddenPackagesStorage();

    LocalPackage install(const Package &) const override;
    LocalPackage install(const PackageId &, const PackageData &) const;
    bool isPackageInstalled(const Package &p) const;

    std::unordered_set<LocalPackage> getPackages() const;
    void deletePackageDir(const path &sdir) const;
};

struct SW_MANAGER_API LocalStorage : Directories, LocalStorageBase
{
    LocalStorage(const path &local_storage_root_dir);
    virtual ~LocalStorage();

    //LocalPackage download(const PackageId &) const override;
    void remove(const LocalPackage &) const;
    LocalPackage install(const Package &) const override;
    LocalPackage installLocalPackage(const PackageId &, const PackageData &);
    void get(const IStorage2 &source, const PackageId &id, StorageFileType) const /* override*/;
    bool isPackageInstalled(const Package &id) const;
    bool isPackageOverridden(const PackageId &id) const;
    bool isPackageLocal(const PackageId &id) const;
    PackageDataPtr loadData(const PackageId &) const override;
    //ResolveResult resolve(const UnresolvedPackages &pkgs, UnresolvedPackages &unresolved_pkgs) const override;
    bool resolve(ResolveRequest &) const override;

    OverriddenPackagesStorage &getOverriddenPackagesStorage();
    const OverriddenPackagesStorage &getOverriddenPackagesStorage() const;

private:
    std::unordered_map<PackageId, PackageData> local_packages;
    OverriddenPackagesStorage ovs;

    void migrateStorage(int from, int to);
};

//
// if our app is working for a long time,
// our cache will become outdated fast enough
// To overcome this, we can reset it every N minutes,
// but in this case we break per SwBuild stability of resolving.
// Thus it may worth it of movig cache storage into SwBuild
// and use it only in that resolver without any cache resets.
//
// on the other hand lots of deps resolving will be slow without caching
// and we won't be able to create SwBuild always (different cli commands)
//
// To overcome this we added 'use_cache' parameter to SwContext::resolve() method.
// Now SwBuild is able to disable SwContext caching for its purposes, and others can enable it.
//
// We also can reset() our cache when needed.
struct SW_MANAGER_API CachedStorage : IResolvableStorage
{
    //using StoredPackages = ResolveResult;
    using Key = std::pair<UnresolvedPackage, PackageSettings>;
    using Value = PackagePtr;
    using StoredPackages = std::map<Key, PackagePtr>;

    CachedStorage() = default;
    CachedStorage(const CachedStorage &) = delete;
    CachedStorage &operator=(const CachedStorage &) = delete;
    virtual ~CachedStorage() = default;

    // accepts only resolved packages
    void storePackages(const ResolveRequest &);
    bool resolve(ResolveRequest &) const override;

    void clear();
    void reset() { clear(); }

private:
    mutable StoredPackages resolved_packages;
};

} // namespace sw
