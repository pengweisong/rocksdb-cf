#include "Generator.h"

#include <thread>

std::string Generator::kdefaultValue = "sakdjjiJKHSAKJ219012I0SLK";

Generator::Generator(const Options& options)
    : options_(options), space_(new Space(options_)), stop_(false) {}

Generator::~Generator() {
  std::unique_lock<std::mutex> guard(mutex_);
  while (!stop_) cv_.wait(guard);
}

void Generator::doTask(PartId partId) {
  addEdgeOrVertex(partId);
  addEdgeOrVertex(partId, false);

  if (stop_ == false) {
    stop_ = true;
    cv_.notify_one();
  }
}

void Generator::addEdgeOrVertex(PartId partId, bool addVertex) {
  int i = 0;
  RequestType type = (addVertex) ? RequestType::OP_AddVertex : RequestType::OP_AddEdge;
  int32_t maxNum = (addVertex) ? options_.vertexNum : options_.edgeNum;

  measurement_.setRequestType(type);
  measurement_.start();

  while (!stop_) {
    if (addVertex) {
      space_->addVertex(partId, getVertexKey(i), makeRandomString(options_.valueSize));
    } else {
      space_->addEdge(partId, getEdgeKey(i), makeRandomString(options_.valueSize));
    }
    ++i;
    if (i == maxNum) {
      break;
    }
  }

  measurement_.stop();
  measurement_.setRequestNum(i);
  measurement_.showTime();
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

void Generator::start(PartId partId) {
  std::thread t(&Generator::doTask, this, partId);
  t.detach();
}

void Generator::stop() {
  stop_ = true;
  cv_.notify_one();
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
