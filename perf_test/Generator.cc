#include "Generator.h"

std::string Generator::kdefaultValue = "sakdjjiJKHSAKJ219012I0SLK";

Generator::Generator(const Options& options)
    : options_(options),
      space_(new Space(options_)),
      stop_(false),
      finishThread_(0),
      threadPtr_(nullptr) {}

Generator::~Generator() {
  if (!stop_) {
    if (threadPtr_ != nullptr) threadPtr_->join();
    stop_ = true;
  }
}

void Generator::doTask(PartId partId, std::promise<int>& requestNum) {
  addEdgeOrVertex(partId, requestNum);
  ++finishThread_;

  if (stop_ == false && options_.partNum * options_.threadRatio == finishThread_) {
    stop_ = true;
  }
}

bool Generator::finished(Measurement* measurement,
                         const Threshold& threshold,
                         int32_t maxNum,
                         int32_t num,
                         int32_t duration) {
  if (threshold == Threshold::OP_Number) {
    if (num == maxNum) {
      return true;
    } else {
      return false;
    }
  } else if (threshold == Threshold::OP_Time) {
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

void Generator::addEdgeOrVertex(PartId partId, std::promise<int>& requestNum, bool addVertex) {
  int i = 0;
  RequestType type = (addVertex) ? RequestType::OP_AddVertex : RequestType::OP_AddEdge;
  int32_t maxNum = 0;
  int32_t duration = 0;

  if (options_.threShold == Threshold::OP_Number) {
    maxNum = (addVertex) ? options_.vertexNum : options_.edgeNum;
  } else if (options_.threShold == Threshold::OP_Time) {
    duration = options_.duration;
  }

  Measurement measurement;
  measurement.setRequestType(type);
  measurement.start();

  while (!stop_) {
    if (addVertex) {
      space_->addVertex(partId, getVertexKey(i), makeRandomString(options_.valueSize));
    } else {
      space_->addEdge(partId, getEdgeKey(i), makeRandomString(options_.valueSize));
    }
    ++i;
    if (finished(&measurement, options_.threShold, maxNum, i, duration)) {
      break;
    }
  }

  measurement.stop();
  measurement.setRequestNum(i);
  measurement.calculateTime();

  requestNum.set_value(measurement.getRequestNum());
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

void Generator::start(PartId partId, std::promise<int>& requestNum) {
  std::thread t(&Generator::doTask, this, partId, std::ref(requestNum));
  threadPtr_ = &t;
}

void Generator::startThisThread(PartId partId, std::promise<int>& requestNum) {
  doTask(partId, requestNum);
}

void Generator::stop() {
  stop_ = true;
}

void Generator::wait() {}

std::string Generator::makeRandomString(int32_t size) {
  srand(clock());
  std::string result;
  result.reserve(size);
  for (int32_t i = 0; i < size; ++i) {
    int32_t index = rand() % kdefaultValue.size();
    result.push_back(kdefaultValue[index]);
  }
  return result;
}
