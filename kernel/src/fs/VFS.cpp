/*
Copyright (©) 2023  Frosty515

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "VFS.hpp"
#include "FileStream.hpp"

#include "TempFS/TempFileSystem.hpp"

#include <Memory/kmalloc.hpp>

#include <string.h>

VFS* g_VFS = nullptr;

VFS::VFS() : m_root(nullptr) {

}

VFS::VFS(FileSystemType root_type) : m_root(nullptr) {
    MountRoot(root_type);
}

VFS::~VFS() {

}

bool VFS::MountRoot(FileSystemType type) {
    m_root = new VFS_MountPoint;
    m_root->type = type;
    m_root->RootInode = nullptr;
    switch (type) {
        case FileSystemType::TMPFS:
            m_root->fs = (FileSystem*)(new TempFS::TempFileSystem(PAGE_SIZE));
            break;
        default:
            delete m_root;
            return false;
    }
    m_mountPoints.insert(m_root);
    return true;
}

bool VFS::Mount(const char* path, FileSystemType type) {
    fprintf(VFS_DEBUG, "[%s(%s, %u)] ERROR: Unimplemented function.\n", __extension__ __PRETTY_FUNCTION__, path, (unsigned int)type);
    return false;
}

bool VFS::CreateFile(const char* parent, const char* name, size_t size) {
    Inode* parent_inode = nullptr;
    VFS_MountPoint* mountPoint = GetMountPoint(parent, &parent_inode);
    if (mountPoint == nullptr || (parent_inode == nullptr && GetLastError() != FileSystemError::SUCCESS) || name == nullptr || (parent_inode != nullptr && parent_inode->GetType() != InodeType::Folder)) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        return false;
    }
    switch (mountPoint->type) {
        case FileSystemType::TMPFS:
            if (mountPoint->fs == nullptr) {
                SetLastError(FileSystemError::INTERNAL_ERROR);
                return false;
            }
            {
                using namespace TempFS;
                TempFileSystem* fs = (TempFileSystem*)mountPoint->fs;
                bool rc = fs->CreateFile((TempFSInode*)parent_inode, name, size);
                SetLastError(fs->GetLastError());
                return rc;
            }
            break;
        default:
            SetLastError(FileSystemError::INVALID_FS);
            return false;
            break;
    }
    SetLastError(FileSystemError::INTERNAL_ERROR); // should be unreachable
    return false;
}

bool VFS::CreateFolder(const char* parent, const char* name) {
    Inode* parent_inode = nullptr;
    VFS_MountPoint* mountPoint = GetMountPoint(parent, &parent_inode);
    if (mountPoint == nullptr || (parent_inode == nullptr && GetLastError() != FileSystemError::SUCCESS) || name == nullptr || (parent_inode != nullptr && parent_inode->GetType() != InodeType::Folder)) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        return false;
    }
    uint64_t name_length = strlen(name);
    char const* i_name = name;
    if (name[name_length - 1] == PATH_SEPARATOR) {
        char* temp_name = new char[name_length];
        memcpy(temp_name, name, name_length - 1);
        i_name = temp_name;
    }
    switch (mountPoint->type) {
        case FileSystemType::TMPFS:
            if (mountPoint->fs == nullptr) {
                SetLastError(FileSystemError::INTERNAL_ERROR);
                return false;
            }
            {
                using namespace TempFS;
                TempFileSystem* fs = (TempFileSystem*)mountPoint->fs;
                bool rc = fs->CreateFolder((TempFSInode*)parent_inode, i_name);
                SetLastError(fs->GetLastError());
                return rc;
            }
            break;
        default:
            SetLastError(FileSystemError::INVALID_FS);
            return false;
            break;
    }
    SetLastError(FileSystemError::INTERNAL_ERROR); // should be unreachable
    return false;
}

bool VFS::CreateSymLink(const char* parent, const char* name, const char* target) {
    Inode* parent_inode = nullptr;
    VFS_MountPoint* mountPoint = GetMountPoint(parent, &parent_inode);
    Inode* target_inode = nullptr;
    VFS_MountPoint* target_mountPoint = GetMountPoint(target, &target_inode); // currently, parent and target must be on the same mount-point
    if (mountPoint == nullptr || mountPoint != target_mountPoint || (parent_inode == nullptr && GetLastError() != FileSystemError::SUCCESS) || name == nullptr || (parent_inode != nullptr && parent_inode->GetType() != InodeType::Folder) || target_inode == nullptr) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        return false;
    }
    uint64_t name_length = strlen(name);
    char const* i_name = name;
    if (name[name_length - 1] == PATH_SEPARATOR) {
        char* temp_name = new char[name_length];
        memcpy(temp_name, name, name_length - 1);
        i_name = temp_name;
    }
    switch (mountPoint->type) {
        case FileSystemType::TMPFS:
            if (mountPoint->fs == nullptr) {
                SetLastError(FileSystemError::INTERNAL_ERROR);
                return false;
            }
            {
                using namespace TempFS;
                TempFileSystem* fs = (TempFileSystem*)mountPoint->fs;
                bool rc = fs->CreateSymLink((TempFSInode*)parent_inode, i_name, (TempFSInode*)target_inode);
                SetLastError(fs->GetLastError());
                return rc;
            }
            break;
        default:
            SetLastError(FileSystemError::INVALID_FS);
            return false;
            break;
    }
    SetLastError(FileSystemError::INTERNAL_ERROR); // should be unreachable
    return false;
}


bool VFS::DeleteInode(const char* path, bool recursive) {
    Inode* parent_inode = nullptr;
    VFS_MountPoint* mountPoint = GetMountPoint(path, &parent_inode);
    if (mountPoint == nullptr || parent_inode == nullptr) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        return false;
    }
    switch (mountPoint->type) {
        case FileSystemType::TMPFS:
            if (mountPoint->fs == nullptr) {
                SetLastError(FileSystemError::INTERNAL_ERROR);
                return false;
            }
            {
                using namespace TempFS;
                TempFileSystem* fs = (TempFileSystem*)mountPoint->fs;
                bool rc = fs->DeleteInode((TempFSInode*)parent_inode, recursive);
                SetLastError(fs->GetLastError());
                return rc;
            }
            break;
        default:
            SetLastError(FileSystemError::INVALID_FS);
            return false;
            break;
    }
    SetLastError(FileSystemError::INTERNAL_ERROR); // should be unreachable
    return false;
}


FileStream* VFS::OpenStream(const char* path, uint8_t modes) {
    Inode* inode = nullptr;
    VFS_MountPoint* mountPoint = GetMountPoint(path, &inode);
    if (mountPoint == nullptr || inode == nullptr) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        return nullptr;
    }
    switch (mountPoint->type) {
        case FileSystemType::TMPFS:
            if (mountPoint->fs == nullptr) {
                SetLastError(FileSystemError::INTERNAL_ERROR);
                return nullptr;
            }
            {
                using namespace TempFS;
                FileStream* stream = new FileStream(inode, mountPoint, modes);
                if (stream == nullptr) {
                    SetLastError(FileSystemError::ALLOCATION_FAILED);
                    return nullptr;
                }
                if (!stream->Open()) {
                    FileStreamError error = stream->GetLastError();
                    if (error == FileStreamError::INVALID_INODE)
                        SetLastError(FileSystemError::INVALID_ARGUMENTS);
                    else if (error == FileStreamError::INVALID_FS_TYPE)
                        SetLastError(FileSystemError::INVALID_FS);
                    else
                        SetLastError(FileSystemError::INTERNAL_ERROR);
                    return nullptr;
                }
                SetLastError(FileSystemError::SUCCESS);
                m_streams.insert(stream);
                return stream;
            }
            break;
        default:
            SetLastError(FileSystemError::INVALID_FS);
            return nullptr;
            break;
    }
    SetLastError(FileSystemError::INTERNAL_ERROR); // should be unreachable
    return nullptr;
}

bool VFS::CloseStream(FileStream* stream) {
    if (stream == nullptr) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        return false;
    }
    for (uint64_t i = 0; i < m_streams.getCount(); i++) {
        FileStream* i_stream = m_streams.get(i);
        if (i_stream == nullptr) {
            SetLastError(FileSystemError::INTERNAL_ERROR);
            return false;
        }
        if (stream == i_stream) {
            (void)(stream->Close()); // ignore return value
            delete stream;
            m_streams.remove(i);
            SetLastError(FileSystemError::SUCCESS);
            return true;
        }
    }
    SetLastError(FileSystemError::INVALID_ARGUMENTS);
    return false;
}

/* Private functions */

VFS_MountPoint* VFS::GetMountPoint(const char* path, Inode** inode) {
    if (path == nullptr) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        return nullptr;
    }
    if (strcmp(path, "/") == 0) { // TODO: handle the no parent case better with proper path resolution
        SetLastError(FileSystemError::SUCCESS);
        if (inode != nullptr)
            *inode = nullptr;
        return m_root;
    }
    Inode* RootInode = nullptr;
    int64_t split_index = 0;
    switch (m_root->type) {
    case FileSystemType::TMPFS:
        {
            using namespace TempFS;
            TempFileSystem* fs = (TempFileSystem*)(m_root->fs);
            if (fs == nullptr) {
                SetLastError(FileSystemError::INTERNAL_ERROR);
                if (inode != nullptr)
                    *inode = nullptr;
                return nullptr;
            }
            TempFSInode* last_inode = nullptr;
            TempFSInode* raw_inode = fs->GetInode(path, &last_inode, &split_index);
            if (raw_inode == nullptr)
                raw_inode = last_inode;
            if (raw_inode != nullptr && raw_inode->GetType() != InodeType::Folder)
                raw_inode = raw_inode->GetParent();
            RootInode = (Inode*)raw_inode;
        }
        break;
    default:
        SetLastError(FileSystemError::INVALID_FS);
        if (inode != nullptr)
            *inode = nullptr;
        return nullptr;
    }
    bool rootFound = false;
    VFS_MountPoint* mount_point;
    while (true) {
        if (RootInode == nullptr) {
            mount_point = m_root;
            rootFound = true;
            break;
        }
        for (uint64_t i = 0; i < m_mountPoints.getCount(); i++) {
            VFS_MountPoint* mountPoint = m_mountPoints.get(i);
            if (mountPoint == nullptr) {
                SetLastError(FileSystemError::INTERNAL_ERROR);
                if (inode != nullptr)
                    *inode = nullptr;
                return nullptr;
            }
            if (mountPoint->RootInode == RootInode) {
                rootFound = true;
                mount_point = mountPoint;
                break;
            }
        }
        if (rootFound)
            break;
        RootInode = RootInode->GetParent();
    }
    if (!rootFound) {
        SetLastError(FileSystemError::INVALID_ARGUMENTS);
        if (inode != nullptr)
            *inode = nullptr;
        return nullptr;
    }
    if (inode != nullptr) {
        switch (mount_point->type) {
        case FileSystemType::TMPFS:
            {
                using namespace TempFS;
                assert(split_index > 0); // TODO: do proper error handling
                if (mount_point == m_root)
                    split_index = 0;
                TempFSInode* sub_inode = ((TempFileSystem*)(mount_point->fs))->GetInode(&(path[split_index]));
                if (sub_inode == nullptr) {
                    SetLastError(FileSystemError::INVALID_ARGUMENTS);
                    *inode = nullptr;
                    return nullptr;
                }
                *inode = sub_inode;
            }
            break;
        default:
            SetLastError(FileSystemError::INVALID_FS);
            if (inode != nullptr)
                *inode = nullptr;
            return nullptr;
        }
    }
    SetLastError(FileSystemError::SUCCESS);
    return mount_point;
}

bool VFS::isMountpoint(const char* path, size_t len) {
    fprintf(VFS_DEBUG, "[%s(%lp, %lu)] ERROR: Unimplemented function.\n", __extension__ __PRETTY_FUNCTION__, path, len);
    return false;
}