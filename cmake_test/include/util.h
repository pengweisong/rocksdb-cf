
#ifndef __UTIL_H_
#define __UTIL_H_

#include "Manager.h"
#include "rocksdb/db.h"

class Vertex;

rocksdb::Status WriteToCF(const rocksdb::WriteOptions& write_options,
                          Manager* manager, uint64_t cf_id, uint64_t start,
                          uint64_t end, bool is_vertex = true,
                          bool is_out_edge = true, bool save_edge = true);

rocksdb::Status WriteVertexToCF(const rocksdb::WriteOptions& write_options,
                                Manager* manager, uint64_t cf_id,
                                uint64_t start, uint64_t end);
rocksdb::Status WriteOutEdgeToCF(const rocksdb::WriteOptions& write_options,
                                 Manager* manager, uint64_t cf_id,
                                 uint64_t start, uint64_t end);

rocksdb::Status WriteInEdgeToCF(const rocksdb::WriteOptions& write_options,
                                Manager* manager, uint64_t cf_id,
                                uint64_t start, uint64_t end);

rocksdb::Status WriteToDB(const rocksdb::WriteOptions& write_options,
                          Manager* manager, uint64_t db_id, uint64_t start,
                          uint64_t end, bool is_vertex = true,
                          bool is_out_edge = true, bool save_edge = true);

rocksdb::Status WriteVertexToDB(const rocksdb::WriteOptions& write_options,
                                Manager* manager, uint64_t db_id,
                                uint64_t start, uint64_t end);
rocksdb::Status WriteOutEdgeToDB(const rocksdb::WriteOptions& write_options,
                                 Manager* manager, uint64_t db_id,
                                 uint64_t start, uint64_t end);
rocksdb::Status WriteInEdgeToDB(const rocksdb::WriteOptions& write_options,
                                Manager* manager, uint64_t db_id,
                                uint64_t start, uint64_t end);
void VerifyCorrect(uint64_t start, uint64_t end, Manager* manager,
                   const std::vector<std::string>& result);
void VerifyCorrect(uint64_t src_vertex_id, uint64_t dest_vertex_id,
                   Manager* manager, const std::string& src_value,
                   bool get_out_edge = true);
rocksdb::Status GetVertexByCF(const rocksdb::ReadOptions& read_options,
                              Manager* manager, uint64_t cf_id, uint64_t start,
                              uint64_t end, bool need_verify = false);
rocksdb::Status GetOutEdgeOrInEdgeByCF(const rocksdb::ReadOptions& read_options,
                                       Manager* manager, uint64_t cf_id,
                                       uint64_t src_vertex_id,
                                       uint64_t dest_vertex_id,
                                       bool get_out_edge = true,
                                       bool need_verify = true);

rocksdb::Status GetOutEdgeByCF(const rocksdb::ReadOptions& read_options,
                               Manager* manager, uint64_t cf_id,
                               uint64_t src_vertex_id, uint64_t dest_vertex_id,
                               bool need_verify = true);
rocksdb::Status GetInEdgeByCF(const rocksdb::ReadOptions& write_options,
                              Manager* manager, uint64_t cf_id,
                              uint64_t src_vertex_id, uint64_t dest_vertex_id,
                              bool need_verify = true);
rocksdb::Status GetOutEdgeOrInEdgeByDB(const rocksdb::ReadOptions& read_options,
                                       Manager* manager, uint64_t db_id,
                                       uint64_t src_vertex_id,
                                       uint64_t dest_vertex_id,
                                       bool get_out_edge = true);
rocksdb::Status GetOutEdgeByDB(const rocksdb::ReadOptions& read_options,
                               Manager* manager, uint64_t db_id,
                               uint64_t src_vertex_id, uint64_t dest_vertex_id,
                               bool need_verify = true);
rocksdb::Status GetInEdgeByDB(const rocksdb::ReadOptions& read_options,
                              Manager* manager, uint64_t db_id,
                              uint64_t src_vertex_id, uint64_t dest_vertex_id,
                              bool need_verify = true);
void BuildVertexAndEdgeConnection(Manager* manager, Vertex* vertex,
                                  int build_all);
