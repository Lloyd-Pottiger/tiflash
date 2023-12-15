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
#include <IO/MemoryReadWriteBuffer.h>
#include <Storages/KVStore/FFI/FileEncryption.h>
#include <Storages/KVStore/FFI/ProxyFFI.h>
#include <Storages/Page/V3/Universal/UniversalPageStorage.h>
#include <Storages/Page/V3/Universal/UniversalWriteBatchImpl.h>
#include <openssl/md5.h>

namespace DB
{
CSEDataKeyManager::CSEDataKeyManager(
    EngineStoreServerWrap * tiflash_instance_wrap_,
    UniversalPageStoragePtr & ps_write_)
    : tiflash_instance_wrap{tiflash_instance_wrap_}
    , ps_write{ps_write_}
    , master_key(std::make_shared<MasterKey>(tiflash_instance_wrap->proxy_helper->getMasterKey()))
{}

FileEncryptionInfo CSEDataKeyManager::getFile(const EncryptionPath & ep)
{
    if (ep.keyspace_id == 0)
    {
        return FileEncryptionInfo{
            FileEncryptionRes::Disabled,
            EncryptionMethod::Plaintext,
            nullptr,
            nullptr,
            nullptr,
        };
    }
    // Read encrypted key from PageStorage
    String page_id = fmt::format("{}tm", ep.keyspace_id); // FIXME: a better pageID
    Page page = ps_write->read(UniversalPageId(page_id.data(), page_id.size()));
    auto data = page.getFieldData(0);
    auto encryption_key = master_key->decryptEncryptionKey(data.data());
    // Use MD5 of file path as IV
    unsigned char md5_value[MD5_DIGEST_LENGTH];
    static_assert(MD5_DIGEST_LENGTH == sizeof(uint64_t) * 2);
    String file_path = fmt::format("{}/{}", ep.full_path, ep.file_name);
    MD5(reinterpret_cast<const unsigned char *>(file_path.c_str()), file_path.size(), md5_value);
    return encryption_key.generateEncryptionInfo(String(reinterpret_cast<const char *>(md5_value)));
}

FileEncryptionInfo CSEDataKeyManager::newFile(const EncryptionPath & ep)
{
    if (ep.keyspace_id == 0)
    {
        return FileEncryptionInfo{
            FileEncryptionRes::Disabled,
            EncryptionMethod::Plaintext,
            nullptr,
            nullptr,
            nullptr,
        };
    }
    // Generate encryption key
    const auto encryption_key = master_key->generateEncryptionKey();
    // Write encrypted key to PageStorage
    UniversalWriteBatch wb;
    String page_id = fmt::format("{}tm", ep.keyspace_id); // FIXME: a better pageID
    MemoryWriteBuffer wb_buffer;
    writeBinary(encryption_key.exportString(), wb_buffer);
    auto read_buf = wb_buffer.tryGetReadBuffer();
    wb.putPage(UniversalPageId(page_id.data(), page_id.size()), 0, read_buf, wb_buffer.count());
    ps_write->write(std::move(wb));
    // Use MD5 of file path as IV
    unsigned char md5_value[MD5_DIGEST_LENGTH];
    static_assert(MD5_DIGEST_LENGTH == sizeof(uint64_t) * 2);
    String file_path = fmt::format("{}/{}", ep.full_path, ep.file_name);
    MD5(reinterpret_cast<const unsigned char *>(file_path.c_str()), file_path.size(), md5_value);
    return encryption_key.generateEncryptionInfo(String(reinterpret_cast<const char *>(md5_value)));
}

void CSEDataKeyManager::deleteFile(const EncryptionPath & ep, bool /*throw_on_error*/)
{
    if (ep.keyspace_id == 0)
        return;
    String page_id = fmt::format("{}tm", ep.keyspace_id); // FIXME: a better pageID
    UniversalWriteBatch wb;
    wb.delPage(UniversalPageId(page_id.data(), page_id.size()));
}

void CSEDataKeyManager::linkFile(const EncryptionPath & /*src_ep*/, const EncryptionPath & /*dst_ep*/)
{
    // Just do nothing
}

} // namespace DB
