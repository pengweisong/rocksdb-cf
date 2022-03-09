

#pragma once

#include <assert.h>
#include <iostream>
#include <vector>

#include "Options.h"

class Vertex;

class Edge {
public:
    explicit Edge(uint64_t start_vertex_id, uint64_t end_vertex_id,
                  const std::string &node_type, int32_t edge_type,
                  uint64_t rank);

    Edge() = default;

    ~Edge();

    void Encode(std::string rep) { rep_.assign(rep.begin(), rep.end()); }

    std::string GetRep() const { return std::string(rep_.data(), rep_.size()); }
    uint64_t GetStartVertexId() const { return start_vertex_id_; }
    uint64_t GetDestVertex() const { return end_vertex_id_; }
    uint32_t GetEdgeType() const { return edge_type_; }
    uint64_t GetRank() const { return rank_; }
    EdgePlaceHolder GetPlaceHolder() const { return edge_place_holder_; }
    VertexOrEdgeType GetNodeType() const { return is_edge_; }

    void Encode();
    void Decode();

    void Clear();

private:
    void EncodeEdgeOrVertex(uint32_t *index);
    void EncodeEdgeType(uint32_t *index);
    void EncodeVertex(uint64_t vertex_id, uint32_t *index);
    void EncodeRank(uint32_t *index);
    void EncodePlaceHolder(uint32_t *index);

    void DecodeEdgeOrVertex(uint32_t *index);
    void DecodeEdgeType(uint32_t *index);
    void DecodeRank(uint32_t *index);
    void DecodeVertex(uint64_t vertex_id, uint32_t *index);
    void DecodePlaceHolder(uint32_t *index);

    // bool operator==(const Edge &edge)
    // {
    // return dest_vertex_ == edge.dest_vertex_ &&
    // src_vertex_ == edge.src_vertex_;
    // }
    // Vertex *src_vertex_;   //起点
    // Vertex *dest_vertex_;  //终点

    uint64_t start_vertex_id_;
    uint64_t end_vertex_id_;

    std::string node_type_;
    int32_t edge_type_;  //>0出边 <0入边
    uint64_t rank_;
    EdgePlaceHolder edge_place_holder_;
    VertexOrEdgeType is_edge_;
    std::vector<char> rep_;
};

