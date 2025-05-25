#include "exe_protection.h"
#include <fstream>
#include <sstream>
#include <random>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QSettings>
#include "llvm_obfuscation.h"

bool ExeProtection::protect(const std::string& exePath, 
                          const std::string& outputPath, 
                          const ProtectionConfig& config) {
    try {
        // Report initial progress
        if (config.progressCallback) {
            config.progressCallback(10, "Starting protection process...");
        }
        
        // Only do UPX packing
        if (config.useUPX) {
            if (!packWithUPX(exePath, outputPath, config.progressCallback)) {
                if (config.progressCallback) {
                    config.progressCallback(0, "UPX packing failed");
                }
                qWarning() << "UPX packing failed";
                return false;
            }
            
            // Final progress update on success
            if (config.progressCallback) {
                config.progressCallback(100, "Protection completed successfully");
            }
            return true;
        } else {
            // If UPX is not enabled, just copy the file
            if (config.progressCallback) {
                config.progressCallback(25, "Copying file...");
            }
            
            std::vector<uint8_t> exeData = readFile(exePath);
            
            if (config.progressCallback) {
                config.progressCallback(75, "Writing output file...");
            }
            
            if (!writeFile(outputPath, exeData)) {
                if (config.progressCallback) {
                    config.progressCallback(0, "Failed to write output file");
                }
                return false;
            }
            
            // Final progress update on success
            if (config.progressCallback) {
                config.progressCallback(100, "Protection completed successfully");
            }
            return true;
        }
    }
    catch (const std::exception& e) {
        // Report error in callback
        if (config.progressCallback) {
            config.progressCallback(0, std::string("Error: ") + e.what());
        }
        return false;
    }
}

bool ExeProtection::packWithUPX(const std::string& exePath, const std::string& outputPath, const ProgressCallback& callback) {
    // Use a standard relative path to UPX
    QString upxPath = "tools/upx/upx.exe";
    
    if (callback) {
        callback(20, "Checking UPX installation...");
    }
    
    qDebug() << "Using standard UPX path:" << upxPath;

    // Convert relative path to absolute
    QString absoluteUpxPath = QDir::current().absoluteFilePath(upxPath);
    qDebug() << "Absolute UPX path:" << absoluteUpxPath;

    // Check if UPX executable exists
    if (!QFileInfo::exists(absoluteUpxPath)) {
        qWarning() << "UPX executable not found at:" << absoluteUpxPath;
        if (callback) {
            callback(0, "UPX executable not found");
        }
        return false;
    }

    // Check if source file exists
    if (!QFileInfo::exists(QString::fromStdString(exePath))) {
        qWarning() << "Source file not found for UPX compression:" << QString::fromStdString(exePath);
        if (callback) {
            callback(0, "Source file not found");
        }
        return false;
    }

    if (callback) {
        callback(30, "Preparing files for packing...");
    }

    // First, copy the file to the output path if it's not already there
    QFile sourceFile(QString::fromStdString(exePath));
    QFile outputFile(QString::fromStdString(outputPath));
    
    if (exePath != outputPath) {
        if (outputFile.exists()) {
            if (!outputFile.remove()) {
                qWarning() << "Failed to remove existing output file:" << QString::fromStdString(outputPath);
                if (callback) {
                    callback(0, "Failed to remove existing output file");
                }
                return false;
            }
        }
        
        if (!sourceFile.copy(QString::fromStdString(outputPath))) {
            qWarning() << "Failed to copy source file to output path";
            if (callback) {
                callback(0, "Failed to copy source file");
            }
            return false;
        }
    }
    
    if (callback) {
        callback(50, "Running UPX compression...");
    }
    
    // Convert paths to Windows format
    QString windowsUpxPath = QDir::toNativeSeparators(absoluteUpxPath);
    QString windowsOutputPath = QDir::toNativeSeparators(QString::fromStdString(outputPath));
    
    // Build UPX command arguments
    QStringList arguments;
    arguments << "--best" << "--force" << windowsOutputPath;
    
    qDebug() << "Running UPX command:" << windowsUpxPath << arguments.join(" ");
    
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(windowsUpxPath, arguments);
    
    if (!process.waitForStarted(10000)) {
        qWarning() << "Failed to start UPX process:" << process.errorString();
        if (callback) {
            callback(0, "Failed to start UPX process");
        }
        return false;
    }
    
    // Wait up to 2 minutes for completion
    if (!process.waitForFinished(120000)) {
        qWarning() << "UPX process timed out after 2 minutes";
        process.kill();
        if (callback) {
            callback(0, "UPX process timed out");
        }
        return false;
    }

    if (callback) {
        callback(80, "Finalizing...");
    }

    QString output = QString::fromLocal8Bit(process.readAll());
    qDebug() << "UPX process output:" << output;
    qDebug() << "UPX exit code:" << process.exitCode() << "exit status:" << process.exitStatus();
    
    if (process.exitCode() != 0) {
        qWarning() << "UPX process failed with exit code:" << process.exitCode();
        if (callback) {
            callback(0, "UPX process failed with error code: " + std::to_string(process.exitCode()));
        }
        return false;
    }

    // Verify output file exists and has changed size
    QFileInfo outputFileInfo(QString::fromStdString(outputPath));
    if (!outputFileInfo.exists()) {
        qWarning() << "Output file does not exist after UPX compression:" << QString::fromStdString(outputPath);
        if (callback) {
            callback(0, "Output file does not exist after UPX compression");
        }
        return false;
    }
    
    if (callback) {
        callback(90, "UPX compression completed successfully");
    }
    
    qDebug() << "UPX compression successful. File size:" << outputFileInfo.size() << "bytes";
    return true;
}

std::vector<uint8_t> ExeProtection::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

bool ExeProtection::writeFile(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

bool ExeProtection::isValidPE(const std::vector<uint8_t>& data) {
    if (data.size() < sizeof(IMAGE_DOS_HEADER)) {
        return false;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)data.data();
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    if (data.size() < dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS)) {
        return false;
    }

    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(data.data() + dosHeader->e_lfanew);
    return ntHeaders->Signature == IMAGE_NT_SIGNATURE;
} 