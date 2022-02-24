
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

std::string kDBPath = "/tmp/export_the_same_db_test";
std::string kExportdir = "/tmp/export";
std::string kImportdir = "/tmp/import";
int main()
{
    Options options;
    ColumnFamilyOptions cf_options, target_options;
    ImportColumnFamilyOptions importColumnFamilyOptions;
    Checkpoint checkpoints;
    DbPath path;
    DB *db = nullptr;
    ColumnFamilyHandle *cf = nullptr, *import_cf = nullptr;
    rocksdb::Checkpoint *checkpoint = nullptr;
    ExportImportFilesMetaData *metadata = nullptr;
    options.create_if_missing = true;
    path.path = kImportdir;
    target_options.cf_paths = {path};
    target_options.write_buffer_size = 80920;
    importColumnFamilyOptions.move_files = true;

    Status s = DB::Open(options, kDBPath, &db);
    assert(s.ok());

    s = db->CreateColumnFamily(cf_options, "cf", &cf);
    assert(s.ok());

    for (int i = 0; i < 100000; ++i)
        assert(
            db->Put(WriteOptions(), cf, Slice("key1"), Slice("value1")).ok());

    s = db->Flush(FlushOptions(), cf);
    assert(s.ok());

    // 将一个列族中的所有sst导入到文件夹kExportdir中 利用metadata进行导出
    assert(Checkpoint::Create(db, &checkpoint).ok());
    s = checkpoint->ExportColumnFamily(cf, kExportdir, &metadata);
    assert(s.ok());

    //根据导入获得的元数据进行导出
    s = db->CreateColumnFamilyWithImport(target_options, "hehe",
                                         importColumnFamilyOptions, *metadata,
                                         &import_cf);
    assert(s.ok());

    string value;
    assert(db->Get(ReadOptions(), import_cf, "key1", &value).ok());
    assert(value == "value1");
    assert(import_cf->GetName() == "hehe");

    //删除原有的sst
    assert(db->DropColumnFamily(cf).ok());
    // close db
    assert(db->DestroyColumnFamilyHandle(cf).ok());
    assert(db->DestroyColumnFamilyHandle(import_cf).ok());
    delete db;

    return 0;
}

