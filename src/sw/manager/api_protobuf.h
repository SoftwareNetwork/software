// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2016-2018 Egor Pugin <egor.pugin@gmail.com>

#pragma once

#include "remote.h"

#include <grpcpp/grpcpp.h>

#include <sw/protocol/api.grpc.pb.h>

namespace sw
{

struct Remote;

struct ProtobufApi : Api
{
    ProtobufApi(const Remote &);

    bool resolve(
        ResolveRequest &rr,
        std::unordered_map<PackageId, PackageData> &data, const IStorage &) const override;
    /*ResolveResult resolvePackages(
        const UnresolvedPackages &pkgs, UnresolvedPackages &unresolved_pkgs,
        std::unordered_map<PackageId, PackageData> &data, const IStorage &) const override;*/
    void addVersion(const PackagePath &prefix, const PackageDescriptionMap &pkgs, const SpecificationFiles &) const override;

    void addVersion(const PackagePath &prefix, const String &script);
    void addVersion(PackagePath p, const PackageVersion &vnew, const std::optional<PackageVersion> &vold = {});
    void updateVersion(PackagePath p, const PackageVersion &v);
    void removeVersion(PackagePath p, const PackageVersion &v);

    void getNotifications(int n = 10);
    void clearNotifications();

private:
    const Remote &r;
    GrpcChannel c;
    std::unique_ptr<api::ApiService::Stub> api_;
    std::unique_ptr<api::UserService::Stub> user_;

    std::unique_ptr<grpc::ClientContext> getContext() const;
    std::unique_ptr<grpc::ClientContext> getContextWithAuth() const;
};

}
