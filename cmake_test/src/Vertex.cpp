
#include <cstring>

#include "Vertex.h"

Vertex::Vertex(uint64_t vertex_id, const std::string &node_type)
    : vertex_id_(vertex_id),
      node_type_(node_type),
      node_type_len_(static_cast<uint32_t>(node_type.size())),
      vertex_type_(VertexOrEdgeType::kVertex)
{
    node_type_.resize(node_type_len_);
    rep_.resize(sizeof(vertex_type_) + sizeof(vertex_id_) + node_type_len_);
    Encode();
}

Vertex::~Vertex() { Clear(); }

void Vertex::Clear()
{
    dest_vertex_.clear();
    src_vertex_.clear();
    node_type_.clear();
    rep_.clear();
    node_type_len_ = 0;
}

void Vertex::Decode()
{
    DecodeVertexType();
    DecodeNodeType();
    DecodeVertexId();
}
void Vertex::Encode()
{
    ::memcpy(static_cast<void *>(&*rep_.begin()),
             static_cast<const void *>(&vertex_type_), sizeof(vertex_type_));

    ::memcpy(static_cast<void *>(&*rep_.begin() + sizeof(vertex_type_)),
             node_type_.data(), node_type_len_);

    ::memcpy(static_cast<void *>(&*rep_.begin() + sizeof(vertex_type_) +
                                 node_type_len_),
             static_cast<const void *>(&vertex_id_), sizeof(vertex_id_));
}
void Vertex::DecodeVertexType()
{
    ::memcpy(static_cast<void *>(&vertex_type_),
             static_cast<const void *>(&*rep_.begin()), sizeof(vertex_type_));
}

void Vertex::DecodeNodeType()
{
    char temp[node_type_len_] = {'\0'};
    ::memcpy(temp, static_cast<void *>(&*rep_.begin() + sizeof(vertex_type_)),
             node_type_len_);
    node_type_.assign(temp, temp + node_type_len_);
}

void Vertex::DecodeVertexId()
{
    ::memcpy(static_cast<void *>(&vertex_id_),
             static_cast<const void *>(&*rep_.begin() + sizeof(vertex_type_) +
                                       node_type_len_),
             sizeof(vertex_id_));
}

