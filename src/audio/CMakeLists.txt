file(GLOB NFRAGE_AUDIO_SRC_FILES CONFIGURE_DEPENDS
	"*.cpp"
	"*.h"
)

source_group("sources" FILES ${NFRAGE_AUDIO_SRC_FILES})

add_library(${NFRAGE_AUDIO_NAME} ${NFRAGE_LINKING_MODE} ${NFRAGE_AUDIO_SRC_FILES})
if (NFRAGE_SHARED)
	target_compile_definitions(${NFRAGE_AUDIO_NAME} PUBLIC DLL_PLATFORM)
	target_compile_definitions(${NFRAGE_AUDIO_NAME} PRIVATE NFRAGE_AUDIO_EXPORTS)
endif()

target_link_libraries(${NFRAGE_AUDIO_NAME} PUBLIC NFR::Core)
target_precompile_headers(${NFRAGE_AUDIO_NAME} PRIVATE "audio_pch.h")
target_include_directories(${NFRAGE_AUDIO_NAME} PUBLIC "${NFRAGE_ROOT_DIR}/src/audio")
set_target_properties(${NFRAGE_AUDIO_NAME} PROPERTIES FOLDER "Engine")

add_library(NFR::Audio ALIAS ${NFRAGE_AUDIO_NAME})