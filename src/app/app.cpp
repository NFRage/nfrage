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
#include "app_pch.h"

bool bAppInited = false;

#if 0
namespace nfr::app
{

void (*RenderResize)(uint32_t width, uint32_t height) = nullptr;

bool Initialize()
{
	dbg::Log("Initializing \"Application Layer\" module...");

	if (!window::Initialize()) {
		return false;
	}

	if (!nfr::app::imgui::Initialize()) {
		return false;
	}

    bAppInited = true;
	return true;
}

void Destroy()
{
	dbg::Log("Destroying \"Application Layer\" module...");

    if (bAppInited) {
        nfr::app::imgui::Destroy();
        window::Destroy();
    }
}

void* GetWindowHandle()
{
	return window::GetHandle();
}

bool ExecuteWindowEvents() 
{
	return window::ExecuteWindowEvents();
}



void ImGui_ActivateInput(bool isEnable) {
	nfr::app::imgui::ActivateInput(isEnable);
}

void ImGui_ActivateFrame(bool isEnable) {
	nfr::app::imgui::ActivateFrame(isEnable);
}

bool ImGui_Frame_Begin() {
	return nfr::app::imgui::Frame_Begin();
}

void ImGui_Frame_End() {
	nfr::app::imgui::Frame_End();
}

ImGuiContext* ImGui_GetContext() {
	return nfr::app::imgui::Get_Context();
}

}
#else

namespace nfr::app
{

std::unordered_map<std::uint32_t, api::IPluginFactory*> Factories;

api::SafeInterface<api::IRenderPluginInstance> RenderPluginInstance = nullptr;
api::SafeInterface<api::IGamePluginInstance> GamePluginInstance = nullptr;

class EngineFactory : public api::IEngineFactory
{
public:
	std::uint32_t getEngineVersion() override
	{
		return 0;
	}

	void* getGameLogger() override
	{
		return core::GetLogger();
	}

	api::IStream* openFile(std::uint32_t flags, const api::path& filePath) override
	{
		return fs::OpenFile(flags, filePath);
	}

	const api::path& getWorkingDirectory() override
	{
		return fs::GetWorkingDirectory();
	}

	const api::path& getMainDirectory() override
	{
		return fs::GetMainDirectory();
	}

	const api::path& getResourcesDirectory() override
	{
		return fs::GetResourcesDirectory();
	}

	const api::path& getGameDirectory() override
	{
		return fs::GetGameDirectory();
	}

	const api::path& getTempDirectory() override
	{
		return fs::GetTempDirectory();
	}

	bool exists(const api::path& filePath) override 
	{
		return fs::Exists(filePath);
	}
};

EngineFactory Factory;

using LibraryHandle = std::pair<api::CreateFactoryProc*, void*>;

NFRAGE_APPLAYER_API void Loop()
{
	while (true) {
		if (GamePluginInstance.empty()) {
			break;
		}

		if (!GamePluginInstance->tick(0.f)) {
			break;
		}
	}
}

bool Initialize(const api::path& appPath)
{
	dbg::Log("Initializing \"Application Layer\" module...");

	auto getPlatformExtension = []() 
	{
#ifdef _WIN32
		return ".dll";
#else
#error no
		return "";
#endif
	};

	auto getFactoryInterface = [](const api::path& name, const char* procName) -> LibraryHandle
	{
#ifdef _WIN32
		HMODULE library = LoadLibraryW(name.c_str());
		if (library == INVALID_HANDLE_VALUE || library == nullptr) {
			return {};
		}

		return { reinterpret_cast<api::CreateFactoryProc*>(GetProcAddress(library, procName)), library };
#else
		return  {};
#endif
	};

	auto freeLibrary = [](void* library)
	{
#ifdef _WIN32
		FreeLibrary((HMODULE)library); 
#else
#error nos
#endif
	};

	api::path pluginsPath = appPath;
	pluginsPath.append("plugins");
	if (!std::filesystem::exists(pluginsPath)) {
		std::filesystem::create_directories(pluginsPath);
	}

	for (const auto& it : std::filesystem::directory_iterator(pluginsPath)) {
		if (it.is_regular_file() && !it.path().extension().compare(getPlatformExtension())) {
			const std::string pluginName = it.path().filename().generic_string();
;			auto factoryHandle = getFactoryInterface(it.path(), "CreatePluginFactory");
			if (factoryHandle.first == nullptr) {
				freeLibrary(factoryHandle.second);
				continue;
			}

			dbg::Log("Detected \"{}\" plugin. Trying to load...", pluginName);
			api::IPluginFactory* pluginFactory = factoryHandle.first(&Factory);
			if (pluginFactory == nullptr) {
				dbg::Warning("Failed to load \"{}\". Skipping this one...", pluginName);
				freeLibrary(factoryHandle.second);
				continue;
			}

			switch (pluginFactory->getType()) {
			case api::EFactoryType::GameFactory: {
				if (!GamePluginInstance.empty()) {
					dbg::Warning("Game plugin already selected. Skipping this one...");
					freeLibrary(factoryHandle.second);
					continue;
				}

				nfr::api::IGamePluginInstance* gamePluginInstance = static_cast<nfr::api::IGamePluginInstance*>(pluginFactory->createInstance(nullptr));
				if (!gamePluginInstance->isCompatible()) {
					dbg::Warning("Plugin \"{}\" is not compatible with current version of engine...", pluginName);
					freeLibrary(factoryHandle.second);
					continue;
				}

				if (!gamePluginInstance->isRunnable()) {
					dbg::Warning("Plugin \"{}\" can't be runned because game resources is not valid or doesn't exists...", pluginName);
					freeLibrary(factoryHandle.second);
					continue;
				}

				GamePluginInstance.set(gamePluginInstance);
			}
			break;

			case api::EFactoryType::RenderFactory: {
				if (!RenderPluginInstance.empty()) {
					dbg::Warning("Render plugin already selected. Skipping this one...");
					freeLibrary(factoryHandle.second);
					continue;
				}

				nfr::api::IRenderPluginInstance* renderPluginInstance = static_cast<nfr::api::IRenderPluginInstance*>(pluginFactory->createInstance(nullptr));
				if (!renderPluginInstance->isCompatible()) {
					dbg::Warning("Plugin \"{}\" is not compatible with current version of engine...", pluginName);
					freeLibrary(factoryHandle.second);
					continue;
				}

				if (!renderPluginInstance->isRunnable()) {
					dbg::Warning("Plugin \"{}\" can't be runned because game resources is not valid or doesn't exists...", pluginName);
					freeLibrary(factoryHandle.second);
					continue;
				}

				RenderPluginInstance.set(renderPluginInstance);
			}
			break;

			default:
				break;
			}

			Factories[pluginFactory->getId()] = pluginFactory;
		}
	}

	if (GamePluginInstance.empty()) {
		dbg::Warning("No game selected to boot. Aborting...");
		return false;
	}

	if (!GamePluginInstance->initialize()) {
		dbg::Error("Can't initialize game plugin.");
		return false;
	}

	if (!RenderPluginInstance.empty()) {
		if (!RenderPluginInstance->initialize()) {
			dbg::Error("Can't initialize render plugin.");
			return false;
		}
	}

	return true;
}

void Destroy()
{
	dbg::Log("Destroying \"Application Layer\" module...");

	if (!RenderPluginInstance.empty()) {
		RenderPluginInstance->destroy();
		RenderPluginInstance.reset();
	}

	if (!GamePluginInstance.empty()) {
		GamePluginInstance->destroy();
		GamePluginInstance.reset();
	}
}

}
#endif