# cmake の最低バージョン指定.
cmake_minimum_required(VERSION 3.18)

# CMAKE_TOOLCHAIN_FILE が無指定なら、環境変数 VCPKG_ROOT を元に設定.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

# VCPKG_TARGET_TRIPLET が無指定なら、環境変数 VCPKG_DEFAULT_TRIPLET で設定.
if(NOT DEFINED VCPKG_TARGET_TRIPLET AND DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}")
endif()

# プロジェクト名設定.
project(wx_hello)

# c++11 以降を使う指定.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vc ならソース＆charを UTF-8 で扱う指定.
if(WIN32 AND MSVC)
    add_compile_options(-utf-8)
endif()

# wxWidgets を使う指定.
find_package(wxWidgets REQUIRED COMPONENTS core base)

# wxWidgts の準備(setup.h 生成)
include(${wxWidgets_USE_FILE})

# ソース・ファイルの設定.
add_executable(${PROJECT_NAME}
    src/wx_hello.cpp
)

# ターゲット別のリンクに関する設定.
if(WIN32 AND MSVC)
    # vc で GUI アプリにするリンカオプション指定.
    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS)
else()
  if(APPLE)
    # Mac で Cocoa を使う指定.
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
  endif()
endif()

# include ディレクトリ設定.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${wxWidgets_INCLUDE_DIRS}
)

# ライブラリ・ディレクトリ設定.
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${wxWidgets_LIBRARIES}
)
