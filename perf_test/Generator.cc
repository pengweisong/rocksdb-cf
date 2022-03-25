#include <thread>

#include "Generator.h"

std::string Generator::kdefaultValue = "sakdjjiJKHSAKJ219012I0SLK";

Generator::Generator(const Options& options)
    : options_(options), space_(new Space(options_)), stop_(false), needWait_(true) {
  std::thread t(&Generator::running, this);
  t.detach();
}

Generator::~Generator() {
  std::unique_lock<std::mutex> guard(mutex_);
  std::cout << "Generator dtor\n";
  while (!stop_ || needWait_) cv_.wait(guard);
}

void Generator::running() {
  std::unique_lock<std::mutex> guard(mutex_);
  while (!stop_) {
    threadTask task = nullptr;
    while (!stop_ && task_.empty()) {
      cv_.wait(guard);
    }
    if (stop_) return;
    task = std::move(task_.front());
    task_.pop_front();
    //  guard.unlock();
    std::cout << "running task\n";
    if (task != nullptr) task();
  }
}

void Generator::addEdgeOrVertex(PartId partId, bool addVertex) {
  int i = 0;
  RequestType type = (addVertex) ? RequestType::OP_AddVertex : RequestType::OP_AddEdge;
  int32_t maxNum = (addVertex) ? options_.vertexNum : options_.edgeNum;

  measurement_.setRequestType(type);
  measurement_.start();

  while (!stop_ || needWait_) {
    if (addVertex)
      space_->addVertex(partId, getVertexKey(i), makeRandomString(options_.valueSize));
    else
      space_->addEdge(partId, getEdgeKey(i), makeRandomString(options_.valueSize));
    ++i;
    if (i == maxNum) break;
  }
  measurement_.stop();
  measurement_.setRequestNum(i);
  measurement_.showTime();
  if (stop_ == false && task_.empty()) {  //没有任务
    stop_ = true;
    needWait_ = false;
    cv_.notify_one();
  }
}

void Generator::addVertex(PartId partId) {
  addEdgeOrVertex(partId);
}

void Generator::addEdge(PartId partId) {
  addEdgeOrVertex(partId, false);
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

void Generator::startAddVertex(PartId partId) {
  std::unique_lock<std::mutex> guard(mutex_);
  task_.emplace_back(std::bind(&Generator::addVertex, this, partId));
  cv_.notify_one();
}

void Generator::startAddEdge(PartId partId) {
  std::unique_lock<std::mutex> guard(mutex_);
  task_.emplace_back(std::bind(&Generator::addEdge, this, partId));
  cv_.notify_one();
}

void Generator::start(PartId partId) {
  std::unique_lock<std::mutex> guard(mutex_);
  task_.emplace_back(std::bind(&Generator::addVertex, this, partId));
  task_.emplace_back(std::bind(&Generator::addEdge, this, partId));
  cv_.notify_one();
}

void Generator::stop() {
  stop_ = true;
  needWait_ = false;
}

void Generator::waitAndStop() {
  stop_ = true;
}

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