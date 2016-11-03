cmake_minimum_required (VERSION 2.6)
project (WTime)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

include_directories(include include/internal)

file(GLOB SOURCES "src/*.cpp" "include/*.h" "include/internal/*.h")

add_library(WTime STATIC ${SOURCES})

install(TARGETS WTime DESTINATION /usr/lib)