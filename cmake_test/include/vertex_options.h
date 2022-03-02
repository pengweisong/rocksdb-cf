
#ifndef __VERTEX_OPTIONS_H_
#define __VERTEX_OPTIONS_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "one_cf_map_one_partition.h"
#include "one_db_map_one_partition.h"

#include <cstdint>

#include "gflags/gflags.h"

namespace manager {

class Vertex;
class Edge;
class VertexManager;

enum VertexOrEdgeType : char { kInvalidType, kVertex, kEdge };

enum EdgePlaceHolder : char { kPlaceHolder };
struct VertexOptions {
    uint64_t as_src_vertex_min_ = 1;  //作为src最少与多少点关联
    uint64_t as_src_vertex_max_;      //作为src最多与多少点关联

    uint64_t as_dest_vertex_min_ = 1;
    uint64_t as_dest_vertex_max_;

    std::string node_type_;
    uint32_t node_type_len_;
    VertexOrEdgeType vertex_type_;
};

struct Options {
    uint64_t partition_num;
    uint64_t vertex_num;

    VertexOptions vertex_options_;
};
struct DefaultOptions {
    DefaultOptions();
    ~DefaultOptions();
    DefaultOptions(const DefaultOptions &) = delete;
    DefaultOptions &operator=(const DefaultOptions &) = delete;
    DefaultOptions(DefaultOptions &&) = delete;
    DefaultOptions &operator=(DefaultOptions &&) = delete;

    std::unique_ptr<VertexManager> vertex_manager;
    std::vector<Vertex *> all_vertex;
    Options manager_options;
    std::vector<std::string> values;
    std::vector<std::pair<Edge *, Edge *>> in_edge_map_to_out_edge;
    std::unordered_map<std::string, std::string> edge_key_map_to_value;
    std::unique_ptr<CFPartition> cf_partition;
    std::unique_ptr<DBPartition> db_partition;
};

}  // namespace manager

#endif
