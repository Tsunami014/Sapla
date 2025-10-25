cmake_minimum_required(VERSION 3.16)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(GAME_NAME GridGame)

project(${GAME_NAME} LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)

set(SOURCES
    game.cpp
    playScn.cpp
    winScn.cpp
    items/cardIt.cpp
    items/gridLayout.cpp
)

# Do not complain about 'not used directly' things (as they are used, just indirectly)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wno-unused-include-directive)
endif()

add_library(${GAME_NAME} MODULE ${SOURCES})
target_link_libraries(${GAME_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Svg)
target_link_libraries(${GAME_NAME} PRIVATE corelib)
set_target_properties(${GAME_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gamePlugs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gamePlugs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gamePlugs
)

