include(${CMAKE_CURRENT_LIST_DIR}/vc-incl.cmake)
set(TOOLCHAIN_NAME "${TOOLCHAIN_NAME}-md" CACHE STRING "Toolchain name" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
