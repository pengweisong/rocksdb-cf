

#include <cstring>

#include "Edge.h"
#include "Vertex.h"

Edge::Edge(uint64_t start_vertex_id, uint64_t end_vertex_id,
           const std::string &node_type, int32_t edge_type, uint64_t rank)
    : start_vertex_id_(start_vertex_id),
      end_vertex_id_(end_vertex_id),
      node_type_(node_type),
      edge_type_(edge_type),
      rank_(rank),
      edge_place_holder_(EdgePlaceHolder::kPlaceHolder),
      is_edge_(VertexOrEdgeType::kEdge)
{
    rep_.resize(sizeof(is_edge_) + sizeof(edge_type_) + sizeof(rank_) +
                2 * sizeof(uint64_t) + sizeof(edge_place_holder_));
    Encode();
}

Edge::~Edge() { Clear(); }

void Edge::Clear()
{
    edge_place_holder_ = EdgePlaceHolder::kInValidPlaceHolder;
    is_edge_ = VertexOrEdgeType::kInValidType;
    start_vertex_id_ = 0;
    end_vertex_id_ = 0;
    edge_type = 0;
    rank_ = 0;
    rep_.clear();
}

void Edge::Encode()
{
    uint32_t index = 0;
    EncodeEdgeOrVertex(&index);
    EncodeEdgeType(&index);
    EncodeVertex(start_vertex_id_, &index);
    EncodeRank(&index);
    EncodeVertex(end_vertex_id_, &index);
    EncodePlaceHolder(&index);
}

void Edge::Decode()
{
    uint32_t index = 0;
    DecodeEdgeOrVertex(&index);
    DecodeEdgeType(&index);
    DecodeVertex(start_vertex_id_, &index);
    DecodeRank(&index);
    DecodeVertex(end_vertex_id_, &index);
    DecodePlaceHolder(&index);
}
inline void Edge::EncodeEdgeOrVertex(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&*rep_.begin()),
             static_cast<const void *>(&is_edge_), sizeof(is_edge_));
    *index += sizeof(is_edge_);
}
inline void Edge::EncodeEdgeType(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&*rep_.begin() + *index),
             static_cast<const void *>(&edge_type_), sizeof(edge_type_));
    *index += sizeof(edge_type_);
}
inline void Edge::EncodeVertex(uint64_t vertex_id, uint32_t *index)
{
    ::memcpy(static_cast<void *>(&*rep_.begin() + *index),
             static_cast<const void *>(&vertex_id), sizeof(vertex_id));
    *index += sizeof(vertex_id);
}
inline void Edge::EncodeRank(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&*rep_.begin() + *index),
             static_cast<const void *>(&rank_), sizeof(rank_));
    *index += sizeof(rank_);
}
inline void Edge::EncodePlaceHolder(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&*rep_.begin() + *index),
             static_cast<const void *>(&edge_place_holder_),
             sizeof(edge_place_holder_));
    *index += sizeof(edge_place_holder_);
}

inline void Edge::DecodeEdgeOrVertex(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&is_edge_),
             static_cast<const void *>(&*rep_.begin() + *index),
             sizeof(is_edge_));
    *index += sizeof(is_edge_);
}
inline void Edge::DecodeEdgeType(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&edge_type_),
             static_cast<const void *>(&*rep_.begin() + *index),
             sizeof(edge_type_));
    *index += sizeof(edge_type_);
}

inline void Edge::DecodeRank(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&rank_),
             static_cast<const void *>(&*rep_.begin() + *index), sizeof(rank_));
    *index += sizeof(rank_);
}

inline void Edge::DecodeVertex(uint64_t vertex_id, uint32_t *index)
{
    uint64_t vertex_id = 0;

    ::memcpy(static_cast<void *>(&vertex_id),
             static_cast<void *>(&*rep_.begin() + *index), sizeof(vertex_id));

    *index += sizeof(vertex_id);
}

inline void Edge::DecodePlaceHolder(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&edge_place_holder_),
             static_cast<const void *>(&*rep_.begin() + *index),
             sizeof(edge_place_holder_));
    *index += sizeof(edge_place_holder_);
}

