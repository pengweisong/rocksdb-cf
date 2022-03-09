
#pragma once

#include <functional>
#include <string>

enum VertexOrEdgeType : char { kInValidType, kVertex, kEdge };

enum EdgePlaceHolder : char { kInValidPlaceHolder, kPlaceHolder };
struct VertexOptions {
    uint64_t as_src_vertex_min_;  //作为src最少与多少点关联
    uint64_t as_src_vertex_max_;  //作为src最多与多少点关联

    uint64_t as_dest_vertex_min_;
    uint64_t as_dest_vertex_max_;

    std::string node_type_;
    VertexOrEdgeType vertex_type_;
};

struct Options {
    uint64_t partition_num_;

    uint64_t vertex_num_;

    uint64_t value_size_;

    uint64_t min_out_edge_;  //作为src最少与多少点关联

    uint64_t max_out_edge_;  //作为src最多与多少点关联

    uint64_t min_in_edge_;

    uint64_t max_in_edge_;

    std::function<std::string(uint64_t)> generate_values_ = nullptr;

    // VertexOptions vertex_options_;
};

