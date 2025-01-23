cmake_minimum_required(VERSION 3.5)
project(A2)

set(A2_SOURCE_FILES
        definitions.h
        resources.h
        vector.c
        vector.h
        restaurant.c)

add_compile_options(-Wall -Werror)
add_executable(restaurant ${A2_SOURCE_FILES})
target_link_libraries(restaurant pthread)

