#include "KVEngine.h"
#include "Parts.h"
#include "rocksdb/slice.h"

Part::Part(PartId id, KVEngine* engine) : id_(id), engine_(engine) {}

Part::~Part() {}

KVEngine* Part::getEngine() {
  return engine_;
}

void Part::addVertex(const VertexKey& v, const rocksdb::Slice& value) {
  engine_->put(v.toString(), value);
}

void Part::removeVertex(const VertexKey& v) {
  engine_->remove(v.toString());
}

void Part::getVertex(const VertexKey& k, std::string* value) {
  engine_->get(k.toString(), value);
}

void Part::addEdge(const EdgeKey& e, const rocksdb::Slice& value) {
  engine_->put(e.toString(), value);
}

void Part::removeEdge(const EdgeKey& e) {
  engine_->remove(e.toString());
}
void Part::getEdge(const EdgeKey& e, std::string* value) {
  engine_->get(e.toString(), value);
}

rocksdb::Iterator* Part::newIterator() {
  return engine_->newIterator();
}

Space::Space(const GeneratorOptions& options) : options_(options) {
  if (options_.useCf) {
    rocksdb::DB* db = nullptr;
    rocksdb::Options opts;
    opts.create_if_missing = true;
    rocksdb::Status s = rocksdb::DB::Open(opts, options_.dataPath, &db);

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
      opts.create_if_missing = true;
      rocksdb::Status s =
          rocksdb::DB::Open(opts, options_.dataPath + "/part-" + std::to_string(i), &db);
      assert(s.ok());
      auto kv = new WholeEngine(db);
      auto part = std::make_unique<Part>(partId, kv);
      parts_.emplace(partId, std::move(part));
    }
  }
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

Space::~Space() {
  if (options_.useCf) {
    delete parts_[0]->getEngine()->getDB();
  } else {
    for (auto& [id, part] : parts_) {
      delete part->getEngine()->getDB();
    }
  }
}
