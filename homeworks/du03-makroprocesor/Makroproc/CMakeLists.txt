cmake_minimum_required(VERSION 3.8)

project(Translator)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)

add_executable(makroproc mprocLib.cpp main.cpp)
