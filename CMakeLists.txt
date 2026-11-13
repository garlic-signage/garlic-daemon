# 3.20 is the first version that knows CMAKE_CXX_STANDARD 23
cmake_minimum_required(VERSION 3.20)
project(garlic-daemon VERSION 0.1.0 LANGUAGES CXX)
set(THIS garlic-daemon)

# C++23 without GNU extensions, minimum compiler is GCC 14, see CONVENTIONS.md
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR "GCC 14 or newer is required, found ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif ()

# Warnings for the daemon and the tests. GCC 14 must build without any warning, see CONVENTIONS.md.
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors, e.g. in CI" OFF)
if (WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
endif ()

# All classes go into a static library, so the daemon and the tests can share them
set(SOURCE_FILES
    src/configuration.hpp
    src/configuration.cpp
    src/file_descriptor.hpp
    src/cmd_parser.hpp
    src/cmd_parser.cpp
    src/watchdog.hpp
    src/watchdog.cpp
    src/daemon.hpp
    src/daemon.cpp
    src/message_dispatcher.hpp
    src/message_dispatcher.cpp
    src/system_actions.hpp
    src/system_actions.cpp
    src/usb_actions.hpp
    src/usb_actions.cpp
    src/usb_content_handler.hpp
    src/usb_content_handler.cpp
    src/usb_monitor.hpp
    src/usb_monitor.cpp
    src/local_socket_server.hpp
    src/local_socket_server.cpp
    src/player_protocol.hpp
    src/player_protocol.cpp
    src/log.hpp)

add_library(garlic-core STATIC ${SOURCE_FILES})
target_include_directories(garlic-core PUBLIC src)
# libudev reports plugged in USB sticks
find_package(PkgConfig REQUIRED)
pkg_check_modules(UDEV REQUIRED IMPORTED_TARGET libudev)
# nlohmann/json (header-only) for protocol 1: from the system (Arch: nlohmann-json, Debian: nlohmann-json3-dev),
# otherwise downloaded
find_package(nlohmann_json 3 QUIET)
if (NOT nlohmann_json_FOUND)
    include(FetchContent)
    set(JSON_Install OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
                         URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d)
    FetchContent_MakeAvailable(nlohmann_json)
endif ()
target_link_libraries(garlic-core PUBLIC libconfig++.so PkgConfig::UDEV nlohmann_json::nlohmann_json)

# The daemon itself only consists of main.cpp
add_executable(${THIS} src/main.cpp)
target_link_libraries(${THIS} PRIVATE garlic-core)
target_compile_definitions(${THIS} PRIVATE GARLIC_DAEMON_VERSION="${PROJECT_VERSION}")

# Unit tests (disable with -DBUILD_TESTS=OFF)
option(BUILD_TESTS "Build the unit tests" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# Installation, the paths follow CMAKE_INSTALL_PREFIX (default /usr/local).
# For a package use -DCMAKE_INSTALL_PREFIX=/usr, then the config goes to /etc.
include(GNUInstallDirs)

install(TARGETS ${THIS} DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES resource/config/garlic-daemon.conf DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/garlic-daemon)

# The service file gets the real install paths of binary and config
configure_file(resource/systemd/garlic-daemon.service.in garlic-daemon.service @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/garlic-daemon.service DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system)
