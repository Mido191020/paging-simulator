cmake_minimum_required(VERSION 3.10)
project(PagingSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

# Milestone 3 Target (Linear LRU)
add_executable(paging_sim_m3 src/main_m3.cpp)

# Milestone 4 Target (Multi-Level)
add_executable(paging_sim_m4 src/main_m4.cpp)
