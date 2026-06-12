cmake_minimum_required(VERSION 3.30)
project(s3d_512_zh C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(s3d_512_zh SHARED s3d_512_zh.c version.def)

target_compile_options(s3d_512_zh PRIVATE
        -fvisibility=hidden -fdata-sections -ffunction-sections)
target_link_options(s3d_512_zh PRIVATE
        -fvisibility=hidden -s -static-libgcc -Wl,--gc-sections -nostartfiles
)
set_target_properties(s3d_512_zh PROPERTIES OUTPUT_NAME "version" PREFIX "")
