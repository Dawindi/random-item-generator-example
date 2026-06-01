# Random Item Generator

A terminal-based utility application built with C++20, [FTXUI](https://github.com/ArthurSonzogni/FTXUI), and dependency management using [Conan 2](https://conan.io/).

---

## Prerequisites

Before building this project, make sure you have the following installed on your system:

- **C++20 Compliant Compiler** (MSVC 2026, GCC 11+, or Clang 13+)
- **CMake** (v3.20 or higher)
- **Conan** (v2.x package manager)
- **Ninja** (Highly recommended, enables auto-complete/LSP diagnostics in your editor via compilation databases)

> **Note:** On Windows, MSVC 2026 (Visual Studio 18 or Build Tools 2026) is required. MSVC 2022 is **not** supported.

---

## Setup & Build Instructions

This project is configured to **automatically run Conan to install all dependencies** during the CMake configuration step. This means you do not need to run Conan manually!

We highly recommend using **CMake Presets** as they handle both dependency retrieval and Windows compiler/SDK environment setup dynamically.

---

### Recommended: CMake Presets (Dynamic & Robust)

This is the most reliable way to configure and build the project. On Windows, the presets automatically load the MSVC 2026 compiler, the resource compiler (`rc.exe`), and the Windows SDK paths **without requiring you to open a Developer Command Prompt**.

> **Important for other developers:** The Windows preset (`base-windows` in `CMakePresets.json`) contains **hardcoded MSVC and Windows SDK paths** that match the original author's machine. If you are on a different machine, you have three options:
>
> 1. **Create a `CMakeUserPresets.json`** (recommended) — override the environment paths without modifying the tracked `CMakePresets.json`. See [Customizing for your machine](#customizing-for-your-machine) below.
> 2. **Use the Alternative approaches** — run CMake from a Visual Studio Developer Command Prompt (no hardcoded paths needed).
> 3. **Edit `CMakePresets.json` directly** — update the paths in the `base-windows` preset to match your MSVC and Windows SDK installation.

#### 1. Windows (from any PowerShell / Command Prompt terminal)
```bash
# Configure the project (this automatically triggers 'conan install')
cmake --preset windows-debug

# Build the project (uses all CPU cores)
cmake --build --preset build-windows-debug

# Clean rebuild
cmake --build --preset build-windows-debug-clean

# Run the executable
./build/windows-debug/Debug/dsvh.exe
```

> **Tip:** All build presets include `"jobs": 0` which uses all available CPU cores for faster builds. Test presets include `"stopOnFailure": true` — ctest stops on the first failing test.

#### 2. Linux / macOS (from any terminal)
```bash
# Configure the project (this automatically triggers 'conan install')
cmake --preset linux-debug   # or macos-debug

# Build the project
cmake --build --preset build-linux-debug   # or build-macos-debug

# Run the executable
./build/linux-debug/dsvh   # or macos-debug
```

---

### Alternative A: Standard CMake & Ninja (Developer Terminal required on Windows)

If you prefer to configure manually or are not using presets, you can run standard CMake commands.

*Note: On Windows, you **must** execute these commands inside a **Developer Command Prompt for VS 2026** or **Developer PowerShell for VS 2026** so that CMake can find the Windows SDK tools (like `rc.exe` and `mt.exe`) and libraries.*

```bash
# 1. Configure CMake with Ninja
# CMake will automatically run 'conan install' for you during this step!
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug

# 2. Build using Ninja
cmake --build build

# 3. Run the executable
./build/dsvh.exe
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
./build/Debug/dsvh.exe
```

---

## Running Tests

Tests are built by default (`BUILD_TESTS=ON`). They use [Google Test](https://github.com/google/googletest) and are discovered automatically via CTest.

All test presets are configured with:
- `outputOnFailure: true` — prints test output on failure
- `noTestsAction: "error"` — fails if no tests are found (catches misconfiguration)
- `stopOnFailure: true` — stops on the first failing test

### With CMake Presets

```bash
# Build all targets (including tests)
cmake --build --preset build-windows-debug

# Run the tests
ctest --preset test-windows-debug
```

### With standard CMake

```bash
# Build everything (Debug config includes tests)
cmake --build build --config Debug

# Run the tests
ctest --test-dir build -C Debug --output-on-failure
```

### Disabling tests

To configure without tests, pass `-DBUILD_TESTS=OFF`:

```bash
cmake -B build -S . -DBUILD_TESTS=OFF
```

---

## Customizing for your machine

If the hardcoded MSVC paths in `CMakePresets.json` don't match your system, create a **`CMakeUserPresets.json`** file in the project root. This file is gitignored and lets you override settings without modifying the tracked preset file.

```json
{
  "version": 10,
  "configurePresets": [
    {
      "name": "base-windows",
      "hidden": true,
      "environment": {
        "PATH": "<your-msvc-bin-path>;<your-windows-kit-bin-path>;$penv{PATH}",
        "INCLUDE": "<your-msvc-include-path>;<your-windows-kit-ucrt-path>;<your-windows-kit-um-path>;<your-windows-kit-shared-path>",
        "LIB": "<your-msvc-lib-path>;<your-windows-kit-um-lib-path>;<your-windows-kit-ucrt-lib-path>"
      }
    }
  ]
}
```

Replace the `<...>` placeholders with the paths from your Visual Studio and Windows SDK installation. You can find these paths by:

1. Open a **Developer Command Prompt for VS 2026**
2. Run `echo %PATH%` and look for the MSVC `bin` and Windows Kit `bin` directories
3. Run `echo %INCLUDE%` for the include paths
4. Run `echo %LIB%` for the library paths

The `CMakeUserPresets.json` will be merged with `CMakePresets.json` automatically — only the fields you specify will be overridden.

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

### Q: `cmake --preset windows-debug` fails to find the MSVC compiler

The `CMakePresets.json` contains hardcoded paths to the original author's MSVC installation. See [Customizing for your machine](#customizing-for-your-machine) for instructions on creating a `CMakeUserPresets.json` with your own paths, or use one of the Alternative build approaches that run from a Developer Command Prompt.

### Q: Why does CMake complain about missing `"Findftxui.cmake"`?

If you deleted the `build` directory or cleared it, CMake has no knowledge of where Conan stored the libraries.
Simply re-run the Conan installation command to regenerate the integration files in the build folder:

```bash
conan install . -s build_type=Debug -s compiler.cppstd=20 -c tools.cmake.cmaketoolchain:user_presets=False --build=missing -of build
```

Then, rerun the CMake configuration step.
