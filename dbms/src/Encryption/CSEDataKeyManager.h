// Copyright 2023 PingCAP, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <Common/Exception.h>
#include <Common/TiFlashException.h>
#include <Encryption/KeyManager.h>
#include <Encryption/MasterKey.h>
#include <Poco/Path.h>
#include <common/likely.h>

namespace DB
{
struct EngineStoreServerWrap;

/// CSEDataKeyManager is a KeyManager implementation that designed for Cloud Storage Engine.
/// It will store all encryption keys which have been encrypted using MasterKey in PageStorage.
class CSEDataKeyManager : public KeyManager
{
public:
    explicit CSEDataKeyManager(EngineStoreServerWrap * tiflash_instance_wrap_);

    ~CSEDataKeyManager() override = default;

    FileEncryptionInfo getFile(const String & fname) override;

    FileEncryptionInfo newFile(const String & fname) override;

    void deleteFile(const String & fname, bool throw_on_error) override;

    // Note: This function will not be used.
    void linkFile(const String & src_fname, const String & dst_fname) override;

private:
    EngineStoreServerWrap * tiflash_instance_wrap;
    const MasterKeyPtr master_key;
};
} // namespace DB
