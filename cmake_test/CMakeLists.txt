
cmake_minimum_required(VERSION 2.8)
project(column_family_test)



set(CMAKE_CXX_STANDARD 11 ) 
set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


message(STATUS "Compiler_version ${CMAKE_CXX_COMPILER_VERSION}")

find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h PATHS)
include_directories(${ROCKSDB_INCLUDE_DIR})

message(STATUS "finding ${ROCKSDB_INCLUDE_DIR}")

find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h PATHS )
include_directories(${GFLAGS_INCLUDE_DIR})

message(STATUS "finding ${GFLAGS_INCLUDE_DIR}")

find_library(ROCKSDB_LIB rocksdb)
message(STATUS "finding ${ROCKSDB_LIB}")


find_library(GFLAGS_LIB libgflags.so.2.2.2 /usr/lib)
message(STATUS "finding ${GFLAGS_LIB}")

include_directories(${PROJECT_SOURCE_DIR}/include)


add_executable(two_comparator 
    src/two_comparator.cpp
    src/one_db_map_one_partition.cpp
    src/one_cf_map_one_partition.cpp
    src/vertex_options.cpp
    src/generate_vertex_edge.cpp
    src/util.cpp)
target_link_libraries(two_comparator ${ROCKSDB_LIB} ${GFLAGS_LIB} -lpthread)
