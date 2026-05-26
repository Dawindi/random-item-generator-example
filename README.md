# Random Item Generator

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

### Automatic Configuration (Recommended)
The root `CMakeLists.txt` is configured to **automatically run Conan to install all dependencies** if they are not already present. This means you can skip the manual Conan installation step in most cases!

#### Option A: Ninja (Recommended for IDE/Editor Integration)
This generates a `compile_commands.json` database which is picked up automatically by the local `.clangd` file, enabling full code completion and diagnostic error clearing inside your editor.

```bash
# 1. Configure CMake with Ninja (execute in Developer Command Prompt / MSVC environment)
# CMake will automatically run 'conan install' for you during this step!
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug

# 2. Build using Ninja
cmake --build build

# 3. Run the executable
./build/poe_tracker.exe
```

*Note: If running from outside a Developer Command Prompt on Windows, the build environment variables might not be loaded. In that case, you can run:*
```bash
cmd //c "build\conanbuild.bat && cmake --build build"
```

#### Option B: Visual Studio (Windows)
```bash
# 1. Configure CMake
# CMake will automatically run 'conan install' for you during this step!
cmake -B build -S .

# 2. Build using MSBuild
cmake --build build --config Debug

# 3. Run the executable
./build/Debug/poe_tracker.exe
```

---

## CMake Presets Support
We have configured standard CMake Presets in `CMakePresets.json`. On Windows, the preset includes variables for `cl.exe` (pointing to MSVC `14.44.35207`) so that CMake can automatically locate the correct compiler.

You can configure and build using presets:
```bash
# Configure using the Windows Debug preset (this also automatically runs conan install)
cmake --preset windows-debug

# Build using the preset
cmake --build --preset build-windows-debug
```

---

## Manual dependency installation (Fallback/Troubleshooting)
If the automatic `conan install` during CMake configuration fails or if you want to run it manually, you can run:

```bash
conan install . -s build_type=Debug -s compiler.cppstd=20 -c tools.cmake.cmaketoolchain:user_presets=False --build=missing -of build
```

Then rerun the `cmake` configure command.

Expected output for both:
```text
╭───────────────────────────────────────────────────────────────────────────────╮
│Hello World!                                                                   │
╰───────────────────────────────────────────────────────────────────────────────╯
```

---

## Editor & IDE Auto-complete (LSP) Support
This project contains a `.clangd` configuration file at the root. If you build using **Option A (Ninja)**, CMake will write out `build/compile_commands.json`. Your editor's language server (like `clangd` or Zed's built-in LSP) will automatically parse this database and fully resolve external dependency headers like `<ftxui/dom/elements.hpp>`.

*Note: You may need to restart your editor's language server (or restart your editor) after the first Ninja build for it to reload the database.*

---

## Troubleshooting

### Q: Why does CMake complain about missing `"Findftxui.cmake"`?
If you deleted the `build` directory or cleared it, CMake has no knowledge of where Conan stored the libraries. 
Simply re-run the Conan installation command (Step 1) to regenerate the integration files in the build folder:
```bash
conan install . -s build_type=Debug -s compiler.cppstd=20 -c tools.cmake.cmaketoolchain:user_presets=False --build=missing -of build
```
Then, rerun step 2 to configure CMake.
