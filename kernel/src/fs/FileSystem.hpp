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

#ifndef _FILE_SYSTEM_HPP
#define _FILE_SYSTEM_HPP

#include <stdint.h>
#include <stddef.h>

enum class FileSystemError {
    SUCCESS = 0,
    STREAM_CLOSED = 1,
    INVALID_ARGUMENTS = 2, // One or more arguments are invalid.
    INTERNAL_ERROR = 3, // An error with class data structure(s)
    ALLOCATION_FAILED = 4,
    RECURSION_ERROR = 5,
    INVALID_FS = 6 // only for VFS. Filesystem type of mountpoint is invalid
};

enum class FileSystemType {
    TMPFS = 0
};

constexpr char PATH_SEPARATOR = '/';

class FileSystem {
public:
    virtual ~FileSystem() {}

    virtual bool CreateFile(const char* parent, const char* name, size_t size = 0) = 0;
    virtual bool CreateFolder(const char* parent, const char* name) = 0;
    virtual bool CreateSymLink(const char* parent, const char* name, const char* target) = 0;

    virtual bool DeleteInode(const char* path, bool recursive = false) = 0;

    virtual FileSystemError GetLastError() const { return p_lastError; }

protected:
    virtual void SetLastError(FileSystemError error) const { p_lastError = error; } // const so const functions can perform error reporting

protected:
    // Standard members
    size_t p_blockSize;

    // Error management
    mutable FileSystemError p_lastError; // mutable so const functions can perform error reporting

};

#endif /* _FILE_SYSTEM_HPP */