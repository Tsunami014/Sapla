cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(APP_NAME    Sapla)
set(APP_OUTPUT  sapla)
set(SOURCES
    main.cpp
    game.cpp
    cards/cardTyps.cpp
    cards/formElms.cpp
    cards/getCards.cpp
    cards/cardTree.cpp
    scenes/homeScn.cpp
    scenes/browseScn.cpp
    scenes/playScn.cpp
    scenes/winScn.cpp
    items/listWid.cpp
    items/gridLayout.cpp
    items/sideRends.cpp
    items/cardIt.cpp
    items/progress.cpp
    items/treeItem.cpp
    items/base/svgBtnItem.cpp
    items/base/svgItem.cpp
    items/base/rectIt.cpp
)
set(RESOURCES
    resources.qrc
)

project(${APP_NAME} LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Svg Widgets)
add_executable(${APP_NAME} ${SOURCES} ${RESOURCES})
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME ${APP_OUTPUT})
target_link_libraries(${APP_NAME} PRIVATE Qt6::Svg Qt6::Widgets)
