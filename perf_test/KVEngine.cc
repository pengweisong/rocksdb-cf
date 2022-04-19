#include "KVEngine.h"

#include <iostream>

KVEngine::~KVEngine() {}

CFEngine::CFEngine(rocksdb::DB* db, const std::string& cfName, const std::string& path) {
  db_ = db;
  cfName_ = cfName;

  rocksdb::ColumnFamilyOptions options;
  rocksdb::DbPath dbPath;

  options.disable_auto_compactions = true;
  options.write_buffer_size = 134217728;
  options.max_write_buffer_number = 12;
  options.max_bytes_for_level_base = 268435456;
  options.level0_slowdown_writes_trigger = 999999;
  options.level0_stop_writes_trigger = 999999;
  options.soft_pending_compaction_bytes_limit = 137438953472;
  options.hard_pending_compaction_bytes_limit = 274877906944;

  dbPath.path = path;
  options.cf_paths = {dbPath};
  auto s = db_->CreateColumnFamily(options, cfName, &cf_);
  assert(s.ok());
}

CFEngine::~CFEngine() {
  rocksdb::Status s = db_->DropColumnFamily(cf_);
  assert(s.ok());
  s = db_->DestroyColumnFamilyHandle(cf_);
  assert(s.ok());
}

rocksdb::DB* CFEngine::getDB() {
  return db_;
}

rocksdb::ColumnFamilyHandle* CFEngine::getCF() {
  return cf_;
}

void CFEngine::put(const rocksdb::Slice& k, const rocksdb::Slice& v) {
  auto s = db_->Put(rocksdb::WriteOptions(), cf_, k, v);
  assert(s.ok());
}

void CFEngine::write(rocksdb::WriteBatch* batch) {
  auto s = db_->Write(rocksdb::WriteOptions(), batch);
  assert(s.ok());
}

void CFEngine::get(const rocksdb::Slice& k, std::string* v) {
  auto s = db_->Get(rocksdb::ReadOptions(), cf_, k, v);
  assert(s.ok());
}

void CFEngine::remove(const rocksdb::Slice& key) {
  auto s = db_->Delete(rocksdb::WriteOptions(), cf_, key);
  assert(s.ok());
}

rocksdb::Iterator* CFEngine::newIterator() {
  return db_->NewIterator(rocksdb::ReadOptions(), cf_);
}

WholeEngine::WholeEngine(rocksdb::DB* db) {
  db_ = db;
}

WholeEngine::~WholeEngine() {}

rocksdb::DB* WholeEngine::getDB() {
  return db_;
}

rocksdb::ColumnFamilyHandle* WholeEngine::getCF() {
  return nullptr;
}

void WholeEngine::put(const rocksdb::Slice& k, const rocksdb::Slice& v) {
  auto s = db_->Put(rocksdb::WriteOptions(), k, v);
  assert(s.ok());
}

void WholeEngine::write(rocksdb::WriteBatch* batch) {
  auto s = db_->Write(rocksdb::WriteOptions(), batch);
  assert(s.ok());
}

void WholeEngine::get(const rocksdb::Slice& k, std::string* v) {
  auto s = db_->Get(rocksdb::ReadOptions(), k, v);
  assert(s.ok());
}

void WholeEngine::remove(const rocksdb::Slice& key) {
  auto s = db_->Delete(rocksdb::WriteOptions(), key);
  assert(s.ok());
}

rocksdb::Iterator* WholeEngine::newIterator() {
  return db_->NewIterator(rocksdb::ReadOptions());
}
