#include "Generator.h"
#include "Parts.h"

std::string Generator::kdefaultValue = "sakdjjiJKHSAKJ219012I0SLK";

Generator::Generator(const GeneratorOptions& options)
    : options_(options), space_(new Space(options_)) {}

Generator::~Generator() {}

void Generator::addVertex(PartId partId) {
  for (int32_t i = 0; i < options_.vertexNum; ++i) {
    VertexKey key;
    key.type = i;
    key.id = i;
    key.version = i;
    space_->addVertex(partId, key, makeRandomString(options_.valueSize));
  }
}
void Generator::addEdge(PartId partId) {
  for (int32_t i = 0; i < options_.edgeNum; ++i) {
    EdgeKey key;
    key.type = i;
    key.src = i;
    key.dst = options_.edgeNum - i;
    key.rank = i;
    key.version = i;
    space_->addEdge(partId, key, makeRandomString(options_.valueSize));
  }
}
void Generator::removeVertex(PartId partId) {
  for (int32_t i = 0; i < options_.vertexNum; ++i) {
    VertexKey key;
    key.type = i;
    key.id = i;
    key.version = i;
    space_->removeVertex(partId, key);
  }
}
void Generator::removeEdge(PartId partId) {
  for (int32_t i = 0; i < options_.edgeNum; ++i) {
    EdgeKey key;
    key.type = i;
    key.src = i;
    key.dst = options_.edgeNum - i;
    key.rank = i;
    key.version = i;
    space_->removeEdge(partId, key);
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