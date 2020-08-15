cmake_minimum_required(VERSION 3.0)
project(mapmanager C)

set(CMAKE_C_STANDARD 99)

find_package(Iconv REQUIRED)
include_directories(${Iconv_INCLUDES})
add_executable(mapmanager main.c record.c map.h list.c array.c)
set_target_properties(mapmanager PROPERTIES COMPILE_FLAGS "-Wno-unused-result -fsanitize=address")
target_link_libraries(mapmanager -fsanitize=address)

if (APPLE)
    target_link_libraries(mapmanager -liconv)
endif()
