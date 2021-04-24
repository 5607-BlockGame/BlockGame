cmake_minimum_required(VERSION 3.17)
project(BlockGame)

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
message(STATUS "SDL@ included at ${SDL2_INCLUDE_DIR}")

find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)
message(STATUS "Boost included at ${BOOST_INCLUDE_DIR}")

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

include_directories(${SDL2_INCLUDE_DIRS})

include_directories(src)

include_directories(lib/glad)

add_subdirectory(lib/glad)

set(SOURCES src/main.cpp src/repr/World.cpp src/repr/World.h src/vis/Scene.h src/State.cpp src/State.h src/utils.cpp src/utils.h src/generator/Generator.h src/repr/location.h src/repr/Chunk.cpp src/repr/Chunk.h src/repr/Block.cpp src/repr/Block.h src/generator/SimpleGenerator.cpp src/generator/SimpleGenerator.h src/vis/Models.cpp src/vis/Models.h)

add_executable(BlockGame ${SOURCES})

target_link_libraries(BlockGame glad ${SDL2_LIBRARIES} Boost::boost ${GLM_LIBRARIES})

# add_library(proj4-lib ${SOURCES})
# target_link_libraries(proj4-lib glm glad ${SDL2_LIBRARIES} Boost::boost)

