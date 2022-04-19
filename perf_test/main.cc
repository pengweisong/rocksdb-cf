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
#include "rocksdb/table.h"

static const std::string cfPartition = "/data/CF";
static const std::string dbPartition = "/data";  // rocksdb创建级联目录有坑
static const int edgeNum = 1000;
static const int vertexNum = 10000;
static const int partNum = 1000;
static const int maxPartNum = 1000;
static const int valueSize = 50;
static const int growthNum = 10;

void TestCF(PartId partId, int32_t initPartNum) {
  int num = 1;

  while (num <= 2) {
    Options options;
    options.useCf = true;
    options.randomKey = false;
    options.threShold = Threshold::kOPTime;
    if (num > 1) {
      std::cout << "Not Use Batch\n";
      options.useBatch = false;
    } else {
      std::cout << "Use Batch\n";
      options.useBatch = true;
    }
    options.duration = 1800;
    options.numOfBatch = 1000;
    options.entriesPerBatch = 300;
    options.dataPath = cfPartition;
    options.valueSize = valueSize;
    options.partNum = initPartNum;

    std::unique_ptr<PartTracker> partTracker(new PartTracker(options));

    partTracker->setMaxPartNum(100);
    partTracker->setGrowthNum(10);

    partTracker->start();

    num++;
  }
}

void TestDB(PartId partId, int32_t initPartNum) {
  int num = 1;

  while (num <= 2) {
    Options options;
    options.useCf = false;
    options.randomKey = false;
    options.threShold = Threshold::kOPTime;
    options.duration = 1800;
    if (num > 1) {
      std::cout << "Not Use Batch\n";
      options.useBatch = false;
    } else {
      std::cout << "Use Batch\n";
      options.useBatch = true;
    }
    options.entriesPerBatch = 300;
    options.numOfBatch = 1000;
    options.dataPath = dbPartition;
    options.valueSize = valueSize;
    options.partNum = initPartNum;

    std::unique_ptr<PartTracker> partTracker(new PartTracker(options));

    partTracker->setMaxPartNum(100);
    partTracker->setGrowthNum(10);

    partTracker->start();

    num++;
  }
}

void Test(PartId partId, int32_t initPartNum) {
  std::cout << "============Test CF============\n";
  TestCF(partId, initPartNum);
  std::cout << "============Test DB============\n";
  TestDB(partId, initPartNum);
}

int main(int argc, char **argv) {
  int32_t initPartNum = 1;
  Test(0, initPartNum);
  return 0;
}
