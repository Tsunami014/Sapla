cmake_minimum_required(VERSION 3.16)
set(PLUG_NAME grid)
set(PLUG_SOURCES
    game.cpp gridResources.qrc
    playScn.cpp
    items/gridLayout.cpp
    items/cardIt.cpp
)
include("${CMAKE_CURRENT_LIST_DIR}/../base.cmake")
