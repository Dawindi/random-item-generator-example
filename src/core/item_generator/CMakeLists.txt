add_library(item_generator STATIC
    base_item.cc
    base_item.h
    interface_item.h
    item_builder.cc
    item_builder.h
    item_rarity.h
    item_status.h
    item_type.h
)
target_include_directories(item_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(item_generator::item_generator ALIAS item_generator)
target_link_libraries(item_generator
    PUBLIC
        uuid::uuid
        nlohmann_json::nlohmann_json
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
