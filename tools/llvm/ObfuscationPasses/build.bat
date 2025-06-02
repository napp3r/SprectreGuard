@echo off
rem Build script for LLVM Obfuscation Passes

echo Building LLVM Obfuscation Passes...

rem Create build directory
if not exist build mkdir build
cd build

rem Configure with CMake
echo Configuring with CMake...
cmake -G "Visual Studio 16 2019" -A x64 -DLLVM_DIR="..\..\..\lib\cmake\llvm" ..
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b %ERRORLEVEL%
)

rem Build with CMake
echo Building with CMake...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

rem Copy the built DLL to the parent directory
echo Copying DLL to bin directory...
if not exist "..\..\..\bin" mkdir "..\..\..\bin"
copy /Y "Release\LLVMObfuscation.dll" "..\..\..\bin\"
if %ERRORLEVEL% neq 0 (
    echo Failed to copy DLL!
    exit /b %ERRORLEVEL%
)

cd ..
echo Build completed successfully!
echo DLL is in tools\llvm\bin\LLVMObfuscation.dll 