# 3.20 is the first version that knows CMAKE_CXX_STANDARD 23
cmake_minimum_required(VERSION 3.20)
project(garlic-daemon LANGUAGES CXX)
set(THIS garlic-daemon)

# C++23 without GNU extensions, minimum compiler is GCC 14, see CONVENTIONS.md
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR "GCC 14 or newer is required, found ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
endif ()

# All classes go into a static library, so the daemon and the tests can share them
set(SOURCE_FILES
    source/configuration.hpp
    source/configuration.cpp
    source/cmd_parser.hpp
    source/cmd_parser.cpp
    source/watchdog.hpp
    source/watchdog.cpp
    source/daemon.hpp
    source/daemon.cpp
    source/message_dispatcher.hpp
    source/message_dispatcher.cpp
    source/system_actions.hpp
    source/system_actions.cpp
    source/ipc_message_receiver.hpp
    source/ipc_message_receiver.cpp
    source/log.hpp)

add_library(garlic-core STATIC ${SOURCE_FILES})
target_include_directories(garlic-core PUBLIC source)
target_link_libraries(garlic-core PUBLIC libconfig++.so)

# The daemon itself only consists of main.cpp
add_executable(${THIS} source/main.cpp)
target_link_libraries(${THIS} PRIVATE garlic-core)

# Unit tests (disable with -DBUILD_TESTS=OFF)
option(BUILD_TESTS "Build the unit tests" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# Install the config file
install(FILES resource/config/garlic-daemon.conf DESTINATION /etc/garlic-daemon)

# Intall the systemd file
install(FILES resource/systemd/garlic-daemon.service DESTINATION /etc/systemd/system)

# Install the binary program
install(TARGETS ${THIS} DESTINATION /usr/bin)
