#pragma once

#include <cstdint>
#include <iostream>

#include "KVEngine.h"
#include "Keys.h"
#include "Options.h"
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

  PartId id_;
  KVEngine* engine_;
};
