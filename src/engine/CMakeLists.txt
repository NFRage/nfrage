file(GLOB NFRAGE_ENG_SRC_FILES CONFIGURE_DEPENDS
	"*.cpp"
	"*.h"
)

source_group("sources" FILES ${NFRAGE_ENG_SRC_FILES})

add_library(${NFRAGE_ENG_NAME} ${NFRAGE_LINKING_MODE} ${NFRAGE_ENG_SRC_FILES})
if (NFRAGE_SHARED)
	target_compile_definitions(${NFRAGE_ENG_NAME} PUBLIC DLL_PLATFORM)
	target_compile_definitions(${NFRAGE_ENG_NAME} PRIVATE NFRAGE_ENGINE_EXPORTS)
endif()

target_precompile_headers(${NFRAGE_ENG_NAME} PRIVATE "engine_pch.h")
target_include_directories(${NFRAGE_ENG_NAME} PUBLIC "${NFRAGE_ROOT_DIR}/src/engine")
target_link_libraries(${NFRAGE_ENG_NAME} PUBLIC 
	NFR::Core 
	NFR::Physics 
	NFR::Audio
	NFR::Application
)

set_target_properties(${NFRAGE_ENG_NAME} PROPERTIES FOLDER "Engine")

add_library(NFR::Engine ALIAS ${NFRAGE_ENG_NAME})