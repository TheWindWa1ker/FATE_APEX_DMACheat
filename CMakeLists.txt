cmake_minimum_required(VERSION 3.18)

project(FATE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_GENERATOR_PLATFORM x64)

# vcpkg
set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# Find required packages
find_package(CURL CONFIG REQUIRED)

# Include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann
        ${CMAKE_CURRENT_SOURCE_DIR}/include/game
        ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/freetype/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/freetype/include/freetype
        ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/freetype/include/freetype/config
        ${CMAKE_CURRENT_SOURCE_DIR}/include/kmbox
        ${CMAKE_CURRENT_SOURCE_DIR}/include/module
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tools
        ${CMAKE_CURRENT_SOURCE_DIR}/include/apexsky/src
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/Memory
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/nt
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/libs
)

# Collecting header and source files
FILE(GLOB_RECURSE FILE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*)
FILE(GLOB_RECURSE FILE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*)

add_executable(FATE ${FILE_INCLUDES} ${FILE_SOURCES})

# Link libraries
target_link_libraries(FATE PRIVATE
        CURL::libcurl
        d3d11
        d3dx11
        d3dcompiler
        dxgi
        Ws2_32
        vmm
        leechcore
        apexsky
        Userenv
        Ntdll
        Bcrypt
)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/outputs)
