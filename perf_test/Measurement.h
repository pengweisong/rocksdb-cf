#pragma once

#include <chrono>
#include <iostream>

enum RequestType : char { OP_AddVertex = 0x01, OP_AddEdge = 0x02, OP_Error = 0x03 };

class Measurement {
 public:
  Measurement() : requestNum_(0), type_(OP_Error) {}

  ~Measurement() {
    requestNum_ = 0;
    type_ = OP_Error;
  }

  void showTime() {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime_ - startTime_);
    double seconds = static_cast<double>(duration.count()) / 1000000;

    printType();
    std::cout << seconds << "---"
              << "Requests:" << requestNum_ << "\n";
    std::cout << "Per Seconds requests:" << (1.0 / seconds) * requestNum_ << "\n";
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

  void printType() {
    switch (type_) {
      case RequestType::OP_AddEdge: {
        std::cout << "Running ReadEdge Seconds is:";
        break;
      }
      case RequestType::OP_AddVertex: {
        std::cout << "Running ReadVertex Seconds is:";
      }
      default: {
        break;
      }
    }
  }

 private:
  std::chrono::steady_clock::time_point startTime_;
  std::chrono::steady_clock::time_point endTime_;

  uint64_t requestNum_;
  RequestType type_;
};
