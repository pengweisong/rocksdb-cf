
#include "Manager.h"
#include "CFPartition.h"
#include "DBPartition.h"
#include "Edge.h"
#include "Options.h"
#include "Vertex.h"

Manager::Manager(const Options &options) : options_(options) {}

Manager::~Manager()
{
    for (auto &vertex : all_vertex_) delete vertex;

    for (auto &edge : in_edge_map_to_out_edge_) {
        delete edge.first;
        delete edge.second;
    }
}

uint64_t Manager::GetVertexNum() const
{
    return static_cast<uint64_t>(all_vertex_.size());
}

Options &Manager::GetOptions() { return options_; }

Vertex *Manager::GetVertex(uint64_t index) const
{
    assert(index >= 0 && index < static_cast<uint64_t>(all_vertex_.size()));
    return all_vertex_[index];
}
Vertex *Manager::GetVertex(uint64_t index)
{
    assert(index >= 0 && index < static_cast<uint64_t>(all_vertex_.size()));
    return all_vertex_[index];
}

std::string Manager::GetValue(uint64_t index) const
{
    assert(index >= 0 && index < static_cast<uint64_t>(values_.size()));
    return values_[index];
}

uint64_t Manager::GetValueNum() const
{
    return static_cast<uint64_t>(values_.size());
}

uint64_t Manager::GetEdgeNum() const
{
    return static_cast<uint64_t>(in_edge_map_to_out_edge_.size());
}

rocksdb::Status Manager::CreateDBPartition(
    const rocksdb::Options &options, const std::vector<std::string> &node_type)
{
    db_partition_.reset(new DBPartition());
    return db_partition_->CreateDB(options_.partition_num_, options, node_type);
}
rocksdb::Status Manager::CreateCFPartition(
    const rocksdb::Options &options,
    const rocksdb::ColumnFamilyOptions &cf_options,
    const std::vector<std::string> &node_type)
{
    cf_partition_.reset(new CFPartition(options));
    return cf_partition_->CreateCF(options_.partition_num_, cf_options,
                                   node_type);
}
void Manager::CreateVertexManager()
{
    vertex_manager_.reset(new VertexManager());
}

void Manager::CreateVertexByCFId(uint64_t cf_id)
{
    std::string node_type = cf_partition_->GetNodeType(cf_id);
    vertex_manager_->CreateVertex(&node_type_to_vertex_[node_type], options_,
                                  node_type);
}

void Manager::CreateVertexVector()
{
    vertex_manager_->CreateVertexVector(&all_vertex_, options_.vertex_num_,
                                        options_.vertex_options_);
}

void Manager::CreateEdgeByVertex(Vertex *vertex, int build_all)
{
    switch (build_all) {
        case 0: {
            vertex_manager_->ThisVertexAsSrcVertex(vertex, &all_vertex_,
                                                   &values_, options_,
                                                   &in_edge_map_to_out_edge_);
            break;
        }
        case 1: {
            vertex_manager_->ThisVertexAsDestVertex(vertex, &all_vertex_,
                                                    &values_, options_,
                                                    &in_edge_map_to_out_edge_);
            break;
        }
        default: {
            vertex_manager_->ThisVertexAsSrcVertex(vertex, &all_vertex_,
                                                   &values_, options_,
                                                   &in_edge_map_to_out_edge_);
            vertex_manager_->ThisVertexAsDestVertex(vertex, &all_vertex_,
                                                    &values_, options_,
                                                    &in_edge_map_to_out_edge_);

            break;
        }
    }
}

void Manager::CreateValues(uint64_t generate_num)
{
    for (uint64_t i = 0; i < generate_num; ++i)
        values_.emplace_back(options_.generate_values_(options_.value_size_));
}

std::vector<Edge *> Manager::GetWrittenOutEdge()
{
    std::vector<Edge *> result;

    result.reserve(in_edge_map_to_out_edge_.size());
    for (auto &it : in_edge_map_to_out_edge_) {
        result.emplace_back(it.second);
    }
    return result;
}

std::vector<Edge *> Manager::GetWrittenInEdge()
{
    std::vector<Edge *> result;

    result.reserve(in_edge_map_to_out_edge_.size());
    for (auto &it : in_edge_map_to_out_edge_) {
        result.emplace_back(it.first);
    }
    return result;
}

std::unordered_set<Vertex *> Manager::GetSortVertex() const
{
    return vertex_manager_->GetSortVertex();
}

rocksdb::Iterator *Manager::NewIteratorByCFId(
    const rocksdb::ReadOptions &options, uint64_t cf_id)
{
    return cf_partition_->NewIterator(options, cf_id);
}
rocksdb::Iterator *Manager::NewIteratorByDBId(
    const rocksdb::ReadOptions &options, uint64_t db_id)
{
    return db_partition_->NewIterator(options, db_id);
}

rocksdb::Status Manager::ScanDBPartition(const rocksdb::ReadOptions &options,
                                         uint64_t db_id)
{
    rocksdb::Iterator *iter = NewIteratorByDBId(options, db_id);

    rocksdb::Status s;

    for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
        std::string temp = std::move(iter->key().ToString());
        std::string value;
        switch (temp[0]) {
            case VertexOrEdgeType::kVertex: {
                Vertex vertex(db_partition_->GetNodeType(db_id));
                vertex.Encode(temp);
                vertex.Decode();

                s = db_partition_->GetVertex(options, db_id,
                                             vertex.GetVertexId(), &value);
                if (!s.ok()) return s;

                break;
            }
            case VertexOrEdgeType::kEdge: {
                Edge edge;
                edge.Encode(temp);
                edge.Decode();

                bool is_out_edge = (edge.GetEdgeType() > 0) ? true : false;

                if (is_out_edge) {
                    s = db_partition_->GetOutEdgeByVertex(
                        options, db_id, edge.GetSrcVertex()->GetVertexId(),
                        edge.GetDestVertex()->GetVertexId(), &value);
                }
                else {
                    s = db_partition_->GetInEdgeByVertex(
                        options, db_id, edge.GetSrcVertex()->GetVertexId(),
                        edge.GetDestVertex()->GetVertexId(), &value);
                }
                if (!s.ok()) return s;

                break;
            }
            default: {
            }
        }
        assert(value == iter->value().ToString());
        value.clear();
    }
    return s;
}

rocksdb::Status Manager::WriteToCF(const rocksdb::WriteOptions &write_options,
                                   uint64_t cf_id, uint64_t start, uint64_t end,
                                   bool is_vertex, bool is_out_edge,
                                   bool save_edge)
{
    assert(start >= 0 && end < static_cast<uint64_t>(values_.size()));

    std::string node_type = cf_partition_->GetNodeType(cf_id);

    assert(end < static_cast<uint64_t>(node_type_to_vertex_[node_type].size()));

    for (uint64_t i = start; i <= end; ++i) {
        if (is_vertex) {
            rocksdb::Status s = cf_partition_->Put(
                write_options, cf_id,
                node_type_to_vertex_[node_type][i]->GetRep(), values_[i]);
            if (!s.ok()) return s;
        }
        else {
            Edge *edge = (is_out_edge) ? in_edge_map_to_out_edge_[i].first
                                       : in_edge_map_to_out_edge_[i].second;

            rocksdb::Status s = cf_partition_->Put(write_options, cf_id,
                                                   edge->GetRep(), values_[i]);

            if (save_edge) edge_key_map_to_value_[edge->GetRep()] = values_[i];

            if (!s.ok()) return s;
        }
    }
    return rocksdb::Status::OK();
}
//写入在[start,end]区间点所有点 相同的写入配置
rocksdb::Status Manager::WriteVertexToCF(
    const rocksdb::WriteOptions &write_options, uint64_t cf_id, uint64_t start,
    uint64_t end)
{
    return WriteToCF(write_options, cf_id, start, end, true);
}

rocksdb::Status Manager::WriteOutEdgeToCF(
    const rocksdb::WriteOptions &write_options, uint64_t cf_id, uint64_t start,
    uint64_t end)
{
    return WriteToCF(write_options, cf_id, start, end, false, true);
}
rocksdb::Status Manager::WriteInEdgeToCF(
    const rocksdb::WriteOptions &write_options, uint64_t cf_id, uint64_t start,
    uint64_t end)
{
    return WriteToCF(write_options, cf_id, start, end, false, false);
}
rocksdb::Status Manager::GetVertexByCF(const rocksdb::ReadOptions &read_options,
                                       uint64_t cf_id, uint64_t start,
                                       uint64_t end, bool need_verify)
{
    assert(start >= 0 && end < all_vertex_.size());
    assert(end < static_cast<uint64_t>(values_.size()));

    std::vector<std::string> result;
    result.reserve(end - start + 1);
    std::string value;

    for (uint64_t i = start; i <= end; ++i) {
        rocksdb::Status s =
            cf_partition_->GetVertex(read_options, cf_id, i, &value);
        result.emplace_back(value);
        if (!s.ok()) return s;
    }
    if (need_verify) VerifyCorrect(start, end, result);
    return rocksdb::Status::OK();
}

void Manager::VerifyCorrect(uint64_t src_vertex_id, uint64_t dest_vertex_id,
                            const std::string &src_value, bool get_out_edge)
{
    Vertex src_vertex(src_vertex_id, options_.vertex_options_);
    Vertex dest_vertex(dest_vertex_id, options_.vertex_options_);

    int32_t edge_type = (get_out_edge) ? 1 : -1;

    Edge edge(&src_vertex, &dest_vertex, edge_type, 8);

    assert(edge_key_map_to_value_[edge.GetRep()] == src_value);
}
rocksdb::Status Manager::GetOutEdgeOrInEdgeByCF(
    const rocksdb::ReadOptions &read_options, uint64_t cf_id,
    uint64_t src_vertex_id, uint64_t dest_vertex_id, bool get_out_edge,
    bool need_verify)
{
    assert(src_vertex_id >= 0 && src_vertex_id < all_vertex_.size());
    assert(dest_vertex_id >= 0 && dest_vertex_id < all_vertex_.size());

    rocksdb::Status s;
    std::string value;
    if (get_out_edge)
        s = cf_partition_->GetOutEdgeByVertex(
            read_options, cf_id, src_vertex_id, dest_vertex_id, &value);
    else
        s = cf_partition_->GetInEdgeByVertex(read_options, cf_id, src_vertex_id,
                                             dest_vertex_id, &value);

    if (need_verify)
        VerifyCorrect(src_vertex_id, dest_vertex_id, value, get_out_edge);
    return s;
}
rocksdb::Status Manager::GetOutEdgeByCF(
    const rocksdb::ReadOptions &read_options, uint64_t cf_id,
    uint64_t src_vertex_id, uint64_t dest_vertex_id, bool need_verify)
{
    return GetOutEdgeOrInEdgeByCF(read_options, cf_id, src_vertex_id,
                                  dest_vertex_id, true, need_verify);
}
rocksdb::Status Manager::GetInEdgeByCF(const rocksdb::ReadOptions &read_options,
                                       uint64_t cf_id, uint64_t src_vertex_id,
                                       uint64_t dest_vertex_id,
                                       bool need_verify)
{
    return GetOutEdgeOrInEdgeByCF(read_options, cf_id, src_vertex_id,
                                  dest_vertex_id, false, need_verify);
}

rocksdb::Status Manager::GetVertexByDB(const rocksdb::ReadOptions &read_options,
                                       uint64_t db_id, uint64_t start,
                                       uint64_t end, bool need_verify)
{
    assert(start >= 0 && end < all_vertex_.size());
    assert(end < static_cast<uint64_t>(values_.size()));

    std::vector<std::string> result;
    result.reserve(end - start + 1);
    std::string value;

    for (uint64_t i = start; i <= end; ++i) {
        rocksdb::Status s =
            db_partition_->GetVertex(read_options, db_id, i, &value);
        result.emplace_back(value);
        if (!s.ok()) return s;
    }
    if (need_verify) VerifyCorrect(start, end, result);
    return rocksdb::Status::OK();
}

rocksdb::Status Manager::GetOutEdgeOrInEdgeByDB(
    const rocksdb::ReadOptions &read_options, uint64_t db_id,
    uint64_t src_vertex_id, uint64_t dest_vertex_id, bool get_out_edge,
    bool need_verify)
{
    assert(src_vertex_id >= 0 && src_vertex_id < all_vertex_.size());
    assert(dest_vertex_id >= 0 && dest_vertex_id < all_vertex_.size());

    rocksdb::Status s;
    std::string value;
    if (get_out_edge)
        s = db_partition_->GetOutEdgeByVertex(
            read_options, db_id, src_vertex_id, dest_vertex_id, &value);
    else
        s = db_partition_->GetInEdgeByVertex(read_options, db_id, src_vertex_id,
                                             dest_vertex_id, &value);

    if (need_verify)
        VerifyCorrect(src_vertex_id, dest_vertex_id, value, get_out_edge);
    return s;
}

rocksdb::Status Manager::GetOutEdgeByDB(
    const rocksdb::ReadOptions &read_options, uint64_t db_id,
    uint64_t src_vertex_id, uint64_t dest_vertex_id, bool need_verify)
{
    return GetOutEdgeOrInEdgeByDB(read_options, db_id, src_vertex_id,
                                  dest_vertex_id, true, need_verify);
}

VertexManager::VertexManager() : next_vertex_id_(0) {}
VertexManager::~VertexManager() {}

void VertexManager::CreateVertex(std::vector<Vertex *> *vertex_vector,
                                 const Options &options,
                                 const std::string &node_type)
{
    for (uint64_t i = 0; i < options.vertex_num_; ++i) {
        vertex_vector->emplace_back(CreateVertex(node_type));
    }
}

Vertex *VertexManager::CreateVertex(const std::string &node_type)
{
    Vertex *vertex = new Vertex(next_vertex_id_, node_type);
    all_vertex_.insert(vertex);
    ++next_vertex_id_;
    return vertex;
}

void VertexManager::ThisVertexAsSrcVertex(
    Vertex *vertex, std::vector<Vertex *> *all_vertex,
    std::vector<std::string> *values, const Options &options,
    std::vector<std::pair<Edge *, Edge *>> *edge)
{
    const VertexOptions &vertex_options = options.vertex_options_;

    srand(clock());
    //该点作为src点
    uint64_t as_src_vertex_num =
        (rand() % (vertex_options.as_src_vertex_max_ -
                   vertex_options.as_src_vertex_min_ + 1) +
         vertex_options.as_src_vertex_min_);
    //首先利用生成的所有点建立链接
    //不够的话重新建立点

    std::cout << "Generate " << as_src_vertex_num << " Edge "
              << "\n";

    uint64_t num = 0;
    for (auto &it : all_vertex_) {
        if (it != vertex && !vertex->IsInSrcVertexSet(it) &&
            !it->IsInDestVertexSet(vertex)) {
            Edge *e1 = new Edge(vertex, it, 1, 8);
            Edge *e2 = new Edge(it, vertex, -1, 8);

            vertex->InsertDestVertexSet(it);
            it->InsertSrcVertexSet(vertex);
            edge->push_back({e1, e2});
            ++num;
        }
        if (num == as_src_vertex_num) break;
    }
    uint64_t need_regenerate_num = 0;
    if (num < as_src_vertex_num) need_regenerate_num = as_src_vertex_num - num;

    for (uint64_t i = 0; i < need_regenerate_num; ++i) {
        Vertex *new_vertex = CreateVertex(vertex_options);

        Edge *e1 = new Edge(vertex, new_vertex, 1, 8);
        Edge *e2 = new Edge(new_vertex, vertex, -1, 8);
        vertex->InsertDestVertexSet(new_vertex);

        new_vertex->InsertSrcVertexSet(vertex);
        edge->push_back({e1, e2});

        all_vertex->emplace_back(new_vertex);

        values->emplace_back(options.generate_values_(options.value_size_));
    }
}
void VertexManager::ThisVertexAsDestVertex(
    Vertex *vertex, std::vector<Vertex *> *all_vertex,
    std::vector<std::string> *values, const Options &options,
    std::vector<std::pair<Edge *, Edge *>> *edge)
{
    const VertexOptions &vertex_options = options.vertex_options_;
    srand(clock());
    //该点作为dest点
    uint64_t as_dest_vertex_num =
        (rand() % (vertex_options.as_dest_vertex_max_ -
                   vertex_options.as_dest_vertex_min_ + 1) +
         vertex_options.as_src_vertex_min_);
    //首先利用生成的所有点建立链接
    //不够的话重新建立点

    uint64_t num = 0;
    for (auto &it : all_vertex_) {
        if (it != vertex && !vertex->IsInDestVertexSet(it) &&
            !it->IsInSrcVertexSet(vertex)) {
            Edge *e1 = new Edge(vertex, it, -1, 8);
            Edge *e2 = new Edge(it, vertex, 1, 8);
            edge->push_back({e2, e1});
            vertex->InsertSrcVertexSet(it);
            it->InsertDestVertexSet(vertex);
            ++num;
        }
        if (num == as_dest_vertex_num) break;
    }
    uint64_t need_regenerate_num = 0;
    if (num < as_dest_vertex_num) {
        need_regenerate_num = as_dest_vertex_num - num;
    }

    for (uint64_t i = 0; i < need_regenerate_num; ++i) {
        Vertex *new_vertex = CreateVertex(vertex_options);

        Edge *e1 = new Edge(vertex, new_vertex, -1, 8);
        Edge *e2 = new Edge(new_vertex, vertex, 1, 8);
        vertex->InsertSrcVertexSet(new_vertex);
        new_vertex->InsertDestVertexSet(vertex);
        edge->push_back({e2, e1});
        all_vertex->emplace_back(new_vertex);

        values->emplace_back(options.generate_values_(options.value_size_));
    }
}

rocksdb::Status Manager::WriteToDB(const rocksdb::WriteOptions &write_options,
                                   uint64_t db_id, uint64_t start, uint64_t end,
                                   bool is_vertex, bool is_out_edge,
                                   bool save_edge)
{
    assert(start >= 0 && end < all_vertex_.size());
    assert(end < static_cast<uint64_t>(values_.size()));

    for (uint64_t i = start; i <= end; ++i) {
        if (is_vertex) {
            rocksdb::Status s = db_partition_->Put(
                write_options, db_id, all_vertex_[i]->GetRep(), values_[i]);
            if (!s.ok()) return s;
        }
        else {
            Edge *edge = (is_out_edge) ? in_edge_map_to_out_edge_[i].first
                                       : in_edge_map_to_out_edge_[i].second;

            rocksdb::Status s = db_partition_->Put(write_options, db_id,
                                                   edge->GetRep(), values_[i]);

            if (save_edge) edge_key_map_to_value_[edge->GetRep()] = values_[i];

            if (!s.ok()) return s;
        }
    }
    return rocksdb::Status::OK();
}
//写入在[start,end]区间点所有点 相同的写入配置
rocksdb::Status Manager::WriteVertexToDB(
    const rocksdb::WriteOptions &write_options, uint64_t db_id, uint64_t start,
    uint64_t end)
{
    return WriteToDB(write_options, db_id, start, end, true);
}

rocksdb::Status Manager::WriteOutEdgeToDB(
    const rocksdb::WriteOptions &write_options, uint64_t db_id, uint64_t start,
    uint64_t end)
{
    return WriteToDB(write_options, db_id, start, end, false, true);
}
rocksdb::Status Manager::WriteInEdgeToDB(
    const rocksdb::WriteOptions &write_options, uint64_t db_id, uint64_t start,
    uint64_t end)
{
    return WriteToDB(write_options, db_id, start, end, false, false);
}
void Manager::VerifyCorrect(uint64_t start, uint64_t end,
                            const std::vector<std::string> &result)
{
    for (uint64_t i = start; i <= start; ++i) {
        assert(result[i] == values_[i]);
    }
}

