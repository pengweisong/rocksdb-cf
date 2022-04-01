#include <dirent.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "Generator.h"
#include "Options.h"

static const std::string cfPartition = "/tmp/CF";
static const std::string dbPartition = "/tmp/";  // Rocksdb cannot create a cascade directory
static const int edgeNum = 1000;
static const int vertexNum = 1000;
static const int partNum = 10;
static const int valueSize = 1000;

void TestCF(PartId partId, bool needWait) {
  Options options;
  options.useCf = true;
  options.edgeNum = edgeNum;
  options.vertexNum = vertexNum;
  options.dataPath = cfPartition;
  options.valueSize = valueSize;
  options.partNum = partNum;
  options.randomKey = false;

  std::unique_ptr<Generator> generator(new Generator(options));

  generator->start(partId);

  if (needWait) {
    generator->wait();
  } else {
    generator->stop();
  }
}
void TestDB(PartId partId, bool needWait) {
  Options options;
  options.useCf = false;
  options.edgeNum = edgeNum;
  options.vertexNum = vertexNum;
  options.dataPath = dbPartition;
  options.valueSize = valueSize;
  options.partNum = partNum;
  options.randomKey = false;

  std::unique_ptr<Generator> generator(new Generator(options));

  generator->start(partId);

  if (needWait) {
    generator->wait();
  } else {
    generator->stop();  // segmentation fault may occur
    // Because the Generator class is already destructed
    // Another thread is using a destructed member variable
  }
}
void Test(PartId partId, bool needWait) {
  TestCF(partId, needWait);

  TestDB(partId, needWait);
}

int main(int argc, char **argv) {
  bool needWait = true;
  if (argc > 1) needWait = false;
  Test(0, needWait);
  return 0;
}
