# 🛡️ Anti-Reverse Engineering Wrapper for Executables

**Authors**: Meirambek Bekarys & Karatay Dair  
**Supervisor**: Alibek Aigerim  
**University**: Astana IT University  
**Department**: Intelligent Systems and Cybersecurity  
**Degree**: BSc in Cybersecurity (6B06301)  
**Date**: June 2025

---

## 📌 Overview

This project presents an open-source, lightweight, and educational anti-reverse engineering wrapper for C++ executable files. It offers a modular desktop application built with **C++** and **Qt Creator**, enabling:

- ✅ Code Obfuscation  
- ✅ String Encryption (XOR and AES-based)  
- ✅ Executable Packing (via UPX)  

The tool is designed to make reverse engineering more difficult for attackers, ideal for students, small developers, and cybersecurity educators.

---

## 🎯 Key Features

- 🔐 **Code Obfuscation**  
  Renames variables, changes control flow, inserts dead code and complex expressions to confuse static and dynamic analysis tools.

- 🔒 **String Encryption**  
  Supports XOR-based and AES-based encryption to hide sensitive strings like URLs, keys, and paths.

- 📦 **Executable Packing (UPX)**  
  Compresses binaries to alter their structure, hiding logic from disassemblers.

- 🖥️ **Cross-platform Qt GUI**  
  Simple user interface to toggle each protection module independently.

- 🧪 **Reverse Engineering Tools Used for Testing**  
  - [IDA Pro](https://hex-rays.com/ida-pro/)  
  - [x64dbg](https://x64dbg.com/)  
  - [VirusTotal](https://www.virustotal.com/)

---

## 📂 Project Structure

```
/anti-re-wrapper/
├── src/                # C++ source files
├── include/            # Header files
├── ui/                 # Qt GUI forms
├── resources/          # Icons, UI assets
├── examples/           # Sample code before/after protection
└── README.md
```

---

## 🧠 How It Works

Each selected protection method is applied at compile-time. Strings are decrypted at runtime, while obfuscation and packing alter the final binary layout.

The wrapper increases reverse engineering time from **minutes to hours or even days**, without requiring heavyweight tools or paid licenses.

---

## 🚀 Getting Started

### Prerequisites

- C++ Compiler (e.g., `g++`)
- [Qt Creator](https://www.qt.io/)

### Installation
git clone https://github.com/napp3r/SprectreGuard.git
# Open with Qt Creator and build the project
```

---

## 🔬 Evaluation Results

| Protection Method | Effectiveness | Comments |
|-------------------|---------------|----------|
| Code Obfuscation  | ⭐⭐⭐⭐☆        | Significantly alters control flow and structure |
| String Encryption | ⭐⭐⭐⭐⭐        | AES hides sensitive data from both static and dynamic tools |
| UPX Packing       | ⭐⭐⭐☆☆        | Effective, but common; advanced unpackers exist |

---

## 📄 License

This project is open-source and licensed under the MIT License.

---

## 📬 Contact

- **Meirambek Bekarys**: [bekarys.meirambek@gmail.com]  
- **Karatay Dair**: [karatay.dair@gmail.com]
