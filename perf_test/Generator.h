#pragma once

#include <cstdint>
#include <memory>
#include <string>

class Space;

using PartId = int32_t;

struct GeneratorOptions {
  int32_t edgeNum;
  int32_t vertexNum;
  int32_t valueSize;
  int32_t partNum;
  bool useCf;
  std::string dataPath;
};

class Generator {
 public:
  Generator(const GeneratorOptions& options);

  ~Generator();

  void addVertex(PartId partId);

  void addEdge(PartId partId);

  void removeVertex(PartId partId);

  void removeEdge(PartId partId);

  static std::string makeRandomString(int32_t size);

 private:
  static std::string kdefaultValue;
  GeneratorOptions options_;
  std::unique_ptr<Space> space_;
};
