
#pragma once

#include "gflags/gflags.h"

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

