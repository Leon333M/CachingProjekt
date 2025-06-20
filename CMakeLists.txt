cmake_minimum_required(VERSION 3.15)
project(CachingProjekt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# === Build-Typ auf Debug setzen, falls nicht gesetzt ===
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

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
include_directories("F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/include/QtGui")


# === Quellcode-Dateien ===
add_executable(CachingProjekt
     src/main.cpp
     src/VirtuelleFestplatte.cpp
     src/CacheInterface.cpp
     src/SsdCache.cpp
     src/RamCache.cpp
     src/Cache.cpp
     src/ConfigLoader.cpp
     src/LogManager.cpp
     src/Benachrichtiger.cpp
     src/GUI/GuiManager.cpp
     src/GUI/MainWindow.cpp
     src/GUI/CacheWindow.cpp
     src/GUI/VhddWindow.cpp
     src/GUI/KreisWidget.cpp
     src/GUI/QtSymbolHandler.cpp
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
target_link_libraries(CachingProjekt PRIVATE "F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/lib/Qt6Gui.lib")

# === Qt-DLLs und Plugins automatisch ins Build-Verzeichnis kopieren ===
add_custom_command(TARGET CachingProjekt POST_BUILD
    COMMAND "F:/Users/Leon/Desktop/HA-Schule/Uni/Softwareprojekt/qt/6.5.1/msvc2019_64/bin/windeployqt6.exe"
            "$<TARGET_FILE:CachingProjekt>"
            --release
            --no-translations
            >NUL
    COMMENT "Qt-Runtime (DLLs, Plugins) wird via windeployqt in das Build-Verzeichnis kopiert..."
)

# === exe Signieren ===
# Pfad zu signtool.exe
set(SIGNTOOL "E:/Windows Kits/10/bin/10.0.26100.0/x64/signtool.exe")
# Pfad zum Zertifikat (PFX)
set(CERT_PFX "E:/Code/MeinCodeSigningZertifikat.pfx")
set(CERT_PASS "DrD2jUDvyB")
# Pfad zur gebauten EXE
set(EXE_TO_SIGN "$<TARGET_FILE:CachingProjekt>")
# Signieren nach Build
add_custom_command(TARGET CachingProjekt POST_BUILD
    COMMAND "${SIGNTOOL}" sign /f "${CERT_PFX}" /p "${CERT_PASS}" /fd sha256 /tr http://timestamp.digicert.com /td sha256 "${EXE_TO_SIGN}"
    COMMENT "Signiere CachingProjekt.exe automatisch"
)
