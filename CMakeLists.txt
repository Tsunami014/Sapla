cmake_minimum_required(VERSION 3.16)

set(APP_NAME    Sapla)
set(APP_OUTPUT  sapla)
set(SOURCES
    main.cpp
    game.cpp
)

project(${APP_NAME} LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_executable(${APP_NAME} ${SOURCES})
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME ${APP_OUTPUT})
target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets)

