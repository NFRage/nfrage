file(GLOB NFRAGE_MISC_SRC_FILES CONFIGURE_DEPENDS
	"*.cpp"
	"*.h"
)

source_group("sources" FILES ${NFRAGE_MISC_SRC_FILES})

add_executable(${NFRAGE_ENTRYPOINT_NAME} ${NFRAGE_MISC_SRC_FILES})
if (NFRAGE_SHARED)
	target_compile_definitions(${NFRAGE_ENTRYPOINT_NAME} PUBLIC DLL_PLATFORM)
	target_compile_definitions(${NFRAGE_ENTRYPOINT_NAME} PRIVATE NFRAGE_MISC_EXPORTS)
endif()

target_link_libraries(${NFRAGE_ENTRYPOINT_NAME} NFR::Engine)
target_precompile_headers(${NFRAGE_ENTRYPOINT_NAME} PRIVATE "main_pch.h")
target_include_directories(${NFRAGE_ENTRYPOINT_NAME} PRIVATE "${NFRAGE_ROOT_DIR}/src/misc")
set_target_properties(${NFRAGE_ENTRYPOINT_NAME} PROPERTIES FOLDER "Game")
