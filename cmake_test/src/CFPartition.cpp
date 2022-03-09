

#include "CFPartition.h"
#include "Edge.h"
#include "Vertex.h"

std::string CFPartition::kDefaultDBPath;

CFPartition::CFPartition(const rocksdb::Options &options) : db_options_(options)
{
    rocksdb::Status s =
        rocksdb::DB::Open(db_options_, CFPartition::kDefaultDBPath, &db_);
    assert(s.ok());
}

CFPartition::~CFPartition()
{
    for (auto &it : cf_id_map_to_handle_) {
        db_->DestroyColumnFamilyHandle(it);
    }

    delete db_;
}

rocksdb::Status CFPartition::CreateCF(
    uint64_t cf_num,
    const std::vector<rocksdb::ColumnFamilyOptions> &cf_options)
{
    rocksdb::Status s;
    cf_id_map_to_handle_.resize(cf_num + 1);
    for (uint64_t i = 0; i < cf_num; ++i) {
        rocksdb::ColumnFamilyHandle *cf = nullptr;
        std::string cf_name = CFPartition::kDefaultDBPath + std::to_string(i);

        s = db_->CreateColumnFamily(cf_options[i], cf_name, &cf);
        if (!s.ok()) return s;

        cf_id_to_name_[cf->GetID()] = cf_name;

        cf_id_map_to_handle_[cf->GetID()] = cf;

        cf_id_to_option_[cf->GetID()] = cf_options[i];
    }
    return s;
}
rocksdb::Status CFPartition::CreateCF(
    uint64_t cf_num, const rocksdb::ColumnFamilyOptions &cf_options,
    const std::vector<std::string> &node_type)
{
    assert(cf_num == static_cast<uint64_t>(node_type.size()));

    rocksdb::Status s;
    cf_id_map_to_handle_.resize(cf_num + 1);
    for (uint64_t i = 0; i < cf_num; ++i) {
        rocksdb::ColumnFamilyHandle *cf = nullptr;
        std::string cf_name = CFPartition::kDefaultDBPath + std::to_string(i);

        s = db_->CreateColumnFamily(cf_options, cf_name, &cf);
        if (!s.ok()) return s;

        cf_id_to_node_type_[cf->GetID()] = node_type[i];

        cf_id_to_name_[cf->GetID()] = cf_name;

        cf_id_map_to_handle_[cf->GetID()] = cf;

        cf_id_to_option_[cf->GetID()] = cf_options;
    }
    return s;
}

rocksdb::Status CFPartition::Put(const rocksdb::WriteOptions &options,
                                 uint64_t cf_id, const rocksdb::Slice &key,
                                 const rocksdb::Slice &value)
{
    return db_->Put(options, cf_id_map_to_handle_[cf_id], key, value);
}
rocksdb::Status CFPartition::GetVertex(const rocksdb::ReadOptions &options,
                                       uint64_t cf_id, uint64_t vertex_id,
                                       std::string *value)
{
    //一个partition对应一个CF node_type相同
    //

    Vertex vertex(vertex_id, cf_id_to_node_type_[cf_id]);
    return db_->Get(options, cf_id_map_to_handle_[cf_id], vertex.GetRep(),
                    value);
}

rocksdb::Status CFPartition::GetOutEdgeByVertex(
    const rocksdb::ReadOptions &options, uint64_t cf_id, uint64_t src_vertex_id,
    uint64_t dest_vertex_id, std::string *value)
{
    return GetByEdgeType(options, cf_id, src_vertex_id, dest_vertex_id, value,
                         true);
}
rocksdb::Status CFPartition::GetInEdgeByVertex(
    const rocksdb::ReadOptions &options, uint64_t cf_id, uint64_t src_vertex_id,
    uint64_t dest_vertex_id, std::string *value)
{
    return GetByEdgeType(options, cf_id, src_vertex_id, dest_vertex_id, value,
                         false);
}
rocksdb::Status CFPartition::GetByEdgeType(
    const rocksdb::ReadOptions &options, uint64_t cf_id, uint64_t src_vertex_id,
    uint64_t dest_vertex_id, std::string *value, bool get_out_edge)
{
    std::string node_type = cf_id_to_node_type_[cf_id];

    int32_t num = (get_out_edge == true) ? 1 : -1;

    Edge edge(src_vertex_id, dest_vertex_id, node_type, num, 8);
    return db_->Get(options, cf_id_map_to_handle_[cf_id], edge.GetRep(), value);
}

rocksdb::Iterator *CFPartition::NewIterator(const rocksdb::ReadOptions &options,
                                            uint64_t cf_id)
{
    return db_->NewIterator(options, cf_id_map_to_handle_[cf_id]);
}
rocksdb::Status CFPartition::DropColumnFamily(uint64_t cf_id)
{
    return db_->DropColumnFamily(cf_id_map_to_handle_[cf_id]);
}
rocksdb::Status CFPartition::DestoryColumnFamilyHandle(uint64_t cf_id)
{
    return db_->DestroyColumnFamilyHandle(cf_id_map_to_handle_[cf_id]);
}
rocksdb::Status CFPartition::Flush(const rocksdb::FlushOptions &flush_options,
                                   uint64_t cf_id)
{
    return db_->Flush(flush_options, cf_id_map_to_handle_[cf_id]);
}
rocksdb::Status CFPartition::FlushAllCF(
    const rocksdb::FlushOptions &flush_options)
{
    rocksdb::Status s;
    for (auto &it : cf_id_map_to_handle_)
        if (!(s = db_->Flush(flush_options, it)).ok()) return s;

    return s;
}

