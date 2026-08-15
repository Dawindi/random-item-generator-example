add_library(item_preset INTERFACE
    armor_preset.h
    item_preset.h
    weapon_preset.h
)

target_include_directories(item_preset INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_library(item_preset::item_preset ALIAS item_preset)

target_link_libraries(item_preset
    INTERFACE
        item_data_types::item_data_types
)
