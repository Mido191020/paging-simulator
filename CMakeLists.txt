cmake_minimum_required(VERSION 3.10)
project(PagingSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the 'src' directory so headers are found
include_directories(src)

# Create the executable from src/main.cpp
add_executable(paging_sim src/main.cpp)
