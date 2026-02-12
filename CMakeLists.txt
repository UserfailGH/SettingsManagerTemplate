cmake_minimum_required(VERSION 3.16)

project(My1stProj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "C:/Qt6/install")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_add_executable(My1stProj
        main.cpp
        checkboxfactory.cpp
        colordialogfactory.cpp
        comboboxfactory.cpp
        filebrowsefactory.cpp
        lineeditfactory.cpp
        pushbuttonfactory.cpp
        spinboxfactory.cpp
        settingscache.cpp
        settingscontrolfactory.cpp
        settingsitem.cpp
        settingswidgetbuilder.cpp
        settingswindow.cpp

        checkboxfactory.h
        colordialogfactory.h
        comboboxfactory.h
        filebrowsefactory.h
        lineeditfactory.h
        pushbuttonfactory.h
        spinboxfactory.h
        settingscache.h
        settingscontrolfactory.h
        settingsitem.h
        settingswidgetbuilder.h
        settingswindow.h

)

target_link_libraries(My1stProj PRIVATE Qt6::Core Qt6::Widgets)

set_target_properties(My1stProj PROPERTIES WIN32_EXECUTABLE ON)