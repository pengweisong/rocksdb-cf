
#ifndef __ONE_CF_MAP_ONE_PARTITION_H_
#define __ONE_CF_MAP_ONE_PARTITION_H_

#include "rocksdb/db.h"
#include "rocksdb/options.h"

#include <unordered_map>
namespace manager {

class Vertex;
class CFPartition {
public:
    explicit CFPartition(const rocksdb::Options &options);
    void CreateCF(uint64_t cf_num,
                  const std::vector<rocksdb::ColumnFamilyOptions> &cf_options);
    void CreateCF(uint64_t cf_num,
                  const rocksdb::ColumnFamilyOptions &cf_options);

    std::string GetCFName(uint64_t cf_id) { return cf_id_to_name_[cf_id]; }

    rocksdb::Status Put(const rocksdb::WriteOptions &options, uint64_t cf_id,
                        const rocksdb::Slice &key, const rocksdb::Slice &value);

    rocksdb::Status GetVertex(const rocksdb::ReadOptions &options,
                              uint64_t cf_id, uint64_t vertex_id,
                              std::string *value);

    //得到从src->dest的边 该边为出边
    rocksdb::Status GetOutEdgeByVertex(const rocksdb::ReadOptions &options,
                                       uint64_t cf_id, uint64_t src_vertex_id,
                                       uint64_t dest_vertex_id,
                                       std::string *value);
    rocksdb::Status GetInEdgeByVertex(const rocksdb::ReadOptions &options,
                                      uint64_t cf_id, uint64_t src_vertex_id,
                                      uint64_t dest_vertex_id,
                                      std::string *value);

    rocksdb::Status DropColumnFamily(uint64_t cf_id);
    rocksdb::Status DestoryColumnFamilyHandle(uint64_t cf_id);

    ~CFPartition();
    rocksdb::Iterator *NewIterator(const rocksdb::ReadOptions &options,
                                   uint64_t cf_id);

    rocksdb::Status Flush(const rocksdb::FlushOptions &flush_options,
                          uint64_t cf_id);
    rocksdb::Status FlushAllCF(const rocksdb::FlushOptions &flush_options);

    static void SetDefaultDBPath(const std::string &name)
    {
        kDefaultDBPath = name;
    }

private:
    rocksdb::Status GetByEdgeType(const rocksdb::ReadOptions &options,
                                  uint64_t cf_id, uint64_t src_vertex_id,
                                  uint64_t dest_vertex_id, std::string *value,
                                  bool get_out_edge = true);

    rocksdb::DB *db_;
    rocksdb::Options db_options_;

    static std::string kDefaultDBPath;

    std::vector<rocksdb::ColumnFamilyHandle *> cf_id_map_to_handle_;
    std::unordered_map<uint64_t, std::string> cf_id_to_name_;
    std::unordered_map<uint64_t, rocksdb::ColumnFamilyOptions> cf_id_to_option_;
};
};  // namespace manager

#endif
