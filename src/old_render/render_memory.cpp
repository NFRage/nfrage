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
#include "RenderPch.h"

template<typename T, typename A> constexpr T nri_mem_allocator_align(T x, A alignment) { 
    return (T)((size_t(x) + (size_t)alignment - 1) & ~((size_t)alignment - 1)); 
}

nri_memory_allocator::nri_memory_allocator() {
    memalloci.Allocate = &nri_mem_allocate;
    memalloci.Reallocate = &nri_mem_reallocate;
    memalloci.Free = &nri_mem_free;
    memalloci.userArg = this;
}

nri_memory_allocator::nri_mem_allocator_header* nri_memory_allocator::nri_mem_allocator_get_header(void* memory) {
    return (nri_mem_allocator_header*)memory - 1;
}

void* nri_memory_allocator::nri_mem_allocate(void* userArg, size_t size, size_t alignment) {
    if (alignment == 0) return nullptr;

    const size_t alignedHeaderSize = nri_mem_allocator_align(sizeof(nri_mem_allocator_header), alignment);
    const size_t allocationSize = size + alignment - 1 + alignedHeaderSize;

    uint8_t* memory = (uint8_t*)malloc(allocationSize);

    if (memory == nullptr)
        return nullptr;

    uint8_t* alignedMemory = nri_mem_allocator_align(memory, alignment) + alignedHeaderSize;

    nri_mem_allocator_header* header = nri_mem_allocator_get_header(alignedMemory);
    *header = {};
    header->size = allocationSize;
    header->alignment = alignment;
    header->offset = (uint32_t)(alignedMemory - memory);

    return alignedMemory; 
}

void* nri_memory_allocator::nri_mem_reallocate(void* userArg, void* memory, size_t size, size_t alignment) {
    if (alignment == 0) return nullptr;
    if (!memory) return nri_mem_allocate(userArg, size, alignment);

    const nri_mem_allocator_header prevHeader = *nri_mem_allocator_get_header(memory);

    if (prevHeader.alignment != alignment) {
        printf("DebugAlignedRealloc() failed: memory alignment mismatch.\n");
    }

    const size_t alignedHeaderSize = nri_mem_allocator_align(sizeof(nri_mem_allocator_header), alignment);
    const size_t allocationSize = size + alignment - 1 + alignedHeaderSize;

    uint8_t* prevMemoryBegin = (uint8_t*)memory - prevHeader.offset;

    uint8_t* newMemory = (uint8_t*)realloc(prevMemoryBegin, allocationSize);

    if (newMemory == nullptr)
        return nullptr;

    uint8_t* alignedMemory = nri_mem_allocator_align(newMemory, alignment) + alignedHeaderSize;

    nri_mem_allocator_header* newHeader = nri_mem_allocator_get_header(alignedMemory);
    *newHeader = {};
    newHeader->size = allocationSize;
    newHeader->alignment = alignment;
    newHeader->offset = (uint32_t)(alignedMemory - newMemory);

    return alignedMemory;
}

void nri_memory_allocator::nri_mem_free(void* userArg, void* memory) {
    if (!memory) return;
    const nri_mem_allocator_header* header = nri_mem_allocator_get_header(memory);
    free((uint8_t*)memory - header->offset);
}
