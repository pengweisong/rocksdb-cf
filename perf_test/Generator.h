#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <string>

#include "Keys.h"
#include "Measurement.h"
#include "Options.h"
#include "Parts.h"

using PartId = int32_t;

#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <string>

#include "Keys.h"
#include "Measurement.h"
#include "Options.h"
#include "Space.h"

using PartId = int32_t;

class Generator {
 public:
  Generator(const Options& options);

  ~Generator();

  void start(PartId partId);

  void startThisThread(PartId partId);

  void wait();

  void stop();

 private:
  void addEdgeOrVertex(PartId partId, bool addVertex = true);

  VertexKey getVertexKey(int32_t num);

  EdgeKey getEdgeKey(int32_t num);

  void doTask(PartId partId);

  static std::string makeRandomString(int32_t size);

  bool finished(Measurement* measurement,
                const Threshold& threshold,
                int32_t maxNum,
                int32_t num,
                int32_t duration);

  Options options_;
  Measurement totalMeasurement_;

  std::unique_ptr<Space> space_;

  std::atomic<bool> stop_;
  std::mutex mutex_;
  std::condition_variable cv_;

  static std::string kdefaultValue;
};
