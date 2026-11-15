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

# -----------------------------------------------------------------------------
#  ArmorItem Unit Tests
# -----------------------------------------------------------------------------
add_executable(armor_item_test
    armor_item_test.cc
)

target_compile_definitions(armor_item_test PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

target_link_libraries(armor_item_test
    PRIVATE
        item_generator::item_generator
        item_data_types::item_data_types
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(base_item_test)
gtest_discover_tests(item_builder_test)
gtest_discover_tests(weapon_item_test)
gtest_discover_tests(armor_item_test)
