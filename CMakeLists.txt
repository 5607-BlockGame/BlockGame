cmake_minimum_required(VERSION 3.17)
project(BlockGame)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)

include_directories(${SDL2_INCLUDE_DIRS})

include_directories(src)

include_directories(lib/glad)
include_directories(lib/glm)
include_directories(lib/glm/detail)
include_directories(lib/glm/ext)
include_directories(lib/glm/gtc)
include_directories(lib/glm/gtx)
include_directories(lib/glm/simd)

add_subdirectory(lib/glad)
add_subdirectory(lib/glm)

set(SOURCES src/main.cpp)

add_executable(BlockGame ${SOURCES})

target_link_libraries(BlockGame glm glad ${SDL2_LIBRARIES} Boost::boost)

# add_library(proj4-lib ${SOURCES})
# target_link_libraries(proj4-lib glm glad ${SDL2_LIBRARIES} Boost::boost)

