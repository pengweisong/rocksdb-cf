#pragma once

#include "rocksdb/db.h"
#include "rocksdb/slice.h"

class KVEngine {
 public:
  virtual rocksdb::DB* getDB() = 0;

  virtual void put(const rocksdb::Slice& k, const rocksdb::Slice& v) = 0;

  virtual void get(const rocksdb::Slice& k, std::string* v) = 0;

  virtual void remove(const rocksdb::Slice& k) = 0;

  virtual rocksdb::Iterator* newIterator() = 0;

  virtual ~KVEngine() = 0;
};

class CFEngine : public KVEngine {
 public:
  CFEngine(rocksdb::DB* db, const std::string& cfName, const std::string& path);

  rocksdb::DB* getDB() override;

  void put(const rocksdb::Slice& k, const rocksdb::Slice& v) override;

  void get(const rocksdb::Slice& k, std::string* v) override;

  void remove(const rocksdb::Slice& key) override;

  rocksdb::Iterator* newIterator() override;

  ~CFEngine();

 private:
  std::string cfName_;
  rocksdb::DB* db_;
  rocksdb::ColumnFamilyHandle* cf_;
};

class WholeEngine : public KVEngine {
 public:
  WholeEngine(rocksdb::DB* db);

  rocksdb::DB* getDB() override;

  void put(const rocksdb::Slice& k, const rocksdb::Slice& v) override;

  void get(const rocksdb::Slice& k, std::string* v) override;

  void remove(const rocksdb::Slice& key) override;

  rocksdb::Iterator* newIterator() override;

  ~WholeEngine();

 private:
  rocksdb::DB* db_;
};
