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

namespace nfr::render::shaders
{
    // temp code for load shader binaryes continue rewrine new code


    struct ShaderBinary {
        std::string     idname;
        nri::ShaderDesc data;
    };
    
    std::vector<ShaderBinary> shaderlist;
    
    
    void InitShaderList() {
        // todo
    }

    void FreeShaderList() {
        // todo
        for (auto& shaderinfo : shaderlist ) {
            if (shaderinfo.data.bytecode) {
                memory_allocator.memalloci.Free(nullptr, (void*)shaderinfo.data.bytecode);
                shaderinfo.data.bytecode = nullptr;
            }
        }
        shaderlist.clear();
    }

    bool FindAndLoadShaderBinary(const char* ShaderIdName, nri::ShaderDesc& Find) {
        // find cache
        for (auto& shaderinfo : shaderlist ) {
            if (shaderinfo.idname == ShaderIdName) {
                Find = shaderinfo.data;
                return true;
            }
        }


        ShaderBinary temp = {};


        // parse type by name
        std::string path = ShaderIdName;

        if (path.find("_vs") != std::string::npos) {
            temp.data.entryPointName = "VS"; // sed def
            temp.data.stage = nri::ShaderStage::VERTEX;
        } else if (path.find("_ps") != std::string::npos) {
            temp.data.entryPointName = "PS"; // sed def
            temp.data.stage = nri::ShaderStage::FRAGMENT;
        } else if (path.find("_cs") != std::string::npos) {
            temp.data.stage = nri::ShaderStage::COMPUTE;
        } else {
            return false;
        }
        
        // find and load file
#ifdef NRI_USE_D3D12

        if (device && NRI.GetDeviceDesc) {
            auto desc = NRI.GetDeviceDesc(*device);
            if (desc.isTextureFilterMinMaxSupported && desc.vendor != nri::Vendor::INTEL) { // неработает на интеле никак
                path = std::string(SDL_GetBasePath()) + "shaders/dx12/" + path;
            } else {
                path = std::string(SDL_GetBasePath()) + "shaders/dx11/" + path;
            }
        } else {
            path = std::string(SDL_GetBasePath()) + "shaders/dx12/" + path;
        }

#elif NRI_USE_VULKAN
        path = std::string(SDL_GetBasePath()) +  "shaders/vk/" + path;
#else
#error "not support"
#endif

        SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
        if (!file) return false;

        temp.idname = ShaderIdName;
        temp.data.size = SDL_RWsize(file); 
        temp.data.bytecode  = memory_allocator.memalloci.Allocate(nullptr, temp.data.size, 1);
        SDL_RWread(file, (void*)temp.data.bytecode, temp.data.size, 1);
        SDL_RWclose(file);

        shaderlist.push_back(temp);

        Find = temp.data;

        return true;
    }
}
