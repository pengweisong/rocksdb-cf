#include <thread>

#include "Generator.h"

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

void Generator::removeVertex(PartId partId) {
  for (int32_t i = 0; i < options_.vertexNum; ++i) {
    space_->removeVertex(partId, getVertexKey(i));
  }
}

void Generator::getVertex(PartId partId, std::vector<std::string>* values) {
  values->reserve(options_.vertexNum);
  for (int32_t i = 0; i < options_.vertexNum; ++i) {
    std::string value;
    space_->getVertex(partId, getVertexKey(i), &value);
    values->emplace_back(value);
  }
}

void Generator::removeEdge(PartId partId) {
  for (int32_t i = 0; i < options_.edgeNum; ++i) {
    space_->removeEdge(partId, getEdgeKey(i));
  }
}

void Generator::getEdge(PartId partId, std::vector<std::string>* values) {
  values->reserve(options_.edgeNum);
  for (int32_t i = 0; i < options_.edgeNum; ++i) {
    std::string value;
    space_->getEdge(partId, getEdgeKey(i), &value);
    values->emplace_back(value);
  }
}

void Generator::scanForNext(PartId partId) {
  rocksdb::Iterator* iter = space_->newIterator(partId);

  for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
  }
}

void Generator::scanForPrev(PartId partId) {
  rocksdb::Iterator* iter = space_->newIterator(partId);

  for (iter->SeekToLast(); iter->Valid(); iter->Prev()) {
  }
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