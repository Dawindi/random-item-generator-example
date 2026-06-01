# Core library — aggregates all core sub-libraries
add_library(core INTERFACE)

# UUID — unique identifier wrapper
add_library(uuid STATIC
    uuid.cc
    uuid.h
)
target_include_directories(uuid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(uuid::uuid ALIAS uuid)

add_subdirectory(item_generator)

target_link_libraries(core
    INTERFACE
        uuid
        item_generator
)
