/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2022-2023. All rights reserved.
* nfrage - engine code for NFRage project
**********************************************************************
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free 
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
* Boston, MA 02110-1301 USA
*****************************************************************/
#pragma once

struct nri_memory_allocator {
public:
    nri::MemoryAllocatorInterface memalloci;

    nri_memory_allocator();

private:
    struct nri_mem_allocator_header {
        size_t   size;
        uint32_t alignment;
        uint32_t offset;
    };

    static nri_mem_allocator_header* nri_mem_allocator_get_header(void* memory);
    static void* nri_mem_allocate(void* userArg, size_t size, size_t alignment);
    static void* nri_mem_reallocate(void* userArg, void* memory, size_t size, size_t alignment);
    static void nri_mem_free(void* userArg, void* memory);
};
