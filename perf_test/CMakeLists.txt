cmake_minimum_required(VERSION 3.1)

project(rocksdb_test VERSION 0.1
                  DESCRIPTION "Performance test using RocksDB column family"
                  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")



add_executable(rocksdb_perf  main.cc Generator.cc Parts.cc Keys.cc KVEngine.cc Space.cc)
target_compile_features(rocksdb_perf PRIVATE cxx_std_17)
# rocksdb include and lib
find_path(ROCKSDB_INCLUDE_DIR 
    NAMES rocksdb/db.h
    PATHS /home/pengwei.song/code/rocksdb/include)
message(STATUS "finding rocksdb including: ${ROCKSDB_INCLUDE_DIR}")
find_library(ROCKSDB_LIB 
    NAMES rocksdb 
    PATHS /home/pengwei.song/code/rocksdb)
message(STATUS "finding rocksdb lib: ${ROCKSDB_LIB}")

# gflags include and lib
find_path(GFLAGS_INCLUDE_DIR NAMES gflags PATHS /home/pengwei.song/3rd-party/gflags/include)
message(STATUS "finding gflags including: ${GFLAGS_INCLUDE_DIR}")
find_library(GFLAGS_LIB NAMES libgflags.a PATHS /home/pengwei.song/3rd-party/gflags/lib)
message(STATUS "finding gflags lib ${GFLAGS_LIB}")

find_library(LZ4_LIB lz4)
message(STATUS "finding lz3 lib: ${LZ4_LIB}")

target_include_directories(rocksdb_perf PUBLIC ${ROCKSDB_INCLUDE_DIR} ${GFLAGS_INCLUDE_DIR})
target_link_libraries(rocksdb_perf ${ROCKSDB_LIB} ${LZ4_LIB} -lpthread  -lz -lsnappy -lbz2 -lzstd -ldl)
