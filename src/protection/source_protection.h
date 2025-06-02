#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <cstdint>
#include <Windows.h>
#include <fstream>
#include <random>
#include <memory>
#include <functional>

class SourceProtection {
public:
    // Progress callback type
    using ProgressCallback = std::function<void(int progress, const std::string& status)>;

    struct ProtectionConfig {
        bool obfuscateNames = true;        // Obfuscate names
        bool encryptStrings = true;        // Encrypt strings
        bool addAntiDebug = true;          // Anti-debug
        bool addJunkCode = true;           // Add junk code
        bool splitCode = false;            // Split code with junk
        bool useVirtualization = false;    // Use code virtualization
        int junkCodeAmount = 5000;         // Amount of junk code to add
        // String encryption options
        bool xorEncryptStrings = false;       // Encrypt strings using XOR
        bool aesEncryptStrings = false;       // Encrypt strings using AES

        std::vector<std::string> xorStringsToEncrypt; // Strings selected for XOR encryption
        std::vector<std::string> aesStringsToEncrypt; // Strings selected for AES encryption
        
        ProgressCallback progressCallback;  // Progress callback function
    };

    static bool protect(const std::string& sourcePath, const std::string& outputPath, const ProtectionConfig& config);
    static bool protectSourceCode(const std::string& sourceCode, const std::string& outputPath, const ProtectionConfig& config);
    static std::string readFile(const std::string& path);

private:
    // Name obfuscation
    static std::string obfuscateIdentifiers(const std::string& sourceCode, const ProgressCallback& callback);
    static std::string obfuscateLocalVariables(const std::string& sourceCode, const ProgressCallback& callback);
    
    // String encryption
    static std::string encryptStrings(const std::string& sourceCode, 
                                    const std::vector<std::string>& stringsToEncrypt,
                                    const ProgressCallback& callback);
    
    // Anti-debug checks
    static std::string insertAntiDebugChecks(const std::string& sourceCode, const ProgressCallback& callback);
    
    // Junk code insertion
    static std::string addJunkCode(const std::string& sourceCode, int amount, const ProgressCallback& callback);
    static std::string splitWithJunkCode(const std::string& sourceCode, const ProgressCallback& callback);
    
    // Code virtualization
    static std::string virtualizeCode(const std::string& sourceCode, const ProgressCallback& callback);
    
    // Helper functions
    static std::string generateRandomName(int length);
    static std::string generateJunkCode();
    static std::vector<uint8_t> encryptData(const std::vector<uint8_t>& data, const std::string& key);
    static std::string generateDecryptionStub(const std::string& key);
    static std::string generateStringEncryptionStub();

    // File operations
    static bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return file.good();
    }
}; 