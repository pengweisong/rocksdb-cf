
#include "generate_vertex_edge.h"
#include "util.h"
#include "vertex_options.h"

#include "gflags/gflags.h"

#include <ctime>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace manager;

static const std::string VALUE =
    "13ijo12joiijiKNADNOAJFOAKOPKAPOFKOQEI)I@)_$I@_)$_@#_EO_)Qoijworwq";

std::string MakeRandomString(manager::DefaultOptions &default_options)
{
    std::string result;
    result.reserve(default_options.manager_options_.value_size_);

    srand(clock());
    for (uint64_t i = 0; i < default_options.manager_options_.value_size_;
         ++i) {
        int index = rand() % VALUE.size();
        result += VALUE[index];
    }
    return result;
}

void GenerateVertex(manager::DefaultOptions &default_options,
                    uint64_t vertex_num_)
{
    default_options.vertex_manager_->CreateVertexVector(
        &default_options.all_vertex_, vertex_num_,
        default_options.manager_options_.vertex_options_);
}
void GenerateValues(manager::DefaultOptions &default_options, uint64_t size)
{
    default_options.values_.reserve(size);
    for (uint64_t i = 0; i < size; ++i) {
        default_options.values_.emplace_back(MakeRandomString(default_options));
    }
}
void Init(manager::DefaultOptions &default_options)
{
    CFPartition::SetDefaultDBPath(FLAGS_kCFPartitionDBPath);
    DBPartition::SetDefaultDBPreixPath(FLAGS_kDBPartitionDBPreixName);

    rocksdb::Options options;
    options.create_if_missing = true;
    default_options.cf_partition_.reset(new CFPartition(options));
    default_options.db_partition_.reset(new DBPartition());
    default_options.vertex_manager_.reset(new VertexManager());
    default_options.manager_options_.vertex_num_ = FLAGS_kDefaultVertexNum;
    default_options.manager_options_.value_size_ = FLAGS_kDefaultValueSize;
    default_options.manager_options_.partition_num_ =
        FLAGS_kDefaultPartitionNum;
    default_options.manager_options_.vertex_options_.as_src_vertex_max_ =
        FLAGS_kDefaultAsSrcVertexMax;
    default_options.manager_options_.vertex_options_.as_src_vertex_min_ =
        FLAGS_kDefaultAsSrcVertexMin;
    default_options.manager_options_.vertex_options_.as_dest_vertex_max_ =
        FLAGS_kDefaultAsDestVertexMax;
    default_options.manager_options_.vertex_options_.as_dest_vertex_min_ =
        FLAGS_kDefaultAsDestVertexMin;

    rocksdb::ColumnFamilyOptions cf_options;

    default_options.cf_partition_->CreateCF(FLAGS_kDefaultPartitionNum,
                                            cf_options);
    default_options.db_partition_->CreateDB(FLAGS_kDefaultPartitionNum,
                                            options);

    GenerateVertex(default_options, FLAGS_kDefaultVertexNum);
    GenerateValues(default_options, FLAGS_kDefaultVertexNum);

    uint64_t vertex_start_index = 1;
    uint64_t vertex_end_index = 0;

    srand(clock());
    // while (vertex_start_index > vertex_end_index) {
    // vertex_start_index = rand() % default_options.all_vertex_.size();
    // vertex_end_index = rand() % default_options.all_vertex_.size();
    // }

    //对于这个区间的点建立边
    // for (uint64_t i = vertex_start_index; i <= vertex_end_index; ++i) {
    // util::BuildVertexAndEdgeConnection(default_options,
    // default_options.all_vertex_[i], 0);
    // }
}
rocksdb::Status WriteVertex(manager::DefaultOptions &default_options,
                            const rocksdb::WriteOptions &write_options,
                            uint64_t cf_id, uint64_t db_id)
{
    std::cout << "Write "
              << "\n";
    clock_t cf_start_time, cf_end_time;
    clock_t db_start_time, db_end_time;

    cf_start_time = clock();

    rocksdb::Status s = util::WriteVertexToCF(
        write_options, default_options, cf_id, 0,
        static_cast<uint64_t>(default_options.all_vertex_.size() - 1));

    cf_end_time = clock();
    db_start_time = clock();

    s = util::WriteVertexToDB(
        write_options, default_options, db_id, 0,
        static_cast<uint64_t>(default_options.all_vertex_.size() - 1));
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

void PrintArgument()
{
    std::cout << "DefaultValueSize " << FLAGS_kDefaultValueSize << "\n";
    std::cout << "DefaultVertexNum " << FLAGS_kDefaultVertexNum << "\n";
    std::cout << "DefualtPartitionNum " << FLAGS_kDefaultPartitionNum << "\n";
    std::cout << "DefaultNodeTypeLen " << FLAGS_kDefaultNodeTypeLen << "\n";
    std::cout << "DefaultNodeType " << FLAGS_kDefaultNodeType << "\n";
    std::cout << "DefaultAsSrcVertexMin " << FLAGS_kDefaultAsSrcVertexMin
              << "\n";
    std::cout << "DefaultAsSrcVertexMax " << FLAGS_kDefaultAsSrcVertexMax
              << "\n";
    std::cout << "DefaultAsDestVertexMin " << FLAGS_kDefaultAsDestVertexMin
              << "\n";
    std::cout << "DefaultAsDestVertexMax " << FLAGS_kDefaultAsDestVertexMax
              << "\n";
}

int main(int argc, char **argv)
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    PrintArgument();
    manager::DefaultOptions default_options;
    Init(default_options);
    // WriteVertex(default_options, rocksdb::WriteOptions(), 1, 0);
}
