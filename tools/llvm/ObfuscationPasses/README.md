# LLVM Obfuscation Passes

This directory contains LLVM-based obfuscation passes for executable protection.

## Setup Instructions

1. Download LLVM and Clang (version 13.0.0 or later) from https://llvm.org/releases/.
2. Install into the `tools/llvm` directory, ensuring that the `bin` directory contains all necessary executables.
3. Build the obfuscation passes using the provided build script.

## Available Obfuscation Passes

The following obfuscation passes are implemented:

1. **Control Flow Flattening**: Transforms function control flow into a flat switch statement containing all basic blocks, making it harder to understand the control flow.

2. **Instruction Substitution**: Replaces instructions with functionally equivalent but more complex sequences.

3. **Bogus Control Flow**: Adds fake branches and conditions that never execute but are difficult to statically analyze.

4. **Dead Code Insertion**: Inserts code that never executes but appears statically reachable.

5. **String Encryption**: Encrypts string literals in the binary and adds decryption routines.

## Usage

These passes can be applied to LLVM bitcode using the `opt` tool:

```bash
opt -load LLVMObfuscation.dll -flatten < input.bc > output.bc
```

The SpectreGuard application integrates these passes into its executable protection workflow.

## Implementation Details

Each pass is implemented as an LLVM module pass that transforms the program's LLVM IR. The passes are designed to be modular and can be applied in any combination.

## Development

To add a new obfuscation pass:

1. Create a new file for your pass in the `src` directory
2. Implement your pass as an LLVM module pass
3. Register your pass in the `PassManager`
4. Update the build system to include your pass
5. Add UI options in the SpectreGuard application

## References

- [LLVM Documentation](https://llvm.org/docs/)
- [LLVM Developer's Manual](https://llvm.org/docs/ProgrammersManual.html)
- [LLVM Pass Framework](https://llvm.org/docs/WritingAnLLVMPass.html) 