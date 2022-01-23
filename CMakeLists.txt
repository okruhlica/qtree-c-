cmake_minimum_required(VERSION 3.21)
project(QuadTree)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES  main.cpp qtree.cpp qtree.h)
add_executable(QuadTree ${SOURCE_FILES} qtree.cpp qtree.h)

