#pragma once

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

  std::string toString() const;

  static VertexKey fromString(const std::string& s);
};

struct EdgeKey {
  EdgeType type;
  VertexId src;
  VertexId dst;
  int32_t rank;
  int32_t version;

  std::string toString() const;

  static EdgeKey fromString(const std::string& s);
};