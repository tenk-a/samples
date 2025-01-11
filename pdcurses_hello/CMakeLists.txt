cmake_minimum_required(VERSION 3.24)

# cmake:プロジェクト.
project(hello C)

# ソース・ディレクトリのパス.
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
# thirdparty ディレクトリのパス.
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")

# ソース・ファイル.
set(SRCS
  ${SRC_DIR}/hello.c
)
# include ディレクトリ.
set(INC_DIRS
  ${SRC_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
# コンパイラ・オプション.
set(OPTS )
# ライブラリ・ディレクトリ.
set(LIB_DIRS )
# ライブラリ・ファイル.
set(LIBS )


if(TOOLCHAIN_NAME MATCHES "mac" OR TOOLCHAIN_NAME MATCHES "unix")
  # mac unix(linux) では ncurses を使う.
  list(APPEND LIBS "ncurses")
elseif(TOOLCHAIN_NAME MATCHES "vc")
  # VS で開いた時 hello プロジェクトがカレントになるようにする指定.
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

# Win / Dos なら PDCurses を使うための設定.
if(TOOLCHAIN_TARGET_PLATFORM MATCHES "win" OR TOOLCHAIN_TARGET_PLATFORM MATCHES "dos")
  list(APPEND LIB_DIRS "${THIRDPARTY_DIR}/lib/${TOOLCHAIN_NAME}")
  list(APPEND INC_DIRS "${THIRDPARTY_DIR}/include")
  list(APPEND LIBS "pdcurses")
  list(APPEND OPTS "-DUSE_PDCURSES")
endif()

# cmake:実行ファイル生成
add_executable(${PROJECT_NAME}
  ${SRCS}
  ${TOOLCHAIN_DEFAULT_SOURCES}
)
# cmake:インストール先を設定.
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION "${CMAKE_SOURCE_DIR}/bin/${TOOLCHAIN_NAME}"
)
# cmake:コンパイル・オプション設定.
target_compile_options(${PROJECT_NAME} PRIVATE
  ${TOOLCHAIN_DEFAULT_OPTIONS}
  ${OPTS}
)
# cmake: include ディレクトリ設定.
target_include_directories(${PROJECT_NAME} PRIVATE
  ${TOOLCHAIN_DEFAULT_INCLUDE_DIRS}
  ${INC_DIRS}
)
# cmake: ライブラリ・ディレクトリ設定.
target_link_directories(${PROJECT_NAME} PRIVATE
  ${LIB_DIRS}
  ${TOOLCHAIN_DEFAULT_LINK_DIRS}
)
# cmake: ライブラリ設定.
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${LIBS}
  ${TOOLCHAIN_DEFAULT_LIBS}
)
