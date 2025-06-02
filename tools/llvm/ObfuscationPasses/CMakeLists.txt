cmake_minimum_required(VERSION 3.13.4)
project(LLVMObfuscation)

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build shared library
add_library(LLVMObfuscation MODULE
  src/Flattening.cpp
  # Add other passes here as they are implemented
)

# Get proper shared-library behavior on Windows
if(WIN32)
  set_target_properties(LLVMObfuscation PROPERTIES
    PREFIX ""
    SUFFIX ".dll")
endif()

# Link against LLVM libraries
target_link_libraries(LLVMObfuscation
  PRIVATE
  ${LLVM_AVAILABLE_LIBS}
)

# Installation
install(TARGETS LLVMObfuscation DESTINATION lib) 