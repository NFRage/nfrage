#*********************************************************************
# Copyright (C) Anton Kovalev (vertver), 2022. All rights reserved.
# nfrage - engine code for NFRage project
#*********************************************************************
# LGPL v2.1 License
#****************************************************************/
cmake_minimum_required(VERSION 3.16)
project(nfrage C CXX)

if (APPLE)
    enable_language(OBJC)
    set(CMAKE_XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE Metal) 
endif()

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# fix version c++
if (MSVC)
	add_compile_options(/Zc:__cplusplus)
endif()

set(NFRAGE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(NFRAGE_ENG_NAME "Engine")
set(NFRAGE_CORE_NAME "Core")
set(NFRAGE_AUDIO_NAME "Audio")
set(NFRAGE_FRONTEND_NAME "Frontend")
set(NFRAGE_MISC_NAME "Misc")
set(NFRAGE_RENDER_NAME "Render")
set(NFRAGE_WORLD_NAME "World")
set(NFRAGE_PHYSICS_NAME "Physics")
set(NFRAGE_UI_NAME "UI")
set(NFRAGE_RESOURCES_NAME "Resources")
set(NFRAGE_APPLICATION_NAME "AppLayer")
set(NFRAGE_PLUGINS_NAME "PluginsLayer")
set(NFRAGE_ENTRYPOINT_NAME "nfrage")
set(NFRAGE_FOLDER_TESTS FOLDER "Tests")
set(NFRAGE_FOLDER_THIRD_PARTY FOLDER "3rd-party")
set(NFRAGE_FOLDER_PREGEN FOLDER "Pre-gen shit")

option(NFRAGE_SHIPPING "Shipping build of NFRAGE" OFF)
option(NFRAGE_SHARED "Build all modules with dynamic linking" OFF)
option(NFRAGE_TOOLS "Shipping engine with extented support for " OFF)

add_compile_definitions("FMT_HEADER_ONLY")

if (NFRAGE_SHIPPING)
    add_compile_definitions("NFRAGE_SHIPPING")
endif()

if (NFRAGE_SHARED)
    add_compile_definitions("NFRAGE_SHARED")
endif()

if (NFRAGE_TOOLS)
    add_compile_definitions("NFRAGE_TOOLS")
endif()

if (NFRAGE_SHARED)
    set(NFRAGE_LINKING_MODE SHARED)
else()
    set(NFRAGE_LINKING_MODE STATIC)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bins/lib/$<0:>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bins/lib/$<0:>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bins/bin/$<0:>") 

include_directories("submodules")
include_directories("include")

add_subdirectory("submodules")

if (UNIX)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
endif()

add_subdirectory("src")
add_subdirectory("plugins")