#include <cstdint>
#include <iostream>

#include "KVEngine.h"
#include "VertexEdge.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"

using PartId = int32_t;

class Part {
 public:
  Part(PartId id, KVEngine* engine) : id_(id), engine_(engine) {}

  ~Part() {}

  KVEngine* getEngine() {
    return engine_;
  }

  void addVertex(const VertexKey& v) {
    engine_->put(v.toString(), "");
  }

  void removeVertex(const VertexKey& v) {
    engine_->remove(v.toString());
  }

  void addEdge(cosnt EdgeKey& e) {
    engine_->put(e.toString(), "");
  }

  void removeEdge(const EdgeKey& e) {
    engine_->remove(e.toString());
  }

 private:
  KVEngine* engine_;
  PartId id_;
};

struct PartsOptions {
  bool useCf;
  int partNum;
  std::string dataPath;
};

class PartsManager {
 public:
  PartsManager(const PartsOptions& options) : options_(options) {
    assert(options_.partNum > 0);

    if (options_.useCf) {
      rocksdb::DB* db = nullptr;
      rocksdb::Options opts;
      opts.create_if_missing = true;
      Status s = DB::Open(db, options_.dataPath, &db);

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
        Status s = DB::Open(db, options_.dataPath + "/part-" + std::to_string(i), &db);

        auto kv = new WholeEngine(db);
        auto part = std::make_unique<Part>(partId, kv);
        parts_.emplace(partId, std::move(part));
      }
    }
  }

  ~PartsManager() {
    if (options_.useCf) {
      delete parts_[0]->getEngine()->getDB();
    } else {
      for (auto [id, part] : parts_) {
        delete part->getEngine()->getDB();
      }
    }
  }

 private:
  PartsOptions options_;
  std::map<PartId, std::unique_ptr<Part>> parts_;
};
