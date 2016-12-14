cmake_minimum_required(VERSION 3.6)
project(revolver)

# general configurations
set(CMAKE_C_FLAGS "--std=c99 -Wall -pedantic")

# executable
set(src_exe src/main.c)
add_executable(revolver ${src_exe})
install(TARGETS revolver DESTINATION bin)
