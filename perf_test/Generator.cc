#include "Generator.h"

std::string Generator::kdefaultValue = "sakdjjiJKHSAKJ219012I0SLK";

Generator::Generator(const Options& options) : options_(options), space_(new Space(options_)) {}

Generator::~Generator() {}

void Generator::addVertex(PartId partId) {
  for (int32_t i = 0; i < options_.vertexNum; ++i) {
    VertexKey key;
    if (options_.randomKey == false) {
      key.type = i;
      key.id = i;
      key.version = i;
    } else {
      srand(clock());
      key.type = rand() % options_.vertexNum;
      key.id = rand() % options_.vertexNum;
      key.version = rand() % options_.vertexNum;
    }
    space_->addVertex(partId, key, makeRandomString(options_.valueSize));
  }
}

void Generator::removeVertex(PartId partId) {
  for (int32_t i = 0; i < options_.vertexNum; ++i) {
    VertexKey key;
    if (options_.randomKey == false) {
      key.type = i;
      key.id = i;
      key.version = i;
    } else {
      srand(clock());
      key.type = rand() % options_.vertexNum;
      key.id = rand() % options_.vertexNum;
      key.version = rand() % options_.vertexNum;
    }
    space_->removeVertex(partId, key);
  }
}

void Generator::getVertex(PartId partId, std::vector<std::string>* values) {
  values->reserve(options_.vertexNum);
  for (int32_t i = 0; i < options_.vertexNum; ++i) {
    if (options_.randomKey == false) {
      VertexKey key;
      key.type = i;
      key.id = i;
      key.version = i;
      std::string value;
      space_->getVertex(partId, key, &value);
      values->emplace_back(value);
    }
  }
}

void Generator::addEdge(PartId partId) {
  for (int32_t i = 0; i < options_.edgeNum; ++i) {
    EdgeKey key;
    if (options_.randomKey == false) {
      key.type = i;
      key.src = i;
      key.dst = options_.edgeNum - i;
      key.rank = i;
      key.version = i;
    } else {
      key.type = rand() % options_.edgeNum;
      key.src = rand() % options_.edgeNum;
      key.dst = rand() % options_.edgeNum;
      key.rank = rand() % options_.edgeNum;
      key.version = rand() % options_.edgeNum;
      if (key.src == key.dst) ++key.dst;
    }
    space_->addEdge(partId, key, makeRandomString(options_.valueSize));
  }
}

void Generator::removeEdge(PartId partId) {
  for (int32_t i = 0; i < options_.edgeNum; ++i) {
    EdgeKey key;
    if (options_.randomKey == false) {
      key.type = i;
      key.src = i;
      key.dst = options_.edgeNum - i;
      key.rank = i;
      key.version = i;
    } else {
      key.type = rand() % options_.edgeNum;
      key.src = rand() % options_.edgeNum;
      key.dst = rand() % options_.edgeNum;
      key.rank = rand() % options_.edgeNum;
      key.version = rand() % options_.edgeNum;
      if (key.src == key.dst) ++key.dst;
    }
    space_->removeEdge(partId, key);
  }
}

void Generator::getEdge(PartId partId, std::vector<std::string>* values) {
  values->reserve(options_.edgeNum);
  for (int32_t i = 0; i < options_.edgeNum; ++i) {
    if (options_.randomKey = false) {
      EdgeKey key;
      key.type = i;
      key.src = i;
      key.dst = options_.edgeNum - i;
      key.rank = i;
      key.version = i;
      std::string value;
      space_->getEdge(partId, key, &value);
      values->emplace_back(value);
    }
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