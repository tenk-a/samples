cmake_minimum_required(VERSION 3.24)

#	#	#	#	#	#	#	#	#	#

project(hello C)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")

set(SRCS
  ${SRC_DIR}/hello.c
)

set(INC_DIRS
  ${SRC_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(OPTS )
set(LIB_DIRS )
set(LIBS )

#	#	#	#	#	#	#	#	#	#

if(TOOLCHAIN_NAME MATCHES "mac" OR TOOLCHAIN_NAME MATCHES "unix")
  list(APPEND LIBS "ncurses")
elseif(TOOLCHAIN_NAME MATCHES "vc")
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

if(TOOLCHAIN_TARGET_PLATFORM MATCHES "win" OR TOOLCHAIN_TARGET_PLATFORM MATCHES "dos")
  list(APPEND LIB_DIRS "${THIRDPARTY_DIR}/lib/${TOOLCHAIN_NAME}")
  list(APPEND INC_DIRS "${THIRDPARTY_DIR}/include")
  list(APPEND LIBS "pdcurses")
  list(APPEND OPTS "-DUSE_PDCURSES")
endif()

#	#	#	#	#	#	#	#	#	#

add_executable(${PROJECT_NAME}
  ${SRCS}
  ${TOOLCHAIN_DEFAULT_SOURCES}
)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION "${CMAKE_SOURCE_DIR}/bin/${TOOLCHAIN_NAME}"
)

target_compile_options(${PROJECT_NAME} PRIVATE
  ${TOOLCHAIN_DEFAULT_OPTIONS}
  ${OPTS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${TOOLCHAIN_DEFAULT_INCLUDE_DIRS}
  ${INC_DIRS}
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${LIB_DIRS}
  ${TOOLCHAIN_DEFAULT_LINK_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${LIBS}
  ${TOOLCHAIN_DEFAULT_LIBS}
)
