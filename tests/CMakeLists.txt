find_package(GTest REQUIRED)

# One test file per class
add_executable(garlic-tests
    test_cmd_parser.cpp
    test_configuration.cpp
    test_message_dispatcher.cpp)

target_link_libraries(garlic-tests PRIVATE garlic-core GTest::gtest_main)

# Registers every TEST() with ctest
include(GoogleTest)
gtest_discover_tests(garlic-tests)
