if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

cmake_minimum_required(VERSION 3.11)
project(wx_hello)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32 AND MSVC)
    add_compile_options(-utf-8)
endif()

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(wx_hello 
    src/wx_hello.cpp
)

if(WIN32 AND MSVC)
    target_link_options(wx_hello PRIVATE /SUBSYSTEM:WINDOWS)
else()
  if(APPLE)
    target_link_libraries(wx_hello PRIVATE "-framework Cocoa")
  endif()
endif()

target_include_directories(wx_hello PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${wxWidgets_INCLUDE_DIRS}
)

target_link_libraries(wx_hello PRIVATE
    ${wxWidgets_LIBRARIES}
)
