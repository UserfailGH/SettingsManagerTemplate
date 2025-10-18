cmake_minimum_required(VERSION 3.16)
project(My1stProj LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 (Widgets module)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Add the executable
add_executable(My1stProj
    main.cpp


    settingsitem.h settingsitem.cpp

    comboboxfactory.h comboboxfactory.cpp
    checkboxfactory.h checkboxfactory.cpp
    spinboxfactory.h spinboxfactory.cpp
    settingswindow.h settingswindow.cpp
    settingscontrolfactory.h
    lineeditfactory.h lineeditfactory.cpp
    pushbuttonfactory.h pushbuttonfactory.cpp
    filebrowsefactory.h filebrowsefactory.cpp
    colordialogfactory.h colordialogfactory.cpp
    settingswidgetbuilder.h settingswidgetbuilder.cpp


)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# Ensure moc, uic, and rcc are run (Qt6 handles this automatically via target_link_libraries)
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON  # Enable automatic moc processing
    AUTOUIC ON  # Enable automatic uic processing (if you have .ui files)
    AUTORCC ON  # Enable automatic rcc processing (if you have .qrc files)
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Optionally, add include directories for Qt6
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt6Widgets_INCLUDE_DIRS})
