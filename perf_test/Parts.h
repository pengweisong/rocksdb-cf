#pragma once

#include <cstdint>
#include <iostream>

#include "KVEngine.h"
#include "Options.h"
#include "Parts.h"
#include "VertexEdge.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"

using PartId = int32_t;

class Part {
 public:
  Part(PartId id, KVEngine* engine);

  ~Part();

  KVEngine* getEngine();

  void addVertex(const VertexKey& v, const rocksdb::Slice& value);

  void removeVertex(const VertexKey& v);

  void getVertex(const VertexKey& k, std::string* value);

  void addEdge(const EdgeKey& e, const rocksdb::Slice& value);

  void removeEdge(const EdgeKey& e);

  void getEdge(const EdgeKey& e, std::string* value);

  void getNeighbor(const VertexKey& v, std::vector<EdgeKey>* edges, bool isInEdge = true);

  rocksdb::Iterator* newIterator();

 private:
  void getEdges(const VertexKey& v,
                const std::string& key,
                std::vector<EdgeKey>* edges,
                bool isInEdge);

  KVEngine* engine_;
  PartId id_;
};

class Space {
 public:
  Space(const Options& options);

  void addVertex(PartId partId, const VertexKey& key, const rocksdb::Slice& value);

  void removeVertex(PartId partId, const VertexKey& key);

  void getVertex(PartId partId, const VertexKey& key, std::string* value);

  void addEdge(PartId partId, const EdgeKey& key, const rocksdb::Slice& value);

  void removeEdge(PartId partId, const EdgeKey& key);

  void getEdge(PartId partId, const EdgeKey& key, std::string* value);

  rocksdb::Iterator* newIterator(PartId partId);

  ~Space();

 private:
  Options options_;
  std::map<PartId, std::unique_ptr<Part>> parts_;
};
