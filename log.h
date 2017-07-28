#ifndef LOG_H_
#define LOG_H_

#include <assert.h>
#include <fstream>
#include <string>
#include <iostream>

#define FILE_NAME "gamelog.txt"

class Log {
 public:
  static void init() {
    outputFileStream.open(FILE_NAME, std::ofstream::out);
    assert(outputFileStream.is_open());
  }
  static void closeLog() {
    outputFileStream.close();
  }

  static std::ofstream outputFileStream;

  /* set floating point precision */
  static void precision(int precision) {
    outputFileStream << std::fixed;
    outputFileStream.precision(precision);
  }
};

template <typename T>
Log& operator << (Log& log, const T& data) {
  Log::outputFileStream << data;
  Log::outputFileStream.flush();
  return log;
}

template <typename T>
Log& operator << (Log& log, const T data[]) {
  Log::outputFileStream << data;
  Log::outputFileStream.flush();
  return log;
}

#endif
