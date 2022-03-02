
#ifndef __GENERATE_VERTEX_EDGE_H_
#define __GENERATE_VERTEX_EDGE_H_

#include "vertex_options.h"

#include <iostream>
#include <unordered_set>
#include <vector>

#include <assert.h>
#include <cstring>

namespace manager {

class Edge;

class Vertex {
public:
    explicit Vertex(uint64_t vertex_id, const std::string &vertex_node_type,
                    uint32_t vertex_node_type_len);
    explicit Vertex(uint64_t vertex_id, const VertexOptions &options);
    Vertex(const Vertex &) = delete;
    Vertex(Vertex &&) = delete;

    void Encode(std::string rep) { rep_.assign(rep.begin(), rep.end()); }
    void Decode();

    std::string GetRep() const { return std::string(rep_.data(), rep_.size()); }
    VertexOrEdgeType GetType() const { return vertex_type_; }
    std::string GetNodeType() const { return node_type_; }
    uint64_t GetVertexId() const { return vertex_id_; }
    VertexOptions GetVertexOptions() const { return vertex_options_; }

    ~Vertex();
    void Clear();

    void SetVertexId(uint64_t vertex_id) { vertex_id_ = vertex_id; }

    bool IsInSrcVertexSet(Vertex *vertex) const
    {
        return src_vertex_.count(vertex);
    }
    bool IsInDestVertexSet(Vertex *vertex) const
    {
        return dest_vertex_.count(vertex);
    }

    void InsertDestVertexSet(Vertex *vertex) { dest_vertex_.insert({vertex}); }
    void InsertSrcVertexSet(Vertex *vertex) { src_vertex_.insert({vertex}); }

private:
    void Encode();
    void DecodeVertexType();
    void DecodeNodeType();
    void DecodeVertexId();

    uint64_t vertex_id_;
    VertexOptions vertex_options_;
    std::string node_type_;
    uint32_t node_type_len_;
    VertexOrEdgeType vertex_type_;

    std::unordered_set<Vertex *> dest_vertex_;  //该点作为入点对应的所有出点
    std::unordered_set<Vertex *> src_vertex_;  //该点作为出点对应的所有入店

    std::vector<char> rep_;

    bool operator==(const Vertex &vertex) const
    {
        return vertex_id_ == vertex.GetVertexId();
    }
};

class VertexManager {
public:
    VertexManager();
    ~VertexManager();

    VertexManager(const VertexManager &) = delete;
    VertexManager(VertexManager &&) = delete;

    Vertex *CreateVertex(const std::string &node_type, uint32_t node_type_len)
    {
        VertexOptions options;
        options.node_type_ = node_type;
        options.node_type_len_ = node_type_len;
        return CreateVertex(options);
    }
    Vertex *CreateVertex(const VertexOptions &vertex_options)
    {
        Vertex *vertex = new Vertex(next_vertex_id_, vertex_options);
        all_vertex_.insert(vertex);
        ++next_vertex_id_;
        return vertex;
    }
    void CreateVertexVector(std::vector<Vertex *> *vertex_vector,
                            uint32_t generate_num,
                            const VertexOptions &vertex_options)
    {
        for (uint32_t i = 0; i < generate_num; ++i) {
            vertex_vector->emplace_back(CreateVertex(vertex_options));
        }
    }

    void CreateVertexVector(std::vector<Vertex *> *vertex_vector,
                            uint32_t generate_num, const std::string &node_type,
                            uint32_t node_type_len)
    {
        for (uint32_t i = 0; i < generate_num; ++i) {
            vertex_vector->emplace_back(CreateVertex(node_type, node_type_len));
        }
    }

    //该点作为入点 生成随机数量的边 存在vec中
    void ThisVertexAsSrcVertex(Vertex *vertex,
                               std::vector<std::pair<Edge *, Edge *>> *edge);

    // edge前面返回的是src点 后方全是dest点
    void ThisVertexAsDestVertex(Vertex *vertex,
                                std::vector<std::pair<Edge *, Edge *>> *edge);

    std::vector<Vertex *> BuildConnection(Vertex *vertex)
    {
        std::vector<Vertex *> result;
        return result;
    }

private:
    std::unordered_set<Vertex *> all_vertex_;
    uint64_t next_vertex_id_;
};

class Edge {
public:
    explicit Edge(Vertex *src_vertex, Vertex *dest_vertex, int32_t edge_type,
                  uint64_t rank)
        : src_vertex_(src_vertex),
          dest_vertex_(dest_vertex),
          edge_type_(edge_type),
          rank_(rank),
          edge_place_holder_(EdgePlaceHolder::kPlaceHolder),
          is_edge_(VertexOrEdgeType::kEdge)
    {
        rep_.resize(sizeof(is_edge_) + sizeof(edge_type_) + sizeof(rank_) +
                    2 * sizeof(uint64_t) + sizeof(edge_place_holder_));
        Encode();
    }

    explicit Edge(uint64_t vertex_id, int32_t edge_type)
        : vertex_id_(vertex_id),
          edge_type_(edge_type),
          edge_place_holder_(EdgePlaceHolder::kPlaceHolder),
          is_edge_(VertexOrEdgeType::kEdge)
    {
        EncodeEdgePreix();
    }
    void EncodeEdgePreix()
    {
        uint32_t index = 0;
        EncodeEdgeOrVertex(&index);
        EncodeEdgeType(&index);
        EncodeVertex(vertex_id_, &index);
    }

    ~Edge() { Clear(); }

    void Clear()
    {
        is_edge_ = kEdge;
        rank_ = 0;
        edge_place_holder_ = EdgePlaceHolder::kPlaceHolder;
        dest_vertex_->Clear();
        src_vertex_->Clear();
        rep_.clear();
    }

    void Encode(std::string rep) { rep_.assign(rep.begin(), rep.end()); }

    // const char *GetRep() const { return rep_.data(); }
    std::string GetRep() const { return std::string(rep_.data(), rep_.size()); }
    Vertex *GetSrcVertex() const { return src_vertex_; }
    Vertex *GetDestVertex() const { return dest_vertex_; }
    uint32_t GetEdgeType() const { return edge_type_; }
    uint64_t GetRank() const { return rank_; }
    EdgePlaceHolder GetPlaceHolder() const { return edge_place_holder_; }
    VertexOrEdgeType GetNodeType() const { return is_edge_; }

    void Encode();
    void Decode();

private:
    void EncodeEdgeOrVertex(uint32_t *index);
    void EncodeEdgeType(uint32_t *index);
    void EncodeVertex(uint64_t vertex_id, uint32_t *index);
    void EncodeRank(uint32_t *index);
    void EncodePlaceHolder(uint32_t *index);

    void DecodeEdgeOrVertex(uint32_t *index);
    void DecodeEdgeType(uint32_t *index);
    void DecodeRank(uint32_t *index);
    void DecodeVertex(Vertex *vertex, uint32_t *index);
    void DecodePlaceHolder(uint32_t *index);

    bool operator==(const Edge &edge)
    {
        return dest_vertex_ == edge.dest_vertex_ &&
               src_vertex_ == edge.src_vertex_;
    }
    Vertex *src_vertex_;   //起点
    Vertex *dest_vertex_;  //终点

    uint64_t vertex_id_;

    int32_t edge_type_;  //>0出边 <0入边
    uint64_t rank_;
    EdgePlaceHolder edge_place_holder_;
    VertexOrEdgeType is_edge_;
    std::vector<char> rep_;
};

}  // namespace manager

#endif
