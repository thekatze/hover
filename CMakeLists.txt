cmake_minimum_required(VERSION 3.25.2)
project(Hover)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(hover main.cpp)
target_link_libraries(hover ${CONAN_LIBS})

