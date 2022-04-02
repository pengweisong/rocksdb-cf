#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "Options.h"
#include "PartTracker.h"

static const std::string cfPartition = "/tmp/CF";
static const std::string dbPartition = "/tmp/";  // rocksdb创建级联目录有坑
static const int edgeNum = 1000;
static const int vertexNum = 10000;
static const int partNum = 1000;
static const int maxPartNum = 1000;
static const int valueSize = 1000;
static const int growthNum = 10;

void TestCF(PartId partId, int32_t initPartNum) {
  Options options;
  options.useCf = true;
  options.randomKey = false;
  options.threShold = Threshold::OP_Number;
  options.edgeNum = edgeNum;
  options.vertexNum = vertexNum;
  options.dataPath = cfPartition;
  options.valueSize = valueSize;
  options.partNum = initPartNum;
  options.threadNum = initPartNum;

  std::unique_ptr<PartTracker> partTracker(new PartTracker(options));

  partTracker->setMaxPartNum(partNum);
  partTracker->setGrowthNum(growthNum);

  partTracker->start();
}

void TestDB(PartId partId, int32_t initPartNum) {
  Options options;
  options.useCf = false;
  options.threShold = Threshold::OP_Number;
  options.edgeNum = edgeNum;
  options.vertexNum = vertexNum;
  options.dataPath = dbPartition;
  options.valueSize = valueSize;
  options.partNum = partNum;
  options.randomKey = false;

  std::unique_ptr<PartTracker> partTracker(new PartTracker(options));

  partTracker->setMaxPartNum(partNum);
  partTracker->setGrowthNum(growthNum);

  partTracker->start();
}
void Test(PartId partId, int32_t initPartNum) {
  TestCF(partId, initPartNum);

  TestDB(partId, initPartNum);
}

int main(int argc, char **argv) {
  int32_t initPartNum = partNum;
  if (argc > 1) {
    initPartNum = atoi(argv[1]);
  }
  TestCF(0, initPartNum);
  return 0;
}
