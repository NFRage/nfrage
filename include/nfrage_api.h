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
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>

namespace nfr::api
{

using path = std::filesystem::path;

class IInterface
{
public:
    virtual ~IInterface() {}

	virtual long addRef() = 0;
	virtual long release() = 0;
};

enum class EStreamMode : std::uint32_t
{
    Set,
    Cur,
    End
};

enum EStreamFlags : std::uint32_t
{
    ReadFlag = 1 << 0,
    WriteFlag = 1 << 1,
    AppendFlag = 1 << 2
};

class IStream : public IInterface
{
public:
    virtual bool getLine(std::string& line) = 0;

    virtual std::uint64_t read(void* outData, std::uint64_t dataSize) = 0;
    virtual std::uint64_t write(void* inData, std::uint64_t dataSize) = 0;
    virtual std::uint64_t seek(EStreamMode mode, std::uint64_t position) = 0;
    virtual std::uint64_t getPosition() = 0;    // probable non-const because this one can call io operations
    
    virtual std::uint64_t getSize() const = 0;

    virtual bool isEndOfFile() const = 0;
    virtual bool isOpen() const = 0;
    virtual bool isReal() const = 0;    // means "real" file on disk or just memory representation
    virtual bool isReadOnly() const = 0;
};

enum class EResourceCategory : std::uint32_t
{
    None,
    
    Game_Bundle,                // Game-specific bundle binary
    Game_Resource,              // Typical abstract resource. Used only for internal purposes.
    Game_Reference,             // Reference to any game resource in this (or other) file
    
    Mesh,                       // Mesh with bones
    Mesh_Fragment,              // Mesh without bones, can be a part of "Mesh" (can be a ref to parent mesh)
    Skeleton,                   // Skeleton description for mesh (must be a link to parent mesh)
    Skeleton_Animation,         // Animation for skeleton (must be a ling to parent skeleton)
    
    Collision_Profile,
    Collision_OBB,              // Can be convex mesh
    
    Scene,
    Scene_Config,
    Scene_Fragment,
    Scene_ECS_Prefab,
    Scene_ECS_Entity,
    Scene_ECS_Component,
    Scene_ECS_Attachment,

    Texture,                    // Abstract texture resource (can be a link to parent UI widget or screen)
    Texture_Animation,
    Texture_Palette,
    
    UI_Screen,                  // UI game screen (can be menu or HUD)
    UI_Widget,                  // UI screen element (must be linked to parent screen)
    UI_Font,                    // UI game font (must be converted to universal engine atlas)
    UI_Action,                  // UI action which can be addressed to game or engine event
    
    Video,
    
    Script,
    Event,
    
    Shader,                     // Shader source file. Must be converted to engine shaders source file or it'll be ignored.
    Shader_Compiled,            // Shader binary blob. Can't be used if this is not shader compiled by engine.
    
    Render_Pass,
    Render_Config,
    
    PhysicsBody,
    
    Car_Physics,
    Car_Part,
    Car_Preset,
    
    Unknown = 0xFFFFFFFF
};

/*
    Some little explanation about this class:
 
    "isReal()" - resource is "real" file on disk or just entry inside game chunk
    "isCompressed()" - resource compressed by general purpose compression (such as Deflate or LZ77)
    "isOptimized()" - resource can be used directly in engine and can be used for instance creation
    "isConvertableToOptimized()" - resource can be converted to engine optimized format
 
    "getName()" - get virtual or real name of resource
    "getCategory()" - get category of resource
    "getSize()" - get raw data size
    "getData()" - get raw data pointer
*/
class IResource : public IInterface
{
public:
    virtual bool isReal() const = 0;
    virtual bool isCompressed() const = 0;
    virtual bool isOptimized() const = 0;
    virtual bool isConvertableToOptimized() const = 0;
    
    virtual const std::string_view& getName() const = 0;
    virtual EResourceCategory getCategory() const = 0;
    virtual std::uint64_t getSize() const = 0;
    virtual void* getData() const = 0;
};


class IMeshFragment : public IInterface
{
public:

};

class IDirectoryObserver : public IInterface
{
public:
    
};

class IResourceConverter : public IInterface
{
public:
    
};

class IPluginInstance : public IInterface
{
public:
    virtual bool isCompatible() const = 0;
    virtual bool isRunnable() const = 0;

    virtual bool initialize() = 0;
    virtual void destroy() = 0;

    virtual bool tick(float dt) = 0;
};

class IRenderPluginInstance : public IPluginInstance
{
public:
};

class IGamePluginInstance : public IPluginInstance
{
public:
};

class IEngineFactory
{
public:
    virtual ~IEngineFactory() {}

    virtual std::uint32_t getEngineVersion() = 0;
    virtual void* getGameLogger() = 0;

    virtual api::IStream* openFile(std::uint32_t flags, const api::path& filePath) = 0;

    virtual const api::path& getWorkingDirectory() = 0;
    virtual const api::path& getMainDirectory() = 0;
    virtual const api::path& getResourcesDirectory() = 0;
    virtual const api::path& getGameDirectory() = 0;
    virtual const api::path& getTempDirectory() = 0;

    virtual bool exists(const api::path& filePath) = 0;
};

enum class EFactoryType : std::uint32_t
{
    Unknown,
    GameFactory,
    RenderFactory
};

class IPluginFactory
{
public:
    virtual EFactoryType getType() const = 0;
    virtual std::uint32_t getId() const = 0;
    virtual std::uint32_t getVersion() const = 0;

    virtual IPluginInstance* createInstance(void* options) = 0;

};

using CreateFactoryProc = IPluginFactory*(IEngineFactory*);

template<typename T>
class SafeInterface
{
private:
    T* ptr = nullptr;

public:
    SafeInterface(SafeInterface&& other)
    {
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    SafeInterface(T* inPtr) : ptr(inPtr) {}

    void reset()
    {
        if (ptr != nullptr) {
            ptr->release();
            ptr = nullptr;
        }
    }

    ~SafeInterface()
    {
        reset();
    }

    void set(T* newPtr)
    {
        ptr = newPtr;
    }

    bool empty() const
    {
        return ptr == nullptr;
    }

    T* get() const
    {
        return ptr;
    }

    T* operator->() const
    {
        return ptr;
    }
};

using string = std::string;
using string_view = std::string_view;

inline
std::uint32_t
getBinaryUpperHash(const char* text)
{
    std::uint32_t hash = std::uint32_t(-1);
    while (*text) {
        hash *= 0x21;
        hash += std::toupper(*text);
        text++;
    }

    return hash;
}

inline
std::uint32_t
getBinaryHash(const char* text)
{
    std::uint32_t hash = std::uint32_t(-1);
    while (*text) {
        hash *= 0x21;
        hash += *text;
        text++;
    }

    return hash;
}

inline
std::uint32_t
getChunkId(const char* chunkName)
{
    std::int32_t chunkId = -1;
    while (*chunkName) {
        chunkId *= 0x21;
        chunkId += *chunkName;
        chunkName++;
    }

    return std::uint32_t(chunkId & 0x7FFFFFFF);
}

struct binary_hasher
{
    std::uint32_t operator()(const std::uint32_t& value) const
    {
        // already hashed
        return value;
    }
};

template<typename T>
using binary_hash_map = std::unordered_map<std::uint32_t, T, binary_hasher, std::equal_to<std::uint32_t>>;

using binary_hash_set = std::unordered_set<std::uint32_t, binary_hasher, std::equal_to<std::uint32_t>>;

}
 