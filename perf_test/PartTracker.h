#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <memory>

#include "Generator.h"
#include "Measurement.h"
#include "Options.h"

class PartTracker {
 public:
  PartTracker(const Options& options);

  ~PartTracker();

  void start();

  void setMaxPartNum(int32_t maxPartNum);

  void setGrowthNum(int32_t growthNum);

 private:
  void doTask(const std::unique_ptr<Generator>& generator, const Options& options);

  void RemoveDir(const Options& options);

  void Remove(const std::string& prefixPath, const std::string& suffixPath);

  std::string getPath(const std::string& suffixPath, const std::string& prefixPath);

  void adjustOptions(Options* options);

  Options options_;
  Measurement measurement_;

  int32_t maxPartNum_;
  int32_t growthNum_;

  static constexpr int32_t CPUCore = {112};
};
