#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <Windows.h>
#include <QSettings>
#include <QDebug>
#include <functional>

class ExeProtection {
public:
    // Progress callback type
    using ProgressCallback = std::function<void(int progress, const std::string& status)>;
    
    struct ProtectionConfig {
        bool useUPX = true;               // Use UPX packing
        ProgressCallback progressCallback; // Progress callback function
    };

    static bool protect(const std::string& exePath, 
                       const std::string& outputPath, 
                       const ProtectionConfig& config);

private:
    // UPX packing
    static bool packWithUPX(const std::string& exePath, const std::string& outputPath, const ProgressCallback& callback);
    
    // Helper functions
    static std::vector<uint8_t> readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::vector<uint8_t>& data);
    static bool isValidPE(const std::vector<uint8_t>& data);
}; 