file(GLOB NFRAGE_CORE_SRC_FILES CONFIGURE_DEPENDS 
	"*.cpp"
	"*.h"
)

source_group("sources" FILES ${NFRAGE_CORE_SRC_FILES})

add_library(${NFRAGE_CORE_NAME} ${NFRAGE_LINKING_MODE} ${NFRAGE_CORE_SRC_FILES})
if (NFRAGE_SHARED)
	target_compile_definitions(${NFRAGE_CORE_NAME} PUBLIC DLL_PLATFORM)
	target_compile_definitions(${NFRAGE_CORE_NAME} PRIVATE NFRAGE_CORE_EXPORTS)
endif()

target_precompile_headers(${NFRAGE_CORE_NAME} PRIVATE "core_pch.h")
target_include_directories(${NFRAGE_CORE_NAME} PUBLIC "${NFRAGE_ROOT_DIR}/src/core")

# 	"OptickCore" 
#	EnTT::EnTT
#	LUA::Lua
target_link_libraries(${NFRAGE_CORE_NAME} PUBLIC
	spdlog::spdlog 
)

set_target_properties(${NFRAGE_CORE_NAME} PROPERTIES FOLDER "Engine")

add_library(NFR::Core ALIAS ${NFRAGE_CORE_NAME})