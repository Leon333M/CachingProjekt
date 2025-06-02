cmake_minimum_required(VERSION 3.15)

project(CachingProjekt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# === plog per FetchContent einbinden ===
include(FetchContent)

FetchContent_Declare(
    plog
    GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
    GIT_TAG 1.1.9
)

FetchContent_MakeAvailable(plog)

add_compile_definitions(PLOG_ENABLE_WCHAR_INPUT)

# === Include-Pfade ===
include_directories(include)
include_directories("E:/Code/WinFsp/inc")
include_directories(${plog_SOURCE_DIR}/include)   # plog Include

# === Quellcode-Dateien ===
add_executable(CachingProjekt
     src/main.cpp
     src/VirtuelleFestplatte.cpp
     src/CacheInterface.cpp
     src/SsdCache.cpp
     src/RamCache.cpp
     src/Cache.cpp
     src/ConfigLoader.cpp
     src/SymbolHandler.cpp
)

# === plog mit dem Executable linken ===
target_link_libraries(CachingProjekt PRIVATE plog)

# === WinFsp-Bibliothek verlinken ===
link_directories("E:/Code/WinFsp/lib")
target_link_libraries(CachingProjekt PRIVATE "E:/Code/WinFsp/lib/winfsp-x64.lib")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()