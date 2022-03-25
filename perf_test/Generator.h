#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <string>

#include "Measurement.h"
#include "Options.h"
#include "Parts.h"
#include "VertexEdge.h"

using PartId = int32_t;

class Generator {
 public:
  Generator(const Options& options);

  ~Generator();

  void removeVertex(PartId partId);

  void getVertex(PartId partId, std::vector<std::string>* value);

  void removeEdge(PartId partId);

  void getEdge(PartId partId, std::vector<std::string>* value);

  void scanForPrev(PartId partId);

  void scanForNext(PartId partId);

  static std::string makeRandomString(int32_t size);

  void startAddVertex(PartId partId);

  void startAddEdge(PartId partId);

  void waitAndStop();

  void stop();

  void start(PartId partId);

 private:
  void addVertex(PartId partId);

  void addEdge(PartId partId);

  void addEdgeOrVertex(PartId partId, bool addVertex = true);

  VertexKey getVertexKey(int32_t num);

  EdgeKey getEdgeKey(int32_t num);

  void running();

  static std::string kdefaultValue;
  Options options_;
  std::unique_ptr<Space> space_;

  std::atomic<bool> stop_;
  std::atomic<bool> needWait_;  //控制直接stop还是等待写入完成stop
  std::mutex mutex_;
  std::condition_variable cv_;

  Measurement measurement_;

  using threadTask = std::function<void()>;
  std::deque<threadTask> task_;  //任务队列
};
