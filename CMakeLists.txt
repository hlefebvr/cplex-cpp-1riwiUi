cmake_minimum_required(VERSION 3.13)
project(deterministic)

# folder for cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# using cplex lib
add_definitions(-DUSE_CPLEX)
find_package(CPLEX REQUIRED)
include_directories(${CPLEX_INCLUDE_DIRS})
set(LIBS ${LIBS} ${CPLEX_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-attributes -fPIC -DNDEBUG -DIL_STD -pthread -Wl,--no-as-needed -ldl")
set(CMAKE_CXX_STANDARD 14)

# project files
add_executable(deterministic main.cpp Instance.cpp Instance.h MinimizeTardyJobsModel.cpp MinimizeTardyJobsModel.h)

# create symbolic link to data folder
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/${dir_name}/data/   ${CMAKE_CURRENT_BINARY_DIR}/${dir_name}/data)

# link compiled cplex library
target_link_libraries(deterministic ${CPLEX_LIBRARIES})

# add_custom_target(deterministic COMMAND make -C ${PROJECT_SOURCE_DIR} CLION_EXE_DIR=${PROJECT_BINARY_DIR})