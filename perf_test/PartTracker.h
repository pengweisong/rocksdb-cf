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

  void Remove(const std::string& suffixPath, const std::string& prefixPath);

  std::string getPath(const std::string& suffixPath, const std::string& prefixPath);

  Options options_;
  int32_t maxPartNum_;
  int32_t growthNum_;

  Measurement measurement_;
};
