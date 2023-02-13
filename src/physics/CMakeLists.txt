file(GLOB NFRAGE_PHYSICS_SRC_FILES CONFIGURE_DEPENDS
	"*.cpp"
	"*.h"
)

source_group("sources" FILES ${NFRAGE_PHYSICS_SRC_FILES})

add_library(${NFRAGE_PHYSICS_NAME} ${NFRAGE_LINKING_MODE} ${NFRAGE_PHYSICS_SRC_FILES})
if (NFRAGE_SHARED)
	target_compile_definitions(${NFRAGE_PHYSICS_NAME} PUBLIC DLL_PLATFORM)
	target_compile_definitions(${NFRAGE_PHYSICS_NAME} PRIVATE NFRAGE_PHYSICS_EXPORTS)
endif()

target_link_libraries(${NFRAGE_PHYSICS_NAME} PUBLIC NFR::Core Bullet3::bullet)
target_precompile_headers(${NFRAGE_PHYSICS_NAME} PRIVATE "physics_pch.h")
target_include_directories(${NFRAGE_PHYSICS_NAME} PUBLIC "${NFRAGE_ROOT_DIR}/src/physics")
set_target_properties(${NFRAGE_PHYSICS_NAME} PROPERTIES FOLDER "Engine")

add_library(NFR::Physics ALIAS ${NFRAGE_PHYSICS_NAME})