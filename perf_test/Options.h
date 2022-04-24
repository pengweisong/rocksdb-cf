#pragma once

#include <cstdint>
#include <string>

enum Threshold : char { kOPNumber, kOPTime };

struct Options {
  Threshold threShold;

  int32_t edgeNum;
  int32_t vertexNum;

  int32_t duration;

  int32_t valueSize;
  int32_t partNum;

  int32_t entriesPerBatch;
  int32_t numOfBatch;

  bool useBatch;
  bool useCf;
  bool randomKey;

  std::string dataPath;
};
