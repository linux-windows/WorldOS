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

#ifndef _SYSCALL_H
#define _SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

enum SystemCalls {
    SC_EXIT = 0,
    SC_READ = 1,
    SC_WRITE = 2,
    SC_OPEN = 3,
    SC_CLOSE = 4,
    SC_SEEK = 5,
    SC_MMAP = 6,
    SC_MUNMAP = 7,
    SC_MPROTECT = 8
};

#ifndef _IN_KERNEL

inline unsigned long system_call(unsigned long num, unsigned long arg1, unsigned long arg2, unsigned arg3) {
    // Number is in RAX, arg1 is in RDI, arg2 is in RSI, arg3 is in RDX
    unsigned long ret;
    __asm__ volatile("syscall" : "=a"(ret) : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3) : "rcx", "r11", "memory");
    return ret;
}

#endif /* _IN_KERNEL */


#ifdef __cplusplus
}
#endif

#endif /* _SYS_SYSCALL_H */