cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)
cmake_policy(SET CMP0048 NEW)

project(MouseSim VERSION 1.1.2)

find_package(Git)
if(GIT_FOUND)
  execute_process(COMMAND "${GIT_EXECUTABLE}" "rev-parse" "--short=10" "HEAD"
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                  TIMEOUT 10
                  RESULT_VARIABLE RESULT
                  OUTPUT_VARIABLE GIT_VERSION
                  ERROR_QUIET
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(${RESULT} EQUAL 0 AND NOT "${GIT_VERSION}" EQUAL "${PROJECT_VERSION_TWEAK}")
    set(PROJECT_VERSION_TWEAK ${GIT_VERSION} CACHE STRING "Commit Hash" FORCE)
  endif()
endif()

mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
option(BUILD_STATIC "Build with static Qt libs" OFF)
option(BUILD_PACKAGE "Package with NSIS" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions(/W4 /WX /MP)

find_package(Qt5Widgets REQUIRED)
mark_as_advanced(
  Qt5Core_DIR
  Qt5Gui_DIR
  Qt5Widgets_DIR
)
if(${Qt5Widgets_VERSION} VERSION_LESS "5.4")
  message(FATAL_ERROR "Minimum supported Qt is v5.4")
endif()

configure_file(AboutBox.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/AboutBox.hpp")
configure_file(MouseSim.rc.in "${CMAKE_CURRENT_BINARY_DIR}/MouseSim.rc")
add_executable(MouseSim WIN32 main.cpp MouseSim.cpp MouseSim.qrc "${CMAKE_CURRENT_BINARY_DIR}/MouseSim.rc")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Build Configuration" FORCE)
if(BUILD_STATIC)
  message(STATUS "Note: Static builds require a custom static build of Qt")

  find_package(Qt5OpenGL REQUIRED)
  mark_as_advanced(Qt5OpenGL_DIR)

  add_definitions(-DBUILD_STATIC)

  get_target_property(QtCore_LOCATION Qt5::Core LOCATION)
  get_filename_component(QT_LIB_DIR ${QtCore_LOCATION} PATH)

  target_link_libraries(MouseSim Qt5::OpenGL OpenGL32 imm32 winmm ws2_32)

  find_library(HARFBUZZ qtharfbuzzng ${QT_LIB_DIR})
  find_library(PLATFORM_SUPPORT Qt5PlatformSupport ${QT_LIB_DIR})
  find_library(WIN_PLATFORM qwindows ${QT_LIB_DIR}/../plugins/platforms/)
  mark_as_advanced(HARFBUZZ PLATFORM_SUPPORT WIN_PLATFORM)
  target_link_libraries(MouseSim optimized ${HARFBUZZ} optimized ${PLATFORM_SUPPORT} optimized ${WIN_PLATFORM})

  find_library(HARFBUZZ_D qtharfbuzzngd ${QT_LIB_DIR})
  find_library(PLATFORM_SUPPORT_D Qt5PlatformSupportd ${QT_LIB_DIR})
  find_library(WIN_PLATFORM_D qwindowsd ${QT_LIB_DIR}/../plugins/platforms/)
  mark_as_advanced(HARFBUZZ_D PLATFORM_SUPPORT_D WIN_PLATFORM_D)
  if(NOT ${HARFBUZZ_D} STREQUAL "HARFBUZZ_D-NOTFOUND" AND NOT ${PLATFORM_SUPPORT_D} STREQUAL "PLATFORM_SUPPORT_D-NOTFOUND" AND NOT ${WIN_PLATFORM_D} STREQUAL "WIN_PLATFORM_D-NOTFOUND")
    target_link_libraries(MouseSim debug ${HARFBUZZ_D} debug ${PLATFORM_SUPPORT_D} debug ${WIN_PLATFORM_D})
  else()
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "Build Configuration" FORCE)
  endif()
else()
  target_link_libraries(MouseSim Qt5::Widgets)

  get_target_property(QPA_PLUGIN Qt5::QWindowsIntegrationPlugin LOCATION)
  get_filename_component(QPA_PLUGIN_NAME "${QPA_PLUGIN}" NAME)
  install(PROGRAMS "${QPA_PLUGIN}"
          DESTINATION "platforms"
  )
endif()

install(TARGETS MouseSim
  RUNTIME
  DESTINATION /
)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP ON)
include(InstallRequiredSystemLibraries)
if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
  install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "/")

  if(NOT BUILD_STATIC)
    set(LIBRARY_SEARCH_DIRECTORY "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/Release" "${CMAKE_BINARY_DIR}/Debug")
    install(CODE "
      include(BundleUtilities)
      fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/MouseSim.exe\" \"\${CMAKE_INSTALL_PREFIX}/platforms/${QPA_PLUGIN_NAME}\" \"${LIBRARY_SEARCH_DIRECTORY}\")
    ")
  endif()
endif()

if(BUILD_PACKAGE)
  configure_file("${CMAKE_SOURCE_DIR}/icons/nsis.bmp" "${CMAKE_BINARY_DIR}/nsis.bmp" COPYONLY)

  set(FULL_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

  set(CPACK_PACKAGE_INSTALL_DIRECTORY "MouseSim")
  set(CPACK_PACKAGE_VENDOR "Alex Swindler")
  set(CPACK_PACKAGE_CONTACT "${CPACK_PACKAGE_VENDOR} <acs7x@virginia.edu>")

  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${FULL_VERSION_STRING}")
  if(CMAKE_CL_64)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-Win64")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
  endif()

  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

  set(CPACK_PACKAGE_EXECUTABLES "MouseSim" "MouseSim")

  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MouseSim is a simple tool to simulate mouse movement and avoid computer inactivity.")

  set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/CPack.Description.txt")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/CPack.Description.txt")
  set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/CPack.Welcome.txt")

  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/icons/mouse.ico")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "MouseSim.exe")
  set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/axelstudios/MouseSim")
  set(CPACK_NSIS_HELP_LINK "https://github.com/axelstudios/MouseSim")
  set(CPACK_NSIS_CREATE_ICONS "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\MouseSim.lnk\\\" \\\"$INSTDIR\\\\MouseSim.exe\\\"")

  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    WriteRegStr HKCU 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run' 'MouseSim' '\\\"$INSTDIR\\\\MouseSim.exe\\\"'
  ")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    DeleteRegValue HKCU 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run' 'MouseSim'
  ")

  set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};Required;ALL;/")

  include(CPack)
endif()
