// Removed class definition to avoid redefinition error

#include "source_protection.h"
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <regex>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <array>
#include <chrono>
#include <iomanip>
// #include <openssl/evp.h>
// #include <openssl/rand.h>

namespace _obf_ns {
    using _obf_byte = uint8_t;
    using _obf_word = uint16_t;
    using _obf_dword = uint32_t;
    using _obf_qword = uint64_t;

    inline _obf_dword _obf_generate_key() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<_obf_dword> dis;
        return dis(gen);
    }

    template<typename T>
    class _obf_guard {
    private:
        T _obf_value;
        _obf_dword _obf_key;
        
        _obf_dword _obf_encrypt(T value) const {
            return static_cast<_obf_dword>(value) ^ _obf_key;
        }
        
        T _obf_decrypt(_obf_dword value) const {
            return static_cast<T>(value ^ _obf_key);
        }
        
    public:
        _obf_guard() : _obf_value(T()), _obf_key(_obf_generate_key()) {}
        _obf_guard(T value) : _obf_value(value), _obf_key(_obf_generate_key()) {}
        
        operator T() const { return _obf_value; }
        
        _obf_guard& operator=(T value) {
            _obf_value = value;
            return *this;
        }
    };

    class _obf_timer {
    private:
        std::chrono::high_resolution_clock::time_point _obf_start;
        
    public:
        _obf_timer() : _obf_start(std::chrono::high_resolution_clock::now()) {}
        
        bool _obf_check() {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _obf_start);
            return elapsed.count() < 1000;
        }
    };

    template<size_t N>
    class _obf_buffer {
    private:
        std::array<_obf_byte, N> _obf_data;
        _obf_dword _obf_key;
        
    public:
        _obf_buffer() : _obf_key(_obf_generate_key()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            for (auto& byte : _obf_data) {
                byte = static_cast<_obf_byte>(dis(gen));
            }
        }
        
        _obf_byte operator[](size_t index) const {
            return _obf_data[index] ^ static_cast<_obf_byte>(_obf_key);
        }
    };

    // Если нужно простое шифрование, используйте XOR:
    std::string simpleXor(const std::string& data, const std::string& key) {
        std::string result = data;
        for (size_t i = 0; i < data.size(); ++i) {
            result[i] ^= key[i % key.size()];
        }
        return result;
    }

    class _obf_junk_generator {
    private:
        static const char* _obf_operations[];
        static const size_t _obf_operations_count;
        
    public:
        static std::string generate_junk_code(int complexity) {
            std::string result;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, _obf_operations_count - 1);
            
            for (int i = 0; i < complexity; ++i) {
                result += _obf_operations[dis(gen)];
                result += "\n";
            }
            
            return result;
        }
    };

    // Define junk operations
    const char* _obf_junk_generator::_obf_operations[] = {
        "volatile int _obf_junk_var = 0;",
        "if (_obf_junk_var == 0) { _obf_junk_var = 1; }",
        "for (volatile int i = 0; i < 1; ++i) { }",
        "while (false) { break; }",
        "do { break; } while (false);",
        "switch (0) { case 0: break; }",
        "volatile int _obf_junk_arr[1] = {0};",
        "volatile int* _obf_junk_ptr = nullptr;",
        "volatile int& _obf_junk_ref = _obf_junk_var;",
        "volatile int _obf_junk_func() { return 0; }",
        "volatile int _obf_junk_lambda = [](){ return 0; }();",
        "volatile int _obf_junk_ternary = true ? 0 : 1;",
        "volatile int _obf_junk_bitwise = 0 & 1;",
        "volatile int _obf_junk_shift = 0 << 1;",
        "volatile int _obf_junk_arithmetic = 0 + 1 - 1;"
    };

    const size_t _obf_junk_generator::_obf_operations_count = 
        sizeof(_obf_junk_generator::_obf_operations) / 
        sizeof(_obf_junk_generator::_obf_operations[0]);
}

bool SourceProtection::protect(const std::string& sourcePath, const std::string& outputPath, const ProtectionConfig& config) {
    try {
        // Initial progress update
        if (config.progressCallback) {
            config.progressCallback(10, "Reading source file...");
        }
        
        // Read source file
        std::string sourceCode = readFile(sourcePath);
        if (sourceCode.empty()) {
            if (config.progressCallback) {
                config.progressCallback(0, "Error: Source file is empty");
            }
            return false;
        }

        // Apply obfuscation first
        if (config.obfuscateNames) {
            if (config.progressCallback) {
                config.progressCallback(20, "Applying identifier obfuscation...");
            }
            sourceCode = obfuscateIdentifiers(sourceCode, config.progressCallback);
        } else if (config.progressCallback) {
            // Skip ahead in progress if this step is not performed
            config.progressCallback(50, "Skipping identifier obfuscation...");
        }

        // Apply string encryption after obfuscation
        if (config.encryptStrings && !config.stringsToEncrypt.empty()) {
            if (config.progressCallback) {
                config.progressCallback(60, "Applying string encryption...");
            }
            
            // Add string encryption header
            std::string encryptionHeader = R"(
#include <cstdint>
#include <array>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace _obf_ns {
    using _obf_byte = uint8_t;
    using _obf_word = uint16_t;
    using _obf_dword = uint32_t;
    using _obf_qword = uint64_t;

    // Simple string decryption function
    class _obf_string_decryptor {
    public:
        static std::string decrypt(const char* encoded) {
            // Convert hex string to bytes
            std::string result;
            for (size_t i = 0; encoded[i] && encoded[i+1]; i += 2) {
                char byte = 0;
                for (int j = 0; j < 2; j++) {
                    byte <<= 4;
                    char c = encoded[i+j];
                    if (c >= '0' && c <= '9') byte |= c - '0';
                    else if (c >= 'a' && c <= 'f') byte |= c - 'a' + 10;
                    else if (c >= 'A' && c <= 'F') byte |= c - 'A' + 10;
                }
                result += byte;
            }
            
            if (result.empty()) return "";
            
            // Extract key from first byte
            unsigned char key = static_cast<unsigned char>(result[0]);
            
            // Decrypt each character with simple XOR
            std::string decrypted;
            for (size_t i = 1; i < result.size(); i++) {
                decrypted += static_cast<char>(static_cast<unsigned char>(result[i]) ^ (key + i - 1));
            }
            
            return decrypted;
        }
    };
}

)";
            sourceCode = encryptionHeader + sourceCode;
            
            if (config.progressCallback) {
                config.progressCallback(70, "Generating encrypted strings...");
            }
            
            // Create a map of original strings to their encrypted versions
            std::map<std::string, std::string> encryptedStrings;
            
            // Process only the strings that were selected for encryption
            for (const auto& str : config.stringsToEncrypt) {
                // Skip empty strings
                if (str.empty()) continue;
                
                // Simple encryption: use a random key
                unsigned char key = static_cast<unsigned char>(rand() % 256);
                
                // Encrypt the string
                std::string encrypted;
                encrypted += static_cast<char>(key); // Store key as first byte
                
                // XOR each character with key + position
                for (size_t i = 0; i < str.length(); ++i) {
                    encrypted += static_cast<char>(static_cast<unsigned char>(str[i]) ^ (key + i));
                }
                
                // Convert to hex
                std::stringstream ss;
                for (unsigned char c : encrypted) {
                    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
                }
                
                // Store the encrypted string
                encryptedStrings[str] = ss.str();
            }
            
            if (config.progressCallback) {
                config.progressCallback(80, "Replacing strings in source code...");
            }
            
            // Replace only the selected strings with their encrypted versions
            for (const auto& pair : encryptedStrings) {
                std::string pattern = "\"" + pair.first + "\"";
                std::string replacement = "_obf_ns::_obf_string_decryptor::decrypt(\"" + pair.second + "\")";
                sourceCode = std::regex_replace(sourceCode, std::regex(pattern), replacement);
            }
        } else if (config.progressCallback) {
            // Skip ahead in progress if this step is not performed
            config.progressCallback(80, "Skipping string encryption...");
        }

        // Update progress before writing
        if (config.progressCallback) {
            config.progressCallback(90, "Writing protected source code to file...");
        }

        // Write the protected code to output file
        bool result = writeFile(outputPath, sourceCode);
        
        // Final progress update
        if (config.progressCallback) {
            if (result) {
                config.progressCallback(100, "Protection completed successfully");
            } else {
                config.progressCallback(0, "Failed to write output file");
            }
        }
        
        return result;
    }
    catch (const std::exception& e) {
        if (config.progressCallback) {
            config.progressCallback(0, std::string("Error: ") + e.what());
        }
        return false;
    }
}

std::string SourceProtection::obfuscateIdentifiers(const std::string& sourceCode, const ProgressCallback& callback) {
    // Update progress at the start
    if (callback) {
        callback(25, "Starting identifier obfuscation...");
    }
    
    std::string result = sourceCode;
    
    // Set of C++ keywords and standard library identifiers that should not be obfuscated
    static const std::set<std::string> reservedKeywords = {
        "int", "char", "bool", "void", "float", "double", "long", "short",
        "unsigned", "signed", "const", "volatile", "static", "extern", "register",
        "struct", "class", "union", "enum", "typedef", "namespace", "template",
        "typename", "public", "private", "protected", "virtual", "friend", "using",
        "std", "string", "vector", "map", "set", "auto", "nullptr", "true", "false",
        "return", "if", "else", "while", "for", "do", "switch", "case", "break",
        "continue", "default", "goto", "try", "catch", "throw", "new", "delete",
        "this", "operator", "sizeof", "alignof", "decltype", "typeid", "and", "or",
        "not", "xor", "bitand", "bitor", "compl", "and_eq", "or_eq", "xor_eq",
        "not_eq", "main", "include", "define", "ifdef", "ifndef", "endif", "pragma",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t", "int8_t", "int16_t", "int32_t",
        "int64_t", "size_t", "ptrdiff_t", "nullptr_t", "wchar_t", "char16_t", "char32_t",
        "cout", "cin", "endl", "printf", "scanf", "fopen", "fclose", "malloc", "free"
    };

    // Add expanded header with safe junk code and obfuscation utilities
    std::string obfuscationHeader = R"(
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <chrono>
#define _OBF_CONCAT(a, b) a##b
#define _OBF_STRINGIFY(a) #a
#define _OBF_EXPAND(a) _OBF_STRINGIFY(a)
#define _OBF_RANDOM (rand() % 100)
#define _OBF_CONDITION(a) ((a) > 50 ? 1 : 0)
#define _OBF_JUNK1 do { volatile int _obf_x = rand(); (void)_obf_x; } while(0)
#define _OBF_JUNK2 do { if(rand() < 0) std::cout << "Never prints"; } while(0)
#define _OBF_JUNK3 do { volatile int _obf_arr[1] = {0}; (void)_obf_arr[0]; } while(0)
#define _OBF_JUNK4 do { volatile void* _obf_p = (void*)&_obf_p; (void)_obf_p; } while(0)
#define _OBF_JUNK5 do { volatile auto _obf_t = std::chrono::high_resolution_clock::now(); (void)_obf_t; } while(0)
static void _obf_junk_func1() {
    volatile int x = 1;
    while(x != 0) { x = (x + 1) % 1000; }
}
static int _obf_junk_func2(int x) {
    return x < 0 ? _obf_junk_func2(-x) : x;
}
static void _obf_junk_func3(void* p) {
    if(!p) _obf_junk_func3(&p);
}
static struct _OBF_INIT {
    _OBF_INIT() { srand((unsigned int)time(NULL)); }
} _obf_initializer;
namespace {
    class _obf_dead_class {
    private:
        int value;
    public:
        _obf_dead_class() : value(rand()) {}
        void method() { value = (value * 1103515245 + 12345) & 0x7fffffff; }
    };
    volatile _obf_dead_class _obf_dead_obj;
}
)";

    // Pattern to match function declarations and definitions
    std::regex functionPattern(R"((\b(?:void|int|bool|string|double|float|char|long|short|unsigned|signed|const|static|virtual|inline|explicit|friend|template|typename|class|struct|union|enum|typedef|using|namespace|operator|auto|decltype|std::\w+)\s+)([a-zA-Z_]\w*)\s*\([^)]*\)\s*(?:const\s*)?(?:override\s*)?(?:noexcept\s*)?(?:=\s*0\s*)?(?:;|\{))");

    // Collect function names and generate safe obfuscated names
    std::map<std::string, std::string> identifierMap;
    std::string::const_iterator searchStart(result.cbegin());
    std::smatch match;
    
    // First pass: collect function names only, no variables
    while (std::regex_search(searchStart, result.cend(), match, functionPattern)) {
        std::string functionName = match[2].str();
        
        // Skip if it's a reserved keyword, main function, or already obfuscated
        if (reservedKeywords.find(functionName) != reservedKeywords.end() ||
            functionName == "main" ||  // Always preserve main
            functionName.find("_obf_") == 0) {
            searchStart = match.suffix().first;
            continue;
        }

        // Generate new obfuscated name if not already mapped
        if (identifierMap.find(functionName) == identifierMap.end()) {
            std::string newName = "_obf_func_" + generateRandomName(6);
            identifierMap[functionName] = newName;
        }
        
        searchStart = match.suffix().first;
    }

    // Replace function names
    for (const auto& pair : identifierMap) {
        // Only replace complete identifiers (with word boundaries)
        std::string pattern = "\\b" + pair.first + "\\b";
        result = std::regex_replace(result, std::regex(pattern), pair.second);
    }
    
    // List of safe junk code chunks that can be inserted without breaking functionality
    std::vector<std::string> junkCodeBlocks = {
        "    _OBF_JUNK1;\n",
        "    _OBF_JUNK2;\n",
        "    _OBF_JUNK3;\n",
        "    _OBF_JUNK4;\n",
        "    _OBF_JUNK5;\n",
        "    if (_OBF_RANDOM > 1000) { _obf_junk_func1(); }\n",
        "    { volatile int _obf_tmp = _OBF_RANDOM; (void)_obf_tmp; }\n",
        "    { volatile bool _obf_b = (_OBF_RANDOM > _OBF_RANDOM); (void)_obf_b; }\n",
        "    for (volatile int _obf_i = 0; _obf_i < 0; ++_obf_i) { _OBF_JUNK1; }\n",
        "    { void* _obf_ptr = nullptr; if (_obf_ptr) _obf_junk_func3(_obf_ptr); }\n",
        "    switch (_OBF_RANDOM % 2) { case 0: case 1: break; default: _OBF_JUNK2; }\n",
        "    { struct { int x, y; } _obf_s = {0, 0}; (void)_obf_s; }\n",
        "    { volatile auto _obf_lambda = [](){ return _OBF_RANDOM; }; (void)_obf_lambda; }\n",
        "    { typedef int _obf_typedef; volatile _obf_typedef _obf_var = 0; (void)_obf_var; }\n",
        "    { volatile int _obf_enc = _OBF_RANDOM ^ 0xFF; (void)_obf_enc; }\n"
    };
    
    // Process the code line by line to add junk code and comments
    std::stringstream finalCode;
    finalCode << obfuscationHeader;
    std::istringstream iss(result);
    std::string line;
    int lineCount = 0;
    int braceCount = 0;
    bool inFunction = false;
    
    while (std::getline(iss, line)) {
        lineCount++;
        
        // Count braces to detect function bodies
        for (char c : line) {
            if (c == '{') braceCount++;
            if (c == '}') braceCount--;
        }
        
        // Detect function start
        if (line.find('{') != std::string::npos && !inFunction && braceCount > 0) {
            inFunction = true;
            finalCode << line << "\n";
            
            // Insert junk code at function start (3-5 junk lines)
            int junkLines = 3 + (rand() % 3);
            for (int i = 0; i < junkLines; i++) {
                finalCode << junkCodeBlocks[rand() % junkCodeBlocks.size()];
            }
            continue;
        }
        
        // Detect function end
        if (line.find('}') != std::string::npos && inFunction && braceCount <= 0) {
            inFunction = false;
            
            // Insert junk code before function end (1-2 junk lines)
            int junkLines = 1 + (rand() % 2);
            for (int i = 0; i < junkLines; i++) {
                finalCode << junkCodeBlocks[rand() % junkCodeBlocks.size()];
            }
            finalCode << line << "\n";
            continue;
        }
        
        finalCode << line << "\n";
        
        // Add junk code inside function body randomly (~10% chance per line)
        if (inFunction && braceCount > 0 && (rand() % 10 == 0)) {
            finalCode << junkCodeBlocks[rand() % junkCodeBlocks.size()];
        }
    }

    // Update progress
    if (callback) {
        callback(50, "Code obfuscated successfully with advanced techniques");
    }

    return finalCode.str();
}

std::string SourceProtection::addJunkCode(const std::string& sourceCode, int amount, const ProgressCallback& callback) {
    // Implement junk code insertion logic here
    return sourceCode;
}

std::string SourceProtection::insertAntiDebugChecks(const std::string& sourceCode, const ProgressCallback& callback) {
    // Implement anti-debugging checks here
    return sourceCode;
}

std::string SourceProtection::virtualizeCode(const std::string& sourceCode, const ProgressCallback& callback) {
    // Implement code virtualization logic here
    return sourceCode;
}

std::string SourceProtection::generateRandomName(int length) {
    static const char charset[] = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    
    std::string result;
    result.reserve(length);
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    
    for (int i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
    
    return result;
} 