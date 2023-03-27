/*
 *  Copyright (c) 2023 NetEase Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * Project: Curve
 * Created Date: 2023-03-09
 * Author: Jingli Chen (Wine93)
 */

#ifndef CURVEFS_SRC_CLIENT_FILESYSTEM_OPENFILE_H_
#define CURVEFS_SRC_CLIENT_FILESYSTEM_OPENFILE_H_

#include "src/common/lru_cache.h"
#include "curvefs/src/client/inode_wrapper.h"
#include "curvefs/src/client/filesystem/meta.h"
#include "curvefs/src/client/filesystem/defer_sync.h"
#include "curvefs/src/client/filesystem/dir_cache.h"

namespace curvefs {
namespace client {
namespace filesystem {

using ::curve::common::LRUCache;
using ::curve::common::ReadLockGuard;
using ::curve::common::WriteLockGuard;
using ::curvefs::client::InodeWrapper;
using ::curvefs::client::common::OpenFileOption;

struct OpenFile {
    OpenFile(std::shared_ptr<InodeWrapper> inode)
        : inode(inode), refs(0) {}

    std::shared_ptr<InodeWrapper> inode;
    uint64_t refs;
};

class OpenFiles {
 public:
    using LRUType = LRUCache<Ino, std::shared_ptr<OpenFile>>;

 public:
    explicit OpenFiles(OpenFileOption option,
                       std::shared_ptr<DeferSync> deferSync,
                       std::shared_ptr<DirCache> dirCache);

    void Open(Ino ino, std::shared_ptr<InodeWrapper> inode);

    bool IsOpened(Ino ino, std::shared_ptr<InodeWrapper>* inode);

    void Close(Ino ino);

    void CloseAll();

    bool GetFileAttr(Ino ino, InodeAttr* inode);

 private:
    void UpdateDirEntryLength(Ino ino,
                              const std::shared_ptr<OpenFile>& file);

    void Delete(Ino ino, const std::shared_ptr<OpenFile>& file, bool flush);

    void Evit(size_t size);

 private:
    RWLock rwlock_;
    OpenFileOption option_;
    std::shared_ptr<DeferSync> deferSync_;
    std::shared_ptr<DirCache> dirCache_;
    std::shared_ptr<LRUType> files_;
};

}  // namespace filesystem
}  // namespace client
}  // namespace curvefs

#endif  // CURVEFS_SRC_CLIENT_FILESYSTEM_OPENFILE_H_