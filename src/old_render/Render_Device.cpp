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

#if (/*NRI_USE_D3D12 &&*/ NRI_ON_WINDOWS)
    #include "../NRI/Source/Shared/SharedExternal.h"
    #include <d3d12.h>
    #include <dxgi1_5.h>
    #include <dxgidebug.h>
#endif

namespace nfr::render
{

nri_memory_allocator                    memory_allocator;

std::vector<nri::PhysicalDeviceGroup>	physicaldevices;
nri::PhysicalDeviceGroup*				physicaldevice = nullptr;
uint32_t								physicaldevice_SelectNum = 0;

nri::Device*                            device = nullptr;
NRIInterface                            NRI = {};

#ifdef NRI_ON_APPLE
#endif

nri::CommandQueue*                      commandQueueCopy = nullptr;
nri::CommandQueue*                      commandQueue = nullptr;

nri::SwapChain*                         swapchain = nullptr;
std::vector<BackBuffer>                 swapchain_backbuffers;
nri::Format                             swapchain_Format = nri::Format::UNKNOWN;

const BackBuffer*                       backbuffer_current = nullptr;

uint32_t                                backbuffer_width = 0;
uint32_t                                backbuffer_height = 0;

nri::QueueSemaphore*                    backbuffer_acquire_semaphore = nullptr;
nri::QueueSemaphore*                    backbuffer_release_semaphore = nullptr;
nri::QueueSemaphore*                    backbuffer_frame_semaphore = nullptr;

nri::DeviceSemaphore*					frame_deviceSemaphore = nullptr;
nri::CommandAllocator*					frame_commandAllocator = nullptr;
nri::CommandBuffer*						frame_commandBuffer_begin = nullptr;
nri::CommandBuffer*						frame_commandBuffer_end = nullptr;



bool FrameBegin() 
{
    if (!device || !swapchain || !commandQueue) return false;
    
    // wait prev frame draw
    NRI.WaitForSemaphore(*commandQueue, *frame_deviceSemaphore); // todo fence return false;

    //
    NRI.ResetCommandAllocator(*frame_commandAllocator);

    // 
    const uint32_t backBufferIndex  = NRI.AcquireNextSwapChainTexture(*swapchain, *backbuffer_acquire_semaphore);

    backbuffer_current = &swapchain_backbuffers[backBufferIndex];

    //
    {
        NRI.BeginCommandBuffer(*frame_commandBuffer_begin, nullptr, 0);

        nri::TextureTransitionBarrierDesc textureTransitionBarrierDesc = {};
        textureTransitionBarrierDesc.texture = backbuffer_current->texture;
        textureTransitionBarrierDesc.prevAccess = nri::AccessBits::UNKNOWN;
        textureTransitionBarrierDesc.nextAccess = nri::AccessBits::COLOR_ATTACHMENT;
        textureTransitionBarrierDesc.prevLayout = nri::TextureLayout::UNKNOWN;
        textureTransitionBarrierDesc.nextLayout = nri::TextureLayout::COLOR_ATTACHMENT;
        textureTransitionBarrierDesc.arraySize = 1;
        textureTransitionBarrierDesc.mipNum = 1;

        nri::TransitionBarrierDesc transitionBarriers = {};
        transitionBarriers.textureNum = 1;
        transitionBarriers.textures = &textureTransitionBarrierDesc;
        NRI.CmdPipelineBarrier(*frame_commandBuffer_begin, &transitionBarriers, nullptr, nri::BarrierDependency::ALL_STAGES);

        NRI.CmdBeginRenderPass(*frame_commandBuffer_begin, *backbuffer_current->framebuffer, nri::RenderPassBeginFlag::SKIP_FRAME_BUFFER_CLEAR);
        {
            nri::ClearDesc clearDesc = {};
            clearDesc.colorAttachmentIndex = 0;

            clearDesc.value.rgba32f = { 1.0f, 0.0f, 0.0f, 1.0f };
            nri::Rect rect1 = { 0, 0, backbuffer_width, backbuffer_height / 3 };
            NRI.CmdClearAttachments(*frame_commandBuffer_begin, &clearDesc, 1, &rect1, 1);

            clearDesc.value.rgba32f = { 0.0f, 1.0f, 0.0f, 1.0f };
            nri::Rect rect2 = { 0, (int32_t)backbuffer_height / 3, backbuffer_width, backbuffer_height / 3 };
            NRI.CmdClearAttachments(*frame_commandBuffer_begin, &clearDesc, 1, &rect2, 1);

            clearDesc.value.rgba32f = { 0.0f, 0.0f, 1.0f, 1.0f };
            nri::Rect rect3 = { 0, (int32_t)(backbuffer_height * 2) / 3, backbuffer_width, backbuffer_height / 3 };
            NRI.CmdClearAttachments(*frame_commandBuffer_begin, &clearDesc, 1, &rect3, 1);
        }
        NRI.CmdEndRenderPass(*frame_commandBuffer_begin);

        NRI.EndCommandBuffer(*frame_commandBuffer_begin);
    }

    //
    nri::WorkSubmissionDesc workSubmissionDesc = {};
    workSubmissionDesc.commandBufferNum     = 1;
    workSubmissionDesc.commandBuffers       = &frame_commandBuffer_begin;
    workSubmissionDesc.wait                 = &backbuffer_acquire_semaphore;
    workSubmissionDesc.waitNum              = 1;
    workSubmissionDesc.signal               = &backbuffer_frame_semaphore;
    workSubmissionDesc.signalNum            = 1;

    //
    NRI.SubmitQueueWork(*commandQueue, workSubmissionDesc, nullptr);
    

	return true;
}

void FrameEnd() 
{
    //
    {
        NRI.BeginCommandBuffer(*frame_commandBuffer_end, nullptr, 0);

        nri::TextureTransitionBarrierDesc textureTransitionBarrierDesc = {};
        textureTransitionBarrierDesc.texture = backbuffer_current->texture;
        textureTransitionBarrierDesc.prevAccess = nri::AccessBits::COLOR_ATTACHMENT;
        textureTransitionBarrierDesc.nextAccess = nri::AccessBits::UNKNOWN;
        textureTransitionBarrierDesc.prevLayout = nri::TextureLayout::COLOR_ATTACHMENT;
        textureTransitionBarrierDesc.nextLayout = nri::TextureLayout::PRESENT;
        textureTransitionBarrierDesc.arraySize = 1;
        textureTransitionBarrierDesc.mipNum = 1;

        nri::TransitionBarrierDesc transitionBarriers = {};
        transitionBarriers.textureNum = 1;
        transitionBarriers.textures = &textureTransitionBarrierDesc;
        NRI.CmdPipelineBarrier(*frame_commandBuffer_end, &transitionBarriers, nullptr, nri::BarrierDependency::ALL_STAGES);

        NRI.EndCommandBuffer(*frame_commandBuffer_end);
    }

    //
    nri::WorkSubmissionDesc workSubmissionDesc = {};
    workSubmissionDesc.commandBufferNum     = 1;
    workSubmissionDesc.commandBuffers       = &frame_commandBuffer_end;
    workSubmissionDesc.wait                 = &backbuffer_frame_semaphore;
    workSubmissionDesc.waitNum              = 1;
    workSubmissionDesc.signal               = &backbuffer_release_semaphore;
    workSubmissionDesc.signalNum            = 1;

    NRI.SubmitQueueWork(*commandQueue, workSubmissionDesc, frame_deviceSemaphore);

    //
    NRI.SwapChainPresent(*swapchain, *backbuffer_release_semaphore);
}

bool Swapchain_Create() {

    SDL_Window* window = (SDL_Window*)nfr::app::GetWindowHandle();
    if (!window) return false;

    int window_w, window_h;
    SDL_GetWindowSizeInPixels(window, &window_w, &window_h);
    if (window_w <= 0) window_w = 1;
    if (window_h <= 0) window_h = 1;

    SDL_SysWMinfo wmInfo;
    SDL_GetWindowWMInfo(window, &wmInfo, SDL_SYSWM_CURRENT_VERSION);

    backbuffer_width    = window_w;
    backbuffer_height   = window_h;

    nri::SwapChainDesc swapChainDesc = {};

#if defined(NRI_ON_ANDROID)
#error "not support"
#elif defined(NRI_ON_X11)
    swapChainDesc.windowSystemType = nri::WindowSystemType::X11;
    swapChainDesc.x11.dpy = wmInfo.info.x11.display;
    swapChainDesc.x11.window = wmInfo.info.x11.window;
#elif defined(NRI_ON_WAYLAND)
    swapChainDesc.windowSystemType = nri::WindowSystemType::WAYLAND;
#error "todo"
#elif defined(NRI_ON_WINDOWS)
    swapChainDesc.windowSystemType = nri::WindowSystemType::WINDOWS;
    swapChainDesc.window.windows.hwnd = wmInfo.info.win.window;
#elif defined(NRI_ON_APPLE)
    swapChainDesc.windowSystemType = nri::WindowSystemType::METAL;
    swapChainDesc.window.metal.caMetalLayer = Swapchain_Metal_Create(window);
#endif

    swapChainDesc.commandQueue = commandQueue;
    swapChainDesc.format = nri::SwapChainFormat::BT709_G22_8BIT; // srgb8
    swapChainDesc.verticalSyncInterval = 1; // no vcync
    swapChainDesc.width = backbuffer_width;
    swapChainDesc.height = window_h;
    swapChainDesc.textureNum = 2;
    auto result = NRI.CreateSwapChain(*device, swapChainDesc, swapchain);
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::CreateSwapChain failed");
        return false;
    }

    {
        nri::Format swapChainFormat;
        uint32_t swapChainTextureNum = 0;
        nri::Texture* const* swapChainTextures = NRI.GetSwapChainTextures(*swapchain, swapChainTextureNum, swapChainFormat);

        swapchain_Format = swapChainFormat;

        swapchain_backbuffers.resize(swapChainTextureNum);

        nri::ClearValueDesc clearColor = {};
        nri::FrameBufferDesc frameBufferDesc = {};
        frameBufferDesc.colorAttachmentNum = 1;
        frameBufferDesc.colorClearValues = &clearColor;

        for (uint32_t i = 0; i < swapChainTextureNum; i++) {
            BackBuffer& backBuffer = swapchain_backbuffers[i];

            backBuffer.texture = swapChainTextures[i];

            nri::Texture2DViewDesc textureViewDesc = { backBuffer.texture, nri::Texture2DViewType::COLOR_ATTACHMENT, swapChainFormat };
            auto result = NRI.CreateTexture2DView(textureViewDesc, backBuffer.texture_view);
            if (result != nri::Result::SUCCESS) {
                nfr::dbg::Error("nri::CreateSwapChain CreateTexture2DView failed");
                return false;
            }

            frameBufferDesc.colorAttachments = &backBuffer.texture_view;

            result = NRI.CreateFrameBuffer(*device, frameBufferDesc, backBuffer.framebuffer);
            if (result != nri::Result::SUCCESS) {
                nfr::dbg::Error("nri::CreateSwapChain CreateFrameBuffer failed");
                return false;
            }
        }
    }

    result = NRI.CreateQueueSemaphore(*device, backbuffer_acquire_semaphore);
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::CreateSwapChain CreateQueueSemaphore failed");
        return false;
    }

    result = NRI.CreateQueueSemaphore(*device, backbuffer_release_semaphore);
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::CreateSwapChain CreateQueueSemaphore failed");
        return false;
    }

    result = NRI.CreateQueueSemaphore(*device, backbuffer_frame_semaphore);
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::CreateSwapChain CreateQueueSemaphore failed");
        return false;
    }

    //
    {
        auto result = NRI.CreateDeviceSemaphore(*device, true, frame_deviceSemaphore);
        if (result != nri::Result::SUCCESS) {
            nfr::dbg::Error("nri::CreateSwapChain CreateDeviceSemaphore failed");
            return false;
        }
    }
    {
        auto result = NRI.CreateCommandAllocator(*commandQueue, nri::WHOLE_DEVICE_GROUP, frame_commandAllocator);
        if (result != nri::Result::SUCCESS) {
            nfr::dbg::Error("nri::CreateSwapChain CreateCommandAllocator failed");
        }
    }
    {
        auto result =  NRI.CreateCommandBuffer(*frame_commandAllocator, frame_commandBuffer_begin);
        if (result != nri::Result::SUCCESS) {
            nfr::dbg::Error("nri::CreateSwapChain CreateCommandBuffer failed");
            return false;
        }
    }
    {
        auto result =  NRI.CreateCommandBuffer(*frame_commandAllocator, frame_commandBuffer_end);
        if (result != nri::Result::SUCCESS) {
            nfr::dbg::Error("nri::CreateSwapChain CreateCommandBuffer failed");
            return false;
        }
    }
    
    nfr::dbg::Log("nri::swapchain created");


    // imgui recreate
    if (app::ImGui_GetContext() && !nfr::render::imgui::imgui_Render_Init(app::ImGui_GetContext())) {
        Destroy();
        return false;
    }

    return true;
}

void Swapchain_Destroy()
{
    if (NRI.WaitForIdle == nullptr) {
        return;
    }
    
    NRI.WaitForIdle(*commandQueue);

    if (app::ImGui_GetContext()) {
        nfr::render::imgui::imgui_Render_Free(app::ImGui_GetContext());
    }

    NRI.WaitForIdle(*commandQueue);

    NRI.DestroyCommandBuffer(*frame_commandBuffer_end);
    NRI.DestroyCommandBuffer(*frame_commandBuffer_begin);
    NRI.DestroyCommandAllocator(*frame_commandAllocator);
    NRI.DestroyDeviceSemaphore(*frame_deviceSemaphore);
    
    for (uint32_t i = 0; i < swapchain_backbuffers.size(); i++) {
        NRI.DestroyDescriptor(*swapchain_backbuffers[i].texture_view);
        NRI.DestroyFrameBuffer(*swapchain_backbuffers[i].framebuffer);
    }

    if (swapchain) { NRI.DestroySwapChain(*swapchain); swapchain = nullptr; }
    
#if defined(NRI_ON_APPLE)
    Swapchain_Metal_Destroy();
#endif
}


bool Device_Create() {
    // init device
    nri::DeviceCreationDesc deviceCreationDesc = {};
#ifdef NRI_USE_D3D12
    deviceCreationDesc.graphicsAPI = nri::GraphicsAPI::D3D12;
#elif NRI_USE_VULKAN
    deviceCreationDesc.graphicsAPI = nri::GraphicsAPI::VULKAN;
#else
#error "not support"
#endif
    deviceCreationDesc.enableAPIValidation = true; // todo
    deviceCreationDesc.enableNRIValidation = true; // todo
    deviceCreationDesc.D3D11CommandBufferEmulation = false; // todo
    deviceCreationDesc.spirvBindingOffsets = { 10, 20, 30, 40 };
    deviceCreationDesc.physicalDeviceGroup = physicaldevice;
    deviceCreationDesc.memoryAllocatorInterface = memory_allocator.memalloci;

    auto result = nri::CreateDevice(deviceCreationDesc, device);
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::CreateDevice failed");
        return false;
    }

    // NRI
    result = nri::GetInterface(*device, NRI_INTERFACE(nri::CoreInterface), (nri::CoreInterface*)&NRI) ;
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::GetInterface CoreInterface");
        return false;
    }
    result = nri::GetInterface(*device, NRI_INTERFACE(nri::SwapChainInterface), (nri::SwapChainInterface*)&NRI) ;
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::GetInterface SwapChainInterface");
        return false;
    }
    result = nri::GetInterface(*device, NRI_INTERFACE(nri::HelperInterface), (nri::HelperInterface*)&NRI) ;
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::GetInterface HelperInterface");
        return false;
    }

    // queue
    result = NRI.GetCommandQueue(*device, nri::CommandQueueType::GRAPHICS, commandQueue);
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::GetCommandQueue GRAPHICS");
        return false;
    }
    
#ifndef __APPLE__
    result = NRI.GetCommandQueue(*device, nri::CommandQueueType::COPY, commandQueueCopy);
    if (result != nri::Result::SUCCESS) {
        nfr::dbg::Error("nri::GetCommandQueue COPY");
        return false;
    }
#endif

    nfr::dbg::Log("nri::device created");

    return true;
}

void Device_Destroy() {
    if (commandQueueCopy) {
        NRI.WaitForIdle(*commandQueueCopy);
        commandQueueCopy = nullptr;
    }
    if (commandQueue) {
        NRI.WaitForIdle(*commandQueue);
        commandQueue = nullptr;
    }
    if (device) { 
        nri::DestroyDevice(*device); 
        device = nullptr; 
    }
}

bool EnumerateDevices_Create() {
#if NRI_USE_VULKAN
#ifndef NRI_ON_WINDOWS
    uint32_t deviceGroupNum = 0;
    nri::Result result = nri::GetPhysicalDevices(nullptr, deviceGroupNum);
    if (deviceGroupNum == 0 || result != nri::Result::SUCCESS) {
        nfr::dbg::Error("GetPhysicalDevices failed");
        return false;
    }

    physicaldevices.resize(deviceGroupNum);

    if (nri::GetPhysicalDevices(physicaldevices.data(), deviceGroupNum) != nri::Result::SUCCESS) {
        nfr::dbg::Error("GetPhysicalDevices 2 failed");
        return false;
    }

    nfr::dbg::Log("enumerate rni devices:");
    for (auto i = 0; i < physicaldevices.size(); i++) {
        nfr::dbg::Log("\tdevice {} {}", i, physicaldevices[i].description);
    }
#else
    {
        physicaldevices.resize(0);

        ComPtr<IDXGIFactory4> DXGIFactory;
        HRESULT result = CreateDXGIFactory(IID_PPV_ARGS(&DXGIFactory));
        if (FAILED(result)) return false;

        IDXGIAdapter1* adapter_ = nullptr;
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != DXGIFactory->EnumAdapters1(adapterIndex, &adapter_); ++adapterIndex)  {
            ComPtr<IDXGIAdapter1> adapter(adapter_);
            adapter_->Release();
            adapter_ = nullptr;

            DXGI_ADAPTER_DESC1 desc;
            result = adapter->GetDesc1(&desc);
            if (FAILED(result)) continue;
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)  continue;

            nri::PhysicalDeviceGroup temp ={};
            size_t len = wcslen(desc.Description) + 1;
            ConvertWcharToChar(desc.Description, temp.description, len);

            if (desc.VendorId == 0x00008086u) { // Intel Vendor ID:     0x00008086
                temp.vendor = nri::Vendor::INTEL; 
                temp.type   = nri::PhysicalDeviceType::INTEGRATED;
            } else if (desc.VendorId == 0x00001414u) { // Microsoft Vendor ID: 0x00001414, L"Microsoft Basic Render Driver"
                temp.vendor = nri::Vendor::UNKNOWN;
                temp.type   = nri::PhysicalDeviceType::INTEGRATED;
            } else if (desc.VendorId == 0x00001002) { // AMD Vendor ID:       0x00001002
                temp.vendor = nri::Vendor::AMD;
                temp.type   = nri::PhysicalDeviceType::DISCRETE;
            } else if (desc.VendorId == 0x000010de) { // NVIDIA Vendor ID:    0x000010de
                temp.vendor = nri::Vendor::NVIDIA;
                temp.type   = nri::PhysicalDeviceType::DISCRETE;
            } else { // wtf  todo 
                temp.vendor = nri::Vendor::UNKNOWN;
                temp.type   = nri::PhysicalDeviceType::UNKNOWN;
            }
            temp.deviceID                   = desc.DeviceId;
            temp.luid                       = *(uint64_t*)&desc.AdapterLuid;
            temp.dedicatedVideoMemoryMB     = desc.DedicatedVideoMemory / 1024 / 1024;
            ///temp.physicalDeviceGroupSize    = desc.SharedSystemMemory / 1024 / 1024;
            temp.physicalDeviceGroupSize    = 1;
            temp.displays                   = nullptr; // todo 
            temp.displayNum                 = 0; // todo 

            physicaldevices.push_back(temp);   
        }
    }

    nfr::dbg::Log("enumerate rni devices:");
    for (auto i = 0; i < physicaldevices.size(); i++) {
        nfr::dbg::Log("\tdevice {} {}", i, physicaldevices[i].description);
    }
#endif
#elif NRI_USE_D3D12
#ifdef NRI_ON_WINDOWS
    {
        physicaldevices.resize(0);
        
        ComPtr<IDXGIFactory4> DXGIFactory;
        HRESULT result = CreateDXGIFactory(IID_PPV_ARGS(&DXGIFactory));
        if (FAILED(result)) return false;

        IDXGIAdapter1* adapter_ = nullptr;
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != DXGIFactory->EnumAdapters1(adapterIndex, &adapter_); ++adapterIndex)  {
            ComPtr<IDXGIAdapter1> adapter(adapter_);
            adapter_->Release();
            adapter_ = nullptr;
            
            DXGI_ADAPTER_DESC1 desc;
            result = adapter->GetDesc1(&desc);
            if (FAILED(result)) continue;
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)  continue;
           
            nri::PhysicalDeviceGroup temp ={};
            size_t len = wcslen(desc.Description) + 1;
            ConvertWcharToChar(desc.Description, temp.description, len);

            if (desc.VendorId == 0x00008086u) { // Intel Vendor ID:     0x00008086
                temp.vendor = nri::Vendor::INTEL; 
                temp.type   = nri::PhysicalDeviceType::INTEGRATED;
            } else if (desc.VendorId == 0x00001414u) { // Microsoft Vendor ID: 0x00001414, L"Microsoft Basic Render Driver"
                temp.vendor = nri::Vendor::UNKNOWN;
                temp.type   = nri::PhysicalDeviceType::INTEGRATED;
            } else if (desc.VendorId == 0x00001002) { // AMD Vendor ID:       0x00001002
                temp.vendor = nri::Vendor::AMD;
                temp.type   = nri::PhysicalDeviceType::DISCRETE;
            } else if (desc.VendorId == 0x000010de) { // NVIDIA Vendor ID:    0x000010de
                temp.vendor = nri::Vendor::NVIDIA;
                temp.type   = nri::PhysicalDeviceType::DISCRETE;
            } else { // wtf  todo 
                temp.vendor = nri::Vendor::UNKNOWN;
                temp.type   = nri::PhysicalDeviceType::UNKNOWN;
            }

            temp.physicalDeviceGroupSize    = 1;
            temp.deviceID                   = desc.DeviceId;
            temp.luid                       = *(uint64_t*)&desc.AdapterLuid;
            temp.dedicatedVideoMemoryMB     = desc.DedicatedVideoMemory / 1024 / 1024;
            temp.physicalDeviceGroupSize    = desc.SharedSystemMemory / 1024 / 1024;
            temp.displays                   = nullptr; // todo 
            temp.displayNum                 = 0; // todo 

            physicaldevices.push_back(temp);   
        }
    }

    nfr::dbg::Log("enumerate rni devices:");
    for (auto i = 0; i < physicaldevices.size(); i++) {
        nfr::dbg::Log("\tdevice {} {}", i, physicaldevices[i].description);
    }
#else
    // todo fix!
    physicaldevices.resize(1);
    physicaldevices[0] = { 0 };
#endif
#endif
    
    return true;
}

bool EnumerateDevices_Select() {
#if NRI_USE_VULKAN || (NRI_USE_D3D12 && NRI_ON_WINDOWS)
    // select best
    physicaldevice_SelectNum = 0;
    for (; physicaldevice_SelectNum < physicaldevices.size(); physicaldevice_SelectNum++) {
        if (physicaldevices[physicaldevice_SelectNum].type != nri::PhysicalDeviceType::INTEGRATED) break;
    }
    if (physicaldevice_SelectNum == physicaldevices.size()) physicaldevice_SelectNum = 0; // or select def

    nfr::dbg::Log("select rni device: {} '{}'", physicaldevice_SelectNum, physicaldevices[physicaldevice_SelectNum].description);

    physicaldevice = &physicaldevices[physicaldevice_SelectNum];
#elif NRI_USE_D3D12
    nfr::dbg::Log("select rni device default");
    physicaldevice_SelectNum    = 0;
    physicaldevice              = nullptr;
#endif
    return true;
}

void EnumerateDevices_Destroy() {
    // nop
}



}
