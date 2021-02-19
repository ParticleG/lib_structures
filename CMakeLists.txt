cmake_minimum_required(VERSION 3.5)

project(STRUCTURES_LIB)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(ranges HAS_RANGES)
if (HAS_RANGES)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib_utils lib_utils)

add_library(${PROJECT_NAME} STATIC)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/structures STRUCTURE_SRC)

target_link_libraries(${PROJECT_NAME} PUBLIC UTILS_LIB)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(${PROJECT_NAME} PUBLIC ${STRUCTURE_SRC})
