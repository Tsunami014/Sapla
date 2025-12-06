cmake_minimum_required(VERSION 3.16)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_NAME    Sapla)
set(CORE_NAME   core)
set(APP_OUTPUT  Sapla)
set(SOURCES
    main.cpp
)
set(CORELIB
    core.cpp
    game.cpp styles.cpp
    log.cpp  menu.cpp
    resources.qrc
    plugins/getPlugins.cpp

    base/font.cpp
    base/parseMd.cpp
    base/markdown.cpp
    base/rectIt.cpp
    base/svgRend.cpp
    base/svgUtils.cpp
    base/svgWid.hpp

    wids/topbar.cpp
    wids/ovrlWrap.cpp
    wids/listWid.cpp
    wids/svgBtn.cpp
    wids/svgPic.cpp

    items/cardIt.cpp
    items/progress.cpp
    items/treeItem.cpp

    notes/note.cpp
    notes/schedule.cpp
    notes/features.cpp
    notes/getNotes.cpp
    notes/noteTree.cpp
    notes/cardList.cpp

    scenes/baseScn.cpp
    scenes/gameScn.cpp
    scenes/homeScn.cpp
    scenes/winScn.cpp
    scenes/browseScn.cpp
    scenes/pluginView.cpp
)

# Do not complain about 'not used directly' things (as they are used, just indirectly)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wno-unused-include-directive)
endif()

project(${APP_NAME} LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Core Svg Widgets)

add_library(${CORE_NAME} SHARED ${CORELIB})
set_target_properties(${CORE_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(${CORE_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(${CORE_NAME} PRIVATE Qt6::Core Qt6::Svg Qt6::Widgets)

add_executable(${APP_NAME} ${SOURCES})
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME ${APP_OUTPUT})
target_link_libraries(${APP_NAME} PRIVATE Qt6::Core Qt6::Svg Qt6::Widgets)
target_link_libraries(${APP_NAME} PRIVATE core)

add_subdirectory(plugins/builtin)
option(BUILD_PLUGINS "Build plugins" ON)
if (BUILD_PLUGINS)
    add_subdirectory(plugins/grid)
    add_subdirectory(plugins/winter)
endif()


add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo " \\> Successfully built to \\\"$<TARGET_FILE:${APP_NAME}>\\\"!"
    COMMAND ${CMAKE_COMMAND} -E echo " \\> Plugins built are located in \\\"${CMAKE_BINARY_DIR}/outPlugs/\\\""
)

