cmake_minimum_required (VERSION 3.12)

project (WTime VERSION ${WTIME_VERSION} DESCRIPTION "C++ Time Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (MSVC)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif (MSVC)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")

add_library(WTime SHARED
    src/generated/wtime.pb.cc
    src/SunriseSunsetCalc.cpp
    src/Times.cpp
    src/TimezoneMapper.cpp
    src/worldlocation.cpp
    src/WTimeProto.cpp
    src/Canada_Detail.cpp
    include/internal/SunriseSunsetCalc.h
    include/internal/times_internal.h
    include/internal/Times.h
    include/internal/worldlocation.h
    include/internal/WTimeProto.h
    include/config.h
    include/WTime.h
    include/wtime.pb.h
)

target_include_directories(WTime PUBLIC
    include
    include/internal
    ${PROTOBUF_INCLUDE_DIR}
    ${BOOST_INCLUDE_DIR}
    ${GDAL_INCLUDE_DIRS}
)

set_target_properties(WTime PROPERTIES VERSION ${WTIME_VERSION})
set_target_properties(WTime PROPERTIES SOVERSION ${WTIME_SO_VERSION})
set_target_properties(WTime PROPERTIES DEFINE_SYMBOL "TIMES_EXPORT")

set_target_properties(WTime PROPERTIES PUBLIC_HEADER include/**)

target_link_libraries(WTime protobuf)

configure_file(WTime.pc.in WTime.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/WTime.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
install(TARGETS WTime DESTINATION /usr/lib)

set(WTIME_INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/internal
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
