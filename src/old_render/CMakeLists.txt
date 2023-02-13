file(GLOB NFRAGE_RENDER_SRC_FILES
	"*.cpp"
	"*.h"
)

source_group("sources" FILES ${NFRAGE_RENDER_SRC_FILES} "Render_Metal.mm")
 
if (APPLE)
	add_library(${NFRAGE_RENDER_NAME} ${NFRAGE_LINKING_MODE} 
		${NFRAGE_RENDER_SRC_FILES}
		"Render_Metal.mm"
	)
else()
	add_library(${NFRAGE_RENDER_NAME} ${NFRAGE_LINKING_MODE} 
		${NFRAGE_RENDER_SRC_FILES}
	)
endif()

if (NFRAGE_DYNAMIC)
	target_compile_definitions(${NFRAGE_RENDER_NAME} PUBLIC DLL_PLATFORM)
	target_compile_definitions(${NFRAGE_RENDER_NAME} PRIVATE NFRAGE_RENDER_EXPORTS)
endif()

target_link_libraries(${NFRAGE_RENDER_NAME} PUBLIC "NRI" "imgui" NFR::Core NFR::AppLayer)
target_precompile_headers(${NFRAGE_RENDER_NAME} PRIVATE "RenderPch.h")
target_include_directories(${NFRAGE_RENDER_NAME} PUBLIC "${NFRAGE_ROOT_DIR}/src/Render")
set_target_properties(${NFRAGE_RENDER_NAME} PROPERTIES FOLDER "Engine")

add_library(NFR::Render ALIAS ${NFRAGE_RENDER_NAME})