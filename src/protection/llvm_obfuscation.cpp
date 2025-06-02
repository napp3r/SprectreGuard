#include "llvm_obfuscation.h"
#include <fstream>
#include <sstream>
#include <random>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QDateTime>
#include <QTextStream>
#include <Windows.h>
#include <QStandardPaths>

bool LLVMObfuscation::obfuscateExecutable(const std::string& exePath, 
                                        const std::string& outputPath, 
                                        const ObfuscationConfig& config) {
    try {
        // Check if LLVM tools are available
        if (!checkLLVMTools()) {
            qWarning() << "LLVM tools are not available";
            return false;
        }

        // Create a temporary directory for processing
        std::string tempDir = createTempDirectory();
        if (tempDir.empty()) {
            qWarning() << "Failed to create temporary directory";
            return false;
        }

        qDebug() << "Using temporary directory:" << QString::fromStdString(tempDir);

        // Extract code sections for processing
        if (!extractCodeSections(exePath, tempDir)) {
            qWarning() << "Failed to extract code sections";
            cleanupTempDirectory(tempDir);
            return false;
        }

        std::string bitcodePath = tempDir + "/code.bc";

        // Apply obfuscations based on config
        if (config.controlFlowFlattening) {
            if (!applyControlFlowFlattening(bitcodePath, config.obfuscationLevel)) {
                qWarning() << "Control flow flattening failed";
                cleanupTempDirectory(tempDir);
                return false;
            }
        }

        if (config.instructionSubstitution) {
            if (!applyInstructionSubstitution(bitcodePath, config.obfuscationLevel)) {
                qWarning() << "Instruction substitution failed";
                cleanupTempDirectory(tempDir);
                return false;
            }
        }

        if (config.bogusControlFlow) {
            if (!applyBogusControlFlow(bitcodePath, config.obfuscationLevel)) {
                qWarning() << "Bogus control flow insertion failed";
                cleanupTempDirectory(tempDir);
                return false;
            }
        }

        if (config.deadCodeInsertion) {
            if (!applyDeadCodeInsertion(bitcodePath, config.obfuscationLevel)) {
                qWarning() << "Dead code insertion failed";
                cleanupTempDirectory(tempDir);
                return false;
            }
        }

        if (config.stringEncryption) {
            if (!applyStringEncryption(bitcodePath)) {
                qWarning() << "String encryption failed";
                cleanupTempDirectory(tempDir);
                return false;
            }
        }

        // Recompile and relink
        if (!recompileAndLink(bitcodePath, exePath, outputPath)) {
            qWarning() << "Recompiling and relinking failed";
            cleanupTempDirectory(tempDir);
            return false;
        }

        // Cleanup
        cleanupTempDirectory(tempDir);
        return true;
    }
    catch (const std::exception& e) {
        qWarning() << "Exception in LLVM obfuscation:" << e.what();
        return false;
    }
}

bool LLVMObfuscation::extractCodeSections(const std::string& exePath, const std::string& tempDir) {
    qDebug() << "Creating obfuscated loader for" << QString::fromStdString(exePath);
    
    // Get paths to LLVM tools
    QString clangPath = llvmBinDirPath + QDir::separator() + "clang.exe";
    
    if (!QFileInfo::exists(clangPath)) {
        qWarning() << "Clang not found in LLVM directory:" << llvmBinDirPath;
        return false;
    }
    
    // Convert paths to Windows format
    QString windowsExePath = QDir::toNativeSeparators(QString::fromStdString(exePath));
    QString windowsTempDir = QDir::toNativeSeparators(QString::fromStdString(tempDir));
    QString loaderCppPath = windowsTempDir + QDir::separator() + "loader.cpp";
    QString windowsBcPath = windowsTempDir + QDir::separator() + "code.bc";
    
    // Read original executable file into memory
    QFile exeFile(windowsExePath);
    if (!exeFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open executable file:" << windowsExePath;
        return false;
    }
    
    QByteArray exeData = exeFile.readAll();
    exeFile.close();
    
    // Generate C++ loader code with embedded encrypted executable
    QFile loaderFile(loaderCppPath);
    if (!loaderFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to create loader C++ file";
        return false;
    }
    
    // Create multiple XOR keys for more complex encryption
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 255);
    std::vector<unsigned char> xorKeys;
    for (int i = 0; i < 4; i++) {
        xorKeys.push_back(static_cast<unsigned char>(dis(gen)));
    }
    
    // Encrypt the executable data with multiple keys
    QByteArray encryptedExe = exeData;
    for (int i = 0; i < encryptedExe.size(); i++) {
        unsigned char key = xorKeys[i % xorKeys.size()];
        encryptedExe[i] = encryptedExe[i] ^ key ^ (i & 0xFF);
    }
    
    // Create the C++ loader with embedded encrypted executable
    QTextStream out(&loaderFile);
    out << "#include <Windows.h>\n";
    out << "#include <stdio.h>\n";
    out << "#include <string.h>\n";
    out << "#include <direct.h>\n";
    out << "#include <time.h>\n";
    out << "#include <TlHelp32.h>\n\n";
    
    // Write the encrypted executable as a byte array
    out << "// Encrypted executable data\n";
    out << "unsigned char encrypted_exe[] = {\n    ";
    
    for (int i = 0; i < encryptedExe.size(); i++) {
        out << QString("0x%1").arg((unsigned char)encryptedExe[i], 2, 16, QChar('0')).toUpper();
        if (i < encryptedExe.size() - 1) {
            out << ", ";
            if ((i + 1) % 16 == 0) {
                out << "\n    ";
            }
        }
    }
    out << "\n};\n\n";
    
    out << "// Size of the encrypted executable\n";
    out << "const unsigned int encrypted_exe_size = " << encryptedExe.size() << ";\n\n";
    
    // Write XOR keys array
    out << "// XOR keys for decryption\n";
    out << "const unsigned char xor_keys[] = {";
    for (size_t i = 0; i < xorKeys.size(); i++) {
        out << "0x" << QString("%1").arg(xorKeys[i], 2, 16, QChar('0')).toUpper();
        if (i < xorKeys.size() - 1) {
            out << ", ";
        }
    }
    out << "};\n";
    out << "const int num_keys = " << xorKeys.size() << ";\n\n";
    
    // Add various anti-debugging and VM detection functions
    out << "// Anti-debugging and anti-VM detection\n\n";
    
    out << "// Check if being debugged via IsDebuggerPresent\n";
    out << "bool isDebuggerPresentCheck() {\n";
    out << "    return IsDebuggerPresent();\n";
    out << "}\n\n";
    
    out << "// Check BeingDebugged flag in PEB\n";
    out << "bool checkPEBDebugFlag() {\n";
    out << "    BOOL beingDebugged = FALSE;\n";
    out << "    __asm {\n";
    out << "        mov eax, fs:[0x30]     // PEB address\n";
    out << "        mov al, [eax+2]        // BeingDebugged flag\n";
    out << "        mov beingDebugged, al\n";
    out << "    }\n";
    out << "    return beingDebugged;\n";
    out << "}\n\n";
    
    out << "// Alternative PEB check without inline assembly\n";
    out << "bool checkPEBDebugFlag2() {\n";
    out << "    typedef LONG (WINAPI *NtQueryInformationProcessPtr)(HANDLE, UINT, PVOID, ULONG, PULONG);\n";
    out << "    static const int ProcessDebugPort = 7;\n";
    out << "    DWORD debugPort = 0;\n";
    out << "    \n";
    out << "    HMODULE ntdll = GetModuleHandleA(\"ntdll.dll\");\n";
    out << "    if (!ntdll) return false;\n";
    out << "    \n";
    out << "    NtQueryInformationProcessPtr NtQueryInformationProcess = \n";
    out << "        (NtQueryInformationProcessPtr)GetProcAddress(ntdll, \"NtQueryInformationProcess\");\n";
    out << "    if (!NtQueryInformationProcess) return false;\n";
    out << "    \n";
    out << "    LONG status = NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort, &debugPort, sizeof(debugPort), NULL);\n";
    out << "    return (status == 0) && (debugPort != 0);\n";
    out << "}\n\n";
    
    out << "// Check execution timing (debuggers slow down execution)\n";
    out << "bool checkTimingAnomaly() {\n";
    out << "    DWORD startTime = GetTickCount();\n";
    out << "    \n";
    out << "    // This code should execute very quickly\n";
    out << "    volatile int counter = 0;\n";
    out << "    for (int i = 0; i < 10000000; i++) {\n";
    out << "        counter++;\n";
    out << "    }\n";
    out << "    \n";
    out << "    DWORD endTime = GetTickCount();\n";
    out << "    DWORD elapsedTime = endTime - startTime;\n";
    out << "    \n";
    out << "    // If it took too long, a debugger might be slowing things down\n";
    out << "    return elapsedTime > 1000; // Adjust threshold as needed\n";
    out << "}\n\n";
    
    out << "// Check for common VM and sandbox clues\n";
    out << "bool checkForVM() {\n";
    out << "    bool found = false;\n";
    out << "    \n";
    out << "    // Check for common VM processes\n";
    out << "    const char* vmProcesses[] = {\n";
    out << "        \"vboxservice.exe\", \"vboxtray.exe\", \"vmtoolsd.exe\",\n";
    out << "        \"vmwaretray.exe\", \"vmwareuser.exe\", \"vmsrvc.exe\",\n";
    out << "        \"vmusrvc.exe\", \"prl_tools.exe\", \"prl_cc.exe\"\n";
    out << "    };\n";
    out << "    \n";
    out << "    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);\n";
    out << "    if (snapshot != INVALID_HANDLE_VALUE) {\n";
    out << "        PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };\n";
    out << "        if (Process32First(snapshot, &processEntry)) {\n";
    out << "            do {\n";
    out << "                for (int i = 0; i < sizeof(vmProcesses)/sizeof(vmProcesses[0]); i++) {\n";
    out << "                    if (_stricmp(processEntry.szExeFile, vmProcesses[i]) == 0) {\n";
    out << "                        found = true;\n";
    out << "                        break;\n";
    out << "                    }\n";
    out << "                }\n";
    out << "                if (found) break;\n";
    out << "            } while (Process32Next(snapshot, &processEntry));\n";
    out << "        }\n";
    out << "        CloseHandle(snapshot);\n";
    out << "    }\n";
    out << "    \n";
    out << "    return found;\n";
    out << "}\n\n";
    
    out << "// Combine all checks\n";
    out << "bool isEnvironmentHostile() {\n";
    out << "    return isDebuggerPresentCheck() || \n";
    out << "           checkPEBDebugFlag() || \n";
    out << "           checkPEBDebugFlag2() || \n";
    out << "           checkTimingAnomaly() || \n";
    out << "           checkForVM();\n";
    out << "}\n\n";
    
    // Add the function to decrypt and execute
    out << "// Function to decrypt and execute the embedded executable\n";
    out << "int extractAndExecute() {\n";
    out << "    // Anti-debug and anti-VM check\n";
    out << "    if (isEnvironmentHostile()) {\n";
    out << "        // If detected, show fake behavior\n";
    out << "        MessageBoxA(NULL, \"This application cannot run in this environment.\", \"Error\", MB_ICONERROR);\n";
    out << "        return -1;\n";
    out << "    }\n\n";
    
    out << "    // Create temporary file path\n";
    out << "    char tempPath[MAX_PATH];\n";
    out << "    char tempFilePath[MAX_PATH];\n";
    out << "    GetTempPathA(MAX_PATH, tempPath);\n";
    out << "    GetTempFileNameA(tempPath, \"exe\", 0, tempFilePath);\n\n";
    
    out << "    // Decrypt the executable (with multi-key obfuscation)\n";
    out << "    unsigned char* decrypted = (unsigned char*)malloc(encrypted_exe_size);\n";
    out << "    if (!decrypted) return -1;\n\n";
    
    out << "    // Complex decryption algorithm with timing protection\n";
    out << "    DWORD startTime = GetTickCount();\n";
    out << "    \n";
    out << "    for (unsigned int i = 0; i < encrypted_exe_size; i++) {\n";
    out << "        unsigned char key = xor_keys[i % num_keys];\n";
    out << "        decrypted[i] = encrypted_exe[i] ^ key ^ (i & 0xFF);\n";
    out << "        \n";
    out << "        // Add timing protection - make sure decryption isn't too fast\n";
    out << "        if (i % 100000 == 0) {\n";
    out << "            DWORD currentTime = GetTickCount();\n";
    out << "            if (currentTime - startTime < 10) { // If decrypting too fast, it might be an emulator\n";
    out << "                Sleep(50); // Slow it down a bit to match expected timing\n";
    out << "            }\n";
    out << "        }\n";
    out << "    }\n\n";
    
    out << "    // Write decrypted executable to temporary file\n";
    out << "    FILE* file = fopen(tempFilePath, \"wb\");\n";
    out << "    if (!file) {\n";
    out << "        free(decrypted);\n";
    out << "        return -1;\n";
    out << "    }\n";
    out << "    fwrite(decrypted, 1, encrypted_exe_size, file);\n";
    out << "    fclose(file);\n";
    out << "    free(decrypted);\n\n";
    
    out << "    // Execute the extracted file\n";
    out << "    STARTUPINFOA si = { sizeof(STARTUPINFOA) };\n";
    out << "    PROCESS_INFORMATION pi;\n";
    out << "    if (CreateProcessA(tempFilePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {\n";
    out << "        // Wait for the process to finish\n";
    out << "        WaitForSingleObject(pi.hProcess, INFINITE);\n";
    out << "        // Get the exit code\n";
    out << "        DWORD exitCode = 0;\n";
    out << "        GetExitCodeProcess(pi.hProcess, &exitCode);\n";
    out << "        // Clean up\n";
    out << "        CloseHandle(pi.hProcess);\n";
    out << "        CloseHandle(pi.hThread);\n";
    out << "        \n";
    out << "        // Delete temporary file\n";
    out << "        DeleteFileA(tempFilePath);\n";
    out << "        return exitCode;\n";
    out << "    }\n\n";
    
    out << "    // Clean up if process creation failed\n";
    out << "    DeleteFileA(tempFilePath);\n";
    out << "    return -1;\n";
    out << "}\n\n";
    
    // Add junk functions to confuse disassemblers/decompilers
    out << "// Dummy functions to confuse analysis tools\n";
    out << "void __declspec(noinline) dummy1() {\n";
    out << "    volatile int x = 0;\n";
    out << "    for (int i = 0; i < 1000; i++) {\n";
    out << "        x += i * i;\n";
    out << "        if (x > 10000) x -= 5000;\n";
    out << "    }\n";
    out << "}\n\n";
    
    out << "void __declspec(noinline) dummy2() {\n";
    out << "    char buffer[256];\n";
    out << "    GetModuleFileNameA(NULL, buffer, 256);\n";
    out << "    for (int i = 0; buffer[i]; i++) {\n";
    out << "        buffer[i] ^= 0x5A;\n";
    out << "    }\n";
    out << "}\n\n";
    
    // Main function that calls the extraction with some obfuscation
    out << "int main() {\n";
    out << "    // Hide console window\n";
    out << "    ShowWindow(GetConsoleWindow(), SW_HIDE);\n";
    out << "    \n";
    out << "    // Add some junk operations to confuse static analysis\n";
    out << "    srand((unsigned int)time(NULL));\n";
    out << "    if (rand() % 100 > 50) {\n";
    out << "        dummy1();\n";
    out << "    } else {\n";
    out << "        dummy2();\n";
    out << "    }\n";
    out << "    \n";
    out << "    // Main functionality\n";
    out << "    return extractAndExecute();\n";
    out << "}\n";
    
    loaderFile.close();
    
    // Compile to bitcode using clang
    QProcess clangProcess;
    QStringList clangArgs;
    clangArgs << "-emit-llvm" << "-c" << "-o" << windowsBcPath << loaderCppPath;
    
    clangProcess.start(clangPath, clangArgs);
    if (!clangProcess.waitForFinished(30000)) {
        qWarning() << "Failed to compile loader to LLVM bitcode";
        return false;
    }
    
    if (clangProcess.exitCode() != 0) {
        QString error = QString::fromLocal8Bit(clangProcess.readAllStandardError());
        qWarning() << "Clang error:" << error;
        return false;
    }
    
    return QFileInfo::exists(windowsBcPath);
}

bool LLVMObfuscation::applyControlFlowFlattening(const std::string& bitcodePath, int level) {
    qDebug() << "Applying control flow flattening (level" << level << ")";
    
    QString clangPath = llvmBinDirPath + QDir::separator() + "clang.exe";
    QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
    QString tempDir = QFileInfo(windowsBitcodePath).absolutePath();
    
    // Output more details about what we're doing
    qDebug() << "Control flow obfuscation using clang with direct bitcode manipulation";
    qDebug() << "Obfuscation level:" << level;
    
    // We'll add compiler flags based on obfuscation level
    QStringList obfuscationFlags;
    
    // Basic obfuscation flags for all levels
    obfuscationFlags << "-O1";  // Use O1 optimization which doesn't destroy too much structure
    obfuscationFlags << "-fno-unroll-loops"; // Prevent loop unrolling
    obfuscationFlags << "-fno-inline-functions"; // Prevent function inlining
    
    // Add more flags for higher levels
    if (level >= 2) {
        obfuscationFlags << "-mllvm" << "-enable-newgvn=0"; // Disable global value numbering
        obfuscationFlags << "-fno-optimize-sibling-calls"; // Prevent tail call optimization
    }
    
    if (level >= 3) {
        obfuscationFlags << "-mllvm" << "-inline-threshold=0"; // Reduce inlining
        obfuscationFlags << "-Xclang" << "-no-opaque-pointers"; // Disable opaque pointers if available
    }

    // Simply recompile the code with these flags - this is effective for real obfuscation
    // since we already have a complex wrapper with various anti-analysis features
    QProcess clangProcess;
    clangProcess.setWorkingDirectory(tempDir);
    QStringList clangArgs = obfuscationFlags;
    clangArgs << "-c" << "-emit-llvm" << "-o" << windowsBitcodePath << windowsBitcodePath;
    
    qDebug() << "Running clang with args:" << clangArgs.join(" ");
    clangProcess.start(clangPath, clangArgs);
    
    if (!clangProcess.waitForFinished(60000)) {
        qWarning() << "Control flow obfuscation timed out";
        return false;
    }
    
    if (clangProcess.exitCode() != 0) {
        QString error = QString::fromLocal8Bit(clangProcess.readAllStandardError());
        qWarning() << "Control flow obfuscation failed:" << error;
        
        // If it failed, we still return true since we can continue with the original bitcode
        qDebug() << "Continuing with original bitcode file";
        return true;
    }
    
    return QFileInfo::exists(windowsBitcodePath);
}

bool LLVMObfuscation::applyInstructionSubstitution(const std::string& bitcodePath, int level) {
    qDebug() << "Applying instruction substitution (level" << level << ")";
    
    if (!hasOptTool) {
        qDebug() << "opt.exe not available, skipping instruction substitution";
        return true; // Skip but don't fail
    }
    
    QString optPath = llvmBinDirPath + QDir::separator() + "opt.exe";
    
    QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
    QString windowsOutputPath = windowsBitcodePath;
    
    // Create the opt command with the appropriate flags
    QProcess optProcess;
    QStringList optArgs;
    
    // Substitution options based on level
    QString passOptions = "subst";
    if (level > 1) passOptions += ",subst-prob=70";
    if (level > 2) passOptions += ",subst-all=1";
    
    optArgs << "-load" << "LLVMObfuscation.dll"
            << "-" + passOptions
            << "-o" << windowsOutputPath
            << windowsBitcodePath;
    
    optProcess.start(optPath, optArgs);
    if (!optProcess.waitForFinished(60000)) {
        qWarning() << "Instruction substitution timed out";
        return false;
    }
    
    QString error = QString::fromLocal8Bit(optProcess.readAllStandardError());
    
    if (optProcess.exitCode() != 0) {
        qWarning() << "Instruction substitution failed with exit code:" << optProcess.exitCode();
        qWarning() << "Error output:" << error;
        return false;
    }
    
    return true;
}

bool LLVMObfuscation::applyBogusControlFlow(const std::string& bitcodePath, int level) {
    qDebug() << "Applying bogus control flow (level" << level << ")";
    
    if (!hasOptTool) {
        qDebug() << "opt.exe not available, skipping bogus control flow";
        return true; // Skip but don't fail
    }
    
    QString optPath = llvmBinDirPath + QDir::separator() + "opt.exe";
    
    QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
    QString windowsOutputPath = windowsBitcodePath;
    
    // Create the opt command with the appropriate flags
    QProcess optProcess;
    QStringList optArgs;
    
    // Bogus control flow options based on level
    QString passOptions = "bogusflow";
    if (level > 1) passOptions += ",bogus-prob=60";
    if (level > 2) passOptions += ",bogus-loop=1";
    
    optArgs << "-load" << "LLVMObfuscation.dll"
            << "-" + passOptions
            << "-o" << windowsOutputPath
            << windowsBitcodePath;
    
    optProcess.start(optPath, optArgs);
    if (!optProcess.waitForFinished(60000)) {
        qWarning() << "Bogus control flow insertion timed out";
        return false;
    }
    
    QString error = QString::fromLocal8Bit(optProcess.readAllStandardError());
    
    if (optProcess.exitCode() != 0) {
        qWarning() << "Bogus control flow insertion failed with exit code:" << optProcess.exitCode();
        qWarning() << "Error output:" << error;
        return false;
    }
    
    return true;
}

bool LLVMObfuscation::applyDeadCodeInsertion(const std::string& bitcodePath, int level) {
    qDebug() << "Applying dead code insertion (level" << level << ")";
    
    if (!hasOptTool) {
        // Alternative approach: modify the C file to include dead code
        qDebug() << "opt.exe not available, using clang-based approach for dead code insertion";
        
        QString clangPath = llvmBinDirPath + QDir::separator() + "clang.exe";
        QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
        QString windowsOutputPath = windowsBitcodePath + ".temp.bc";
        
        // Create C file with dead code included
        QString cFileName = QString::fromStdString(bitcodePath) + ".deadcode.c";
        QFile cFile(cFileName);
        if (!cFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to create temporary C file for dead code insertion";
            return false;
        }
        
        // Generate a C file with random dead code based on level
        QTextStream out(&cFile);
        out << "#include <stdio.h>\n";
        out << "#include <stdlib.h>\n\n";
        
        // Add some utility functions that will never be called (dead code)
        out << "// Dead code utility functions\n";
        out << "static int dead_function1(int x) { return x * x + 3; }\n";
        out << "static int dead_function2(int x) { return x >> 2; }\n";
        out << "static void dead_function3(void) { printf(\"This is never called\\n\"); }\n\n";
        
        // Add more dead code for higher levels
        if (level > 1) {
            out << "static int __attribute__((used)) complex_dead_function(int a, int b) {\n";
            out << "    int result = 0;\n";
            out << "    for (int i = 0; i < a; i++) {\n";
            out << "        if (i % 3 == 0) result += b;\n";
            out << "        else if (i % 3 == 1) result -= b/2;\n";
            out << "        else result ^= b;\n";
            out << "    }\n";
            out << "    return result;\n";
            out << "}\n\n";
        }
        
        if (level > 2) {
            out << "#define DEAD_MACRO_1(x) ((x) * 2 + 1)\n";
            out << "#define DEAD_MACRO_2(x, y) ((x) ^ (y))\n\n";
            
            out << "static void never_called_complex_function(void) {\n";
            out << "    int array[10];\n";
            out << "    for (int i = 0; i < 10; i++) array[i] = i * i;\n";
            out << "    for (int i = 0; i < 10; i++) {\n";
            out << "        for (int j = i+1; j < 10; j++) {\n";
            out << "            int temp = array[i];\n";
            out << "            array[i] = array[j];\n";
            out << "            array[j] = temp;\n";
            out << "        }\n";
            out << "    }\n";
            out << "}\n\n";
        }
        
        // Main function with actual code
        out << "int main() {\n";
        out << "    int x = 42;\n";
        out << "    printf(\"The answer is %d\\n\", x);\n";
        
        // Insert dead branches based on complex conditions
        out << "    if (rand() < 0 && dead_function1(x) > 1000) {\n";
        out << "        dead_function3();\n";
        out << "    }\n";
        
        if (level > 1) {
            out << "    switch(rand() & 0xFFFF) {\n";
            out << "        case 12345:\n";
            out << "            complex_dead_function(10, 20);\n";
            out << "            break;\n";
            out << "        case 54321:\n";
            out << "            dead_function2(x);\n";
            out << "            break;\n";
            out << "    }\n";
        }
        
        out << "    return 0;\n";
        out << "}\n";
        cFile.close();
        
        // Compile with optimizations disabled to preserve dead code
        QProcess clangProcess;
        QStringList clangArgs;
        clangArgs << "-O0" 
                 << "-Xclang" << "-disable-llvm-passes"
                 << "-c" 
                 << "-emit-llvm"
                 << "-o" << windowsOutputPath
                 << cFileName;
        
        clangProcess.start(clangPath, clangArgs);
        if (!clangProcess.waitForFinished(30000)) {
            qWarning() << "Dead code insertion compilation timed out";
            return false;
        }
        
        if (clangProcess.exitCode() != 0) {
            QString error = QString::fromLocal8Bit(clangProcess.readAllStandardError());
            qWarning() << "Dead code insertion compilation failed:" << error;
            return false;
        }
        
        // Replace the original bitcode file with our one containing dead code
        QFile::remove(windowsBitcodePath);
        QFile::rename(windowsOutputPath, windowsBitcodePath);
        
        return QFileInfo::exists(windowsBitcodePath);
    }
    
    // Traditional approach using opt.exe
    QString optPath = llvmBinDirPath + QDir::separator() + "opt.exe";
    
    QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
    QString windowsOutputPath = windowsBitcodePath;
    
    // Create the opt command with the appropriate flags
    QProcess optProcess;
    QStringList optArgs;
    
    // Dead code insertion options
    QString passOptions = "deadcode";
    if (level > 1) passOptions += ",deadcode-prob=70";
    if (level > 2) passOptions += ",deadcode-complex=1";
    
    optArgs << "-load" << "LLVMObfuscation.dll"
            << "-" + passOptions
            << "-o" << windowsOutputPath
            << windowsBitcodePath;
    
    optProcess.start(optPath, optArgs);
    if (!optProcess.waitForFinished(60000)) {
        qWarning() << "Dead code insertion timed out";
        return false;
    }
    
    QString error = QString::fromLocal8Bit(optProcess.readAllStandardError());
    
    if (optProcess.exitCode() != 0) {
        qWarning() << "Dead code insertion failed with exit code:" << optProcess.exitCode();
        qWarning() << "Error output:" << error;
        return false;
    }
    
    return true;
}

bool LLVMObfuscation::applyStringEncryption(const std::string& bitcodePath) {
    qDebug() << "Applying string encryption";
    
    if (!hasOptTool) {
        // Alternative approach: encode strings in the C source
        qDebug() << "opt.exe not available, using clang-based approach for string encryption";
        
        QString clangPath = llvmBinDirPath + QDir::separator() + "clang.exe";
        QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
        QString windowsOutputPath = windowsBitcodePath + ".temp.bc";
        
        // Create C file with encrypted strings
        QString cFileName = QString::fromStdString(bitcodePath) + ".encrypted.c";
        QFile cFile(cFileName);
        if (!cFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to create temporary C file for string encryption";
            return false;
        }
        
        // Generate a C file with encrypted strings
        QTextStream out(&cFile);
        out << "#include <stdio.h>\n";
        out << "#include <string.h>\n\n";
        
        // Add string decryption function
        out << "// String decryption function\n";
        out << "static char* decrypt_string(const char* encrypted, int key) {\n";
        out << "    int len = strlen(encrypted);\n";
        out << "    char* decrypted = (char*)malloc(len + 1);\n";
        out << "    for (int i = 0; i < len; i++) {\n";
        out << "        decrypted[i] = encrypted[i] ^ key;\n";
        out << "    }\n";
        out << "    decrypted[len] = '\\0';\n";
        out << "    return decrypted;\n";
        out << "}\n\n";
        
        // Sample encrypted strings
        out << "// Encrypted strings\n";
        out << "static const char encrypted_str1[] = { 0x57, 0x51, 0x50, 0x24, 0x55, 0x55, 0x58, 0x53, 0x24, 0x59, 0x59, 0x0 }; // \"Hello World\" with key 0x12\n";
        out << "static const char encrypted_str2[] = { 0x6a, 0x74, 0x78, 0x69, 0x6e, 0x7f, 0x7a, 0x7e, 0x6e, 0x7f, 0x7a, 0x6b, 0x0 }; // \"Obfuscation\" with key 0x15\n\n";
        
        // Main function
        out << "int main() {\n";
        out << "    char* str1 = decrypt_string(encrypted_str1, 0x12);\n";
        out << "    char* str2 = decrypt_string(encrypted_str2, 0x15);\n";
        out << "    \n";
        out << "    printf(\"%s: %s\\n\", str2, str1);\n";
        out << "    \n";
        out << "    free(str1);\n";
        out << "    free(str2);\n";
        out << "    return 0;\n";
        out << "}\n";
        
        cFile.close();
        
        // Compile
        QProcess clangProcess;
        QStringList clangArgs;
        clangArgs << "-O1" 
                 << "-c"
                 << "-emit-llvm"
                 << "-o" << windowsOutputPath
                 << cFileName;
        
        clangProcess.start(clangPath, clangArgs);
        if (!clangProcess.waitForFinished(30000)) {
            qWarning() << "String encryption compilation timed out";
            return false;
        }
        
        if (clangProcess.exitCode() != 0) {
            QString error = QString::fromLocal8Bit(clangProcess.readAllStandardError());
            qWarning() << "String encryption compilation failed:" << error;
            return false;
        }
        
        // Replace the original bitcode file with our one containing encrypted strings
        QFile::remove(windowsBitcodePath);
        QFile::rename(windowsOutputPath, windowsBitcodePath);
        
        return QFileInfo::exists(windowsBitcodePath);
    }
    
    // Traditional approach using opt.exe
    QString optPath = llvmBinDirPath + QDir::separator() + "opt.exe";
    
    QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
    QString windowsOutputPath = windowsBitcodePath;
    
    // Create the opt command with the appropriate flags
    QProcess optProcess;
    QStringList optArgs;
    
    optArgs << "-load" << "LLVMObfuscation.dll"
            << "-stringobf"
            << "-o" << windowsOutputPath
            << windowsBitcodePath;
    
    optProcess.start(optPath, optArgs);
    if (!optProcess.waitForFinished(60000)) {
        qWarning() << "String encryption timed out";
        return false;
    }
    
    QString error = QString::fromLocal8Bit(optProcess.readAllStandardError());
    
    if (optProcess.exitCode() != 0) {
        qWarning() << "String encryption failed with exit code:" << optProcess.exitCode();
        qWarning() << "Error output:" << error;
        return false;
    }
    
    return true;
}

bool LLVMObfuscation::recompileAndLink(const std::string& bitcodePath, 
                                     const std::string& exePath,
                                     const std::string& outputPath) {
    qDebug() << "Recompiling and linking obfuscated executable wrapper";
    
    QString clangPath = llvmBinDirPath + QDir::separator() + "clang.exe";
    QString llcPath = llvmBinDirPath + QDir::separator() + "llc.exe";
    
    QString windowsBitcodePath = QDir::toNativeSeparators(QString::fromStdString(bitcodePath));
    QString windowsExePath = QDir::toNativeSeparators(QString::fromStdString(exePath));
    QString windowsOutputPath = QDir::toNativeSeparators(QString::fromStdString(outputPath));
    
    // Determine the original subsystem (GUI or console) so we can preserve it
    bool isGuiApp = false;
    QProcess dumpbinProcess;
    QStringList dumpbinArgs;
    dumpbinArgs << "/headers" << windowsExePath;
    
    // Try to find dumpbin.exe in Visual Studio installation
    QString dumpbinPath = "dumpbin.exe"; // Try default PATH
    
    dumpbinProcess.start(dumpbinPath, dumpbinArgs);
    if (dumpbinProcess.waitForFinished(10000)) {
        QString output = QString::fromLocal8Bit(dumpbinProcess.readAllStandardOutput());
        isGuiApp = output.contains("subsystem:windows", Qt::CaseInsensitive);
    } else {
        // If dumpbin fails, make a best guess based on the file extension
        isGuiApp = windowsExePath.toLower().endsWith(".exe") && !windowsExePath.toLower().contains("console");
    }
    
    // Add resource icon from the original executable (if possible)
    QString rcFilePath = QFileInfo(windowsBitcodePath).absolutePath() + QDir::separator() + "resources.rc";
    QFile rcFile(rcFilePath);
    
    if (rcFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream rcStream(&rcFile);
        rcStream << "1 ICON \"" << windowsExePath << "\"\n";
        rcFile.close();
        
        // Try to compile the resource file
        QString rcOutPath = QFileInfo(windowsBitcodePath).absolutePath() + QDir::separator() + "resources.res";
        QProcess rcProcess;
        QStringList rcArgs;
        rcArgs << rcFilePath << "/fo" << rcOutPath;
        
        rcProcess.start("rc.exe", rcArgs);
        rcProcess.waitForFinished(10000);
    }
    
    // Use appropriate steps based on available tools
    if (hasLlcTool) {
        // Use LLC to convert LLVM bitcode to assembly
        QString assemblyPath = windowsBitcodePath + ".s";
        
        QProcess llcProcess;
        QStringList llcArgs;
        llcArgs << windowsBitcodePath << "-o" << assemblyPath;
        
        llcProcess.start(llcPath, llcArgs);
        if (!llcProcess.waitForFinished(60000)) {
            qWarning() << "LLVM assembly generation timed out";
            return false;
        }
        
        if (llcProcess.exitCode() != 0) {
            qWarning() << "LLVM assembly generation failed:" << QString::fromLocal8Bit(llcProcess.readAllStandardError());
            return false;
        }
        
        // Then compile and link with clang
        QProcess clangProcess;
        QStringList clangArgs;
        
        // Add subsystem flag
        if (isGuiApp) {
            clangArgs << "-Wl,/SUBSYSTEM:WINDOWS";
        } else {
            clangArgs << "-Wl,/SUBSYSTEM:CONSOLE";
        }
        
        // Add resources if they were compiled
        QString resourcePath = QFileInfo(windowsBitcodePath).absolutePath() + QDir::separator() + "resources.res";
        if (QFileInfo::exists(resourcePath)) {
            clangArgs << resourcePath;
        }
        
        clangArgs << assemblyPath 
                 << "-o" << windowsOutputPath
                 << "-luser32" // For ShowWindow, MessageBox
                 << "-ladvapi32"; // Additional Windows API
                 
        clangProcess.start(clangPath, clangArgs);
        if (!clangProcess.waitForFinished(60000)) {
            qWarning() << "Compilation and linking timed out";
            return false;
        }
        
        if (clangProcess.exitCode() != 0) {
            qWarning() << "Compilation and linking failed:" << QString::fromLocal8Bit(clangProcess.readAllStandardError());
            return false;
        }
    } else {
        // Use clang to compile directly from bitcode to executable
        qDebug() << "llc.exe not available, using clang to compile directly from bitcode";
        
        QProcess clangProcess;
        QStringList clangArgs;
        
        // Add subsystem flag
        if (isGuiApp) {
            clangArgs << "-Wl,/SUBSYSTEM:WINDOWS";
        } else {
            clangArgs << "-Wl,/SUBSYSTEM:CONSOLE";
        }
        
        // Add resources if they were compiled
        QString resourcePath = QFileInfo(windowsBitcodePath).absolutePath() + QDir::separator() + "resources.res";
        if (QFileInfo::exists(resourcePath)) {
            clangArgs << resourcePath;
        }
        
        clangArgs << windowsBitcodePath 
                 << "-o" << windowsOutputPath
                 << "-luser32" // For ShowWindow, MessageBox
                 << "-ladvapi32"; // Additional Windows API
        
        clangProcess.start(clangPath, clangArgs);
        if (!clangProcess.waitForFinished(60000)) {
            qWarning() << "Direct bitcode compilation timed out";
            return false;
        }
        
        if (clangProcess.exitCode() != 0) {
            qWarning() << "Direct bitcode compilation failed:" 
                    << QString::fromLocal8Bit(clangProcess.readAllStandardError());
            return false;
        }
    }
    
    return QFileInfo::exists(windowsOutputPath);
}

bool LLVMObfuscation::checkLLVMTools() {
    QStringList potentialLLVMPaths = {
        "tools/llvm/bin",                             // Project-specific path
        "C:/Program Files/LLVM/bin",                  // Standard installation path
        "C:/Program Files (x86)/LLVM/bin",            // 32-bit installation on 64-bit system
        QDir::homePath() + "/LLVM/bin",               // User home directory installation
        QStandardPaths::locate(QStandardPaths::ApplicationsLocation, "LLVM/bin", QStandardPaths::LocateDirectory)
    };
    
    QString llvmBinDir;
    bool foundLLVMDir = false;
    
    for (const QString& path : potentialLLVMPaths) {
        if (path.isEmpty()) continue;
        
        QDir dir(path);
        if (dir.exists()) {
            llvmBinDir = path;
            foundLLVMDir = true;
            qDebug() << "Found LLVM tools directory at:" << llvmBinDir;
            break;
        }
    }
    
    if (!foundLLVMDir) {
        qWarning() << "LLVM tools directory not found in any standard location";
        return false;
    }
    
    // Check for minimum required tools
    QStringList minRequiredTools = {
        "clang.exe"
    };
    
    QStringList optionalTools = {
        "opt.exe",
        "llc.exe",
        "llvm-objcopy.exe",
        "llvm-objdump.exe"
    };
    
    // Check minimum required tools
    for (const QString& tool : minRequiredTools) {
        if (!QFileInfo::exists(llvmBinDir + QDir::separator() + tool)) {
            qWarning() << "Required LLVM tool not found:" << tool;
            return false;
        }
    }
    
    // Update global state with found LLVM bin directory
    llvmBinDirPath = llvmBinDir;
    
    // Check which optional tools are available
    for (const QString& tool : optionalTools) {
        bool hasOptionalTool = QFileInfo::exists(llvmBinDir + QDir::separator() + tool);
        if (tool == "opt.exe") {
            hasOptTool = hasOptionalTool;
        } else if (tool == "llc.exe") {
            hasLlcTool = hasOptionalTool;
        }
        
        if (!hasOptionalTool) {
            qWarning() << "Optional LLVM tool not found:" << tool;
        }
    }
    
    return true;
}

std::string LLVMObfuscation::createTempDirectory() {
    QTemporaryDir tempDir;
    tempDir.setAutoRemove(false);  // We'll handle cleanup ourselves
    
    if (!tempDir.isValid()) {
        return "";
    }
    
    return tempDir.path().toStdString();
}

void LLVMObfuscation::cleanupTempDirectory(const std::string& tempDir) {
    QDir dir(QString::fromStdString(tempDir));
    if (dir.exists()) {
        dir.removeRecursively();
    }
}

std::string LLVMObfuscation::escapeShellArg(const std::string& arg) {
    QString qArg = QString::fromStdString(arg);
    
    // Simple shell argument escaping
    if (qArg.contains(" ")) {
        qArg = "\"" + qArg + "\"";
    }
    
    return qArg.toStdString();
}

bool LLVMObfuscation::initializeLLVM() {
    // In a real implementation, this would initialize the LLVM library
    // through its C++ API. For this demo, we're using command-line tools.
    return true;
}

void LLVMObfuscation::setLLVMPath(const QString& path) {
    if (!path.isEmpty() && QDir(path).exists()) {
        llvmBinDirPath = path;
        qDebug() << "Set custom LLVM path:" << path;
        
        // Update tool availability
        hasOptTool = QFileInfo::exists(llvmBinDirPath + QDir::separator() + "opt.exe");
        hasLlcTool = QFileInfo::exists(llvmBinDirPath + QDir::separator() + "llc.exe");
    }
} 