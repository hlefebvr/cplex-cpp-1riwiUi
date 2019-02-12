cmake_minimum_required(VERSION 3.13)
project(deterministic)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

add_definitions(-DUSE_CPLEX)

find_package(CPLEX REQUIRED)

include_directories(${CPLEX_INCLUDE_DIRS})

set(LIBS ${LIBS} ${CPLEX_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DNDEBUG -DIL_STD")
set(CMAKE_CXX_STANDARD 14)

add_executable(deterministic main.cpp)

target_link_libraries(deterministic ${CPLEX_LIBRARIES})

# add_custom_target(deterministic COMMAND make -C ${PROJECT_SOURCE_DIR} CLION_EXE_DIR=${PROJECT_BINARY_DIR})