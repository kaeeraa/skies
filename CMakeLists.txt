cmake_minimum_required(VERSION 4.1.2)
set(CMAKE_CXX_STANDARD 23)

project("skies")

# NixOS clangd workaround
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_executable(skies src/main.cpp)

find_package(boost_filesystem 1.81 REQUIRED)
target_include_directories(skies PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(skies PRIVATE ${Boost_LIBRARIES})
