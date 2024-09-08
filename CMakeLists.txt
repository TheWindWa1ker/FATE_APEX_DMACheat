cmake_minimum_required(VERSION 3.18)

project(FATE)
set(CMAKE_CXX_STANDARD 20)

# 移除特定于Windows的设置
# set(CMAKE_GENERATOR_PLATFORM x64)
# set(VCPKG_ROOT "C:/vcpkg")
# set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# 使用环境变量或CMake参数来设置vcpkg
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# 移除特定路径
# set(CURL_DIR "C:\\vcpkg\\installed\\x64-windows\\share\\curl")
# set(ZLIB_ROOT "C:\\vcpkg\\installed\\x64-windows")

# DirectX SDK 路径 (仅在Windows上使用)
if(WIN32)
    set(DIRECTX_SDK_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
    if(EXISTS ${DIRECTX_SDK_PATH})
        include_directories("${DIRECTX_SDK_PATH}/Include")
        link_directories("${DIRECTX_SDK_PATH}/Lib/x64")
    else()
        message(WARNING "DirectX SDK not found at ${DIRECTX_SDK_PATH}")
    endif()
endif()

# 寻找curl库
find_package(CURL CONFIG REQUIRED)

# 设置架构
if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
    set(ARCHITECTURE "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i686|x86")
    set(ARCHITECTURE "x86")
else()
    set(ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/outputs/${ARCHITECTURE}/)
#CMake头文件设置
FILE(GLOB_RECURSE FILE_INCLUDES # 遍历子目录下所有符合情况的头文件
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/Memory/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/libs/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/nt/*.h*
)

include_directories( # 设置全局头文件目录 使其他源码文件可在任意目录<头文件.h>
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/auth
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
#CMake源文件设置
FILE(GLOB_RECURSE FILE_SOURCES # 遍历子目录下所有符合情况的源文件
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/Memory/*.c*
        "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/*.cpp"
)

# 排除 CMake 生成的文件
list(FILTER SOURCES EXCLUDE REGEX ".*CMakeCXXCompilerId\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*CMakeCCompilerId\\.c$")

add_executable(FATE
        ${FILE_INCLUDES} # 头文件
        ${FILE_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/DMALibrary/Memory/Memory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/game/Game_struct.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/prediction.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/Vector.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/skynade.h
        include/game/Drawmisc.hpp
        include/game/ReadList.hpp
        auth/Auth.hpp
        # 源文件
)

# 添加额外的包含目录
target_include_directories(FATE PRIVATE
    libs/DMALibrary
)

# 添加链接库目录
target_link_directories(FATE PRIVATE
    libs/DMALibrary/libs
    include/apexsky/target/release
)

# 添加链接的库 (根据平台条件编译)
target_link_libraries(FATE PRIVATE
    CURL::libcurl
    apexsky
)

if(WIN32)
    target_link_libraries(FATE PRIVATE
        d3d11
        d3dx11
        d3dcompiler
        dxgi
        Ws2_32
        vmm
        leechcore
        Userenv
        Ntdll
        Bcrypt
    )
endif()

# 设置编译选项
target_compile_options(FATE PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest>")
