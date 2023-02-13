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
#ifdef _WIN32
#ifdef DLL_PLATFORM
#ifdef NFRAGE_RENDER_EXPORTS
#define NFRAGE_RENDER_API __declspec(dllexport)
#else
#define NFRAGE_RENDER_API __declspec(dllimport)
#endif
#else
#define NFRAGE_RENDER_API
#endif
#else
#ifdef LIB_EXPORTS
#define NFRAGE_RENDER_API __attribute__((__visibility__("default")))
#else
#define NFRAGE_RENDER_API 
#endif
#endif


#include "Core.h"

#include "SDL3/SDL.h"

#if defined(__ANDROID__)
    #define SDL_ENABLE_SYSWM_ANDROID 1
    
    #define NRI_ON_ANDROID 1

    #ifndef NRI_USE_VULKAN
        #error "not support api on android"
    #endif

#elif defined(__LINUX__)
    #define SDL_ENABLE_SYSWM_X11 1

    #define NRI_ON_X11 1

    #ifndef NRI_USE_VULKAN
        #error "not support api on linux"
    #endif
    
#elif defined(__WINDOWS__)
    #define SDL_ENABLE_SYSWM_WINDOWS 1
    
    #include <windows.h>
    #include <windowsx.h>
    
    #define NRI_ON_WINDOWS 1

    #ifndef NRI_USE_D3D12
        //#error "not support api on windows"
    #endif

#elif defined(__APPLE__)
    #define SDL_ENABLE_SYSWM_COCOA 1
    #define NRI_ON_APPLE 1
#endif

struct NSWindow;
#include "SDL3/SDL_syswm.h"

// include nri
#include "NRI.h"
#include "NRIDescs.h"
#include "NRIDescs.hpp"
#include "Extensions/NRIDeviceCreation.h"
#include "Extensions/NRIHelper.h"
#include "Extensions/NRIMeshShader.h"
#include "Extensions/NRIRayTracing.h"
#include "Extensions/NRISwapChain.h"
#include "Extensions/NRIWrapperD3D11.h"
#include "Extensions/NRIWrapperD3D12.h"
#include "Extensions/NRIWrapperVK.h"

// include imgui
#include "imgui.h"
#define   IMGUI_DEFINE_MATH_OPERATORS
#include  "imgui_internal.h"

#include "Render_ImGui.h" // public imgui

namespace nfr::render
{

bool Initialize();
void Destroy();

void RenderResize(uint32_t width, uint32_t height);

bool FrameBegin();
void FrameEnd();

void FrameImgui();

}
