
#include "generate_vertex_edge.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace manager;
using namespace std;
void DeleteAllVertex(const vector<Vertex *> &vec)
{
    for (auto &it : vec) delete it;
}
void GenerateVertexVector(uint32_t num, std::string node_type,
                          uint32_t node_type_len = kDefaultNodeTypeLen)
{
    VertexManager vertex_manager;
    std::vector<Vertex *> vec;

    vertex_manager.GenerateVertexVector(&vec, num, node_type);

    std::vector<std::string> reps;
    for (uint32_t i = 0; i < num; ++i) {
        vec[i]->Decode();
        reps.emplace_back(vec[i]->GetRep());
        assert(vec[i]->GetNodeType() == node_type);
        assert(vec[i]->GetVertexId() == static_cast<uint64_t>(i));
        assert(vec[i]->GetType() == kVertex);
        vec[i]->Clear();
    }

    //利用之前Encode rep 进行还原
    for (uint32_t i = 0; i < num; ++i) {
        vec[i]->Encode(reps[i]);
        vec[i]->Decode();
        assert(vec[i]->GetNodeType() == node_type);
        assert(vec[i]->GetVertexId() == static_cast<uint64_t>(i));
        assert(vec[i]->GetType() == kVertex);
    }

    DeleteAllVertex(vec);
}

void TestEdge(uint32_t num)
{
    VertexManager vertex_manager;
    vector<Vertex *> src, dest;
    std::string src_name = "src_name";
    std::string dest_name = "dest_name";

    vertex_manager.GenerateVertexVector(&src, num, src_name, src_name.size());//id 从0到num-1
    vertex_manager.GenerateVertexVector(&dest, num, dest_name,//id 从num 到2*num-1
                                        dest_name.size());

    vector<Edge *> edges;
    vector<uint64_t> ranks;
    vector<pair<string, string>> vec;
    edges.reserve(num);
    ranks.reserve(num);
    vec.reserve(num);

    srand(clock());

    uint64_t MAX = 999999;
    for (uint32_t i = 0; i < num; ++i) {
        uint64_t rank = rand() % MAX;
        ranks.emplace_back(rank);
        edges[i] = new Edge(src[i], dest[i], 1, rank);
        edges[i]->GetSrcVertex()->Decode();
        edges[i]->GetDestVertex()->Decode();
        // edges[i]->Decode();
        vec.push_back({edges[i]->GetSrcVertex()->GetRep(),
                       edges[i]->GetDestVertex()->GetRep()});
    }

    for (uint32_t i = 0; i < num; ++i) {
        Vertex *src_vertex = edges[i]->GetSrcVertex();
        Vertex *dest_vertex = edges[i]->GetDestVertex();

        assert(src_vertex->GetType() == VertexOrEdgeType::kVertex);
        assert(src_vertex->GetNodeType() == src_name);
        assert(src_vertex->GetRep() == vec[i].first);
        assert(src_vertex->GetVertexId() == i);

        assert(dest_vertex->GetNodeType() == dest_name);
        assert(dest_vertex->GetType() == VertexOrEdgeType::kVertex);
        assert(dest_vertex->GetRep() == vec[i].second);
        assert(dest_vertex->GetVertexId() == i + num);

        assert(edges[i]->GetRank() == ranks[i]);
        assert(edges[i]->GetEdgeType() == 1);
        assert(edges[i]->GetPlaceHolder() == EdgePlaceHolder::kPlaceHolder);
    }
}
int main(int argc, char **argv)
{
    uint32_t num = static_cast<uint32_t>(atoi(argv[1]));
    // GenerateVertexVector(num, "NodeType");
    TestEdge(num);
}

