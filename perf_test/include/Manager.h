
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Options.h"
#include "rocksdb/db.h"

class VertexManager;
class Vertex;
class Edge;
class DBPartition;
class CFPartition;

class Manager {
public:
    Manager(const Options &options);

    ~Manager();

    Manager(const Manager &) = delete;
    Manager &operator=(const Manager &) = delete;

    uint64_t GetVertexNum() const;
    Options &GetOptions();
    Vertex *GetVertex(uint64_t index) const;
    Vertex *GetVertex(uint64_t index);
    std::string GetValue(uint64_t index) const;
    uint64_t GetValueNum() const;
    uint64_t GetEdgeNum() const;
    std::vector<Edge *> GetWrittenOutEdge();
    std::vector<Edge *> GetWrittenInEdge();

    void CreateVertexByCFId(uint64_t cf_id);

    void CreateVertexManager();
    void CreateVertexVector();

    // build_all=0表示以该点建立出边
    // build_all=1 表示以该点建立入边
    // default既建立出边也建立入边
    void CreateEdgeByVertex(Vertex *vertex, int build_all = 2);

    void CreateValues(uint64_t generate_num);

    rocksdb::Status CreateDBPartition(
        const rocksdb::Options &options,
        const std::vector<std::string> &node_type);

    rocksdb::Status CreateCFPartition(
        const rocksdb::Options &options,
        const rocksdb::ColumnFamilyOptions &cf_options,
        const std::vector<std::string> &node_type);

    rocksdb::Iterator *NewIteratorByCFId(const rocksdb::ReadOptions &options,
                                         uint64_t cf_id);

    rocksdb::Iterator *NewIteratorByDBId(const rocksdb::ReadOptions &options,
                                         uint64_t db_id);

    rocksdb::Status ScanDBPartition(const rocksdb::ReadOptions &options,
                                    uint64_t db_id);
    // rocksdb::Status ScanCFPartition(const rocksdb::ReadOptions
    // &options,uint64_t cf_id);

    rocksdb::Status WriteVertexToCF(const rocksdb::WriteOptions &write_options,
                                    uint64_t cf_id, uint64_t start,
                                    uint64_t end);
    rocksdb::Status WriteOutEdgeToCF(const rocksdb::WriteOptions &write_options,
                                     uint64_t cf_id, uint64_t start,
                                     uint64_t end);

    rocksdb::Status WriteInEdgeToCF(const rocksdb::WriteOptions &write_options,
                                    uint64_t cf_id, uint64_t start,
                                    uint64_t end);
    rocksdb::Status GetVertexByCF(const rocksdb::ReadOptions &read_options,
                                  uint64_t cf_id, uint64_t start, uint64_t end,
                                  bool need_verify = true);

    rocksdb::Status GetOutEdgeByCF(const rocksdb::ReadOptions &read_options,
                                   uint64_t cf_id, uint64_t src_vertex_id,
                                   uint64_t dest_vertex_id,
                                   bool need_verify = true);
    rocksdb::Status GetInEdgeByCF(const rocksdb::ReadOptions &read_options,
                                  uint64_t cf_id, uint64_t src_vertex_id,
                                  uint64_t dest_vertex_id,
                                  bool need_verify = true);

    rocksdb::Status WriteVertexToDB(const rocksdb::WriteOptions &write_options,
                                    uint64_t db_id, uint64_t start,
                                    uint64_t end);
    rocksdb::Status WriteOutEdgeToDB(const rocksdb::WriteOptions &write_options,
                                     uint64_t db_id, uint64_t start,
                                     uint64_t end);
    rocksdb::Status WriteInEdgeToDB(const rocksdb::WriteOptions &write_options,
                                    uint64_t db_id, uint64_t start,
                                    uint64_t end);

    rocksdb::Status GetVertexByDB(const rocksdb::ReadOptions &read_options,
                                  uint64_t db_id, uint64_t start, uint64_t end,
                                  bool need_verify = true);

    rocksdb::Status GetOutEdgeByDB(const rocksdb::ReadOptions &read_options,
                                   uint64_t db_id, uint64_t src_vertex_id,
                                   uint64_t dest_vertex_id,
                                   bool need_verify = true);

    rocksdb::Status GetInEdgeByDB(const rocksdb::ReadOptions &read_options,
                                  uint64_t db_id, uint64_t src_vertex_id,
                                  uint64_t dest_vertex_id,
                                  bool need_verify = true);

private:
    void VerifyCorrect(uint64_t src_vertex_id, uint64_t dest_vertex_id,
                       const std::string &src_value, bool get_out_edge);

    void VerifyCorrect(uint64_t start, uint64_t end,
                       const std::vector<std::string> &result);

    rocksdb::Status WriteToCF(const rocksdb::WriteOptions &write_options,
                              uint64_t cf_id, uint64_t start, uint64_t end,
                              bool is_vertex = true, bool is_out_edge = true,
                              bool save_edge = true);
    rocksdb::Status WriteToDB(const rocksdb::WriteOptions &write_options,
                              uint64_t db_id, uint64_t start, uint64_t end,
                              bool is_vertex = true, bool is_out_edge = true,
                              bool save_edge = true);

    rocksdb::Status GetOutEdgeOrInEdgeByCF(
        const rocksdb::ReadOptions &read_options, uint64_t cf_id,
        uint64_t src_vertex_id, uint64_t dest_vertex_id,
        bool get_out_edge = true, bool need_verify = true);

    rocksdb::Status GetOutEdgeOrInEdgeByDB(
        const rocksdb::ReadOptions &read_options, uint64_t db_id,
        uint64_t src_vertex_id, uint64_t dest_vertex_id,
        bool get_out_edge = true, bool need_verify = true);

    std::unique_ptr<VertexManager> vertex_manager_;

    Options options_;

    std::vector<Vertex *> all_vertex_;

    std::unordered_map<std::string, std::vector<Vertex *>> node_type_to_vertex_;

    std::vector<std::string> values_;

    std::vector<std::pair<Edge *, Edge *>> in_edge_map_to_out_edge_;

    std::unordered_map<std::string, std::string> edge_key_map_to_value_;

    std::unique_ptr<CFPartition> cf_partition_;

    std::unique_ptr<DBPartition> db_partition_;
};

class VertexManager {
public:
    VertexManager();
    ~VertexManager();

    VertexManager(const VertexManager &) = delete;
    VertexManager(VertexManager &&) = delete;

    void CreateVertex(std::vector<Vertex *> *vertex_vector,
                      const Options &options, const std::string &node_type);

    void CreateVertex(const std::string &node_type);

    //该点作为入点 生成随机数量的边 多产生的点放在all_vertex中
    //随之对应的value存放在value中
    void ThisVertexAsSrcVertex(Vertex *vertex,
                               std::vector<Vertex *> *all_vertex,
                               std::vector<std::string> *values,
                               const Options &options,
                               std::vector<std::pair<Edge *, Edge *>> *edge);

    // edge前面返回的是src点 后方全是dest点
    void ThisVertexAsDestVertex(Vertex *vertex,
                                std::vector<Vertex *> *all_vertex,
                                std::vector<std::string> *values,
                                const Options &options,
                                std::vector<std::pair<Edge *, Edge *>> *edge);

private:
    std::unordered_set<Vertex *> all_vertex_;
    uint64_t next_vertex_id_;
};

