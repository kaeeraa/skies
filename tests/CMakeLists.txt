include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz)
FetchContent_MakeAvailable(googletest)

add_executable(query_tests QueryTests.cpp)
target_link_libraries(query_tests PRIVATE gtest_main ${Boost_LIBRARIES})
target_include_directories(query_tests PRIVATE ${PROJECT_SOURCE_DIR}/src
                                               ${PROTO_BINARY_DIR})

add_executable(containers_handler_tests ContainersHandlerTests.cpp)
target_include_directories(
  containers_handler_tests PRIVATE ${PROJECT_SOURCE_DIR}/src
                                   ${PROTO_BINARY_DIR})
target_link_libraries(containers_handler_tests PRIVATE gtest_main skies_core
                                                       ${SKIES_LIBS})

include(GoogleTest)
gtest_discover_tests(query_tests)
gtest_discover_tests(containers_handler_tests)
