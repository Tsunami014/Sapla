cmake_minimum_required(VERSION 3.16)
set(PLUG_NAME builtin)
set(PLUG_SOURCES
    game.cpp BIResources.qrc
    playScn.cpp
)
include("${CMAKE_CURRENT_LIST_DIR}/../base.cmake")
set_target_properties(${PLUG_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

