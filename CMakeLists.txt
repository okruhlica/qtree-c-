#cmake_minimum_required(VERSION 3.8)

#project(QuadTree)
## target_compile_options(QuadTree PRIVATE /std:c++latest)
#add_subdirectory(src)
#add_subdirectory(test)

# ........................................
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
project (QuadTree)
#add_subdirectory(src)
add_library(QTreeLib src/qtree.cpp src/quadtree.h)
add_executable(QuadDemo src/main.cpp)
target_link_libraries(QuadDemo QTreeLib)

##################################
# Download and install GoogleTest

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(QuadTest test/qtree_test.cc)
target_link_libraries(QuadTest gtest_main)
target_link_libraries(QuadTest QTreeLib)

##################################
# Specify tests
add_test(NAME TrivialInsert COMMAND QuadTest)
add_test(NAME InsertValueProper COMMAND QuadTest)
add_test(NAME InsertValueOutOfBounds COMMAND QuadTest)
add_test(NAME InsertValueDuplicate COMMAND QuadTest)
add_test(NAME InsertSet01 COMMAND QuadTest)
add_test(NAME ContainsTrivial COMMAND QuadTest)