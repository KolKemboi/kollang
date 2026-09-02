cmake_minimum_required(VERSION 3.20)
set(project_name kompiler)

project(${project_name})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)


add_executable(
	${project_name}
	src/main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src/headers)
