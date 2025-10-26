cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 23)

project("skies")

# NixOS clangd workaround
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_executable(skies src/main.cpp)

target_include_directories(skies PRIVATE ${CMAKE_SOURCE_DIR}/vendor)