/*
Copyright (©) 2024  Frosty515

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

#include "mount.hpp"

#include <errno.h>
#include <string.h>

#include <Scheduling/Process.hpp>

int Mount(const char* source, const char* target, FileSystemType type, FilePrivilegeLevel current_privilege) {
    (void)source;
    if (!g_VFS->IsValidPath(target))
        return -ENOENT;
    return g_VFS->Mount(current_privilege, target, type);
}

int Unmount(const char* target, FilePrivilegeLevel current_privilege) {
    if (!g_VFS->IsValidPath(target))
        return -ENOENT;
    return g_VFS->Unmount(current_privilege, target);
}

int sys_mount(Scheduling::Thread* thread, const char* target, const char* type, const char* device) {
    Scheduling::Process* process = thread->GetParent();
    if (process == nullptr || !process->ValidateStringRead(target) || !process->ValidateStringRead(type) || !process->ValidateStringRead(device))
        return -EFAULT;

    FileSystemType fsType;

    if (strcmp(type, "tempfs") == 0 || strcmp(type, "tmpfs") == 0)
        fsType = FileSystemType::TMPFS;
    else
        return -ENOSYS;

    return Mount(device, target, fsType, {process->GetEUID(), process->GetEGID(), 0});
}

int sys_unmount(Scheduling::Thread* thread, const char* target) {
    Scheduling::Process* process = thread->GetParent();
    if (process == nullptr || !process->ValidateStringRead(target))
        return -EFAULT;

    return Unmount(target, {process->GetEUID(), process->GetEGID(), 0});
}