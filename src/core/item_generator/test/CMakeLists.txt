# -----------------------------------------------------------------------------
#  BaseItem Unit Tests
# -----------------------------------------------------------------------------
add_executable(base_item_test
    base_item_test.cc
)

target_compile_definitions(base_item_test PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

target_link_libraries(base_item_test
    PRIVATE
        item_generator::item_generator
        item_data_types::item_data_types
        GTest::gtest
        GTest::gtest_main
)

# Copy test data files to the build directory so tests can find them at runtime
add_custom_command(TARGET base_item_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:base_item_test>/data
)

# -----------------------------------------------------------------------------
#  ItemBuilder Unit Tests
# -----------------------------------------------------------------------------
add_executable(item_builder_test
    item_builder_test.cc
)

target_compile_definitions(item_builder_test PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

target_link_libraries(item_builder_test
    PRIVATE
        item_generator::item_generator
        item_data_types::item_data_types
        GTest::gtest
        GTest::gtest_main
)

# Copy test data files to the build directory so tests can find them at runtime
add_custom_command(TARGET item_builder_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:item_builder_test>/data
)

# -----------------------------------------------------------------------------
#  WeaponItem Unit Tests
# -----------------------------------------------------------------------------
add_executable(weapon_item_test
    weapon_item_test.cc
)

target_compile_definitions(weapon_item_test PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

target_link_libraries(weapon_item_test
    PRIVATE
        item_generator::item_generator
        item_data_types::item_data_types
        GTest::gtest
        GTest::gtest_main
)

# Copy test data files to the build directory so tests can find them at runtime
add_custom_command(TARGET weapon_item_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:weapon_item_test>/data
)

include(GoogleTest)
gtest_discover_tests(base_item_test)
gtest_discover_tests(item_builder_test)
gtest_discover_tests(weapon_item_test)
