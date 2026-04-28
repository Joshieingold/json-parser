cmake_minimum_required(VERSION 3.10)

project(MyCppProject)

set(CMAKE_CXX_STANDARD 17)

add_executable(Parser ./src/main.cpp ./src/JsonParser.cpp)
 
