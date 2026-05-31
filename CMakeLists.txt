cmake_minimum_required(VERSION 3.20)
project(dsvh VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

# Print Generator and Compiler details
message(STATUS "--------------------------------------------------------")
message(STATUS "CMake Generator : ${CMAKE_GENERATOR}")
message(STATUS "CMake Version   : ${CMAKE_VERSION}")
message(STATUS "Compiler ID     : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler Path   : ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler Ver    : ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "--------------------------------------------------------")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable C++ exception handling on MSVC
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")

# 1. Handle Conan
set(CONAN_TOOLCHAIN_FOUND FALSE)
if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    set(CONAN_TOOLCHAIN_FOUND TRUE)
elseif(EXISTS "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
    set(CONAN_TOOLCHAIN_FOUND TRUE)
endif()

if(NOT CONAN_TOOLCHAIN_FOUND)
    message(STATUS "Conan toolchain not found. Attempting to run 'conan install' automatically...")

    set(CONAN_ARGS install "${CMAKE_SOURCE_DIR}" --build=missing -of "${CMAKE_BINARY_DIR}")

    # Pass build type
    if(CMAKE_BUILD_TYPE)
        list(APPEND CONAN_ARGS -s build_type=${CMAKE_BUILD_TYPE})
    else()
        list(APPEND CONAN_ARGS -s build_type=Debug)
    endif()

    # Pass cppstd
    list(APPEND CONAN_ARGS -s compiler.cppstd=20)

    # Avoid user presets
    list(APPEND CONAN_ARGS -c tools.cmake.cmaketoolchain:user_presets=False)

    # Pass generator
    if(CMAKE_GENERATOR)
        list(APPEND CONAN_ARGS -c tools.cmake.cmaketoolchain:generator=${CMAKE_GENERATOR})
    endif()

    string(REPLACE ";" " " CONAN_ARGS_STR "${CONAN_ARGS}")
    message(STATUS "Running: conan ${CONAN_ARGS_STR}")

    execute_process(
        COMMAND conan ${CONAN_ARGS}
        RESULT_VARIABLE CONAN_RESULT
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    if(CONAN_RESULT EQUAL 0)
        message(STATUS "Conan install succeeded.")
        # Check again if toolchain exists now
        if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
            set(CONAN_TOOLCHAIN_PATH "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
            set(CONAN_TOOLCHAIN_FOUND TRUE)
        elseif(EXISTS "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
            set(CONAN_TOOLCHAIN_PATH "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
            set(CONAN_TOOLCHAIN_FOUND TRUE)
        endif()
    else()
        message(WARNING "Automatic conan install failed with exit code: ${CONAN_RESULT}")
    endif()
endif()

if(CONAN_TOOLCHAIN_FOUND)
    include("${CONAN_TOOLCHAIN_PATH}")
else()
    message(FATAL_ERROR
        "\n=========================================================================\n"
        "ERROR: Conan toolchain could not be loaded.\n"
        "CMake attempted to automatically run 'conan install' to fetch dependencies,\n"
        "but the toolchain still does not exist. There might be something else wrong.\n\n"
        "Please verify that:\n"
        "  1. Conan is installed and in your system PATH (test with 'conan --version').\n"
        "  2. Conan has access to download packages (check your internet connection).\n"
        "  3. You are running CMake in an environment matching your compiler settings.\n\n"
        "To try running the installation manually, use:\n"
        "  conan install . -s build_type=Debug -s compiler.cppstd=20 -c tools.cmake.cmaketoolchain:user_presets=False --build=missing -of build\n"
        "=========================================================================\n"
    )
endif()

# 2. Options for Sanitizers (controlled by CMakePresets)
option(ENABLE_SANITIZERS "Enable Address and Undefined Behavior sanitizers" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_SANITIZERS)
    if(MSVC)
        # Windows specific: MSVC AddressSanitizer
        add_compile_options(/fsanitize:address)
        add_link_options(/fsanitize:address)
        # Note: MSVC does not have a direct UBSan equivalent in the same form
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Linux/macOS specific
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Find FTXUI
find_package(ftxui REQUIRED)

# Find nlohmann_json
find_package(nlohmann_json REQUIRED)

# 3. Add Subdirectories
# We add them in order of dependency: Core -> UI -> App -> Tests
add_subdirectory(src/core)
add_subdirectory(src/ui)

# Add main executable
add_executable(dsvh src/main.cc)

target_link_libraries(dsvh
    PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
        nlohmann_json::nlohmann_json
        item_generator
)

# 4. Tests (Only if enabled)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
