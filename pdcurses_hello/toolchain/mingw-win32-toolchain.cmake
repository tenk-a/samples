set(TOOLCHAIN_NAME "mingw" CACHE STRING "Toolchain name")
set(TOOLCHAIN_TARGET_PLATFORM "win32" CACHE STRING "Toolchain Target Platform" FORCE)

add_compile_options(-finput-charset=utf-8 -fexec-charset=utf-8 -fwide-exec-charset=utf-32LE)
