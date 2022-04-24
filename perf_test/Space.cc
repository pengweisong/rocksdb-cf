#include "Space.h"

#include "rocksdb/db.h"
#include "rocksdb/table.h"

Space::Space(const Options& options) : options_(options) {
  if (options_.useCf) {
    rocksdb::DB* db = nullptr;
    rocksdb::Options opts;
    rocksdb::BlockBasedTableOptions table_options;

    table_options.block_size = 8192;
    auto table_factory = rocksdb::NewBlockBasedTableFactory(table_options);

    opts.create_if_missing = true;
    opts.disable_auto_compactions = true;
    opts.max_subcompactions = 4;
    opts.max_background_compactions = 4;
    opts.table_factory.reset(table_factory);

    rocksdb::Status s = rocksdb::DB::Open(opts, options_.dataPath, &db);
    assert(s.ok());

    for (int i = 0; i < options_.partNum; ++i) {
      auto partId = static_cast<PartId>(i);
      auto cfName = "part-" + std::to_string(i);
      auto cfPath = options_.dataPath + "/" + cfName;

      auto kv = new CFEngine(db, cfName, cfPath);
      auto part = std::make_unique<Part>(partId, kv);
      parts_.emplace(partId, std::move(part));
    }
  } else {
    for (int i = 0; i < options_.partNum; ++i) {
      auto partId = static_cast<PartId>(i);
      rocksdb::DB* db = nullptr;
      rocksdb::Options opts;
      rocksdb::BlockBasedTableOptions table_options;

      table_options.block_size = 8192;
      auto table_factory = rocksdb::NewBlockBasedTableFactory(table_options);
      opts.table_factory.reset(table_factory);
      opts.create_if_missing = true;
      opts.disable_auto_compactions = true;
      opts.max_subcompactions = 4;
      opts.max_background_jobs = 4;
      rocksdb::Status s =
          rocksdb::DB::Open(opts, options_.dataPath + "/part-" + std::to_string(i), &db);
      assert(s.ok());
      auto kv = new WholeEngine(db);
      auto part = std::make_unique<Part>(partId, kv);
      parts_.emplace(partId, std::move(part));
    }
  }
}

void Space::write(PartId partId, rocksdb::WriteBatch* batch) {
  parts_[partId]->write(batch);
}

void Space::addVertex(PartId partId, const VertexKey& key, const rocksdb::Slice& value) {
  parts_[partId]->addVertex(key, value);
}

void Space::removeVertex(PartId partId, const VertexKey& key) {
  parts_[partId]->removeVertex(key);
}

void Space::getVertex(PartId partId, const VertexKey& key, std::string* value) {
  parts_[partId]->getVertex(key, value);
}

void Space::addEdge(PartId partId, const EdgeKey& key, const rocksdb::Slice& value) {
  parts_[partId]->addEdge(key, value);
}

void Space::removeEdge(PartId partId, const EdgeKey& key) {
  parts_[partId]->removeEdge(key);
}

void Space::getEdge(PartId partId, const EdgeKey& key, std::string* value) {
  parts_[partId]->getEdge(key, value);
}

rocksdb::Iterator* Space::newIterator(PartId partId) {
  return parts_[partId]->newIterator();
}

Part* Space::getPart(PartId partId) {
  return parts_[partId].get();
}

void Space::ResetPart() {
  for (auto& [id, part] : parts_) {
    part.reset(nullptr);
  }
}

Space::~Space() {
  if (options_.useCf) {
    // we shold destory part first
    // beause The destruction of the Part causes the destruction of the cf
    // The DB should be destroyed first with the Coulmn family
    rocksdb::DB* db = parts_[0]->getEngine()->getDB();
    ResetPart();
    delete db;
  } else {
    for (auto& [id, part] : parts_) {
      delete part->getEngine()->getDB();
    }
  }
}
