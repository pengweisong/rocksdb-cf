#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/utilities/checkpoint.h"

std::string kDBPath = "/tmp/different_column_family_test";

int main() {
  // open DB
  rocksdb::Options options;
  options.create_if_missing = true;
  rocksdb::DB *db = nullptr;
  rocksdb::Status s = rocksdb::DB::Open(options, kDBPath, &db);
  assert(s.ok());

  rocksdb::ColumnFamilyOptions cf1_options, cf2_options;
  cf1_options.write_buffer_size = 80920;
  cf2_options.write_buffer_size = 99999;
  rocksdb::DbPath path1, path2;
  path1.path = "/tmp/cf_one_path";
  path2.path = "/tmp/cf_two_path";
  cf1_options.cf_paths = {path1};
  cf2_options.cf_paths = {path2};
  rocksdb::ColumnFamilyHandle *cf1 = nullptr, *cf2 = nullptr;

  s = db->CreateColumnFamily(cf1_options, "cf_one", &cf1);
  assert(s.ok());
  s = db->CreateColumnFamily(cf2_options, "cf_two", &cf2);
  assert(s.ok());

  assert(db->DropColumnFamily(cf1).ok());
  assert(db->DropColumnFamily(cf2).ok());
  assert(db->DestroyColumnFamilyHandle(cf1).ok());
  assert(db->DestroyColumnFamilyHandle(cf2).ok());
  delete db;

  return 0;
}