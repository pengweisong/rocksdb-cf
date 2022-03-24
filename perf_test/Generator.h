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

  void addEdge(PartId partId);

  void removeVertex(PartId partId);

  void removeEdge(PartId partId);

  static std::string makeRandomString(int32_t size);

 private:
  static std::string kdefaultValue;
  Options options_;
  std::unique_ptr<Space> space_;
};
