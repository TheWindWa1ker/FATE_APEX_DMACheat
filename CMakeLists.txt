cmake_minimum_required(VERSION 3.18)

project(FATE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_GENERATOR_PLATFORM x64)

# vcpkg
set(VCPKG_ROOT "C:/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")


set(CURL_DIR "C:\\vcpkg\\installed\\x64-windows\\share\\curl")
set(ZLIB_ROOT "C:\\vcpkg\\installed\\x64-windows")

# DirectX SDK
set(DIRECTX_SDK_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
if(EXISTS ${DIRECTX_SDK_PATH})
    include_directories("${DIRECTX_SDK_PATH}/Include")
    link_directories("${DIRECTX_SDK_PATH}/Lib/x64")
else()
    message(WARNING "DirectX SDK not found at ${DIRECTX_SDK_PATH}")
endif()

# curl
find_package(CURL CONFIG REQUIRED)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    set(ARCHITECTURE "x64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(ARCHITECTURE "x86")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x64")
    set(ARCHITECTURE "x64")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/outputs/${ARCHITECTURE}/)

FILE(GLOB_RECURSE FILE_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/Memory/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/libs/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/nt/*.h*
)

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

FILE(GLOB_RECURSE FILE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/Memory/*.c*
        "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/*.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX ".*CMakeCXXCompilerId\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*CMakeCCompilerId\\.c$")

add_executable(FATE
        ${FILE_INCLUDES}
        ${FILE_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/auth/MachineCodeGenerator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/auth/w3c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/auth/w3c.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/Memory/Memory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/game/Game_struct.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/prediction.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/Vector.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/skynade.h
        include/game/Drawmisc.hpp
        include/game/ReadList.hpp
)

target_include_directories(FATE PRIVATE
        libs/DMALibrary
)

target_link_directories(FATE PRIVATE
        libs/DMALibrary/libs
        include/apexsky/target/release
)

target_link_libraries(FATE PRIVATE
        d3d11
        d3dx11
        d3dcompiler
        dxgi
        Ws2_32
        vmm
        leechcore
        CURL::libcurl
        apexsky
        Userenv # 添加Userenv库解决GetUserProfileDirectoryW相关问题
        Ntdll   # 添加Ntdll库解决NtCreateFile, RtlNtStatusToDosError等问题
        Bcrypt  # 添加Bcrypt库解决BCryptGenRandom相关问题
)

#target_compile_options(Apex PRIVATE /std:c++latest)