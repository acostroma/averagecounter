project (average_counter)

cmake_minimum_required (VERSION 3.18)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories (src)
file (GLOB_RECURSE SRCS "src/*.cpp")

add_executable (${PROJECT_NAME} ${SRCS})
