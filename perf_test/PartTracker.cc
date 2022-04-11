#include "PartTracker.h"

#include <assert.h>

#include <future>
#include <thread>

PartTracker::PartTracker(const Options& options)
    : options_(options), maxPartNum_(0), growthNum_(1) {}

PartTracker::~PartTracker() = default;

void PartTracker::start() {
  Options options = options_;

  while (options.partNum <= maxPartNum_) {
    std::unique_ptr<Generator> generator(new Generator(options));

    doTask(generator, options);

    RemoveDir(options);
    options.partNum += growthNum_;
  }
}

void PartTracker::doTask(const std::unique_ptr<Generator>& generator, const Options& options) {
  std::cout << "PartitionNum " << options.partNum << "\n";

  std::vector<std::thread> threads;
  int32_t threadNum = options.partNum * options.threadRatio;
  std::vector<std::promise<int>> promises(threadNum);
  std::vector<std::future<int>> futures;

  threads.reserve(threadNum);
  futures.reserve(threadNum);

  Measurement tempMeasure;
  tempMeasure.start();

  for (int32_t i = 0; i < options.partNum; ++i) {
    for (int32_t j = 0; j < options.threadRatio; ++j) {
      futures.emplace_back(promises[options.threadRatio * i + j].get_future());
      threads.emplace_back(std::thread(&Generator::startThisThread,
                                       generator.get(),
                                       i,
                                       std::ref(promises[options.threadRatio * i + j])));
    }
  }

  for (int32_t i = 0; i < threadNum; ++i) {
    measurement_.addRequestNum(futures[i].get());
  }

  tempMeasure.stop();
  tempMeasure.calculateTime();
  tempMeasure.setRequestType(RequestType::OP_AddVertex);
  tempMeasure.showTime();
  std::cout << "==============================\n";

  measurement_.addTime(options.duration * 1000000);
  measurement_.setThreadNum(options.partNum * options.threadRatio);
  measurement_.setRequestType(RequestType::OP_AddVertex);
  measurement_.showTime();
  measurement_.reset();

  for (auto& thread : threads) {
    thread.join();
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
    path += std::move(std::string("/"));
  }
  return path;
}

void PartTracker::Remove(const std::string& suffixPath, const std::string& prefixPath) {
  if (suffixPath == ".." || suffixPath == ".") return;

  std::string path = getPath(suffixPath, prefixPath);

  struct stat buf;

  if (::stat(path.data(), &buf) == -1) {
    fprintf(stderr, "%s %s %s", path.data(), "stat error ", ::strerror(errno));
    exit(-1);
  }

  if (S_ISDIR(buf.st_mode)) {
    DIR* dir = nullptr;

    if ((dir = ::opendir(path.data())) == nullptr) {
      fprintf(stderr, "%s %s %s", path.data(), "opendir error ", ::strerror(errno));
      exit(-1);
    }

    struct dirent* d = nullptr;

    while ((d = ::readdir(dir)) != nullptr) {
      Remove(d->d_name, path);
    }

    if (::rmdir(path.data()) == -1) {
      fprintf(stderr, "%s %s %s", path.data(), "rmdir error ", ::strerror(errno));
      exit(-1);
    }

  } else if (S_ISREG(buf.st_mode)) {
    if (::unlink(path.data()) == -1) {
      fprintf(stderr, "%s %s %s", path.data(), "unlink error ", ::strerror(errno));
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
