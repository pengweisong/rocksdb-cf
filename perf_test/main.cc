#include <dirent.h>

#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include "Parts.h"

static const std::string cfPartition = "/tmp/CF";
static const std::string dbPartition = "/tmp/";  // rocksdb创建级联目录有坑
static const int edgeNum = 1000;
static const int vertexNum = 1000;
static const int partNum = 10;
static const int valueSize = 1000;

void TestCF(PartId partId) {
  GeneratorOptions generatorOptions;
  generatorOptions.useCf = true;
  generatorOptions.edgeNum = edgeNum;
  generatorOptions.vertexNum = vertexNum;
  generatorOptions.dataPath = cfPartition;
  generatorOptions.valueSize = valueSize;
  generatorOptions.partNum = partNum;

  std::unique_ptr<Generator> generator(new Generator(generatorOptions));

  generator->addVertex(partId);
  generator->addEdge(partId);
}
void TestDB(PartId partId) {
  GeneratorOptions generatorOptions;
  generatorOptions.useCf = false;
  generatorOptions.edgeNum = edgeNum;
  generatorOptions.vertexNum = vertexNum;
  generatorOptions.dataPath = cfPartition;
  generatorOptions.valueSize = valueSize;
  generatorOptions.partNum = partNum;

  std::unique_ptr<Generator> generator(new Generator(generatorOptions));

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
