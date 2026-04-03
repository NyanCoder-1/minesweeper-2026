cmake_minimum_required (VERSION 3.8)

set(PROJECT "minesweeper-2026")
set(TARGET "game")
project(${PROJECT})

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prepare directories
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_path(NORMAL_PATH PROJECT_DIR)
set(SOURCE_DIR "${PROJECT_DIR}/src")
set(INCLUDE_DIR "${PROJECT_DIR}/include")
set(SUBMODULES_DIR "${PROJECT_DIR}/submodules")
set(THIRDPARTY_DIR "${PROJECT_DIR}/thirdparty")

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_DIR}/bin)

# Scan for source files
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.c")
file(GLOB_RECURSE HEADERS "${SOURCE_DIR}/*.hpp" "${SOURCE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp" "${INCLUDE_DIR}/*.h")

# Set their directories
foreach (SOURCE_FILE ${SOURCES};${HEADERS})

	# Get relative path
	cmake_path(RELATIVE_PATH SOURCE_FILE BASE_DIRECTORY ${PROJECT_DIR})

	# Extract directory
	get_filename_component(SOURCE_FILE_DIR ${SOURCE_FILE} DIRECTORY)
	
	# Set source group
	if (WIN32) # Replace slashes with backslashes for Windows
		string(REPLACE "/" "\\" SOURCE_FILE_DIR ${SOURCE_FILE_DIR})
	endif ()
	source_group("${SOURCE_FILE_DIR}" FILES ${SOURCE_FILE})

endforeach ()

# Thirdparty
set(GLAD_DIR "${THIRDPARTY_DIR}/glad/include")
# Submodules
set(CGLM_DIR "${SUBMODULES_DIR}/cglm/include")
set(SDL_DIR "${SUBMODULES_DIR}/SDL/include")
set(STB_DIR "${SUBMODULES_DIR}/stb")

# Set include directories
set(INCLUDE_DIR
	"${SOURCE_DIR}/"
	"${INCLUDE_DIR}/"
	"${CGLM_DIR}/"
	"${SDL_DIR}/"
	"${STB_DIR}/"
	)


# SDL configuration
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_DEPS_SHARED OFF CACHE BOOL "" FORCE)

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL_DOCS OFF CACHE BOOL "" FORCE)

set(SDL_AUDIO ON CACHE BOOL "" FORCE)
set(SDL_VIDEO ON CACHE BOOL "" FORCE)
set(SDL_JOYSTICK ON CACHE BOOL "" FORCE)
set(SDL_HIDAPI ON CACHE BOOL "" FORCE)
set(SDL_HIDAPI_JOYSTICK ON CACHE BOOL "" FORCE)
set(SDL_OPENGLES ON CACHE BOOL "" FORCE)
set(SDL_OPENGL OFF CACHE BOOL "" FORCE)

set(SDL_RENDER OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_DIALOG OFF CACHE BOOL "" FORCE)
set(SDL_TRAY OFF CACHE BOOL "" FORCE)
set(SDL_VULKAN OFF CACHE BOOL "" FORCE)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
  set(SDL_HIDAPI_JOYSTICK OFF CACHE BOOL "" FORCE)
elseif(WIN32)
  set(SDL_DIRECTX ON CACHE BOOL "" FORCE)
  set(SDL_WASAPI ON CACHE BOOL "" FORCE)
  set(SDL_XINPUT ON CACHE BOOL "" FORCE)
elseif(UNIX)
  set(SDL_X11 ON CACHE BOOL "" FORCE)
  set(SDL_WAYLAND ON CACHE BOOL "" FORCE)

  set(SDL_ALSA ON CACHE BOOL "" FORCE)
  set(SDL_PIPEWIRE ON CACHE BOOL "" FORCE)

  set(SDL_PULSEAUDIO OFF CACHE BOOL "" FORCE)
  set(SDL_JACK OFF CACHE BOOL "" FORCE)
  set(SDL_SNDIO OFF CACHE BOOL "" FORCE)
  set(SDL_OSS OFF CACHE BOOL "" FORCE)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	set(COMPILER_FLAGS "-ffunction-sections -fdata-sections -pthread")
	set(USE_FLAGS "-sFULL_ES3=1 -sASSERTIONS=0 -sSAFE_HEAP=0 -sSTACK_OVERFLOW_CHECK=0 -sFILESYSTEM=1 -sUSE_PTHREADS=1 -sPROXY_TO_PTHREAD -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -Wl,--gc-sections -Oz")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
endif ()

add_subdirectory(${SUBMODULES_DIR}/SDL EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED COMPONENTS GLES3)

add_executable(${TARGET} ${SOURCES} ${HEADERS})
#target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic -Werror)

# make cglm left-handed
target_compile_definitions(${TARGET} PRIVATE CGLM_FORCE_LEFT_HANDED)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

	target_link_libraries(${TARGET} m SDL3::SDL3-static OpenGL::GLES3)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")

	target_link_libraries(${TARGET} m SDL3::SDL3-static)

else ()

	message( FATAL_ERROR "Sorry, bruh, this project is meant to be build only for Linux/Wayland" )

endif ()

include_directories(${INCLUDE_DIR})
add_definitions(-DTARGET="${TARGET}")

# Hide ZERO_CHECK and ALL_BUILD targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER meta)
