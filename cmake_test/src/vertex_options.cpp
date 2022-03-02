

#include <cstdint>
#include <string>

#include "generate_vertex_edge.h"
#include "vertex_options.h"

namespace manager {

DEFINE_uint64(kDefaultPartitionNum, 100,
              "Create Column Family Number Or DB Num");
DEFINE_uint64(kDefaultNodeTypeLen, 9, "VertexOptions kDefaultNodeTypeLen ");
DEFINE_string(kDefaultNodeType, "Partition", "VertexOptions kDefaultNodeType ");
DEFINE_uint64(kDefaultAsSrcVertexMin, 3, "Min Vertex As Src Vertex");
DEFINE_uint64(kDefaultAsSrcVertexMax, 5, "Max Vertex As Src Vertex");
DEFINE_uint64(kDefaultAsDestVertexMin, 3, "Min Vertex As Dest Vertex");
DEFINE_uint64(kDefaultAsDestVertexMax, 3, "Max Vertex As Dest Vertex");
DEFINE_uint64(kDefaultVertexNum, 50, "Create DefaultVertexNum ");

//针对CFPartition单独的参数
DEFINE_string(kCFPartitionDBPath, "/tmp/CFPartition",
              "CFPartition Default DB Path ");
DEFINE_string(kCFPartitionCFPreixName, "CF",
              "CFPartition Column Family Preix Name ");  // Name=preixname+i;

//针对DBPartition单独的参数
DEFINE_string(kDBPartitionDBPreixName, "/tmp/DBPartition",
              "DBPartition Db Preix Name ");

DefaultOptions::DefaultOptions() {}
DefaultOptions::~DefaultOptions() {}

}  // namespace manager
