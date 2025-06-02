cmake_minimum_required(VERSION 3.16)

# Настройка vcpkg
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

project(SpectreGuard VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Настройка статической линковки
set(VCPKG_TARGET_TRIPLET "x64-windows-static")
set(VCPKG_CRT_LINKAGE "static")

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/sidebar.cpp
    src/ui/pages/homewidget.cpp
    src/ui/pages/sourceprotectionwidget.cpp
    src/ui/pages/exeprotectionwidget.cpp
    src/ui/pages/settingswidget.cpp
    src/protection/source_protection.cpp
    src/protection/exe_protection.cpp
    src/protection/llvm_obfuscation.cpp
)

# Header filesO
set(HEADERS
    src/ui/mainwindow.h
    src/ui/sidebar.h
    src/ui/pages/homewidget.h
    src/ui/pages/sourceprotectionwidget.h
    src/ui/pages/exeprotectionwidget.h
    src/ui/pages/settingswidget.h
    src/core/settings.h
    src/protection/source_protection.h
    src/protection/exe_protection.h
    src/protection/llvm_obfuscation.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/pages
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protection
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy UPX executable to build output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/tools/upx"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/upx/upx.exe"
        "${CMAKE_BINARY_DIR}/bin/tools/upx/upx.exe"
    COMMENT "Copying UPX executable to output directory"
)

# Copy AES tools to build output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/tools/aes"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/aes/aes_encrypt_single.exe"
        "${CMAKE_BINARY_DIR}/bin/tools/aes/aes_encrypt_single.exe"
    COMMENT "Copying AES tools to output directory"
)

# Create directory for LLVM tools
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/tools/llvm/bin"
    COMMENT "Creating LLVM tools directory"
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Also install UPX with the application
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/tools/upx/upx.exe"
    DESTINATION bin/tools/upx
    COMPONENT Runtime
)

# Install LLVM tools directory structure
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tools/llvm/"
    DESTINATION bin/tools/llvm
    COMPONENT Runtime
    OPTIONAL
)
