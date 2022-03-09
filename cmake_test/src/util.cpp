

#include "util.h"
#include "generate_vertex_edge.h"

namespace util {

rocksdb::Status WriteToCF(const rocksdb::WriteOptions &write_options,
                          Manager* manager, uint64_t cf_id,
                          uint64_t start, uint64_t end, bool is_vertex,
                          bool is_out_edge, bool save_edge)
{
    assert(start >= 0 && end < manager->all_vertex_.size());
    assert(end < static_cast<uint64_t>(manager->values_.size()));

    for (uint64_t i = start; i <= end; ++i) {
        if (is_vertex) {
            rocksdb::Status s = manager->cf_partition_->Put(
                write_options, cf_id, options.all_vertex_[i]->GetRep(),
                manager->values_[i]);
            if (!s.ok()) return s;
        }
        else {
            Edge *edge =
                (is_out_edge) ? manager->in_edge_map_to_out_edge_[i].first
                              : manager->in_edge_map_to_out_edge_[i].second;

            rocksdb::Status s = manager->cf_partition_->Put(
                write_options, cf_id, edge->GetRep(), options.values_[i]);

            if (save_edge)
                manager->edge_key_map_to_value_[edge->GetRep()] =
                    manager->values_[i];

            if (!s.ok()) return s;
        }
    }
    return rocksdb::Status::OK();
}
//写入在[start,end]区间点所有点 相同的写入配置
rocksdb::Status WriteVertexToCF(const rocksdb::WriteOptions &write_options,
                                Manager* manager,
                                uint64_t cf_id, uint64_t start, uint64_t end)
{
    return WriteToCF(write_options, options, cf_id, start, end, true);
}

rocksdb::Status WriteOutEdgeToCF(const rocksdb::WriteOptions &write_options,
                                 Manager* manager,
                                 uint64_t cf_id, uint64_t start, uint64_t end)
{
    return WriteToCF(write_options, options, cf_id, start, end, false, true);
}
rocksdb::Status WriteInEdgeToCF(const rocksdb::WriteOptions &write_options,
                                Manager* manager,
                                uint64_t cf_id, uint64_t start, uint64_t end)
{
    return WriteToCF(write_options, options, cf_id, start, end, false, false);
}

rocksdb::Status WriteToDB(const rocksdb::WriteOptions &write_options,
                          Manager* manager, uint64_t db_id,
                          uint64_t start, uint64_t end, bool is_vertex,
                          bool is_out_edge, bool save_edge)
{
    assert(start >= 0 && end < manager->all_vertex_.size());
    assert(end < static_cast<uint64_t>(manager->values_.size()));

    for (uint64_t i = start; i <= end; ++i) {
        if (is_vertex) {
            rocksdb::Status s = manager->db_partition_->Put(
                write_options, db_id, options.all_vertex_[i]->GetRep(),
                manager->values_[i]);
            if (!s.ok()) return s;
        }
        else {
            Edge *edge =
                (is_out_edge) ? manager->in_edge_map_to_out_edge_[i].first
                              : manager->in_edge_map_to_out_edge_[i].second;

            rocksdb::Status s = manager->db_partition_->Put(
                write_options, db_id, edge->GetRep(), options.values_[i]);

            if (save_edge)
                manager->edge_key_map_to_value_[edge->GetRep()] =
                    manager->values_[i];

            if (!s.ok()) return s;
        }
    }
    return rocksdb::Status::OK();
}
//写入在[start,end]区间点所有点 相同的写入配置
rocksdb::Status WriteVertexToDB(const rocksdb::WriteOptions &write_options,
                                Manager* manager,
                                uint64_t db_id, uint64_t start, uint64_t end)
{
    return WriteToDB(write_options, options, db_id, start, end, true);
}

rocksdb::Status WriteOutEdgeToDB(const rocksdb::WriteOptions &write_options,
                                 Manager* manager,
                                 uint64_t db_id, uint64_t start, uint64_t end)
{
    return WriteToDB(write_options, options, db_id, start, end, false, true);
}
rocksdb::Status WriteInEdgeToDB(const rocksdb::WriteOptions &write_options,
                                Manager* manager,
                                uint64_t db_id, uint64_t start, uint64_t end)
{
    return WriteToDB(write_options, options, db_id, start, end, false, false);
}

void VerifyCorrect(uint64_t start, uint64_t end,
                   Manager* manager,
                   const std::vector<std::string> &result)
{
    for (uint64_t i = start; i <= start; ++i) {
        assert(result[i] == manager->values_[i]);
    }
}

rocksdb::Status GetVertexByCF(const rocksdb::ReadOptions &read_options,
                              Manager* manager, uint64_t cf_id,
                              uint64_t start, uint64_t end, bool need_verify)
{
    assert(start >= 0 && end < manager->all_vertex_.size());
    assert(end < static_cast<uint64_t>(manager->values_.size()));

    std::vector<std::string> result;
    result.reserve(end - start + 1);
    std::string value;

    for (uint64_t i = start; i <= end; ++i) {
        rocksdb::Status s =
            manager->cf_partition_->GetVertex(read_options, cf_id, i, &value);
        result.emplace_back(value);
        if (!s.ok()) return s;
    }
    if (need_verify) VerifyCorrect(start, end, manager, result);
    return rocksdb::Status::OK();
}

void VerifyCorrect(uint64_t src_vertex_id, uint64_t dest_vertex_id,
                   Manager* manager,
                   const std::string &src_value, bool get_out_edge)
{
    Vertex src_vertex(src_vertex_id,
                               manager.manager_options_.vertex_options_);
    Vertex dest_vertex(dest_vertex_id,
                                manager.manager_options_.vertex_options_);

    int32_t edge_type = (get_out_edge) ? 1 : -1;

    Edge edge(&src_vertex, &dest_vertex, edge_type, 8);

    assert(manager->edge_key_map_to_value_[edge.GetRep()] == src_value);
}
rocksdb::Status GetOutEdgeOrInEdgeByCF(const rocksdb::ReadOptions &read_options,
                                       Manager* manager,
                                       uint64_t cf_id, uint64_t src_vertex_id,
                                       uint64_t dest_vertex_id,
                                       bool get_out_edge, bool need_verify)
{
    assert(src_vertex_id >= 0 && src_vertex_id < manager->all_vertex_.size());
    assert(dest_vertex_id >= 0 && dest_vertex_id < manager->all_vertex_.size());

    rocksdb::Status s;
    std::string value;
    if (get_out_edge)
        s = manager->cf_partition_->GetOutEdgeByVertex(
            read_options, cf_id, src_vertex_id, dest_vertex_id, &value);
    else
        s = manager->cf_partition_->GetInEdgeByVertex(
            read_options, cf_id, src_vertex_id, dest_vertex_id, &value);

    if (need_verify)
        VerifyCorrect(src_vertex_id, dest_vertex_id, manager, value,
                      get_out_edge);
    return s;
}
rocksdb::Status GetOutEdgeByCF(const rocksdb::ReadOptions &read_options,
                               Manager* manager, uint64_t cf_id,
                               uint64_t src_vertex_id, uint64_t dest_vertex_id,
                               bool need_verify)
{
    return GetOutEdgeOrInEdgeByCF(read_options, options, cf_id, src_vertex_id,
                                  dest_vertex_id, true, need_verify);
}
rocksdb::Status GetInEdgeByCF(const rocksdb::ReadOptions &read_options,
                              Manager* manager, uint64_t cf_id,
                              uint64_t src_vertex_id, uint64_t dest_vertex_id,
                              bool need_verify)
{
    return GetOutEdgeOrInEdgeByCF(read_options, options, cf_id, src_vertex_id,
                                  dest_vertex_id, false, need_verify);
}
rocksdb::Status GetOutEdgeOrInEdgeByDB(const rocksdb::ReadOptions &read_options,
                                       Manager* manager,
                                       uint64_t db_id, uint64_t src_vertex_id,
                                       uint64_t dest_vertex_id,
                                       bool get_out_edge, bool need_verify)
{
    assert(src_vertex_id >= 0 && src_vertex_id < manager->all_vertex_.size());
    assert(dest_vertex_id >= 0 && dest_vertex_id < manager->all_vertex_.size());

    rocksdb::Status s;
    std::string value;
    if (get_out_edge)
        s = manager->db_partition_->GetOutEdgeByVertex(
            read_options, db_id, src_vertex_id, dest_vertex_id, &value);
    else
        s = manager->db_partition_->GetInEdgeByVertex(
            read_options, db_id, src_vertex_id, dest_vertex_id, &value);

    if (need_verify)
        VerifyCorrect(src_vertex_id, dest_vertex_id, manager, value,
                      get_out_edge);
    return s;
}
rocksdb::Status GetOutEdgeByDB(const rocksdb::ReadOptions &read_options,
                               Manager* manager, uint64_t db_id,
                               uint64_t src_vertex_id, uint64_t dest_vertex_id,
                               bool need_verify)
{
    return GetOutEdgeOrInEdgeByDB(read_options, options, db_id, src_vertex_id,
                                  dest_vertex_id, true, need_verify);
}
rocksdb::Status GetInEdgeByDB(const rocksdb::ReadOptions &read_options,
                              Manager* manager, uint64_t db_id,
                              uint64_t src_vertex_id, uint64_t dest_vertex_id,
                              bool need_verify)
{
    return GetOutEdgeOrInEdgeByDB(read_options, options, db_id, src_vertex_id,
                                  dest_vertex_id, false, need_verify);
}
// build_all =0 以该点为出点 在之前所有点选取一个范围的所有点作为入点
// build_all =1 //以该点为入点
// default //该点既当入点又当出点
void BuildVertexAndEdgeConnection(Manager* manager,
                                  Vertex *vertex, int build_all)
{
    switch (build_all) {
        case 1: {
            manager.vertex_manager_->ThisVertexAsSrcVertex(
                vertex, manager->in_edge_map_to_out_edge_);
            break;
        }
        case 2: {
            manager.vertex_manager_->ThisVertexAsDestVertex(
                vertex, manager->in_edge_map_to_out_edge_);
            break;
        }
        default: {
            manager.vertex_manager_->ThisVertexAsSrcVertex(
                vertex, manager->in_edge_map_to_out_edge_);
            manager.vertex_manager_->ThisVertexAsDestVertex(
                vertex, manager->in_edge_map_to_out_edge_);

            break;
        }
    }
}

}  // namespace util

