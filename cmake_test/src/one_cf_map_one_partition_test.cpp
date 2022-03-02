
#include <iostream>
#include <memory>
#include <vector>

#include "rocksdb/db.h"

#include "generate_vertex_edge.h"
#include "one_cf_map_one_partition.h"

using namespace std;
using namespace manager;
using namespace cfpartition;

static const std::string VALUE =
    "13ijo12joiijiKNADNOAJFOAKOPKAPOFKOQEI)I@)_$I@_)$_@#_EO_)Qoijworwq";

static const uint64_t Len = 4096;

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

std::vector<std::string> values;

void MakeString(uint64_t num)
{
    for (uint64_t i = 0; i < num; ++i) {
        values.emplace_back(MakeRandomString());
    }
}

void Test(uint32_t vertex_num)
{
    rocksdb::Options options;
    options.create_if_missing = true;

    std::unique_ptr<CFPartition> cfpartition(new CFPartition(options));
    rocksdb::ColumnFamilyOptions cf_options;
    uint64_t cf_num = 100;

    cfpartition->CreateCF(cf_num, cf_options);

    VertexOptions vertex_options;
    vertex_options.as_src_vertex_max_ = 4;
    vertex_options.as_src_vertex_min_ = 1;
    vertex_options.node_type_ = cfpartition->GetCFName(1);
    vertex_options.node_type_len_ = cfpartition->GetCFName(1).size();
    vertex_options.vertex_type_ = VertexOrEdgeType::kVertex;
    VertexManager vertex_manager;
    std::vector<manager::Vertex *> vertex_vec;
    vertex_vec.reserve(vertex_num);
    // CF1
    vertex_manager.CreateVertexVector(&vertex_vec, vertex_num,
                                        vertex_options);

    MakeString(vertex_num);  //生成4k的value

    for (uint32_t i = 0; i < vertex_num; ++i) {  //写入id为1的列族
        rocksdb::Status s = cfpartition->Put(
            rocksdb::WriteOptions(), 1, vertex_vec[i]->GetRep(), values[i]);
        vertex_vec[i]->Decode();
        assert(vertex_vec[i]->GetVertexId() == i);
        assert(vertex_vec[i]->GetType() == vertex_options.vertex_type_);
        assert(s.ok());
    }

    std::vector<std::pair<Edge *, Edge *>> edges;

    vertex_manager.ThisVertexAsSrcVertex(
        vertex_vec[0], &edges);  //将第一个点作为出点随机建立一些边

    auto backward_iter = values.rbegin();
    std::vector<std::string> keys;
    //写入边first->second
    for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
        rocksdb::Status s = cfpartition->Put(
            rocksdb::WriteOptions(), 1, iter->first->GetRep(), *backward_iter);
        ++backward_iter;
    }
    //反向写入
    auto forward_iter = values.begin();
    for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
        rocksdb::Status s = cfpartition->Put(
            rocksdb::WriteOptions(), 1, iter->second->GetRep(), *forward_iter);
        ++forward_iter;
    }

    rocksdb::Status s = cfpartition->Flush(rocksdb::FlushOptions(), 1);
    assert(s.ok());

    std::string value;

    for (uint32_t i = 0; i < vertex_num; ++i) {
        rocksdb::Status s =
            cfpartition->GetVertex(rocksdb::ReadOptions(), 1, i, &value);
        assert(s.ok());
        assert(value == values[i]);
        value.clear();
    }

    //以该点作为出点获得写入的边的V
    backward_iter = values.rbegin();
    forward_iter = values.begin();

    for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
        rocksdb::Status s = cfpartition->GetOutEdgeByVertex(
            rocksdb::ReadOptions(), 1,
            iter->first->GetSrcVertex()->GetVertexId(),
            iter->first->GetDestVertex()->GetVertexId(), &value);

        assert(s.ok());
        assert(value == *backward_iter);
        ++backward_iter;
    }
    for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
        rocksdb::Status s = cfpartition->GetInEdgeByVertex(
            rocksdb::ReadOptions(), 1,
            iter->second->GetSrcVertex()->GetVertexId(),
            iter->second->GetDestVertex()->GetVertexId(), &value);

        assert(s.ok());
        assert(value == *forward_iter);
        ++forward_iter;
    }

    // assert(s.ok());
}
int main(int argc, char **argv)
{
    uint32_t vertex_num = atoi(argv[1]);

    // TestEdge();
    Test(vertex_num);
}
