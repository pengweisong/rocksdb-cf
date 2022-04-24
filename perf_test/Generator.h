#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <thread>

#include "Keys.h"
#include "Measurement.h"
#include "Space.h"

using PartId = int32_t;

class Generator {
 public:
  Generator(const Options& options);

  ~Generator();

  void start(PartId partId, std::promise<std::pair<uint64_t, int64_t>>& pair);

  void startThisThread(PartId partId, std::promise<std::pair<uint64_t, int64_t>>& pair);

  void wait();

  void stop();

  static void makeRandomString(int32_t size);

 private:
  void addEdgeOrVertex(PartId partId,
                       std::promise<std::pair<uint64_t, int64_t>>& pair,
                       bool addVertex = true);

  void writeEdgeOrVertex(PartId partId,
                         std::promise<std::pair<uint64_t, int64_t>>& pair,
                         bool addVertex = true);

  VertexKey getVertexKey(int32_t num);

  EdgeKey getEdgeKey(int32_t num);

  void doTask(PartId partId, std::promise<std::pair<uint64_t, int64_t>>& pair);

  bool finished(Measurement* measurement,
                const Threshold& threshold,
                int32_t maxNum,
                int32_t num,
                int32_t duration);

  Options options_;

  std::unique_ptr<Space> space_;

  std::atomic<bool> stop_;
  std::atomic<int> finishThread_;
  std::thread* threadPtr_;

  static std::string kdefaultValue;

  static constexpr int32_t CPUCore = {112};
};
