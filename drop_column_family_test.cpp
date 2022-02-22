
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
std::string kDBPath = "/tmp/column_family_test";
using ROCKSDB_NAMESPACE::Checkpoint;
using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::DBOptions;
using ROCKSDB_NAMESPACE::DbPath;
using ROCKSDB_NAMESPACE::ExportImportFilesMetaData;
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
    DB *db;
    Status s = DB::Open(options, kDBPath, &db);
    assert(s.ok());

    // create column family
    ColumnFamilyHandle *cf1 = nullptr, *cf2 = nullptr;

    ColumnFamilyOptions cf1_options, cf2_options;
    cf1_options.write_buffer_size = 80920;
    cf2_options.write_buffer_size = 80920;

    DbPath path1, path2;
    path1.path = "/tmp/sst1";
    path2.path = "/tmp/sst2";
    vector<DbPath> paths_{path1};
    cf1_options.cf_paths = paths_;
    paths_.clear();
    paths_.emplace_back(path2);
    cf2_options.cf_paths = paths_;
    s = db->CreateColumnFamily(cf1_options, "cf_one", &cf1);
    assert(s.ok());
    s = db->CreateColumnFamily(cf2_options, "cf_two", &cf2);
    assert(s.ok());

    s = db->DestroyColumnFamilyHandle(cf1);
    assert(s.ok());
    s = db->DestroyColumnFamilyHandle(cf2);
    assert(s.ok());
    delete db;

    std::vector<ColumnFamilyDescriptor> column_families;

    //必须打开所有已经存在的列族
    column_families.push_back(ColumnFamilyDescriptor(kDefaultColumnFamilyName,
                                                     ColumnFamilyOptions()));

    column_families.push_back(ColumnFamilyDescriptor("cf_one", cf1_options));
    column_families.push_back(ColumnFamilyDescriptor("cf_two", cf2_options));
    std::vector<ColumnFamilyHandle *> handles;
    s = DB::Open(DBOptions(), kDBPath, column_families, &handles, &db);

    assert(s.ok());

    for (int i = 0; i < 10000; ++i)
        s = db->Put(WriteOptions(), handles[1], Slice("key1"), Slice("value1"));
    assert(s.ok());
    for (int i = 0; i < 10000; ++i)
        s = db->Put(WriteOptions(), handles[2], Slice("key2"), Slice("value2"));

    string value;
    assert(db->Get(ReadOptions(), handles[1], "key1", &value).ok());
    assert(value == "value1");
    value.clear();
    assert(db->Get(ReadOptions(), handles[2], "key2", &value).ok());
    assert(value == "value2");


    //删除列族以及对应的sst  
    for (int i = 1; i <= 2; ++i) assert(db->DropColumnFamily(handles[i]).ok());

    // close db
    for (auto handle : handles) {
        s = db->DestroyColumnFamilyHandle(handle);
        assert(s.ok());
    }

    delete db;

    return 0;
}

