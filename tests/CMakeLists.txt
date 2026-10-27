find_package(GTest REQUIRED)

# One test file per class
add_executable(garlic-tests
    test_cmd_parser.cpp
    test_configuration.cpp
    test_daemon.cpp
    test_local_socket_server.cpp
    test_log.cpp
    test_message_dispatcher.cpp
    test_usb_content_handler.cpp
    test_usb_monitor.cpp
    test_watchdog.cpp)

target_link_libraries(garlic-tests PRIVATE garlic-core GTest::gtest_main)

# Lets a test check that the shipped config file is valid
target_compile_definitions(garlic-tests PRIVATE CONFIG_FILE="${PROJECT_SOURCE_DIR}/resource/config/garlic-daemon.conf")

# Registers every TEST() with ctest, a hanging test fails after 10 seconds
include(GoogleTest)
gtest_discover_tests(garlic-tests PROPERTIES TIMEOUT 10)

# The daemon itself must end with exit code 1 when the config file is missing or invalid.
# A daemon that does not end fails by the timeout.
foreach (CASE missing invalid-syntax invalid-environment)
    add_test(NAME Daemon.EndsWithConfig-${CASE}
             COMMAND sh -c "\"$1\" --config \"$2\" --socket /tmp/garlic-test-config.sock; test $? -eq 1"
                     sh $<TARGET_FILE:garlic-daemon> ${CMAKE_CURRENT_SOURCE_DIR}/data/${CASE}.conf)
    set_tests_properties(Daemon.EndsWithConfig-${CASE} PROPERTIES TIMEOUT 10)
endforeach ()
