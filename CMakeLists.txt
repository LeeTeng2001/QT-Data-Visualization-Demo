cmake_minimum_required(VERSION 3.21)
project(SemEndProjectQT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# Profiling
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Charts
        Sql
        REQUIRED)

include_directories(src)

file(GLOB SOURCES
        src/*.cpp
        src/*.ui
        src/*.qml
        )

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Charts
        Qt::Sql
        )

