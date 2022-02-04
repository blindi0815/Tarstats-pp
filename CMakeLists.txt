cmake_minimum_required(VERSION 3.21)
project(Tarstats__)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=c++20")

add_executable(Tarstats__ main.cpp)
