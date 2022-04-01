#pragma once

#include <cstdint>
#include <string>

struct Options {
  int32_t edgeNum;
  int32_t vertexNum;
  int32_t valueSize;
  int32_t partNum;

  bool useCf;
  bool randomKey;

  std::string dataPath;
};
