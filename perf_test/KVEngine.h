#include "rocksdb/db.h"
#include "rocksdb/slice.h"

class KVEngine {
 public:
  virtual rocksdb::DB getDB() = 0;
  virtual void put(rocksdb::Slice k, rocksdb : Slice v) = 0;
  virtual void get(rocksdb::Slice k, std::string* v) = 0;
  virtual void remove(rocksdb::Slice k) = 0;
};

class CFEngine {
 public:
  CFEngine(rocksdb::DB* db, const std::string& cfName, const std::string& path) {
    db_ = db;
    cfName_ = cfName;

    ColumnFamilyOptions options;
    options.cf_paths = {path};
    auto s = db_->CreateColumnFamily(options, cfName, &cf_);
    assert(s.ok());
  }

  rocksdb::DB getDB() {
    return db_;
  }

  void put(rocksdb::Slice k, rocksdb::Slice v) {
    auto s = db_->Put(rocksdb::WriteOptions(), cf_, k, v);
    assert(s.ok());
  }

  void get(rocksdb::Slice k, std::string* v) {
    auto s = db->Get(rocksdb::ReadOptions(), cf_, k, &v);
    assert(s.ok());
  }

  void remove(rocksdb::Slice k) {
    auto s = db_->Delete(rocksdb::WriteOptions(), key);
    assert(s.ok());
  }

 private:
  std::string cfName_;
  rocksdb::DB db_;
  rocksdb::ColumnFamilyHandle cf_;
};

class WholeEngine {
 public:
  WholeEngine(rocksdb::DB* db) {
    db_ = db;
  }

  rocksdb::DB getDB() {
    return db_;
  }

  void put(rocksdb::Slice k, rocksdb : Slice v) {
    auto s = db_->Put(rocksdb::WriteOptions(), k, v);
    assert(s.ok());
  }

  void get(rocksdb::Slice k, std::string* v) {
    auto s = db->Get(rocksdb::ReadOptions(), k, &v);
    assert(s.ok());
  }

 private:
  rocksdb::DB db_;
};