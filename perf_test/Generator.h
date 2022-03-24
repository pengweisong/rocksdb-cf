#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "Options.h"
#include "Parts.h"

using PartId = int32_t;

class Generator {
 public:
  Generator(const Options& options);

  ~Generator();

  void addVertex(PartId partId);

  void removeVertex(PartId partId);

  void getVertex(PartId partId, std::vector<std::string>* value);

  void addEdge(PartId partId);

  void removeEdge(PartId partId);

  void getEdge(PartId partId, std::vector<std::string>* value);

  void scanForPrev(PartId partId);

  void scanForNext(PartId partId);

  static std::string makeRandomString(int32_t size);

 private:
  static std::string kdefaultValue;
  Options options_;
  std::unique_ptr<Space> space_;
};
