cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER "E:/Code/msys64/ucrt64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "E:/Code/msys64/ucrt64/bin/g++.exe")

project(CachingProjekt)

set(CMAKE_CXX_STANDARD 17)

# === Include-Pfade ===
include_directories(include)
include_directories("E:/Code/WinFsp/include")  # WinFsp-Header


# === Quellcode-Dateien ===
add_executable(CachingProjekt
    src/main.cpp
    src/PassthroughFS.cpp
)

# === WinFsp-Bibliothek verlinken ===
link_directories("E:/Code/WinFsp/lib")
target_link_libraries(CachingProjekt "E:/Code/WinFsp/lib/winfsp-x64.lib")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()