#include "Generator.h"

std::string Generator::kdefaultValue = "";

Generator::Generator(const Options& options)
    : options_(options),
      space_(new Space(options_)),
      stop_(false),
      finishThread_(0),
      threadPtr_(nullptr) {}

Generator::~Generator() {
  if (!stop_) {
    if (threadPtr_ != nullptr) {
      threadPtr_->join();
    }
    stop_ = true;
  }
}

void Generator::doTask(PartId partId, std::promise<std::pair<uint64_t, int64_t>>& requestNum) {
  if (options_.useBatch) {
    writeEdgeOrVertex(partId, requestNum);
  } else {
    addEdgeOrVertex(partId, requestNum);
  }

  ++finishThread_;

  if (CPUCore == finishThread_) {
    stop_ = true;
  }
}

bool Generator::finished(Measurement* measurement,
                         const Threshold& threshold,
                         int32_t maxNum,
                         int32_t num,
                         int32_t duration) {
  if (threshold == Threshold::kOPNumber) {
    if (num == maxNum) {
      return true;
    } else {
      return false;
    }
  } else if (threshold == Threshold::kOPTime) {
    Measurement lastTime;
    lastTime.start();
    if (std::chrono::duration_cast<std::chrono::seconds>(lastTime.getStartTime() -
                                                         measurement->getStartTime())
            .count() >= duration) {
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}

void Generator::addEdgeOrVertex(PartId partId,
                                std::promise<std::pair<uint64_t, int64_t>>& pair,
                                bool addVertex) {
  int i = 0;
  RequestType type = (addVertex) ? RequestType::OP_AddVertex : RequestType::OP_AddEdge;
  int32_t maxNum = 0;
  int32_t duration = 0;

  if (options_.threShold == Threshold::kOPNumber) {
    maxNum = (addVertex) ? options_.vertexNum : options_.edgeNum;

  } else if (options_.threShold == Threshold::kOPTime) {
    duration = options_.duration;
  }

  Measurement measurement;
  measurement.setRequestType(type);
  measurement.start();

  while (!stop_) {
    if (addVertex) {
      space_->addVertex(partId, getVertexKey(i), kdefaultValue);
    } else {
      space_->addEdge(partId, getEdgeKey(i), kdefaultValue);
    }
    ++i;
    if (finished(&measurement, options_.threShold, maxNum, i, duration)) {
      break;
    }
  }

  measurement.stop();
  measurement.setRequestNum(i);
  measurement.calculateTime();

  pair.set_value({measurement.getRequestNum(), measurement.getTotalTime()});
}

void Generator::writeEdgeOrVertex(PartId partId,
                                  std::promise<std::pair<uint64_t, int64_t>>& pair,
                                  bool addVertex) {
  int i = 0;
  RequestType type = (addVertex) ? RequestType::OP_AddVertex : RequestType::OP_AddEdge;
  int32_t maxNum = 0;
  int32_t duration = 0;

  if (options_.threShold == Threshold::kOPNumber) {
    maxNum = (addVertex) ? options_.numOfBatch : options_.numOfBatch;
    maxNum *= options_.entriesPerBatch;

  } else if (options_.threShold == Threshold::kOPTime) {
    duration = options_.duration;
  }

  rocksdb::ColumnFamilyHandle* cf = space_->getPart(partId)->getEngine()->getCF();
  Measurement measurement;

  measurement.setRequestType(type);
  measurement.start();

  bool exit = false;

  while (!stop_) {
    rocksdb::WriteBatch batch;
    int32_t j = 0;
    for (j = 0; j < options_.entriesPerBatch; ++j) {
      batch.Put(cf, getVertexKey(i).toString(), kdefaultValue);
      if (finished(&measurement, options_.threShold, maxNum, i, duration)) {
        exit = true;
        break;
      }
    }
    if (exit) break;
    space_->write(partId, &batch);
    i += options_.entriesPerBatch;
  }

  measurement.stop();
  measurement.setRequestNum(i);
  measurement.calculateTime();

  pair.set_value({measurement.getRequestNum(), measurement.getTotalTime()});
}

VertexKey Generator::getVertexKey(int32_t num) {
  if (options_.randomKey == false) {
    return VertexKey(num, num, num);
  } else {
    srand(clock());
    return VertexKey(
        rand() % options_.vertexNum, rand() % options_.vertexNum, rand() % options_.vertexNum);
  }
}

EdgeKey Generator::getEdgeKey(int32_t num) {
  if (options_.randomKey == false) {
    return EdgeKey(num, num, options_.edgeNum - num, num, num);
  } else {
    return EdgeKey(rand() % options_.edgeNum,
                   rand() % options_.edgeNum,
                   rand() % options_.edgeNum,
                   rand() % options_.edgeNum,
                   rand() % options_.edgeNum);
  }
}

void Generator::start(PartId partId, std::promise<std::pair<uint64_t, int64_t>>& requestNum) {
  std::thread t(&Generator::doTask, this, partId, std::ref(requestNum));
  threadPtr_ = &t;
}

void Generator::startThisThread(PartId partId,
                                std::promise<std::pair<uint64_t, int64_t>>& requestNum) {
  doTask(partId, requestNum);
}

void Generator::stop() {
  stop_ = true;
}

void Generator::wait() {}

void Generator::makeRandomString(int32_t size) {
  srand(clock());

  static const std::string temp = "asfoijqowjeo1290480";

  for (int32_t i = 0; i < size; ++i) {
    int32_t index = rand() % temp.size();
    kdefaultValue.push_back(temp[index]);
  }
}
