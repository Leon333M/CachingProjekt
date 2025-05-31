cmake_minimum_required(VERSION 3.15)

project(CachingProjekt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# === Include-Pfade ===
include_directories(include)
include_directories("E:/Code/WinFsp/inc")  # WinFsp-Header


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

# === WinFsp-Bibliothek verlinken ===
link_directories("E:/Code/WinFsp/lib")
target_link_libraries(CachingProjekt "E:/Code/WinFsp/lib/winfsp-x64.lib")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()