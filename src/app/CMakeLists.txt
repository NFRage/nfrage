file(GLOB NFRAGE_FRONTEND_SRC_FILES CONFIGURE_DEPENDS
	"*.cpp"
	"*.h"
)

source_group("sources" FILES ${NFRAGE_FRONTEND_SRC_FILES})

add_library(${NFRAGE_APPLICATION_NAME} ${NFRAGE_LINKING_MODE} ${NFRAGE_FRONTEND_SRC_FILES})
if (NFRAGE_DYNAMIC)
	target_compile_definitions(${NFRAGE_APPLICATION_NAME} PUBLIC DLL_PLATFORM)
	target_compile_definitions(${NFRAGE_APPLICATION_NAME} PRIVATE NFRAGE_APPLAYER_EXPORTS)
endif()

# "SDL3-static" "imgui"
target_link_libraries(${NFRAGE_APPLICATION_NAME} PUBLIC NFR::Core)
target_precompile_headers(${NFRAGE_APPLICATION_NAME} PRIVATE "app_pch.h")
target_include_directories(${NFRAGE_APPLICATION_NAME} PUBLIC "${NFRAGE_ROOT_DIR}/src/app")
set_target_properties(${NFRAGE_APPLICATION_NAME} PROPERTIES FOLDER "Engine")

add_library(NFR::Application ALIAS ${NFRAGE_APPLICATION_NAME})