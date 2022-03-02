
// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/utilities/checkpoint.h"
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
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Slice;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;

using namespace std;

std::string kDBPath = "/tmp/drop_column_family_test";
std::string kCfPath1 = "/tmp/sst_files1";
std::string kCfPath2 = "/tmp/sst_files2";
int main()
{
    Options options;
    options.create_if_missing = true;
    DB *db = nullptr;

    Status s = DB::Open(options, kDBPath, &db);
    assert(s.ok());

    ColumnFamilyOptions cf1_options, cf2_options;
    ColumnFamilyHandle *cf1 = nullptr, *cf2 = nullptr;
    DbPath path1, path2;
    path1.path = kCfPath1;
    path2.path = kCfPath2;
    cf1_options.cf_paths = {path1};
    cf2_options.cf_paths = {path2};

    s = db->CreateColumnFamily(cf1_options, "cf1", &cf1);
    assert(s.ok());
    s = db->CreateColumnFamily(cf2_options, "cf2", &cf2);
    assert(s.ok());

    for (int i = 0; i < 100000; ++i)
        assert(
            db->Put(WriteOptions(), cf1, Slice("key1"), Slice("value1")).ok());
    for (int i = 0; i < 100000; ++i)
        assert(
            db->Put(WriteOptions(), cf2, Slice("key2"), Slice("value2")).ok());

    assert(s.ok());
    s = db->Flush(FlushOptions(), cf1);
    s = db->Flush(FlushOptions(), cf2);
    assert(s.ok());

    //删除cf1 对应的sst
    assert(db->DropColumnFamily(cf1).ok());
    assert(db->DestroyColumnFamilyHandle(cf1).ok());
    assert(db->DestroyColumnFamilyHandle(cf2).ok());
    delete db;

    return 0;
}

