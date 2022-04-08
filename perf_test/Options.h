#pragma once

#include <cstdint>
#include <string>

enum Threshold : char { OP_Number, OP_Time };

struct Options {
  Threshold threShold;

  int32_t edgeNum;
  int32_t vertexNum;

  int32_t duration;

  int32_t valueSize;
  int32_t partNum;

  int32_t threadRatio;  // Each part corresponds to several threads

  bool useCf;
  bool randomKey;

  std::string dataPath;
};
