
#pragma once

#include <assert.h>
#include <cstring>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "Options.h"

class Edge;

class Vertex {
public:
    explicit Vertex(uint64_t vertex_id, const std::string &node_type);

    Vertex(const Vertex &) = delete;
    Vertex(Vertex &&) = delete;

    void Encode(std::string rep) { rep_.assign(rep.begin(), rep.end()); }
    void Decode();

    std::string GetRep() const { return std::string(rep_.data(), rep_.size()); }
    VertexOrEdgeType GetType() const { return vertex_type_; }
    std::string GetNodeType() const { return node_type_; }
    uint64_t GetVertexId() const { return vertex_id_; }

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

    bool operator<(const Vertex &vertex) const
    {
        return ::strcmp(rep_.data(), vertex.rep_.data()) < 0;
    }
};

