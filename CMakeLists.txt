cmake_minimum_required(VERSION 3.6)
project(revolver)

# general configurations
set(CMAKE_C_FLAGS "--std=c99 -Wall -pedantic")
set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${root_dir}/include)

# library
set(src_lib
    src/revolver.c)
add_library(revolver SHARED ${src_lib})

# executable
set(src_exe src/main.c)
add_executable(revolver_exe ${src_exe})
set_target_properties(revolver_exe PROPERTIES OUTPUT_NAME revolver)
add_dependencies(revolver_exe revolver)
target_link_libraries(revolver_exe revolver)

# installation
install(TARGETS revolver DESTINATION lib)
install(TARGETS revolver_exe DESTINATION bin)
install(FILES include/revolver.h DESTINATION include)
