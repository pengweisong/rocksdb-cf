#include <cstdint>
#include <string>

using NodeType = int32_t;
using PropertyId = int32_t;
using VertexId = int64_t;
using EdgeType = int32_t;

struct VertexKey {
  NodeType type;
  VertexId id;
  int32_t version;

  std::string toString() {
    std::string key;
    key.reserve(sizeof(NodeType) + sizeof(VertexId) + sizeof(int32_t));
    key.append(reinterpret_cast<const char*>(&type), sizeof(NodeType))
        .append(reinterpret_cast<const char*>(&id), sizeof(VertexId))
        .append(reinterpret_cast<const char*>(&version), sizeof(int32_t));
    return key;
  }

  static VertexKey fromString(const std::string& s) {
    VertexKey v;
    v.type = *reinterpret_cast<const NodeType*>(s.data());
    v.id = *reinterpret_cast<const VertexId*>(s.data() + sizeof(NodeType));
    v.version = *reinterpret_cast<const int32_t*>(s.data() + sizeof(NodeType) + sizeof(VertexId));
    return v;
  }
};

struct EdgeKey {
  EdgeType type;
  VertexId src;
  VertexId dst;
  int32_t rank;
  int32_t version;

  std::string toString() {
    std::string key;
    key.reserve();
    key.append(reinterpret_cast<const char*>(&type), sizeof(EdgeType))
        .append(reinterpret_cast<const char*>(&src), sizeof(VertexId))
        .append(reinterpret_cast<const char*>(&dst), sizeof(VertexId))
        .append(reinterpret_cast<const char*>(&rank), sizeof(int32_t))
        .append(reinterpret_cast<const char*>(&version), sizeof(int32_t));
    return key;
  }

  static EdgeKey fromString(const std::string& s) {
    EdgeKey e;
    e.type = *reinterpret_cast<const EdgeType*>(s.data());
    e.src = *reinterpret_cast<const VertexId*>(s.data() + sizeof(EdgeType));
    e.dst = *reinterpret_cast<const VertexId*>(s.data() + sizeof(EdgeType) + sizeof(VertexId));
    e.rank = *reinterpret_cast<const int32_t*>(s.data() + sizeof(NodeType) + sizeof(VertexId) * 2);
    e.version = *reinterpret_cast<const int32_t*>(s.data() + sizeof(EdgeType) +
                                                  sizeof(VertexId) * 2 + sizeof(int32_t));
    return e;
  }
};