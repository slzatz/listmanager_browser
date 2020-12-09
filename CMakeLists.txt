cmake_minimum_required(VERSION 3.3.2)

project(lm_browser C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(ZeroMQ QUIET)

if(NOT ZeroMQ_FOUND)
  message(STATUS "CMake libzmq package not found, trying again with pkg-config (normal install of zeromq)")
  list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/libzmq-pkg-config)
  find_package(ZeroMQ REQUIRED)
endif()

include(cmake/App.cmake)

set(SOURCES "src/Browser.h"
            "src/Browser.cpp"
            "src/Tab.h"
            "src/Tab.cpp"
            "src/UI.h"
            "src/UI.cpp"
            "src/main.cpp")

add_app("${SOURCES}")

target_link_directories(${PROJECT_NAME} PUBLIC . ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${ZeroMQ_LIBRARY})
