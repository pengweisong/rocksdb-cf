cmake_minimum_required(VERSION 3.1)

project(rocksdb_test VERSION 0.1
                  DESCRIPTION "Performance test using RocksDB column family"
                  LANGUAGES CXX)

add_executable(rocksdb_perf main.cc)
target_compile_features(rocksdb_perf PRIVATE cxx_std_17)

# rocksdb include and lib
find_path(Rocksdb_INCLUDE_DIR NAMES rocksdb)
message(STATUS "finding rocksdb including ${ROCKSDB_INCLUDE_DIR}")
find_library(Rocksdb_LIB NAMES librocksdb.a)
message(STATUS "finding rocksdb lib ${ROCKSDB_LIB}")

# gflags include and lib
find_library(Glog_LIBRARY NAMES libglog.a)
message(STATUS "finding gflags including ${GFLAGS_INCLUDE_DIR}")
find_library(GFLAGS_LIB libgflags.a /usr/lib)
message(STATUS "finding gflags lib ${GFLAGS_LIB}")

target_include_directories(rocksdb_perf ${Rocksdb_INCLUDE_DIR} ${GFLAGS_INCLUDE_DIR})
target_link_libraries(rocksdb_perf ${ROCKSDB_LIB} ${GFLAGS_LIB} -lpthread)
