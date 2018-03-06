# Builds the test/sample program(s)
project(config)

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# select the "Release" build type by default
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

# The library can use regex from c++11 or from boost
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  message(STATUS "Detected gcc!")
  set (CMAKE_CXX_FLAGS "-std=c++11")
  set (USE_BOOST_REGEX 1)
else()
  set (CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")
  set (USE_BOOST_REGEX 0)
endif()

# Source files
file (GLOB SRCS "src/*.cpp")
include_directories("include")

if(${USE_BOOST_REGEX})
  set_source_files_properties(ext/config/config.cpp PROPERTIES COMPILE_DEFINITIONS USE_BOOST_REGEX)
endif()

### Boost librairies ###
# the list of boost librairies we need:
if(${USE_BOOST_REGEX})
  set(REQ_BOOST_LIBS system filesystem regex)
else()
  set(REQ_BOOST_LIBS system filesystem)
endif()
# NOTE: boost must be compiled with c++11 flags
find_package(Boost REQUIRED ${REQ_BOOST_LIBS})
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Boost is required")
endif()

add_executable(test1 "tests/test1.cpp" ${SRCS})

target_link_libraries (test1
  ${Boost_LIBRARIES}
  )

# Installation
set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/")
install(TARGETS test1 DESTINATION "run")
