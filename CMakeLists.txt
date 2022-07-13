cmake_minimum_required(VERSION 3.22.0)

# No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator.
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
    # set(CMAKE_XCODE_GENERATE_SCHEME ON)
    # set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else()
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release"  CACHE STRING "Build type" FORCE)
    endif()
endif()

project(nano-unicode VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# option(NANO_UNICODE_BUILD_EXAMPLES "Build examples." OFF)
# option(NANO_UNICODE_DEV "Development build" OFF)

# nano-unicode interface.
set(NANO_UNICODE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nano/unicode.h")
add_library(${PROJECT_NAME} INTERFACE ${NANO_UNICODE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NANO_UNICODE_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(nano::unicode ALIAS ${PROJECT_NAME})