/*
Copyright (©) 2022-2024  Frosty515

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

#ifndef _X86_64_TASK_UTIL_HPP
#define _X86_64_TASK_UTIL_HPP

#include "task.h"

#include "../interrupts/isr.hpp"

#include <stddef.h>

#include <Memory/PageManager.hpp>

#include <Scheduling/Thread.hpp>

void x86_64_PrepareNewRegisters(x86_64_Interrupt_Registers* out, const x86_64_Registers* in);
void x86_64_ConvertToStandardRegisters(x86_64_Registers* out, const x86_64_Interrupt_Registers* in);

void x86_64_GetNewStack(PageManager* pm, x86_64_Registers* regs, size_t size);

void x86_64_SaveIRegistersToThread(const Scheduling::Thread* thread, const x86_64_Interrupt_Registers* regs);

extern "C" void x86_64_PrepareThreadExit(Scheduling::Thread* thread, int status, bool was_running, void (*func)(Scheduling::Thread*, int, bool));

// We generate the signal return instructions and copy them into a HEAP allocated buffer. The buffer MUST be freed after use. The size is outputted into the size parameter.
void* x86_64_GetSignalReturnInstructions(size_t* size, int signum);

extern "C" uint64_t x86_64_GetReturnAddress();

extern "C" void x86_64_Prep_SemaphoreAcquire(Scheduling::Semaphore* semaphore, Scheduling::Thread* thread);

extern "C" void x86_64_HandleSemaphoreAcquire(Scheduling::Semaphore* semaphore, Scheduling::Thread* thread, x86_64_Registers* regs);

#define semaphore_acquire(semaphore, thread) x86_64_Prep_SemaphoreAcquire(semaphore, thread)


#endif /* _X86_64_TASK_UTIL_HPP */