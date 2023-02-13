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

namespace nfr::render
{

    struct NRIInterface : public nri::CoreInterface, public nri::SwapChainInterface , public nri::HelperInterface { };

    struct BackBuffer {
        nri::Texture*       texture = nullptr;
        nri::Descriptor*    texture_view = nullptr;
        nri::FrameBuffer*   framebuffer = nullptr;
    };

    extern nri_memory_allocator                     memory_allocator;
     
    extern std::vector<nri::PhysicalDeviceGroup>	physicaldevices;
    extern nri::PhysicalDeviceGroup*				physicaldevice;
    extern uint32_t								    physicaldevice_SelectNum;
     
    extern nri::Device*                             device;
    extern NRIInterface                             NRI;
    
    extern nri::CommandQueue*                       commandQueue;
    extern nri::CommandQueue*                       commandQueueCopy;
     
    extern nri::SwapChain*                          swapchain;
    extern std::vector<BackBuffer>                  swapchain_backbuffers;
    extern nri::Format                              swapchain_Format;
     
    extern const BackBuffer*                        backbuffer_current;
     
    extern uint32_t                                 backbuffer_width;
    extern uint32_t                                 backbuffer_height;
     
    extern nri::QueueSemaphore*                     backbuffer_acquire_semaphore;
    extern nri::QueueSemaphore*                     backbuffer_release_semaphore;
    extern nri::QueueSemaphore*                     backbuffer_frame_semaphore;
     
    extern nri::DeviceSemaphore*					frame_deviceSemaphore;
    extern nri::CommandAllocator*					frame_commandAllocator;
    extern nri::CommandBuffer*						frame_commandBuffer_begin;
    extern nri::CommandBuffer*						frame_commandBuffer_end ;

    extern bool EnumerateDevices_Create();
    extern bool EnumerateDevices_Select();
    extern void EnumerateDevices_Destroy();

    extern bool Device_Create();
    extern void Device_Destroy();

    extern bool Swapchain_Create();
    extern void Swapchain_Destroy();
}

#ifdef NRI_ON_APPLE
void* Swapchain_Metal_Create(SDL_Window* window);
void Swapchain_Metal_Destroy();
#endif
