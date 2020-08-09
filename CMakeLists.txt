cmake_minimum_required(VERSION 3.17)
project(mapmanager C)

set(CMAKE_C_STANDARD 99)

find_package(Iconv REQUIRED)
include_directories(${Iconv_INCLUDES})
add_executable(mapmanager main.c record.c map.h list.c array.c)
target_link_libraries(mapmanager ${Iconv_LIBS})
