
#include "generate_vertex_edge.h"
#include "util.h"
#include "vertex_options.h"

#include "gflags/gflags.h"

#include <ctime>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace fLS;

DECLARE_uint64(kDefaultPartitionNum);
DECLARE_uint64(kDefaultNodeTypeLen);
DECLARE_string(kDefaultNodeType);
DECLARE_uint64(kDefaultAsSrcVertexMax);
DECLARE_uint64(kDefaultAsSrcVertexMin);
DECLARE_uint64(kDefaultAsDestVertexMax);
DECLARE_uint64(kDefaultAsDestVertexMin);
DECLARE_uint64(kDefaultVertexNum);
DECLARE_string(kCFPartitionDBPath);
DECLARE_string(kCFPartitionCFPreixName);
DECLARE_string(kDBPartitionDBPreixName);

using namespace std;
using namespace manager;
using namespace fLS;
using namespace fLU64;

static const std::string VALUE =
    "13ijo12joiijiKNADNOAJFOAKOPKAPOFKOQEI)I@)_$I@_)$_@#_EO_)Qoijworwq";

static const uint64_t Len = 4096;

manager::DefaultOptions default_options;

std::string MakeRandomString()
{
    std::string result;
    result.reserve(Len);

    srand(clock());
    for (uint64_t i = 0; i < Len; ++i) {
        int index = rand() % VALUE.size();
        result += VALUE[index];
    }
    return result;
}

void GenerateVertex(uint64_t vertex_num)
{
    default_options.vertex_manager->CreateVertexVector(
        &default_options.all_vertex, vertex_num,
        default_options.manager_options.vertex_options_);
}
void GenerateValues(uint64_t size)
{
    default_options.values.reserve(size);
    for (uint64_t i = 0; i < size; ++i) {
        default_options.values.emplace_back(MakeRandomString());
    }
}
void Init()
{
    CFPartition::SetDefaultDBPath(fLS::FLAGS_kCFPartitionCFPreixName);
    DBPartition::SetDefaultDBPreixPath(FLAGS_kDBPartitionDBPreixName);

    rocksdb::Options options;
    options.create_if_missing = true;
    default_options.cf_partition.reset(new CFPartition(options));
    default_options.db_partition.reset(new DBPartition());
    default_options.vertex_manager.reset(new VertexManager());

    rocksdb::ColumnFamilyOptions cf_options;

    default_options.cf_partition->CreateCF(FLAGS_kDefaultPartitionNum,
                                           cf_options);
    default_options.db_partition->CreateDB(FLAGS_kDefaultPartitionNum, options);

    GenerateVertex(FLAGS_kDefaultVertexNum);
    GenerateValues(FLAGS_kDefaultVertexNum);

    uint64_t vertex_start_index = 1;
    uint64_t vertex_end_index = 0;

    while (vertex_start_index > vertex_end_index) {
        vertex_start_index = rand() % default_options.all_vertex.size();
        vertex_end_index = rand() % default_options.all_vertex.size();
    }

    //对于这个区间的点建立边
    for (uint64_t i = vertex_start_index; i <= vertex_end_index; ++i) {
        util::BuildVertexAndEdgeConnection(default_options,
                                           default_options.all_vertex[i], 0);
    }
}
rocksdb::Status WriteVertex(const rocksdb::WriteOptions &write_options,
                            uint64_t cf_id, uint64_t db_id)
{
    clock_t cf_start_time, cf_end_time;
    clock_t db_start_time, db_end_time;

    cf_start_time = clock();

    rocksdb::Status s = util::WriteVertexToCF(
        write_options, default_options, cf_id, 0,
        static_cast<uint64_t>(default_options.all_vertex.size() - 1));

    cf_end_time = clock();
    db_start_time = clock();

    s = util::WriteVertexToDB(
        write_options, default_options, db_id, 0,
        static_cast<uint64_t>(default_options.all_vertex.size() - 1));
    db_end_time = clock();

    std::cout << "CFPartition Time: "
              << static_cast<double>(cf_end_time - cf_start_time) /
                     CLOCKS_PER_SEC
              << " "
              << " DBPartition Time: "
              << static_cast<double>(db_end_time - db_start_time) /
                     CLOCKS_PER_SEC
              << "\n";
    return s;
}

int main(int argc, char **argv)
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    Init();
    WriteVertex(rocksdb::WriteOptions(), 1, 0);
}
