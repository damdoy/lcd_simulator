#cmake for qt from https://github.com/jasondegraw/Qt-CMake-HelloWorld/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME lcd_simulator)

project(${PROJECT_NAME})

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#install of verilator is a bit broken on linux mint and "verilated.h" is hidden here
include_directories("/usr/share/verilator/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pthread")

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Widgets)

include_directories("verilator_output/")

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.hpp" "verilator_output/*.cpp" "/usr/share/verilator/include/verilated.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Add the Qt5 Widgets for linking
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
