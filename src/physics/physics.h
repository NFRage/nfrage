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
#ifdef NFRAGE_PHYSICS_EXPORTS
#define NFRAGE_PHYSICS_API __declspec(dllexport)
#else
#define NFRAGE_PHYSICS_API __declspec(dllimport)
#endif
#else
#define NFRAGE_PHYSICS_API
#endif
#else
#ifdef LIB_EXPORTS
#define NFRAGE_PHYSICS_API __attribute__((__visibility__("default")))
#else
#define NFRAGE_PHYSICS_API 
#endif
#endif

#include "core.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

namespace nfr::physics
{

NFRAGE_PHYSICS_API bool Initialize();
NFRAGE_PHYSICS_API void Destroy();

}