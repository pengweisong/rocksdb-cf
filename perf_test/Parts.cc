#include "Parts.h"

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

void Part::getNeighbor(const VertexKey& v, std::vector<EdgeKey>* edges, bool isInEdge) {
  rocksdb::Iterator* iter = engine_->newIterator();

  constexpr int32_t isVertex = sizeof(NodeType) + sizeof(VertexId) + sizeof(int32_t);

  constexpr int32_t isEdge =
      sizeof(EdgeType) + sizeof(VertexId) + sizeof(VertexId) + sizeof(int32_t) + sizeof(int32_t);

  for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
    std::string key = iter->key().ToString();
    switch (key.size()) {
      case isEdge: {
        getEdges(v, key, edges, isInEdge);
        break;
      }
      case isVertex: {
        break;
      }
      default: {
        return;
      }
    }
  }
}

void Part::getEdges(const VertexKey& v,
                    const std::string& key,
                    std::vector<EdgeKey>* edges,
                    bool isInEdge) {
  if (isInEdge) {
    int32_t dst = -1;
    ::memcpy(&dst, static_cast<const void*>(key.data() + sizeof(int32_t) * 2), sizeof(int32_t));
    if (dst == v.id) {
      edges->emplace_back(EdgeKey::fromString(key));
    }
  } else {  //出边
    int32_t src = -1;
    ::memcpy(&src, static_cast<const void*>(key.data() + sizeof(int32_t)), sizeof(int32_t));
    if (src == v.id) {
      edges->emplace_back(EdgeKey::fromString(key));
    }
  }
}

rocksdb::Iterator* Part::newIterator() {
  return engine_->newIterator();
}
