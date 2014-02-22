cmake_minimum_required(VERSION 2.8.9)

project(MouseSim)

cmake_policy(SET CMP0020 NEW)
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
mark_as_advanced(Qt5Core_DIR)
mark_as_advanced(Qt5Gui_DIR)
mark_as_advanced(Qt5Widgets_DIR)

qt5_add_resources(MouseSim_qrc MouseSim.qrc)
add_executable(MouseSim WIN32 main.cpp MouseSim.cpp ${MouseSim_qrc} MouseSim.rc)
qt5_use_modules(MouseSim Widgets)
