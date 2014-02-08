CMake_Minimum_Required(VERSION 2.8.12)
Project(simplex)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
Include_Directories("src")
FILE(GLOB_RECURSE sources "src/*.cpp")
Add_Executable (simplex ${sources}) 

