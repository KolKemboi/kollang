cmake_minimum_required(VERSION 3.20)

set(project_name kompiler)
project(${project_name})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM_DIR: ${LLVM_DIR}")

# LLVM configuration
separate_arguments(LLVM_DEFINITIONS_LIST
    NATIVE_COMMAND ${LLVM_DEFINITIONS}
)

add_executable(
    ${project_name}
    src/main.cpp
)

target_include_directories(${project_name} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/headers
    ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(${project_name} PRIVATE
    ${LLVM_DEFINITIONS_LIST}
)

# LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
    IRReader
)

target_link_libraries(${project_name} PRIVATE
    ${LLVM_LIBS}
)

file(COPY
    ${CMAKE_SOURCE_DIR}/src/main.kol
    DESTINATION ${CMAKE_BINARY_DIR}
)


include_directories(${CMAKE_SOURCE_DIR}/src/headers)
