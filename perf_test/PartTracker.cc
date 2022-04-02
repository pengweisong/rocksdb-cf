#include "PartTracker.h"

#include <assert.h>

#include <thread>

PartTracker::PartTracker(const Options& options)
    : options_(options), maxPartNum_(0), growthNum_(1) {}

PartTracker::~PartTracker() = default;

void PartTracker::start() {
  Options options = options_;

  while (options.partNum <= maxPartNum_) {
    std::unique_ptr<Generator> generator(new Generator(options));

    doTask(generator, options);

    options.partNum += growthNum_;
    options.threadNum += growthNum_;

    RemoveDir(options);
  }
}

void PartTracker::doTask(const std::unique_ptr<Generator>& generator, const Options& options) {
  std::cout << "PartitionNum " << options.partNum << "\n";

  for (int32_t i = 0; i < options.partNum; ++i) {
    std::thread taskThread(&Generator::startThisThread, generator.get(), i);
    taskThread.detach();
  }
}

void PartTracker::RemoveDir(const Options& options) {
  if (options.useCf) {
    Remove(options.dataPath, "");
  } else {
    for (int i = 0; i < options.partNum; ++i) {
      std::string name = options.dataPath + "/part-" + std::to_string(i);
      Remove(name, "");
    }
  }
}

std::string PartTracker::getPath(const std::string& suffixPath, const std::string& prefixPath) {
  std::string path;
  if (prefixPath.size()) {
    path = prefixPath;
    path += std::move(std::string("/"));
    path += suffixPath;
  } else {
    path = suffixPath;
  }
  return path;
}

void PartTracker::Remove(const std::string& suffixPath, const std::string& prefixPath) {
  if (suffixPath == ".." || suffixPath == ".") return;

  std::string path = getPath(suffixPath, prefixPath);

  struct stat buf;

  if (::stat(path.data(), &buf) == -1) {
    fprintf(stderr, "%s ", ::strerror(errno));
    exit(-1);
  }

  if (S_ISDIR(buf.st_mode)) {
    DIR* dir = nullptr;

    if ((dir = ::opendir(path.data())) == nullptr) {
      fprintf(stderr, "%s ", ::strerror(errno));
      exit(-1);
    }

    struct dirent* d = nullptr;

    while ((d = ::readdir(dir)) != nullptr) {
      Remove(d->d_name, suffixPath);
    }

    if (::rmdir(path.data()) == -1) {
      fprintf(stderr, "%s ", ::strerror(errno));
      exit(-1);
    }

  } else if (S_ISREG(buf.st_mode)) {
    if (::unlink(path.data()) == -1) {
      fprintf(stderr, "%s ", ::strerror(errno));
      exit(-1);
    }
  }
}

void PartTracker::setMaxPartNum(int32_t maxPartNum) {
  maxPartNum_ = maxPartNum;
}

void PartTracker::setGrowthNum(int32_t growthNum) {
  growthNum_ = growthNum;
}
