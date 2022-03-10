cmake_minimum_required(VERSION 3.16)

project(NDSR_grb_allpaths VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 17)

option(CXX "enable C++ compilation" ON)
if(CXX)
    enable_language(CXX)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


find_package(GUROBI REQUIRED)

include_directories(${GUROBI_INCLUDE_DIRS})

add_subdirectory(src)


if(CXX)
    set(CMAKE_CXX_STANDARD 17)
    target_link_libraries(${CMAKE_PROJECT_NAME}  optimized ${GUROBI_CXX_LIBRARY}  debug ${GUROBI_CXX_DEBUG_LIBRARY})
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}  ${GUROBI_LIBRARY} )


if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    include(FeatureSummary)
    feature_summary(WHAT ALL)
endif()




