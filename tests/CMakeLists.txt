find_package(GTest REQUIRED)

# One test file per class
add_executable(garlic-tests
    test_cmd_parser.cpp
    test_configuration.cpp
    test_daemon.cpp
    test_ipc_message_receiver.cpp
    test_message_dispatcher.cpp
    test_watchdog.cpp)

target_link_libraries(garlic-tests PRIVATE garlic-core GTest::gtest_main)

# Lets a test check that the shipped config file is valid
target_compile_definitions(garlic-tests PRIVATE CONFIG_FILE="${PROJECT_SOURCE_DIR}/resource/config/garlic-daemon.conf")

# Registers every TEST() with ctest, a hanging test fails after 10 seconds
include(GoogleTest)
gtest_discover_tests(garlic-tests PROPERTIES TIMEOUT 10)
