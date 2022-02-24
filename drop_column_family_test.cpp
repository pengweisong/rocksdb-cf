
// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/utilities/checkpoint.h"
std::string kDBPath = "/tmp/drop_column_family_test";
using ROCKSDB_NAMESPACE::Checkpoint;
using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::DBOptions;
using ROCKSDB_NAMESPACE::DbPath;
using ROCKSDB_NAMESPACE::ExportImportFilesMetaData;
using ROCKSDB_NAMESPACE::FlushOptions;
using ROCKSDB_NAMESPACE::ImportColumnFamilyOptions;
using ROCKSDB_NAMESPACE::kDefaultColumnFamilyName;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Slice;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;
using namespace std;
int main()
{
    // open DB
    Options options;
    options.create_if_missing = true;
    DB *db = nullptr;
    DbPath path1, path2;
    path1.path = "/tmp/sst1";
    path2.path = "/tmp/sst2";

    ColumnFamilyHandle *cf1 = nullptr, *cf2 = nullptr;
    ColumnFamilyOptions cf1_options, cf2_options;
    cf1_options.write_buffer_size = 80920;
    cf2_options.write_buffer_size = 80920;
    cf1_options.cf_paths = {path1};
    cf2_options.cf_paths = {path2};

    Status s = DB::Open(options, kDBPath, &db);
    assert(s.ok());

    s = db->CreateColumnFamily(cf1_options, "cf_one", &cf1);
    assert(s.ok());
    s = db->CreateColumnFamily(cf2_options, "cf_two", &cf2);
    assert(s.ok());

    for (int i = 0; i < 10000; ++i) {
        s = db->Put(WriteOptions(), cf1, Slice("key1"), Slice("value1"));
        assert(s.ok());
    }
    string put_value = "value";
    string key;
    for (int i = 0; i < 26; ++i) {
        char temp = static_cast<char>('a' + i);
        key.push_back(temp);
        s = db->Put(WriteOptions(), cf2, key, put_value);
        assert(s.ok());
        key.clear();
    }

    string value;
    assert(db->Get(ReadOptions(), cf1, "key1", &value).ok());
    assert(value == "value1");
    value.clear();
    for (int i = 0; i < 26; ++i) {
        char temp = static_cast<char>('a' + i);
        string key;
        key.push_back(temp);
        s = db->Get(ReadOptions(), cf2, key, &value);
        assert(s.ok());
        assert(value == put_value);
        value.clear();
    }

    //删除cf1对应的sst文件不删除cf2对应的sst文件
    assert(db->DropColumnFamily(cf1).ok());
    assert(db->DestroyColumnFamilyHandle(cf1).ok());
    assert(db->Flush(FlushOptions(), cf2).ok());
    assert(db->DestroyColumnFamilyHandle(cf2).ok());

    delete db;

    return 0;
}

