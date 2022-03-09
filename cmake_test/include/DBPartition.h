
#pragma once

#include <vector>

#include "rocksdb/db.h"
#include "rocksdb/options.h"

class DBPartition {
public:
    explicit DBPartition(){};
    ~DBPartition();

    rocksdb::Status CreateDB(uint64_t db_num, const rocksdb::Options &options,
                             const std::vector<std::string> &node_type);

    rocksdb::Status CreateDB(uint64_t db_num,
                             const std::vector<rocksdb::Options> &options);

    rocksdb::Status Put(const rocksdb::WriteOptions &options, uint64_t db_id,
                        const rocksdb::Slice &key, const rocksdb::Slice &value);

    std::string GetNodeType(uint64_t db_id)
    {
        return id_map_to_node_type_[db_id];
    }

    rocksdb::Status GetVertex(const rocksdb::ReadOptions &options,
                              uint64_t db_id, uint64_t vertex_id,
                              std::string *value);

    rocksdb::Status GetOutEdgeByVertex(const rocksdb::ReadOptions &options,
                                       uint64_t db_id, uint64_t src_vertex_id,
                                       uint64_t dest_vertex_id,
                                       std::string *value);
    rocksdb::Status GetInEdgeByVertex(const rocksdb::ReadOptions &options,
                                      uint64_t db_id, uint64_t src_vertex_id,
                                      uint64_t dest_vertex_id,
                                      std::string *value);

    size_t GetDBSize() const { return id_map_to_db_.size(); }

    rocksdb::Iterator *NewIterator(const rocksdb::ReadOptions &options,
                                   uint64_t db_id);

    std::string GetDBName(uint64_t db_id) { return id_map_to_name_[db_id]; }

    static void SetDefaultDBPreixPath(const std::string &name)
    {
        kDefaultDBPreixPath = name;
    }

private:
    rocksdb::Status GetByEdgeType(const rocksdb::ReadOptions &options,
                                  uint64_t db_id, uint64_t src_vertex_id,
                                  uint64_t dest_vertex_id, std::string *value,
                                  bool get_out_edge = true);

    std::vector<rocksdb::DB *> id_map_to_db_;

    std::vector<rocksdb::Options> options_;

    static std::string kDefaultDBPreixPath;

    std::unordered_map<uint64_t, std::string> id_map_to_name_;

    std::unordered_map<uint64_t, std::string> id_map_to_node_type_;
};

