cmake_minimum_required(VERSION 3.16)

project(udo VERSION 2.0.1
        DESCRIPTION "µDo! 2.0.1-rc1 - A minimalist, command-driven web browser by Tintin (github.com/Woofson/udo)"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineCore WebEngineWidgets)

add_executable(udo src/main.cpp src/MainWindow.h src/MainWindow.cpp src/ConfigManager.h src/ConfigManager.cpp src/DoBar.h src/DoBar.cpp src/AboutDialog.h src/AboutDialog.cpp src/HelpDialog.h src/HelpDialog.cpp src/HttpAuthDialog.h src/HttpAuthDialog.cpp)

target_link_libraries(udo PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::WebEngineCore Qt6::WebEngineWidgets)
