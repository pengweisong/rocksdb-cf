cmake_minimum_required(VERSION 3.1)

project(rocksdb_test VERSION 0.1
                  DESCRIPTION "Performance test using RocksDB column family"
                  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


message(STATUS "cmake flag:" ${CMAKE_BUILD_TYPE})



add_executable(rocksdb_perf  main.cc Generator.cc Parts.cc Keys.cc Space.cc KVEngine.cc  PartTracker.cc)
target_compile_features(rocksdb_perf PRIVATE cxx_std_17)
# rocksdb include and lib
find_path(ROCKSDB_INCLUDE_DIR 
    NAMES rocksdb/db.h
    PATHS /usr/include/rocksdb)
message(STATUS "finding rocksdb including: ${ROCKSDB_INCLUDE_DIR}")
find_library(ROCKSDB_LIB 
    NAMES rocksdb 
    PATHS /usr/lib)
message(STATUS "finding rocksdb lib: ${ROCKSDB_LIB}")

# gflags include and lib
find_path(GFLAGS_INCLUDE_DIR NAMES gflags PATH /usr/include)
message(STATUS "finding gflags including: ${GFLAGS_INCLUDE_DIR}")
find_library(GFLAGS_LIB NAMES libgflags.a PATHS /usr/lib)
message(STATUS "finding gflags lib ${GFLAGS_LIB}")

find_library(LZ4_LIB lz4)
message(STATUS "finding lz3 lib: ${LZ4_LIB}")

target_include_directories(rocksdb_perf PUBLIC ${ROCKSDB_INCLUDE_DIR} ${GFLAGS_INCLUDE_DIR})
target_link_libraries(rocksdb_perf ${ROCKSDB_LIB} ${LZ4_LIB} -lpthread  -lz -lsnappy -lbz2 -lzstd -ldl)
