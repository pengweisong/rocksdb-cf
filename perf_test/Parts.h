#pragma once

#include <cstdint>
#include <iostream>

#include "Generator.h"
#include "VertexEdge.h"
#include "rocksdb/db.h"

using PartId = int32_t;

class KVEngine;
class Slice;

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

  rocksdb::Iterator* newIterator();

 private:
  KVEngine* engine_;
  PartId id_;
};

class Space {
 public:
  Space(const GeneratorOptions& options);

  void addVertex(PartId partId, const VertexKey& key, const rocksdb::Slice& value);

  void removeVertex(PartId partId, const VertexKey& key);

  void getVertex(PartId partId, const VertexKey& key, std::string* value);

  void addEdge(PartId partId, const EdgeKey& key, const rocksdb::Slice& value);

  void removeEdge(PartId partId, const EdgeKey& key);

  void getEdge(PartId partId, const EdgeKey& key, std::string* value);

  rocksdb::Iterator* newIterator(PartId partId);

  ~Space();

 private:
  GeneratorOptions options_;
  std::map<PartId, std::unique_ptr<Part>> parts_;
};