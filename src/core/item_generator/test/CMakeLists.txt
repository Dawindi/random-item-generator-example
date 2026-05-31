# -----------------------------------------------------------------------------
#  BaseItem Unit Tests
# -----------------------------------------------------------------------------
add_executable(base_item_test
    base_item_test.cc
)

target_include_directories(base_item_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_compile_definitions(base_item_test PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

target_link_libraries(base_item_test
    PRIVATE
        item_generator
        nlohmann_json::nlohmann_json
        GTest::gtest
        GTest::gtest_main
)

# Copy test data files to the build directory so tests can find them at runtime
add_custom_command(TARGET base_item_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:base_item_test>/data
)

include(GoogleTest)
gtest_discover_tests(base_item_test)
