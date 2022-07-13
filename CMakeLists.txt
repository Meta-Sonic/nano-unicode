cmake_minimum_required(VERSION 3.22.0)

# No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator.
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
    set(CMAKE_XCODE_GENERATE_SCHEME ON)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else()
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release"  CACHE STRING "Build type" FORCE)
    endif()
endif()

project(nano-unicode VERSION 1.0.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

option(NANO_UNICODE_BUILD_TESTS "Build tests." OFF)
option(NANO_UNICODE_DEV "Development build" OFF)

# nano-unicode interface.
set(NANO_UNICODE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nano/unicode.h")
add_library(${PROJECT_NAME} INTERFACE ${NANO_UNICODE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NANO_UNICODE_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    XCODE_GENERATE_SCHEME OFF)

add_library(nano::unicode ALIAS ${PROJECT_NAME})


if (NANO_UNICODE_DEV)
    set(NANO_UNICODE_BUILD_TESTS ON)

    find_program(CLANG_FORMAT clang-format REQUIRED)
    add_custom_target(check-nano-unicode-formatting DEPENDS ${NANO_UNICODE_SOURCES})
    add_custom_command(TARGET check-nano-unicode-formatting PRE_BUILD
        COMMAND "${CLANG_FORMAT}" --Werror  --dry-run ${NANO_UNICODE_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    add_dependencies(${PROJECT_NAME} check-nano-unicode-formatting)

    set_target_properties(check-nano-unicode-formatting PROPERTIES XCODE_GENERATE_SCHEME OFF)
endif()


if (NANO_UNICODE_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(nano-test
        GIT_REPOSITORY https://github.com/Meta-Sonic/nano-test.git
        GIT_TAG master)

    FetchContent_MakeAvailable(nano-test)

    file(GLOB_RECURSE TEST_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h")

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/tests" FILES ${TEST_SOURCE_FILES})

    add_executable(nano-unicode-tests ${TEST_SOURCE_FILES})
    target_include_directories(nano-unicode-tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(nano-unicode-tests PUBLIC nano::test nano::unicode)

    set(CLANG_OPTIONS -Weverything -Wno-c++98-compat)
    set(MSVC_OPTIONS /W4)

    target_compile_options(nano-unicode-tests PUBLIC
        "$<$<CXX_COMPILER_ID:Clang,AppleClang>:${CLANG_OPTIONS}>"
        "$<$<CXX_COMPILER_ID:MSVC>:${MSVC_OPTIONS}>")

    set_target_properties(nano-unicode-tests PROPERTIES CXX_STANDARD 17)
endif()
