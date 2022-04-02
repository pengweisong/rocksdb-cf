#pragma once

#include <assert.h>

#include <chrono>
#include <iostream>

enum RequestType : char { OP_AddVertex = 0x01, OP_AddEdge = 0x02 };

class Measurement {
 public:
  Measurement() : totalTime_(0), threadNum_(1), requestNum_(0) {}
  Measurement(int32_t threadNum) : totalTime_(0), threadNum_(threadNum), requestNum_(0) {}

  void calculateTime() {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime_ - startTime_);
    if (threadNum_ == 1) totalTime_ = duration.count();
  }

  void showTime() {
    printByType();

    std::cout << totalTime_ / 1000000.0 << "---"
              << "Requests:" << requestNum_ << "-----";
    std::cout << "Per Seconds requests:" << (1.0 / (totalTime_ / 1000000.0)) * requestNum_ << "\n";
  }

  void printByType() {
    switch (type_) {
      case RequestType::OP_AddEdge: {
        fprintf(stdout, "%31s", "Running ReadEdge   Seconds is:");
        break;
      }
      case RequestType::OP_AddVertex: {
        fprintf(stdout, "%31s", "Running ReadVertex Seconds is:");
        break;
      }
      default: {
        break;
      }
    }
  }

  void start() {
    startTime_ = std::chrono::steady_clock::now();
  }

  void stop() {
    endTime_ = std::chrono::steady_clock::now();
  }

  void setRequestNum(uint64_t requestNum) {
    requestNum_ = requestNum;
  }

  void setRequestType(RequestType type) {
    type_ = type;
  }

  void addTime(int64_t time) {
    totalTime_ += time;
  }

  void addRequestNum(uint64_t requestNum) {
    requestNum_ += requestNum;
  }

  int64_t getTotalTime() const {
    return totalTime_;
  }

  uint64_t getRequestNum() const {
    return requestNum_;
  }

  std::chrono::steady_clock::time_point getStartTime() const {
    return startTime_;
  }

  std::chrono::steady_clock::time_point getEndTime() const {
    return endTime_;
  }

 private:
  std::chrono::steady_clock::time_point startTime_;
  std::chrono::steady_clock::time_point endTime_;

  int64_t totalTime_;
  int32_t threadNum_;

  uint64_t requestNum_;
  RequestType type_;
};