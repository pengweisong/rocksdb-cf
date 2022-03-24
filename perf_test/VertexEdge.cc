#include "VertexEdge.h"

std::string VertexKey::toString() const {
  std::string key;
  key.reserve(sizeof(NodeType) + sizeof(VertexId) + sizeof(int32_t));
  key.append(reinterpret_cast<const char*>(&type), sizeof(NodeType))
      .append(reinterpret_cast<const char*>(&id), sizeof(VertexId))
      .append(reinterpret_cast<const char*>(&version), sizeof(int32_t));
  return key;
}

VertexKey VertexKey::fromString(const std::string& s) {
  VertexKey v;
  v.type = *reinterpret_cast<const NodeType*>(s.data());
  v.id = *reinterpret_cast<const VertexId*>(s.data() + sizeof(NodeType));
  v.version = *reinterpret_cast<const int32_t*>(s.data() + sizeof(NodeType) + sizeof(VertexId));
  return v;
}

std::string EdgeKey::toString() const {
  std::string key;
  key.reserve(sizeof(EdgeKey) + sizeof(VertexId) + sizeof(VertexId) + sizeof(int32_t) +
              sizeof(int32_t));
  key.append(reinterpret_cast<const char*>(&type), sizeof(EdgeType))
      .append(reinterpret_cast<const char*>(&src), sizeof(VertexId))
      .append(reinterpret_cast<const char*>(&dst), sizeof(VertexId))
      .append(reinterpret_cast<const char*>(&rank), sizeof(int32_t))
      .append(reinterpret_cast<const char*>(&version), sizeof(int32_t));
  return key;
}

EdgeKey EdgeKey::fromString(const std::string& s) {
  EdgeKey e;
  e.type = *reinterpret_cast<const EdgeType*>(s.data());
  e.src = *reinterpret_cast<const VertexId*>(s.data() + sizeof(EdgeType));
  e.dst = *reinterpret_cast<const VertexId*>(s.data() + sizeof(EdgeType) + sizeof(VertexId));
  e.rank = *reinterpret_cast<const int32_t*>(s.data() + sizeof(NodeType) + sizeof(VertexId) * 2);
  e.version = *reinterpret_cast<const int32_t*>(s.data() + sizeof(EdgeType) + sizeof(VertexId) * 2 +
                                                sizeof(int32_t));
  return e;
}
