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

  void start(PartId partId, std::promise<int>& requestNum);

  void startThisThread(PartId partId, std::promise<int>& requestNum);

  void wait();

  void stop();

 private:
  void addEdgeOrVertex(PartId partId, std::promise<int>& requestNum, bool addVertex = true);

  VertexKey getVertexKey(int32_t num);

  EdgeKey getEdgeKey(int32_t num);

  void doTask(PartId partId, std::promise<int>& requestNum);

  static std::string makeRandomString(int32_t size);

  bool finished(Measurement* measurement,
                const Threshold& threshold,
                int32_t maxNum,
                int32_t num,
                int32_t duration);

  Options options_;

  std::unique_ptr<Space> space_;

  std::atomic<bool> stop_;

  std::thread* threadPtr_;
  std::atomic<int> finishThread_;

  static std::string kdefaultValue;
};
