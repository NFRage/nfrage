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
#ifdef _WIN32
#ifdef DLL_PLATFORM
#ifdef NFRAGE_CORE_EXPORTS
#define NFRAGE_CORE_API __declspec(dllexport)
#else
#define NFRAGE_CORE_API __declspec(dllimport)
#endif
#else
#define NFRAGE_CORE_API
#endif
#else
#ifdef LIB_EXPORTS
#define NFRAGE_CORE_API __attribute__((__visibility__("default")))
#else
#define NFRAGE_CORE_API 
#endif
#endif

#include <nfrage_api.h>

#include <visit_struct/visit_struct.hpp>
#include <fu2/function2.hpp>
#include <spdlog/spdlog.h>

#include "tl.h"
#include "fs.h"

namespace nfr::core
{

NFRAGE_CORE_API bool Initialize();
NFRAGE_CORE_API void Destroy();
NFRAGE_CORE_API spdlog::logger* GetLogger();

}

namespace nfr::dbg
{

template<typename... Args>
inline void Log(const api::string_view& fmt, Args&&... args)
{
	core::GetLogger()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void Error(const api::string_view& fmt, Args&&... args)
{
	core::GetLogger()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void Warning(const api::string_view& fmt, Args&&... args)
{
	core::GetLogger()->warn(fmt, std::forward<Args>(args)...);
}

}