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
static const std::string dbPartition = "/tmp/";  // rocksdb创建级联目录有坑
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
    //不加sleep有可能死锁 因为先调用Generator的析构函数
    //而running线程去拿主线程的锁
    std::this_thread::sleep_for(std::chrono::microseconds(10000));
    generator->waitAndStop();
  } else {
    generator->stop();
  }
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
}
void Test(PartId partId) {
  TestCF(partId, true);

  TestDB(partId);
}

int main(int argc, char **argv) {
  bool needWait = true;
  if (argc > 1) needWait = false;
  TestCF(0, needWait);
  // Test(0);
  return 0;
}
