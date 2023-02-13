if (NFRAGE_SHARED)
    set(SPDLOG_BUILD_SHARED ON)
endif()

set(OPTICK_INSTALL_TARGETS OFF)

add_subdirectory("optick")
add_subdirectory("spdlog")
add_subdirectory("entt")
add_subdirectory("marl")
add_subdirectory("SDL")

file(GLOB_RECURSE BULLET_COLLISION_SRC
    "bullet3/src/BulletCollision/*.cpp"
    "bullet3/src/BulletCollision/*.h"
)

file(GLOB_RECURSE BULLET_DYNAMICS_SRC
    "bullet3/src/BulletDynamics/*.cpp"
    "bullet3/src/BulletDynamics/*.h"
)

file(GLOB_RECURSE BULLET_LINEARMATH_SRC
    "bullet3/src/LinearMath/*.cpp"
    "bullet3/src/LinearMath/*.h"
)

add_library(bullet STATIC 
    ${BULLET_LINEARMATH_SRC}
    ${BULLET_COLLISION_SRC}
    ${BULLET_DYNAMICS_SRC}
)

target_include_directories(bullet PUBLIC "bullet3/src")
add_library(Bullet3::bullet ALIAS "bullet")

set(LUA_SRC 
    lua/lapi.c
    lua/lcode.c 
    lua/lctype.c 
    lua/ldebug.c 
    lua/ldo.c 
    lua/ldump.c 
    lua/lfunc.c 
    lua/lgc.c 
    lua/llex.c 
    lua/lmem.c 
    lua/lobject.c 
    lua/lopcodes.c 
    lua/lparser.c 
    lua/lstate.c 
    lua/lstring.c 
    lua/ltable.c 
    lua/ltm.c 
    lua/lundump.c 
    lua/lvm.c 
    lua/lzio.c 
    lua/ltests.c
    lua/lauxlib.c
    lua/lbaselib.c 
    lua/ldblib.c 
    lua/liolib.c 
    lua/lmathlib.c 
    lua/loslib.c 
    lua/ltablib.c 
    lua/lstrlib.c
    lua/lutf8lib.c 
    lua/loadlib.c 
    lua/lcorolib.c 
    lua/linit.c
)

add_library("lua" STATIC ${LUA_SRC})
target_include_directories("lua" PUBLIC "lua/")

add_library(LUA::Lua ALIAS "lua")

file(GLOB IMGUI_SRC
   "imgui/imgui.h"
   "imgui/imconfig.h"
   "imgui/imgui_internal.h"
   "imgui/imgui.cpp"
   "imgui/imgui_draw.cpp"
   "imgui/imgui_tables.cpp"
   "imgui/imgui_widgets.cpp"
   "imgui/imgui_rectpack.cpp"
   "imgui/imgui_textedit.cpp"
   "imgui/imgui_truetype.cpp"
)


add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC "imgui/")
target_include_directories(imgui PUBLIC "imgui/backends")

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include_directories("rapidjson/include")

set_target_properties("OptickCore" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
set_target_properties("lua" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
set_target_properties("bullet" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
set_target_properties("spdlog" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
set_target_properties("marl" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
set_target_properties("imgui" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})

set_target_properties("SDL3" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
set_target_properties("SDL3_test" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
set_target_properties("SDL3-static" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})

set_target_properties("uninstall" PROPERTIES ${NFRAGE_FOLDER_PREGEN})

if (NFRAGE_TOOLS)
    # LGPL-licensed library (dynamic linking only if you want to use this library with non-GPL license!)
    file(GLOB MSPACK_SRC_FILES
        mspack/mspack/mspack.h 
        mspack/mspack/system.h 
        mspack/mspack/system.c 
        mspack/mspack/cab.h 
        mspack/mspack/cabc.c 
        mspack/mspack/cabd.c 
        mspack/mspack/chm.h 
        mspack/mspack/chmc.c 
        mspack/mspack/chmd.c 
        mspack/mspack/hlp.h 
        mspack/mspack/hlpc.c 
        mspack/mspack/hlpd.c 
        mspack/mspack/lit.h 
        mspack/mspack/litc.c 
        mspack/mspack/litd.c 
        mspack/mspack/kwaj.h 
        mspack/mspack/kwajc.c 
        mspack/mspack/kwajd.c 
        mspack/mspack/szdd.h 
        mspack/mspack/szddc.c 
        mspack/mspack/szddd.c 
        mspack/mspack/oab.h 
        mspack/mspack/oabc.c 
        mspack/mspack/oabd.c 
        mspack/mspack/lzx.h 
        mspack/mspack/lzxc.c 
        mspack/mspack/lzxd.c 
        mspack/mspack/mszip.h 
        mspack/mspack/mszipc.c 
        mspack/mspack/mszipd.c 
        mspack/mspack/qtm.h 
        mspack/mspack/qtmd.c 
        mspack/mspack/readbits.h 
        mspack/mspack/readhuff.h 
        mspack/mspack/lzss.h 
        mspack/mspack/lzssd.c 
        mspack/mspack/des.h 
        mspack/mspack/sha.h 
        mspack/mspack/crc32.c 
        mspack/mspack/crc32.h
    )

    add_library("mspack" SHARED ${MSPACK_SRC_FILES})
    target_include_directories("mspack" PRIVATE "mspack/mspack")
    target_include_directories("mspack" PUBLIC "mspack")
    set_target_properties("mspack" PROPERTIES ${NFRAGE_FOLDER_THIRD_PARTY})
endif()