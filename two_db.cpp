
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
std::string kDBPath1 = "/tmp/first_db";
std::string kDBPath2 = "/tmp/second_db";
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

string source_column_family_path = "/tmp/source_column_family_path";
string target_column_family_path = "/tmp/target_column_family_path";

ExportImportFilesMetaData *export_import_files_meta_data = nullptr;

DB *db1 = nullptr;
DB *db2 = nullptr;

map<string, string> kv;

static const string s =
    "awjhdiawhfoi0213402940-"
    "1poi0wi1232oiedjmaklwmdladsadiqwjdnanmskxsBUSABUIDHQOID";

static const int MAX = 99999;
string MakeRandomString(int n)
{
    string result;

    result.reserve(n);
    srand(clock());
    for (int i = 0; i < n; ++i) {
        int index = rand() % s.size();
        result.push_back(s[index]);
    }
    return result;
}
void MakeRandomKV(map<string, string> &kv, int n)
{
    string k, v;
    srand(clock());
    for (int i = 0; i < n; ++i) {
        int len = rand() % MAX;
        k = MakeRandomString(len);
        v = MakeRandomString(len);
        kv.insert({k, v});
    }
}
void OpenFirstDb()
{
    Options options;
    options.create_if_missing = true;

    DB *db1 = nullptr;
    options.db_write_buffer_size = 100000;

    Status s = DB::Open(options, kDBPath1, &db1);

    assert(s.ok());

    ColumnFamilyHandle *cf = nullptr;

    ColumnFamilyOptions cf_options;
    cf_options.write_buffer_size = 80920;

    s = db1->CreateColumnFamily(cf_options, "first_cf", &cf);

    assert(s.ok());

    s = db1->DestroyColumnFamilyHandle(cf);
    assert(s.ok());

    delete db1;

    std::vector<ColumnFamilyDescriptor> column_families;
    // have to open default column family
    column_families.push_back(ColumnFamilyDescriptor(
        ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, ColumnFamilyOptions()));
    // open the new one, too
    column_families.push_back(ColumnFamilyDescriptor("first_cf", cf_options));
    std::vector<ColumnFamilyHandle *> handles;

    s = DB::Open(DBOptions(), kDBPath1, column_families, &handles, &db1);

    assert(s.ok());

    for (auto iter = kv.begin(); iter != kv.end(); ++iter) {
        assert(db1->Put(WriteOptions(), handles[1], iter->first, iter->second)
                   .ok());
    }
    assert(s.ok());

    rocksdb::Checkpoint *checkpoint = nullptr;
    assert(Checkpoint::Create(db1, &checkpoint).ok());
    s = checkpoint->ExportColumnFamily(handles[1], source_column_family_path,
                                       &export_import_files_meta_data);
    assert(s.ok());

    s = db1->DestroyColumnFamilyHandle(handles[1]);
}

void OpensecondDB()
{
    Options options;
    options.create_if_missing = true;

    DB *db2 = nullptr;
    options.db_write_buffer_size = 100000;

    Status s = DB::Open(options, kDBPath2, &db2);

    assert(s.ok());

    ImportColumnFamilyOptions importColumnFamilyOptions;

    ColumnFamilyHandle *import_target = nullptr;
    ColumnFamilyOptions target_options;
    target_options.write_buffer_size = 80920;

    vector<DbPath> paths;
    DbPath path;
    path.path = target_column_family_path;
    paths.emplace_back(path);

    target_options.cf_paths = paths;

    s = db2->CreateColumnFamilyWithImport(
        target_options, "second_cf", importColumnFamilyOptions,
        *export_import_files_meta_data, &import_target);

    string value;
    for (auto iter = kv.begin(); iter != kv.end(); ++iter) {
        assert(
            db2->Get(ReadOptions(), import_target, iter->first, &value).ok());

        assert(value == iter->second);
    }

    s = db2->DestroyColumnFamilyHandle(import_target);
}

int main()
{

    MakeRandomKV(kv,1000);
    OpenFirstDb();
    OpensecondDB();

    delete db1;
    delete db2;
}

