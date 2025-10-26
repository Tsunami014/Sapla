# Required variables: GAME_NAME,GAME_SOURCES 
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(${GAME_NAME} LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)

# Do not complain about 'not used directly' things (as they are used, just indirectly)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wno-unused-include-directive)
endif()

add_library(${GAME_NAME} MODULE ${GAME_SOURCES})
target_link_libraries(${GAME_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Svg)
target_link_libraries(${GAME_NAME} PRIVATE core)
set_target_properties(${GAME_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gamePlugs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gamePlugs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gamePlugs
)

