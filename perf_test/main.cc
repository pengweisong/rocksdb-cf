#include <dirent.h>

#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include "Generator.h"
#include "Options.h"

static const std::string cfPartition = "/tmp/CF";
static const std::string dbPartition = "/tmp/";  // rocksdb创建级联目录有坑
static const int edgeNum = 1000;
static const int vertexNum = 1000;
static const int partNum = 10;
static const int valueSize = 1000;

void TestCF(PartId partId) {
  Options options;
  options.useCf = true;
  options.edgeNum = edgeNum;
  options.vertexNum = vertexNum;
  options.dataPath = cfPartition;
  options.valueSize = valueSize;
  options.partNum = partNum;
  options.randomKey = false;

  std::unique_ptr<Generator> generator(new Generator(options));

  generator->addVertex(partId);
  generator->addEdge(partId);
}
void TestDB(PartId partId) {
  Options options;
  options.useCf = false;
  options.edgeNum = edgeNum;
  options.vertexNum = vertexNum;
  options.dataPath = dbPartition;
  options.valueSize = valueSize;
  options.partNum = partNum;
  options.randomKey = false;

  std::unique_ptr<Generator> generator(new Generator(options));

  generator->addVertex(partId);
  generator->addEdge(partId);
}
void Test(PartId partId) {
  TestCF(partId);

  TestDB(partId);
}
int main() {
  Test(0);
  return 0;
}
