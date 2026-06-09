add_library(item_data_types INTERFACE
    item_rarity.h
    item_status.h
    item_type.h
    item_damage_type.h
)

target_include_directories(item_data_types INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(item_data_types::item_data_types ALIAS item_data_types)
