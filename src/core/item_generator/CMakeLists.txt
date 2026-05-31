add_library(item_generator STATIC
    base_item.cc
    item_rarity.h
    item_type.h
)
target_include_directories(item_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(item_generator
    PUBLIC
        uuid
        nlohmann_json::nlohmann_json
)
