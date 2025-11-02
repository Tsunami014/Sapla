cmake_minimum_required(VERSION 3.16)
set(GAME_NAME gridGame)
set(GAME_SOURCES
    game.cpp gridResources.qrc

    playScn.cpp
    items/cardIt.cpp
    items/gridLayout.cpp
)
include("${CMAKE_CURRENT_LIST_DIR}/../base.cmake")
