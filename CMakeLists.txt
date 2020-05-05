cmake_minimum_required( VERSION "3.10" )

set(this, lpp)
project(lpp CXX)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB Headers "inc/*.h")
file(GLOB Sources "src/*.cpp")

add_executable(lpp ${Sources} ${Headers})
target_include_directories(lpp PRIVATE "inc/")
