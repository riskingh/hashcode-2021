cmake_minimum_required(VERSION 3.0)
project(hashcode)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

find_package(Boost 1.36.0 REQUIRED COMPONENTS locale)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(hashcode src/main.cpp src/io.cpp src/solutions/easy_solution.cpp src/solutions/iteration_solution.cpp)
target_link_libraries(hashcode Boost::locale)
