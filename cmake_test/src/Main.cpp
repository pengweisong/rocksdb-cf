

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "gflags/gflags.h"

#include "CFPartition.h"
#include "Config.h"
#include "DBPartition.h"
#include "Edge.h"
#include "Manager.h"
#include "Options.h"
#include "Vertex.h"

using namespace std;

static const std::string VALUE =
    "13ijo12joiijiKNADNOAJFOAKOPKAPOFKOQEI)I@)_$I@_)$_@#_EO_)Qoijworwq";
Options options;

Manager *manager_ptr = nullptr;

std::string MakeRandomString(uint64_t size)
{
    std::string result;
    result.reserve(size);

    srand(clock());
    for (uint64_t i = 0; i < size; ++i) {
        int index = rand() % VALUE.size();
        result += VALUE[index];
    }
    return result;
}
void InitOptions()
{
    options.partition_num_ = FLAGS_kDefaultPartitionNum;
    options.vertex_num_ = FLAGS_kDefaultVertexNum;
    options.value_size_ = FLAGS_kDefaultValueSize;
    options.generate_values_ =
        std::bind(MakeRandomString, std::placeholders::_1);

    options.vertex_options_.as_src_vertex_min_ = FLAGS_kDefaultAsSrcVertexMin;
    options.vertex_options_.as_src_vertex_max_ = FLAGS_kDefaultAsSrcVertexMax;
    options.vertex_options_.as_dest_vertex_min_ = FLAGS_kDefaultAsDestVertexMin;
    options.vertex_options_.as_dest_vertex_max_ = FLAGS_kDefaultAsDestVertexMax;
    options.vertex_options_.node_type_ = FLAGS_kDefaultNodeType;
    options.vertex_options_.vertex_type_ = VertexOrEdgeType::kVertex;
}

void GenerateVertex(Manager *manager)
{
    manager->CreateVertexVector();
    manager->CreateValues(FLAGS_kDefaultVertexNum);
}

//以该点为起始点随机生成一些边
void GenerateEdge(uint64_t index, Manager *manager)
{
    Vertex *start_vertex = manager->GetVertex(index);

    manager->CreateEdgeByVertex(start_vertex, 0);
}
void CreateDBAndCFPartition(Manager *manager)
{
    rocksdb::Options options;
    options.create_if_missing = true;

    std::vector<std::string> node_type(FLAGS_kDefaultPartitionNum,
                                       FLAGS_kDefaultNodeType);

    rocksdb::Status s = manager->CreateDBPartition(options, node_type);
    assert(s.ok());

    rocksdb::ColumnFamilyOptions cf_options;

    s = manager->CreateCFPartition(options, cf_options, node_type);
    assert(s.ok());
}

void Init(Manager *manager)
{
    CFPartition::SetDefaultDBPath(FLAGS_kCFPartitionDBPath);
    DBPartition::SetDefaultDBPreixPath(FLAGS_kDBPartitionDBPreixName);

    manager->CreateVertexManager();

    CreateDBAndCFPartition(manager);

    GenerateVertex(manager);
    GenerateEdge(0, manager);
}

void PrintArgument()
{
    std::cout << "DefaultCFPartitionDBPath " << FLAGS_kCFPartitionDBPath
              << "\n";
    std::cout << "DefaultDBPartitionDBPreixName "
              << FLAGS_kDBPartitionDBPreixName << "\n";

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

void WriteVertex(uint64_t cf_id, uint64_t db_id)
{
    rocksdb::WriteOptions options;

    Options &manager_options = manager_ptr->GetOptions();

    rocksdb::Status s = manager_ptr->WriteVertexToCF(
        options, cf_id, 0, manager_options.vertex_num_ - 1);

    assert(s.ok());

    s = manager_ptr->WriteVertexToDB(options, db_id, 0,
                                     manager_options.vertex_num_ - 1);
    assert(s.ok());
}

void GetVertex(uint64_t cf_id, uint64_t db_id)
{
    rocksdb::ReadOptions options;

    Options &manager_options = manager_ptr->GetOptions();

    rocksdb::Status s = manager_ptr->GetVertexByCF(
        options, cf_id, 0, manager_options.vertex_num_ - 1, true);

    assert(s.ok());

    s = manager_ptr->GetVertexByDB(options, db_id, 0,
                                   manager_options.vertex_num_ - 1, true);
    assert(s.ok());
}

void WriteEdge(uint64_t cf_id, uint64_t db_id)
{
    rocksdb::WriteOptions options;
    uint64_t edge_num = manager_ptr->GetEdgeNum();

    //写入0 - edge_num-1 所有的边
    rocksdb::Status s =
        manager_ptr->WriteOutEdgeToCF(options, cf_id, 0, edge_num - 1);

    assert(s.ok());

    s = manager_ptr->WriteOutEdgeToDB(options, db_id, 0, edge_num - 1);
    assert(s.ok());
}
void ReadEdge(uint64_t cf_id, uint64_t db_id)
{
    rocksdb::ReadOptions options;
    uint64_t edge_num = manager_ptr->GetEdgeNum();

    std::vector<Edge *> in_edge = manager_ptr->GetWrittenInEdge();

    for (auto &it : in_edge) {
        rocksdb::Status s = manager_ptr->GetOutEdgeByCF(
            options, cf_id, it->GetSrcVertex()->GetVertexId(),
            it->GetDestVertex()->GetVertexId(), true);
        assert(s.ok());
    }

    for (auto &it : in_edge) {
        rocksdb::Status s = manager_ptr->GetOutEdgeByDB(
            options, db_id, it->GetSrcVertex()->GetVertexId(),
            it->GetDestVertex()->GetVertexId(), true);
        assert(s.ok());
    }
}

void Scan(uint64_t cf_id, uint64_t db_id)
{
    rocksdb::ReadOptions options;

    rocksdb::Status s = manager_ptr->ScanDBPartition(options, db_id);

    std::cout << "status " << s.ToString() << "\n";

    assert(s.ok());
}

int main(int argc, char **argv)
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    PrintArgument();
    InitOptions();
    Manager manager(options);
    manager_ptr = &manager;
    Init(manager_ptr);
    WriteVertex(1, 0);
    GetVertex(1, 0);
    Scan(1, 0);
    // WriteEdge(1, 0);
    // ReadEdge(1, 0);
}
