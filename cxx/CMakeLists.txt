cmake_minimum_required (VERSION 3.13)
if (MSVC)
set (CMAKE_SYSTEM_VERSION 10.0.22000.0)
endif (MSVC)
project (Math VERSION 2.5.0.17 DESCRIPTION "HSS Math Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

SET(XERCES_C_INCLUDE_DIR "error" CACHE STRING "The path to the includes from Xerces-c")
SET(LOWLEVEL_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS LowLevel")
SET(MULTITHREAD_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS Multithread")
SET(ERROR_CALC_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS Error Calc")
SET(MATH_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS Math")
SET(GEOGRAPHY_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS Geography")
SET(THIRD_PARTY_INCLUDE_DIR "error" CACHE STRING "The path to third party include files")
SET(BOOST_INCLUDE_DIR "error" CACHE STRING "The path to the boost libraries includes")
SET(GDAL_INCLUDE_DIR "error" CACHE STRING "The path to the GDAL libraries includes")
SET(PROTOBUF_INCLUDE_DIR "error" CACHE STRING "The path to the protobuf libraries includes")
SET(GTEST_INCLUDE_DIR "error" CACHE STRING "The path to the Google Test includes")
SET(GTEST_LIBRARY_DIR "error" CACHE STRING "The path to the Google Test libraries")

SET(BOOST_LIBRARY_DIR "error" CACHE STRING "The path to the boost libraries")

find_library(FOUND_GDAL_LIBRARY_PATH NAMES gdal gdal_i REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_XERCES_LIBRARY_PATH NAMES xerces xerces-c_3 xerces-c REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_ZLIB_LIBRARY_PATH NAMES zlib z REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_CURL_LIBRARY_PATH NAMES curl libcurl REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_CRYPTO_LIBRARY_PATH NAMES crypto libcrypto REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_MINIZIP_LIBRARY_PATH NAMES minizip REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_PROTOBUF_LIBRARY_PATH NAMES protobuf libprotobuf REQUIRED PATHS ${PROTOBUF_LIBRARY_DIR})
find_library(FOUND_MULTITHREAD_LIBRARY_PATH NAMES Multithread REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_ERROR_CALC_LIBRARY_PATH NAMES ErrorCalc REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_LOWLEVEL_LIBRARY_PATH NAMES LowLevel REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_MATH_LIBRARY_PATH NAMES hssmath REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_GEOGRAPHY_LIBRARY_PATH NAMES hssgeography REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_GTEST_LIBRARY_PATH NAMES gtest PATHS ${GTEST_LIBRARY_DIR})
find_library(FOUND_GTEST_MAIN_LIBRARY_PATH NAMES gtest_main PATHS ${GTEST_LIBRARY_DIR})

enable_testing()

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -DPROTOBUF_USE_DLLS -DBOOST_ALL_DYN_LINK -D_CRT_SECURE_NO_WARNINGS")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD -D_AFXDLL")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd -D_AFXDLL /W4")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS chrono REQUIRED PATHS ${BOOST_LIBRARY_DIR})
else ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_NO_MFC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

set(Boost_LIBRARIES "")
endif (MSVC)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_CANADA -DAUTO_DOWNLOAD=0")

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else ()
    message(WARNING "OpenMP not found, disabling multithreading")
endif ()

add_library(WTime SHARED
    src/generated/wtime.pb.cc
    src/SunriseSunsetCalc.cpp
    src/Times.cpp
    src/TimezoneMapper.cpp
    src/worldlocation.cpp
    src/WTimeProto.cpp
    src/tz/tz.cpp
    src/zonedetect.c
    src/open/out_v1/timezone21.c
    src/open/tzdb-2021e-src/africa.c
    src/open/tzdb-2021e-src/antarctica.c
    src/open/tzdb-2021e-src/asia.c
    src/open/tzdb-2021e-src/australasia.c
    src/open/tzdb-2021e-src/backward.c
    src/open/tzdb-2021e-src/etcetera.c
    src/open/tzdb-2021e-src/europe.c
    src/open/tzdb-2021e-src/leapseconds.c
    src/open/tzdb-2021e-src/northamerica.c
    src/open/tzdb-2021e-src/southamerica.c
    src/open/tzdb-2021e-src/version.c
    src/open/tzdb-2021e-src/windowsZones.c
    include/internal/RegionMap.inl
    include/internal/SunriseSunsetCalc.h
    include/internal/Times.h
    include/internal/times_internal.h
    include/internal/worldlocation.h
    include/internal/WTimeProto.h
    include/config.h
    include/TimeZoneMapper.h
    include/WTime.h
    include/wtime.pb.h
)
if (MSVC)
target_compile_definitions(WTime PRIVATE
    DATE_BUILD_DLL
    TIMES_EXPORT
    "ZD_EXPORT=__declspec(dllexport)"
)
set_source_files_properties(
    src/open/out_v1/timezone21.c
    src/open/tzdb-2021e-src/africa.c
    src/open/tzdb-2021e-src/antarctica.c
    src/open/tzdb-2021e-src/asia.c
    src/open/tzdb-2021e-src/australasia.c
    src/open/tzdb-2021e-src/backward.c
    src/open/tzdb-2021e-src/etcetera.c
    src/open/tzdb-2021e-src/europe.c
    src/open/tzdb-2021e-src/leapseconds.c
    src/open/tzdb-2021e-src/northamerica.c
    src/open/tzdb-2021e-src/southamerica.c
    src/open/tzdb-2021e-src/version.c
    src/open/tzdb-2021e-src/windowsZones.c
    PROPERTIES LANGUAGE CXX
)
else ()
target_compile_definitions(WTime PRIVATE
    DATE_BUILD_DLL
    TIMES_EXPORT
)
endif ()

add_executable(WTimeTest
    test/gtest.cpp
    test/spanGTest.cpp
)

target_include_directories(WTime PUBLIC
    ${XERCES_C_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIR}
    ${BOOST_INCLUDE_DIR}
    ${GEOGRAPHY_INCLUDE_DIR}
    ${ERROR_CALC_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${LOWLEVEL_INCLUDE_DIR}
    ${MULTITHREAD_INCLUDE_DIR}
    ${THIRD_PARTY_INCLUDE_DIR}
    ${GDAL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/include/library
    ${CMAKE_CURRENT_SOURCE_DIR}/include/open/out_v1
    ${CMAKE_CURRENT_SOURCE_DIR}/include/open/date/include
)

target_include_directories(WTimeTest PUBLIC
    ${XERCES_C_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIR}
    ${BOOST_INCLUDE_DIR}
    ${GEOGRAPHY_INCLUDE_DIR}
    ${ERROR_CALC_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${LOWLEVEL_INCLUDE_DIR}
    ${MULTITHREAD_INCLUDE_DIR}
    ${THIRD_PARTY_INCLUDE_DIR}
    ${GDAL_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/include/library
    ${CMAKE_CURRENT_SOURCE_DIR}/include/open/out_v1
    ${CMAKE_CURRENT_SOURCE_DIR}/include/open/date/include
)

set_target_properties(WTime PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})
set_target_properties(WTime PROPERTIES SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})
set_target_properties(WTime PROPERTIES DEFINE_SYMBOL "TIMES_EXPORT")

set_target_properties(WTime PROPERTIES PUBLIC_HEADER include/**)

target_link_libraries(WTime ${Boost_LIBRARIES} ${FOUND_MULTITHREAD_LIBRARY_PATH} ${FOUND_ERROR_CALC_LIBRARY_PATH})
target_link_libraries(WTime ${FOUND_LOWLEVEL_LIBRARY_PATH} ${FOUND_PROTOBUF_LIBRARY_PATH} ${FOUND_GDAL_LIBRARY_PATH} ${OpenMP_LIBRARIES})
target_link_libraries(WTime ${FOUND_ZLIB_LIBRARY_PATH} ${FOUND_CRYPTO_LIBRARY_PATH} ${FOUND_MINIZIP_LIBRARY_PATH})
target_link_libraries(WTime ${FOUND_MATH_LIBRARY_PATH} ${FOUND_GEOGRAPHY_LIBRARY_PATH})
if (MSVC)
else ()
target_link_libraries(WTime quadmath -lstdc++fs)
endif (MSVC)

target_link_libraries(WTimeTest ${Boost_LIBRARIES} ${FOUND_MULTITHREAD_LIBRARY_PATH} ${FOUND_ERROR_CALC_LIBRARY_PATH})
target_link_libraries(WTimeTest ${FOUND_LOWLEVEL_LIBRARY_PATH} ${FOUND_PROTOBUF_LIBRARY_PATH} ${FOUND_GDAL_LIBRARY_PATH} ${OpenMP_LIBRARIES})
target_link_libraries(WTimeTest ${FOUND_ZLIB_LIBRARY_PATH} ${FOUND_CRYPTO_LIBRARY_PATH} ${FOUND_MINIZIP_LIBRARY_PATH})
target_link_libraries(WTimeTest ${FOUND_MATH_LIBRARY_PATH} ${FOUND_GEOGRAPHY_LIBRARY_PATH})
target_link_libraries(WTimeTest ${FOUND_GTEST_LIBRARY_PATH} ${FOUND_GTEST_MAIN_LIBRARY_PATH} WTime)
if (MSVC)
else ()
target_link_libraries(WTimeTest pthread quadmath -lstdc++fs)
endif (MSVC)

add_test(WTimeTests WTimeTest)

configure_file(WTime.pc.in WTime.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/WTime.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
install(TARGETS WTime DESTINATION /usr/lib)
