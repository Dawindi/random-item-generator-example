# Path of Exile Tracker

A terminal-based utility application built with C++20, [FTXUI](https://github.com/ArthurSonzogni/FTXUI), and dependency management using [Conan 2](https://conan.io/).

---

## Prerequisites

Before building this project, make sure you have the following installed on your system:

- **C++20 Compliant Compiler** (MSVC 2022/2026, GCC 11+, or Clang 13+)
- **CMake** (v3.20 or higher)
- **Conan** (v2.x package manager)
- **Ninja** (Highly recommended, enables auto-complete/LSP diagnostics in your editor via compilation databases)

---

## Setup & Build Instructions

## Setup & Build Instructions

This project is configured to **automatically run Conan to install all dependencies** during the CMake configuration step. This means you do not need to run Conan manually!

We highly recommend using **CMake Presets** as they handle both dependency retrieval and Windows compiler/SDK environment setup dynamically.

---

### Recommended: CMake Presets (Dynamic & Robust)

This is the most reliable way to configure and build the project. On Windows, using the presets automatically loads the MSVC compiler, the resource compiler (`rc.exe`), and the Windows SDK paths **without requiring you to open a Developer Command Prompt**.

#### 1. Windows (from any PowerShell / Command Prompt terminal)
```bash
# Configure the project (this automatically triggers 'conan install')
cmake --preset windows-debug

# Build the project
cmake --build --preset build-windows-debug

# Build the project (clean first)
cmake --build --preset build-windows-debug --clean-first

# Run the executable
./build/windows-debug/Debug/poe_tracker.exe
```

#### 2. Linux / macOS (from any terminal)
```bash
# Configure the project (this automatically triggers 'conan install')
cmake --preset linux-debug   # or macos-debug

# Build the project
cmake --build --preset build-linux-debug   # or build-macos-debug

# Run the executable
./build/linux-debug/poe_tracker   # or macos-debug
```

---

### Alternative A: Standard CMake & Ninja (Developer Terminal required on Windows)

If you prefer to configure manually or are not using presets, you can run standard CMake commands. 

*Note: On Windows, you **must** execute these commands inside a **Developer Command Prompt for VS 2022** or **Developer PowerShell for VS 2022** so that CMake can find the Windows SDK tools (like `rc.exe` and `mt.exe`) and libraries.*

```bash
# 1. Configure CMake with Ninja
# CMake will automatically run 'conan install' for you during this step!
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug

# 2. Build using Ninja
cmake --build build

# 3. Run the executable
./build/poe_tracker.exe
```

---

### Alternative B: Visual Studio Generator (Windows only)

If you prefer building with Visual Studio's MSBuild rather than Ninja:

```bash
# 1. Configure CMake using default VS Generator
# CMake will automatically run 'conan install' for you during this step!
cmake -B build -S .

# 2. Build using MSBuild
cmake --build build --config Debug

# 3. Run the executable
./build/Debug/poe_tracker.exe
```

---

## Manual dependency installation (Fallback/Troubleshooting)
If the automatic `conan install` during CMake configuration fails or if you want to run it manually, you can run:

```bash
conan install . -s build_type=Debug -s compiler.cppstd=20 -c tools.cmake.cmaketoolchain:user_presets=False --build=missing -of build
```

Then rerun the `cmake` configure command.

---

## Editor & IDE Auto-complete (LSP) Support
This project contains a `.clangd` configuration file at the root which expects the compile commands database in `build/windows-debug`. 

If you configure using the recommended **CMake Presets** method, CMake writes out `build/windows-debug/compile_commands.json` automatically. Your editor's language server (like `clangd` or Zed's built-in LSP) will parse this and fully resolve external dependency headers like `<ftxui/dom/elements.hpp>`.

*Note: You may need to restart your editor's language server (or restart your editor) after the first preset configuration/build for it to reload the database.*

---

## Troubleshooting

### Q: Why does CMake complain about missing `"Findftxui.cmake"`?
If you deleted the `build` directory or cleared it, CMake has no knowledge of where Conan stored the libraries. 
Simply re-run the Conan installation command (Step 1) to regenerate the integration files in the build folder:
```bash
conan install . -s build_type=Debug -s compiler.cppstd=20 -c tools.cmake.cmaketoolchain:user_presets=False --build=missing -of build
```
Then, rerun step 2 to configure CMake.
