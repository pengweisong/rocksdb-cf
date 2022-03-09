
#include "DBPartition.h"
#include "Edge.h"
#include "Vertex.h"

std::string DBPartition::kDefaultDBPreixPath;

DBPartition::~DBPartition()
{
    for (auto &it : id_map_to_db_) {
        delete it;
    }
}

rocksdb::Status DBPartition::CreateDB(uint64_t db_num,
                                      const rocksdb::Options &options,
                                      const std::vector<std::string> &node_type)
{
    assert(static_cast<uint64_t>(node_type.size()) == db_num);

    rocksdb::Status s;
    id_map_to_db_.resize(db_num);
    for (uint64_t i = 0; i < db_num; ++i) {
        rocksdb::DB *db = nullptr;
        std::string db_name =
            DBPartition::kDefaultDBPreixPath + std::to_string(i);
        s = rocksdb::DB::Open(options, db_name, &db);
        if (!s.ok()) return s;

        id_map_to_node_type_[i] = node_type[i];
        id_map_to_db_[i] = db;
        id_map_to_name_[i] = db_name;
    }

    return s;
}
rocksdb::Status DBPartition::CreateDB(
    uint64_t db_num, const std::vector<rocksdb::Options> &options)
{
    rocksdb::Status s;
    id_map_to_db_.resize(db_num);
    for (uint64_t i = 0; i < db_num; ++i) {
        rocksdb::DB *db = nullptr;
        std::string db_name =
            DBPartition::kDefaultDBPreixPath + std::to_string(i);
        s = rocksdb::DB::Open(options[i], db_name, &db);
        if (!s.ok()) return s;

        id_map_to_db_[i] = db;
        id_map_to_name_[i] = db_name;
    }

    return s;
}
rocksdb::Status DBPartition::Put(const rocksdb::WriteOptions &options,
                                 uint64_t db_id, const rocksdb::Slice &key,
                                 const rocksdb::Slice &value)
{
    return id_map_to_db_[db_id]->Put(options, key, value);
}
rocksdb::Status DBPartition::GetVertex(const rocksdb::ReadOptions &options,
                                       uint64_t db_id, uint64_t vertex_id,
                                       std::string *value)
{
    Vertex vertex(vertex_id, id_map_to_node_type_[db_id]);
    return id_map_to_db_[db_id]->Get(options, vertex.GetRep(), value);
}

rocksdb::Status DBPartition::GetOutEdgeByVertex(
    const rocksdb::ReadOptions &options, uint64_t db_id, uint64_t src_vertex_id,
    uint64_t dest_vertex_id, std::string *value)
{
    return GetByEdgeType(options, db_id, src_vertex_id, dest_vertex_id, value,
                         true);
}
rocksdb::Status DBPartition::GetInEdgeByVertex(
    const rocksdb::ReadOptions &options, uint64_t db_id, uint64_t src_vertex_id,
    uint64_t dest_vertex_id, std::string *value)
{
    return GetByEdgeType(options, db_id, src_vertex_id, dest_vertex_id, value,
                         false);
}
rocksdb::Status DBPartition::GetByEdgeType(
    const rocksdb::ReadOptions &options, uint64_t db_id, uint64_t src_vertex_id,
    uint64_t dest_vertex_id, std::string *value, bool get_out_edge)
{
    std::string node_type = id_map_to_node_type_[db_id];

    int32_t num = (get_out_edge == true) ? 1 : -1;

    Edge edge(src_vertex_id, dest_vertex_id, node_type, num, 8);

    return id_map_to_db_[db_id]->Get(options, edge.GetRep(), value);
}
rocksdb::Iterator *DBPartition::NewIterator(const rocksdb::ReadOptions &options,
                                            uint64_t db_id)
{
    return id_map_to_db_[db_id]->NewIterator(options);
}

