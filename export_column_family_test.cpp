
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
std::string kDBPath = "/tmp/export_column_family_test";
using ROCKSDB_NAMESPACE::Checkpoint;
using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::DBOptions;
using ROCKSDB_NAMESPACE::ExportImportFilesMetaData;
using ROCKSDB_NAMESPACE::ImportColumnFamilyOptions;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Slice;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;

using ROCKSDB_NAMESPACE::DbPath;
using namespace std;
int main()
{
    // open DB
    Options options;

    options.create_if_missing = true;
    DB *db;
    options.db_write_buffer_size = 100000;
    Status s = DB::Open(options, kDBPath, &db);
    assert(s.ok());

    string temp = "/tmp";
    string kExportdir = temp + "/sst_files/";
    // create column family
    ColumnFamilyHandle *cf1, *cf2;

    ColumnFamilyOptions cf1_options, cf2_options;
    cf1_options.write_buffer_size = 80920;
    cf2_options.write_buffer_size = 80920;
    s = db->CreateColumnFamily(cf1_options, "cf_one", &cf1);
    assert(s.ok());
    s = db->CreateColumnFamily(cf2_options, "cf_two", &cf2);
    assert(s.ok());

    // close DB
    s = db->DestroyColumnFamilyHandle(cf1);
    s = db->DestroyColumnFamilyHandle(cf2);

    assert(s.ok());
    delete db;

    // open DB with two column families
    std::vector<ColumnFamilyDescriptor> column_families;
    // have to open default column family
    column_families.push_back(ColumnFamilyDescriptor(
        ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, ColumnFamilyOptions()));
    // open the new one, too
    column_families.push_back(ColumnFamilyDescriptor("cf_one", cf1_options));
    column_families.push_back(ColumnFamilyDescriptor("cf_two", cf2_options));
    std::vector<ColumnFamilyHandle *> handles;
    s = DB::Open(DBOptions(), kDBPath, column_families, &handles, &db);

    assert(s.ok());

    for (int i = 0; i < 100000; ++i)
        assert(
            db->Put(WriteOptions(), handles[1], Slice("key1"), Slice("value1"))
                .ok());
    assert(s.ok());
    for (int i = 0; i < 100000; ++i)
        assert(
            db->Put(WriteOptions(), handles[2], Slice("key2"), Slice("value2"))
                .ok());

    // 将一个列族中的所有sst导入到文件夹kExportdir中 利用metadata进行导出
    rocksdb::Checkpoint *checkpoint = nullptr;
    assert(Checkpoint::Create(db, &checkpoint).ok());
    ExportImportFilesMetaData *metadata = nullptr;
    s = checkpoint->ExportColumnFamily(handles[1], kExportdir, &metadata);
    assert(s.ok());

    //根据导入获得的元数据进行导出
    ImportColumnFamilyOptions importColumnFamilyOptions;

    ColumnFamilyHandle *import_target = nullptr;
    ColumnFamilyOptions target_options;
    target_options.write_buffer_size = 77777;
    s = db->CreateColumnFamilyWithImport(target_options, "hehe",
                                         importColumnFamilyOptions, *metadata,
                                         &import_target);

    assert(s.ok());

    string value;
    assert(db->Get(ReadOptions(), import_target, "key1", &value).ok());
    assert(value == "value1");
    assert(import_target->GetName() == "hehe");
    // drop column family

    // for (int i = 1; i <= 2; ++i)
    // assert(db->DropColumnFamily(handles[i]).ok());
    // assert(db->DropColumnFamily(import_target).ok());
    
    // close db
    for (auto handle : handles) {
        assert(db->DestroyColumnFamilyHandle(handle).ok());
    }
    assert(db->DestroyColumnFamilyHandle(import_target).ok());
    delete db;

    return 0;
}

