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

#include <Encryption/CSEDataKeyManager.h>
#include <Storages/KVStore/FFI/FileEncryption.h>
#include <Storages/KVStore/FFI/ProxyFFI.h>

namespace DB
{
CSEDataKeyManager::CSEDataKeyManager(EngineStoreServerWrap * tiflash_instance_wrap_)
    : tiflash_instance_wrap{tiflash_instance_wrap_}
    , master_key(tiflash_instance_wrap->proxy_helper->getMasterKey())
{}

FileEncryptionInfo CSEDataKeyManager::getFile(const String & /*fname*/)
{
    throw DB::TiFlashException("Not implemented yet", Errors::Encryption::Internal);
}

FileEncryptionInfo CSEDataKeyManager::newFile(const String & /*fname*/)
{
    throw DB::TiFlashException("Not implemented yet", Errors::Encryption::Internal);
}

void CSEDataKeyManager::deleteFile(const String & /*fname*/, bool /*throw_on_error*/)
{
    throw DB::TiFlashException("Not implemented yet", Errors::Encryption::Internal);
}

void CSEDataKeyManager::linkFile(const String & /*src_fname*/, const String & /*dst_fname*/)
{
    throw DB::TiFlashException("Not implemented yet", Errors::Encryption::Internal);
}

} // namespace DB
