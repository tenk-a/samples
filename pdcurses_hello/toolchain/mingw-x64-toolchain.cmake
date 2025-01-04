set(TOOLCHAIN_NAME "mingw-x64" CACHE STRING "Toolchain name")
set(TOOLCHAIN_TARGET_PLATFORM "win64" CACHE STRING "Toolchain Target Platform" FORCE)

add_compile_options(-finput-charset=utf-8 -fexec-charset=utf-8 -fwide-exec-charset=utf-32LE)
