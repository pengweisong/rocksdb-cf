#include "PartTracker.h"

#include <assert.h>

#include <future>
#include <thread>

PartTracker::PartTracker(const Options& options)
    : options_(options), maxPartNum_(0), growthNum_(1) {}

PartTracker::~PartTracker() = default;

void PartTracker::start() {
  Options options = options_;

  Generator::makeRandomString(options.valueSize);

  while (options.partNum <= maxPartNum_) {
    std::unique_ptr<Generator> generator(new Generator(options));

    doTask(generator, options);

    RemoveDir(options);

    adjustOptions(&options);
  }
}

void PartTracker::adjustOptions(Options* options) {
  if (options->partNum >= 50) {
    options->partNum += 50;
  } else if (options->partNum >= 20) {
    options->partNum += 30;
  } else {
    if (options->partNum == 1) options->partNum--;
    options->partNum += growthNum_;
  }
}

void PartTracker::doTask(const std::unique_ptr<Generator>& generator, const Options& options) {
  std::cout << "PartitionNum " << options.partNum << "\n";

  std::vector<std::thread> threads;
  std::vector<std::promise<std::pair<uint64_t, int64_t>>> promises(CPUCore);
  std::vector<std::future<std::pair<uint64_t, int64_t>>> futures;

  threads.reserve(CPUCore);
  futures.reserve(CPUCore);

  srand(clock());

  int32_t numOfThreadsPerPart = CPUCore / options.partNum;

  int32_t LastThread = CPUCore - numOfThreadsPerPart * options.partNum;

  for (int32_t i = 0; i < options.partNum; ++i) {
    for (int32_t j = 0; j < numOfThreadsPerPart; ++j) {
      futures.emplace_back(promises[i * numOfThreadsPerPart + j].get_future());
      threads.emplace_back(std::thread(&Generator::startThisThread,
                                       generator.get(),
                                       i,
                                       std::ref(promises[i * numOfThreadsPerPart + j])));
    }
  }

  for (int32_t i = 0; i < LastThread; ++i) {
    futures.emplace_back(promises[numOfThreadsPerPart * options.partNum + i].get_future());
    threads.emplace_back(
        std::thread(&Generator::startThisThread,
                    generator.get(),
                    i,
                    std::ref(promises[numOfThreadsPerPart * options.partNum + i])));
  }

  int64_t time = 0;

  for (int32_t i = 0; i < CPUCore; ++i) {
    std::pair<uint64_t, int64_t> result = futures[i].get();
    measurement_.addRequestNum(result.first);
    time += result.second;
  }

  measurement_.addTime(static_cast<double>(time) / CPUCore);
  measurement_.setThreadNum(CPUCore);
  measurement_.setRequestType(RequestType::OP_AddVertex);
  measurement_.showTime();
  measurement_.reset();

  fflush(stdout);

  for (auto& thread : threads) {
    thread.join();
  }
}

void PartTracker::RemoveDir(const Options& options) {
  if (options.useCf) {
    std::string prefixPath = options.dataPath;
    prefixPath.push_back('/');
    Remove(prefixPath, "");
  } else {
    for (int i = 0; i < options.partNum; ++i) {
      std::string prefixPath = options.dataPath + "/part-" + std::to_string(i);
      prefixPath.push_back('/');
      Remove(prefixPath, "");
    }
  }
}

std::string PartTracker::getPath(const std::string& prefixPath, const std::string& suffixPath) {
  std::string path;

  if (suffixPath.size()) {
    path = prefixPath;
    path += suffixPath;
  } else {
    path = prefixPath;
  }
  return path;
}

void PartTracker::Remove(const std::string& prefixPath, const std::string& suffixPath) {
  if (suffixPath == ".." || suffixPath == ".") return;

  std::string path = getPath(prefixPath, suffixPath);

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

    path.push_back('/');  // directory

    while ((d = ::readdir(dir)) != nullptr) {
      Remove(path, d->d_name);
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
