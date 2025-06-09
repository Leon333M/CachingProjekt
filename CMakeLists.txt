cmake_minimum_required(VERSION 3.15)
project(CachingProjekt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# === Compiler-Optimierungsflags für max Performance ===
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /Ob2 /Oi /Ot /Oy /GF /DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "/O2 /GL /Ob2 /Oi /Ot /Oy /GF /DNDEBUG")

# === plog via FetchContent ===
include(FetchContent)
FetchContent_Declare(
    plog
    GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
    GIT_TAG 1.1.9
)
FetchContent_MakeAvailable(plog)
add_compile_definitions(PLOG_ENABLE_WCHAR_INPUT)

# === Includes ===
include_directories(include)
# WinFsp Include
include_directories("E:/Code/WinFsp/inc")
# plog Include
include_directories(${plog_SOURCE_DIR}/include) 
# qt Include
add_compile_options(/Zc:__cplusplus) 
include_directories("F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/include")
include_directories("F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/include/QtCore")
include_directories("F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/include/QtWidgets")

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
     src/LogManager.cpp
)

# === plog mit dem Executable linken ===
target_link_libraries(CachingProjekt PRIVATE plog)

# === WinFsp-Bibliothek verlinken ===
link_directories("E:/Code/WinFsp/lib")
target_link_libraries(CachingProjekt PRIVATE "E:/Code/WinFsp/lib/winfsp-x64.lib")

# === QT-Bibliothek verlinken ===
link_directories("F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/lib")
target_link_libraries(CachingProjekt PRIVATE "F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/lib/Qt6Core.lib")
target_link_libraries(CachingProjekt PRIVATE "F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/lib/Qt6Widgets.lib")

# === Qt-DLLs und Plugins automatisch ins Build-Verzeichnis kopieren ===
add_custom_command(TARGET CachingProjekt POST_BUILD
    COMMAND "F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/bin/windeployqt6.exe"
            "$<TARGET_FILE:CachingProjekt>"
            --release
            --no-translations
            >NUL
    COMMENT "Qt-Runtime (DLLs, Plugins) wird via windeployqt in das Build-Verzeichnis kopiert..."
)

# === Build-Typ auf Debug setzen, falls nicht gesetzt ===
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
