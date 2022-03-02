
#include "generate_vertex_edge.h"

namespace manager {

Vertex::Vertex(uint64_t vertex_id, const std::string &vertex_node_type,
               uint32_t vertex_node_type_len)
    : vertex_id_(vertex_id),
      node_type_(vertex_node_type),
      node_type_len_(vertex_node_type_len)
{
    node_type_.resize(node_type_len_);
    rep_.resize(sizeof(vertex_type_) + sizeof(vertex_id_) + node_type_len_);
    Encode();
}
Vertex::Vertex(uint64_t vertex_id, const VertexOptions &options)
    : vertex_id_(vertex_id),
      vertex_options_(options),
      node_type_(vertex_options_.node_type_),
      node_type_len_(vertex_options_.node_type_len_),
      vertex_type_(vertex_options_.vertex_type_)
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

    rep_.clear();
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

VertexManager::VertexManager() : next_vertex_id_(0) {}
VertexManager::~VertexManager() {}

void Edge::Encode()
{
    uint32_t index = 0;
    EncodeEdgeOrVertex(&index);
    EncodeEdgeType(&index);
    EncodeVertex(src_vertex_->GetVertexId(), &index);
    EncodeRank(&index);
    EncodeVertex(dest_vertex_->GetVertexId(), &index);
    EncodePlaceHolder(&index);
}

void Edge::Decode()
{
    uint32_t index = 0;
    src_vertex_->Decode();
    dest_vertex_->Decode();
    DecodeEdgeOrVertex(&index);
    DecodeEdgeType(&index);
    DecodeVertex(src_vertex_, &index);
    DecodeRank(&index);
    DecodeVertex(dest_vertex_, &index);
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

inline void Edge::DecodeVertex(Vertex *vertex, uint32_t *index)
{
    uint64_t vertex_id = 0;

    ::memcpy(static_cast<void *>(&vertex_id),
             static_cast<void *>(&*rep_.begin() + *index), sizeof(vertex_id));

    vertex->SetVertexId(vertex_id);
    *index += sizeof(vertex_id);
}

inline void Edge::DecodePlaceHolder(uint32_t *index)
{
    ::memcpy(static_cast<void *>(&edge_place_holder_),
             static_cast<const void *>(&*rep_.begin() + *index),
             sizeof(edge_place_holder_));
    *index += sizeof(edge_place_holder_);
}

void VertexManager::ThisVertexAsSrcVertex(
    Vertex *vertex, std::vector<std::pair<Edge *, Edge *>> *edge)
{
    VertexOptions vertex_options = vertex->GetVertexOptions();
    srand(clock());
    //该点作为src点
    uint64_t as_src_vertex_num =
        (rand() % (vertex_options.as_src_vertex_max_ -
                   vertex_options.as_src_vertex_min_ + 1) +
         vertex_options.as_src_vertex_min_);
    //首先利用生成的所有点建立链接 不够的话重新建立点

    uint64_t num = 0;
    for (auto &it : all_vertex_) {
        if (it != vertex && !vertex->IsInSrcVertexSet(it) &&
            !it->IsInDestVertexSet(vertex)) {
            Edge *e1 = new Edge(vertex, it, 1, 8);
            Edge *e2 = new Edge(it, vertex, -1, 8);
            edge->push_back({e1, e2});
            vertex->InsertDestVertexSet(it);
            it->InsertSrcVertexSet(vertex);
            ++num;
        }
    }
    uint64_t need_regenerate_num = 0;
    if (num < as_src_vertex_num) {
        need_regenerate_num = as_src_vertex_num - num;
    }

    for (uint64_t i = 0; i < need_regenerate_num; ++i) {
        Vertex *new_vertex = CreateVertex(vertex->GetVertexOptions());

        Edge *e1 = new Edge(vertex, new_vertex, 1, 8);
        Edge *e2 = new Edge(new_vertex, vertex, -1, 8);
        vertex->InsertDestVertexSet(new_vertex);
        new_vertex->InsertSrcVertexSet(vertex);
        edge->push_back({e1, e2});
    }
}
void VertexManager::ThisVertexAsDestVertex(
    Vertex *vertex, std::vector<std::pair<Edge *, Edge *>> *edge)
{
    VertexOptions vertex_options = vertex->GetVertexOptions();
    srand(clock());
    //该点作为dest点
    uint64_t as_dest_vertex_num =
        (rand() % (vertex_options.as_dest_vertex_max_ -
                   vertex_options.as_dest_vertex_min_ + 1) +
         vertex_options.as_src_vertex_min_);
    //首先利用生成的所有点建立链接 不够的话重新建立点

    uint64_t num = 0;
    for (auto &it : all_vertex_) {
        if (it != vertex && !vertex->IsInDestVertexSet(it) &&
            !it->IsInSrcVertexSet(vertex)) {
            Edge *e1 = new Edge(vertex, it, -1, 8);
            Edge *e2 = new Edge(it, vertex, 1, 8);
            edge->push_back({e2, e1});
            vertex->InsertSrcVertexSet(it);
            it->InsertDestVertexSet(vertex);
            ++num;
        }
    }
    uint64_t need_regenerate_num = 0;
    if (num < as_dest_vertex_num) {
        need_regenerate_num = as_dest_vertex_num - num;
    }

    for (uint64_t i = 0; i < need_regenerate_num; ++i) {
        Vertex *new_vertex = CreateVertex(vertex->GetVertexOptions());

        Edge *e1 = new Edge(vertex, new_vertex, -1, 8);
        Edge *e2 = new Edge(new_vertex, vertex, 1, 8);
        vertex->InsertSrcVertexSet(new_vertex);
        new_vertex->InsertDestVertexSet(vertex);
        edge->push_back({e2, e1});
    }
}
}  // namespace manager
