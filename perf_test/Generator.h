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

  void wait();

  void stop();

  void start(PartId partId);

 private:
  void addEdgeOrVertex(PartId partId, bool addVertex = true);

  VertexKey getVertexKey(int32_t num);

  EdgeKey getEdgeKey(int32_t num);

  void doTask(PartId partId);

  static std::string kdefaultValue;
  Options options_;
  std::unique_ptr<Space> space_;

  std::atomic<bool> stop_;

  std::mutex mutex_;
  std::condition_variable cv_;

  Measurement measurement_;
};
