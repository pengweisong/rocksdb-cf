#pragma once

#include "Options.h"
#include "Parts.h"

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
  void ResetPart();

  Options options_;
  std::map<PartId, std::unique_ptr<Part>> parts_;
};
