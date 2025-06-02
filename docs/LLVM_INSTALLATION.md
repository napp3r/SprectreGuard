# LLVM Obfuscation Setup Guide

This guide explains how to set up LLVM for executable obfuscation in SpectreGuard.

## Installing LLVM

1. Download LLVM 13.0.0 (or later) pre-built binaries for Windows from the [LLVM releases page](https://github.com/llvm/llvm-project/releases).

2. Extract the downloaded archive to a temporary location.

3. Create the directory structure in your SpectreGuard installation:
   ```
   SpectreGuard/
   └── tools/
       └── llvm/
           └── bin/
   ```

4. Copy the following executable files from the extracted LLVM distribution to `tools/llvm/bin/`:
   - clang.exe
   - opt.exe
   - llc.exe
   - llvm-objcopy.exe
   - llvm-objdump.exe
   - llvm-dis.exe
   - (and any other required binaries)

## Building Obfuscation Passes

The obfuscation passes need to be built separately:

1. Make sure you have Visual Studio 2019 or newer installed with C++ development tools.

2. Run the build script located at `tools/llvm/ObfuscationPasses/build.bat`.
   This will compile the obfuscation passes and place the resulting DLL in the appropriate location.

3. If the build fails, make sure that:
   - LLVM is correctly installed in the tools directory
   - Visual Studio is properly installed with C++ components
   - CMake is installed and available in your PATH

## Testing the Installation

1. Open SpectreGuard and go to the EXE Protection tab.

2. Click the "Test LLVM Installation" button.

3. If everything is set up correctly, you should see a success message showing the LLVM version.

## Troubleshooting

### Missing DLLs

If you encounter missing DLL errors when running LLVM tools, copy the necessary DLLs from the LLVM distribution to the `tools/llvm/bin/` directory.

### Build Errors

If you encounter build errors for the obfuscation passes:

1. Check the CMake output for specific error messages.
2. Make sure you're using a compatible version of LLVM (13.0.0 or later).
3. Ensure your Visual Studio installation includes all required C++ components.

### Obfuscation Fails

If obfuscation seems to complete but the executable doesn't work:

1. Try with a simpler executable first.
2. Disable some of the more aggressive obfuscation options.
3. Check the application logs for specific error messages.

## Advanced Configuration

For advanced users who want to customize the obfuscation passes:

1. Modify the source files in `tools/llvm/ObfuscationPasses/src/`.
2. Add new pass implementations as needed.
3. Update the CMakeLists.txt to include your new files.
4. Rebuild using the build script.

## References

- [LLVM Documentation](https://llvm.org/docs/)
- [LLVM Developer's Manual](https://llvm.org/docs/ProgrammersManual.html)
- [LLVM Pass Framework](https://llvm.org/docs/WritingAnLLVMPass.html) 