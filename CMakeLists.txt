cmake_minimum_required(VERSION 4.1.2)
set(CMAKE_CXX_STANDARD 23)

# User options
option(BUILD_TESTS "Enable unit tests" ON)

set(PROTO_FILES api/v1/containers/Request.proto
                api/v1/containers/Response.proto api/v1/Error.proto)

project("skies")

find_package(Protobuf REQUIRED)

set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_protos)
file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})

add_library(protos STATIC "")
target_include_directories(protos PUBLIC ${PROTO_BINARY_DIR})
target_link_libraries(protos PUBLIC protobuf::libprotobuf)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)

find_package(Boost 1.81 REQUIRED COMPONENTS json log log_setup)
find_package(absl REQUIRED)
find_program(Mold mold)
find_program(CCache ccache)

# NixOS clangd workaround
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
set(CMAKE_CXX_CLANG_TIDY clang-tidy)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

if(ipo_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  message(STATUS "IPO / LTO enabled")
endif()

if(Mold)
  set(CMAKE_LINKER_TYPE MOLD)
  message(STATUS "Using mold as linker")
endif()

if(CCache)
  set(CMAKE_C_COMPILER_LAUNCHER ccache)
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
  message(STATUS "Using ccache")
endif()

protobuf_generate(TARGET protos PROTOS ${PROTO_FILES} PROTOC_OUT_DIR
                  ${PROTO_BINARY_DIR})

set(SKIES_LIBS
    protos
    ${Boost_LIBRARIES}
    protobuf::libprotobuf
    absl::status
    absl::statusor
    absl::log
    absl::strings
    absl::check
    absl::flat_hash_map)

if(BUILD_TESTS)
  enable_testing()
endif()

set(SKIES_SOURCES
    src/core/Router.cpp src/middleware/DockerMiddleware.cpp
    src/client/DockerClient.cpp src/utility/ProtoBuffer.cpp src/core/Server.cpp
    src/handlers/Containers.cpp)

add_library(skies_core STATIC ${SKIES_SOURCES})
target_include_directories(
  skies_core PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${PROTO_BINARY_DIR}
                     ${Boost_INCLUDE_DIRS})
target_compile_options(skies_core PRIVATE -Wall -Wextra -Wpedantic
                                          -Wno-nullability-extension)
target_link_libraries(skies_core PRIVATE ${SKIES_LIBS})

add_executable(skies src/EntryPoint.cpp)
target_compile_options(skies PRIVATE -Wall -Wextra -Wpedantic
                                     -Wno-nullability-extension)
target_include_directories(
  skies PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${PROTO_BINARY_DIR}
                ${Boost_INCLUDE_DIRS})
target_link_libraries(skies PRIVATE skies_core ${SKIES_LIBS})

if(BUILD_TESTS)
  add_subdirectory("tests")
endif()
