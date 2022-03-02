
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

DECLARE_uint64(kDefaultValueSize);
DECLARE_uint64(kDefaultPartitionNum);
DECLARE_uint64(kDefaultNodeTypeLen);
DECLARE_string(kDefaultNodeType);
DECLARE_uint64(kDefaultAsSrcVertexMin);
DECLARE_uint64(kDefaultAsSrcVertexMax);
DECLARE_uint64(kDefaultAsDestVertexMin);
DECLARE_uint64(kDefaultAsDestVertexMax);
DECLARE_uint64(kDefaultVertexNum);

//针对CFPartition单独的参数
DECLARE_string(kCFPartitionDBPath);

//针对DBPartition单独的参数
DECLARE_string(kDBPartitionDBPreixName);

class Vertex;
class Edge;
class VertexManager;

enum VertexOrEdgeType : char { kInvalidType, kVertex, kEdge };

enum EdgePlaceHolder : char { kPlaceHolder };
struct VertexOptions {
    uint64_t as_src_vertex_min_;  //作为src最少与多少点关联
    uint64_t as_src_vertex_max_;  //作为src最多与多少点关联

    uint64_t as_dest_vertex_min_;
    uint64_t as_dest_vertex_max_;

    std::string node_type_;
    uint32_t node_type_len_;
    VertexOrEdgeType vertex_type_;
};

struct Options {
    uint64_t partition_num_;
    uint64_t vertex_num_;
    uint64_t value_size_;

    VertexOptions vertex_options_;
};
struct DefaultOptions {
    DefaultOptions();
    ~DefaultOptions();
    DefaultOptions(const DefaultOptions &) = delete;
    DefaultOptions &operator=(const DefaultOptions &) = delete;
    DefaultOptions(DefaultOptions &&) = delete;
    DefaultOptions &operator=(DefaultOptions &&) = delete;

    Options manager_options_;
    std::unique_ptr<VertexManager> vertex_manager_;
    std::vector<Vertex *> all_vertex_;
    std::vector<std::string> values_;
    std::vector<std::pair<Edge *, Edge *>> in_edge_map_to_out_edge_;
    std::unordered_map<std::string, std::string> edge_key_map_to_value_;
    std::unique_ptr<CFPartition> cf_partition_;
    std::unique_ptr<DBPartition> db_partition_;
};

}  // namespace manager

#endif
