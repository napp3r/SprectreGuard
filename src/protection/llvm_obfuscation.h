#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <QSettings>
#include <QDebug>

class LLVMObfuscation {
public:
    struct ObfuscationConfig {
        bool controlFlowFlattening = true;  // Control flow flattening obfuscation
        bool instructionSubstitution = true; // Replace instructions with equivalent ones
        bool bogusControlFlow = true;       // Insert bogus control flow paths
        bool deadCodeInsertion = true;      // Insert dead code
        bool stringEncryption = true;       // Encrypt string literals
        int obfuscationLevel = 2;           // Obfuscation level (1-3)
    };

    // Obfuscate an executable using LLVM
    static bool obfuscateExecutable(const std::string& exePath, 
                                   const std::string& outputPath, 
                                   const ObfuscationConfig& config);
    
    // Set the LLVM bin directory path
    static void setLLVMPath(const QString& path);

private:
    // Path to LLVM binaries
    static inline QString llvmBinDirPath;
    static inline bool hasOptTool = false;
    static inline bool hasLlcTool = false;
    
    // Extract executable code sections for LLVM processing
    static bool extractCodeSections(const std::string& exePath, const std::string& tempDir);
    
    // Apply control flow flattening
    static bool applyControlFlowFlattening(const std::string& bitcodePath, int level);
    
    // Apply instruction substitution
    static bool applyInstructionSubstitution(const std::string& bitcodePath, int level);
    
    // Insert bogus control flow
    static bool applyBogusControlFlow(const std::string& bitcodePath, int level);
    
    // Insert dead code
    static bool applyDeadCodeInsertion(const std::string& bitcodePath, int level);
    
    // Apply string encryption
    static bool applyStringEncryption(const std::string& bitcodePath);
    
    // Recompile and relink the obfuscated code
    static bool recompileAndLink(const std::string& bitcodePath, 
                               const std::string& exePath,
                               const std::string& outputPath);
    
    // Utility functions
    static bool initializeLLVM();
    static bool checkLLVMTools();
    static std::string createTempDirectory();
    static void cleanupTempDirectory(const std::string& tempDir);
    static std::string escapeShellArg(const std::string& arg);
}; 